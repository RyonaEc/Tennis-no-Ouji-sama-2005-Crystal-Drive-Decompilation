"""Grind the queue unattended: re-spell every unit's best candidate, journal it.

One `respell.py` run costs a tool call and stops when the agent's turn ends.
This is the same search without the turn. It walks the parked walls first (their
best shapes are already written down, so they are the cheapest targets), then
the live unmatched queue smallest-first, re-spelling each unit's closest
candidate along the axes that decide register colouring.

`--daemon` is the 24/7 mode: pass after pass with a sleep between them, a
heartbeat in `build/grind-status.json`, and a graceful stop file, so it can be
left running for hours without a supervising agent and stopped without killing a
process by pattern.

The gate does not move. A variant that scores zero differing slots is handed to
`t10.try_unit`, which byte-compares it again and only keeps it if the linked ROM
still hashes to retail. Wrong variants are rejected by `fm.diff_summary` for
~0.1 s each and never earn a relink, so an hour of wall clock is tens of
thousands of scored variants instead of a few hundred.

Resumable: every finished unit is appended to `build/grind-journal.jsonl`, so a
restart, a crash or a kill skips what was already exhausted. The journal also
records each unit's best residue, which is what a later hand pass should start
from.

Usage:
    python tools/match/grind.py [--lane walls|queue|all] [--module M]...
                                [--hours H] [--limit N] [--budget N]
                                [--bases N] [--no-pad] [--daemon]
                                [--max-passes N] [--sleep S] [--commit]
                                [--retry-slots N] [--status] [--dry-run]
                                [--reset]

    --lane        walls (default), queue, or all (walls first, queue after)
    --module      restrict to a module; repeatable
    --hours       stop after this many hours (default 0: no limit)
    --limit       visit at most N units per pass (default 0: no limit)
    --budget      variants scored per unit (default 600)
    --bases       candidates re-spelled per unit (default 3)
    --no-pad      do not add the dead-local variants (respell's --pad)
    --daemon      run pass after pass until stopped
    --max-passes  stop the daemon after N passes (default 0: forever)
    --sleep       seconds between daemon passes (default 60)
    --commit      commit each kept unit locally (never pushes, never edits docs)
    --retry-slots re-sweep only the journal's near-misses (<= N slots, exhausted),
                  cheapest residue first - where a deeper budget pays most
    --status      print the journal and heartbeat, then exit
    --dry-run     list the units and their candidate sources, score nothing
    --reset       delete the journal first

Stopping a daemon run: create `build/grind.stop` (any content). It is noticed
between units, honoured, and removed, so a stopped run can be restarted without
touching the file again. Killing the exact PID also works - `t10.try_unit`
restores the tree in a `finally` block - but loses at most the unit in flight.

The docs are not this tool's job: after a run, `progress.py`, `README.md`, the
push and the report upload are still the hand pass, and `--commit` exists so an
unattended run's matches are at least in local git history rather than only in
the working tree.

One instance per build tree: `t10.try_unit` runs `configure.py` and `ninja rom`,
which are single-tree operations. Parallel workers belong in their own
worktrees, one module each.
"""
import importlib.util
import itertools
import json
import os
import pathlib
import re
import subprocess
import sys
import time


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


fm = load("tools/match/fastmatch.py", "fm")
t10 = load("tools/match/t10.py", "t10")
respell = load("tools/match/respell.py", "respell")
fresh = load("tools/match/fresh.py", "fresh")
clone = load("tools/match/clone.py", "clone")

JOURNAL = pathlib.Path("build/grind-journal.jsonl")
STATUS = pathlib.Path("build/grind-status.json")
STOP = pathlib.Path("build/grind.stop")
WALLS = pathlib.Path("tools/match/walls.txt")
INCLUDE = re.compile(r'^\s*#include\s+"types\.h"\s*\n', re.M)
HEX8 = re.compile(r"[0-9a-fA-F]{8}")
WALL_RE = re.compile(r"^(\w+)\s+0x0*([0-9a-fA-F]+)\s+(\d+)B\s+(\S+)")

# Candidate files live under one flat directory and are named either for the
# function (`func_ov008_0204c810.c`) or for the bare address (`0204c810_x.c`),
# so the address is the reliable key.  Every overlay loads at the same base, so
# a bare address is ambiguous across modules: a name that mentions a module is
# indexed under that module and the lookup prefers the module-specific entry.
INDEX = {}
CLONE_INDEX = {}


def strip_include(text):
    return INCLUDE.sub("", text, count=1)


def candidate_index():
    if INDEX:
        return INDEX
    root = pathlib.Path("build/cand")
    if not root.exists():
        return INDEX
    for path in root.iterdir():
        if not path.is_file() or path.suffix != ".c":
            continue
        mod = re.search(r"(ov\d{3})", path.stem)
        for hexed in set(HEX8.findall(path.stem)):
            key = int(hexed, 16)
            INDEX.setdefault((mod.group(1) if mod else None, key), []).append(path)
    return INDEX


def stored_candidates(mod, addr):
    index = candidate_index()
    paths = list(index.get((mod, addr), []))
    paths += [p for p in index.get((None, addr), []) if p not in paths]
    return paths


def clone_candidates(mod, addr, size, cap):
    """Clone matched siblings: the shape most of the queue has nowhere else.

    Most of the queue never had a `build/cand` file, so re-spelling alone would
    reach under a third of it.  A unit whose instruction skeleton equals an
    already-matched unit's is the cheapest kind of new candidate, and
    `clone.py` builds it without weakening anything: the constants are mapped
    by program order and refused on a clash.

    The strict skeleton (same mnemonics *and* register allocation) is tried
    first; if nothing matches, `clone.LOOSE` allows a sibling that differs in
    colouring, which is the wall class this project keeps hitting.  A loose
    clone still has to be byte-exact to be installed, so the extra candidates
    cost compile time and nothing else.
    """
    for loose in (False, True):
        key = (mod, loose)
        if key not in CLONE_INDEX:
            previous = clone.LOOSE
            clone.LOOSE = loose
            try:
                CLONE_INDEX[key] = clone.index_for(mod)
            except Exception:
                CLONE_INDEX[key] = {}
            finally:
                clone.LOOSE = previous
        previous = clone.LOOSE
        clone.LOOSE = loose
        try:
            found = clone.siblings(mod, addr, size, CLONE_INDEX[key])
        except Exception:
            found = []
        finally:
            clone.LOOSE = previous
        if found:
            return [("clone:" + name, text) for name, text in found[:cap]]
    return []


def gen_candidate(mod, addr, size):
    """Fall back to the code generator when nothing stored compiles."""
    auto = fm.auto
    try:
        auto.set_module(mod)
        fn = auto.gen(addr, size)
        if not fn:
            return []
        return [("auto", auto.emit(fn))]
    except Exception:
        return []


def bail_reason(mod, addr, size):
    """Why the generator produced nothing usable for a unit.

    `no-candidate` covers three different situations -- the instruction walk
    bailed, the emitter failed, or the emitted source does not compile -- and
    they want different fixes. Record which one it was so the journal is a
    worklist and not a wall.
    """
    auto = fm.auto
    try:
        auto.set_module(mod)
        fn = auto.gen(addr, size)
    except (Exception, SystemExit) as exc:
        return str(exc) or type(exc).__name__
    try:
        text = auto.emit(fn)
    except (Exception, SystemExit) as exc:
        return "emit: %s" % (str(exc) or type(exc).__name__)
    if fm.compile_text(text, "bail"):
        return "compiles but scored nothing"
    stub = pathlib.Path("build/_bail_%d" % os.getpid())
    stub_c = stub.with_suffix(".c")
    try:
        stub_c.write_text('#include "types.h"\n\n' + strip_include(text), encoding="utf-8")
        err = subprocess.run(["./tools/sjiswrap.exe", fm.MWCC_EXE, *fm.MWCC_FLAGS,
                              "-c", str(stub_c), "-o", str(stub.with_suffix(".o"))],
                             capture_output=True, text=True).stderr
    except Exception:
        return "source does not compile"
    finally:
        stub_c.unlink(missing_ok=True)
        stub.with_suffix(".o").unlink(missing_ok=True)
    msg = [ln.strip() for ln in err.splitlines() if "error" in ln.lower()]
    return "compile: %s" % (msg[0] if msg else (err.strip().splitlines() or ["no message"])[0])


def score(mod, addr, size, rows, dry):
    """Keep the candidates that compile, with their differing-slot count."""
    scored = []
    for label, text in rows:
        if "{" not in text:
            continue
        if dry:
            scored.append((0, label, text))
            continue
        try:
            _ok, n, diffs = fm.diff_summary(mod, addr, size, strip_include(text))
        except Exception:
            continue
        if n:
            scored.append((len(diffs), label, text))
    return scored


def bases_for(mod, addr, size, cap, dry):
    """The closest candidates for a unit, best first.

    Three sources know different things: a stored candidate from a previous hand
    pass, a clone of a matched sibling with the same skeleton, and the code
    generator's own reading of the instructions. Take the best of *each* rather
    than stopping at the first family that scores -- a stored candidate that
    merely compiles used to shadow the generator completely, which is how
    arm9 0x0201f26c sat exhausted with a byte-exact generator candidate that the
    sweep never compiled.
    """
    stored = []
    for path in stored_candidates(mod, addr):
        try:
            stored.append((path.name, path.read_text(encoding="utf-8", errors="replace")))
        except OSError:
            continue
    picked = list(score(mod, addr, size, stored, dry))
    picked.sort(key=lambda row: row[0])
    picked = picked[:cap]
    # Append the best of the other two families rather than letting the stored
    # list stand or fall alone: `cap` still bounds the re-spelling cost, and this
    # cannot lose a candidate the old ordering tried.
    for extra in (clone_candidates(mod, addr, size, cap), gen_candidate(mod, addr, size)):
        rows = score(mod, addr, size, extra, dry)
        if rows:
            rows.sort(key=lambda row: row[0])
            picked.append(rows[0])
    picked.sort(key=lambda row: row[0])
    return picked


def commit_unit(mod, name):
    """Commit one kept unit locally.  Never pushes; the docs are the hand pass."""
    src = pathlib.Path("src", mod, name + ".c").as_posix()
    delinks = pathlib.Path(t10.delinks_for(mod)).as_posix()
    message = ("Match %s (%s)\n\n"
               "Installed by tools/match/grind.py after t10.try_unit confirmed\n"
               "the ROM digest on this machine.\n" % (name, mod))
    try:
        subprocess.run(["git", "commit", "-q", "-m", message, "--", src, delinks],
                       capture_output=True, text=True)
        return True
    except Exception:
        return False


def grind_unit(mod, addr, size, name, opts, journal):
    """Spell one unit's candidates.  Returns (status, variants, best)."""
    dry = opts["dry"]
    bases = bases_for(mod, addr, size, opts["bases"], dry)
    if dry:
        label = ", ".join(b[1] for b in bases) or "no candidate"
        print("[grind] %-6s %#010x %5dB %-24s %s" % (mod, addr, size, name, label),
              flush=True)
        return "dry", 0, None
    if not bases:
        why = bail_reason(mod, addr, size)
        note(journal, dict(t="unit", mod=mod, addr=addr, name=name, size=size,
                           status="no-candidate", variants=0, best=None, why=why))
        print("[grind] %-6s %#010x %5dB %-24s no candidate (%s)" % (mod, addr, size, name, why),
              flush=True)
        return "no-candidate", 0, None

    tried, best, hit = 0, None, None
    for _score, base_label, text in bases:
        # Materialise the grid before scoring it: `respell.split_unit` reports a
        # candidate it cannot parse with SystemExit, and a generator raises that
        # in the middle of the loop, where a 24/7 process must not die.  The
        # budget bounds the list, so this costs no more memory than the run.
        try:
            variants = list(itertools.islice(respell.variants(text, pad=opts["pad"]),
                                             max(0, opts["budget"] - tried)))
        except (Exception, SystemExit):
            continue
        for vlabel, source in variants:
            if tried >= opts["budget"]:
                break
            body = strip_include(source)
            try:
                ok, n, diffs = fm.diff_summary(mod, addr, size, body)
            except Exception:
                continue
            tried += 1
            if not n:
                continue
            label = base_label + "+" + vlabel
            if best is None or len(diffs) < best[0]:
                best = (len(diffs), label)
            if ok:
                out = pathlib.Path("build/cand/grind_%s.c" % name)
                out.write_text(source)
                kept = t10.try_unit(name, addr, size, body, mod=mod)
                committed = commit_unit(mod, name) if (kept and opts["commit"]) else False
                note(journal, dict(t="match", mod=mod, addr=addr, name=name, size=size,
                                   label=label, kept=bool(kept), committed=committed,
                                   candidate=out.as_posix()))
                if kept:
                    print("[grind] MATCH  %-6s %#010x %5dB %-24s %s%s"
                          % (mod, addr, size, name, label,
                             " (committed)" if committed else ""), flush=True)
                else:
                    print("[grind] bytes  %-6s %#010x %5dB %-24s %s "
                          "-> rejected by the digest, continuing"
                          % (mod, addr, size, name, label), flush=True)
                hit = label
                break
        if hit or tried >= opts["budget"]:
            break

    status = "matched" if hit else "exhausted"
    note(journal, dict(t="unit", mod=mod, addr=addr, name=name, size=size,
                       status=status, variants=tried,
                       best=(best[1] if best else None),
                       slots=(best[0] if best else None)))
    if not hit:
        where = "%d slots %s" % best if best else "no compiling variant"
        print("[grind] %-6s %#010x %5dB %-24s %d variants, best %s"
              % (mod, addr, size, name, tried, where), flush=True)
    return status, tried, best


def note(journal, record):
    with journal.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(record) + "\n")


def heartbeat(**fields):
    """Overwrite the status file: the cheap answer to 'is it still alive?'."""
    record = dict(t="beat", pid=os.getpid(),
                  when=time.strftime("%Y-%m-%dT%H:%M:%S"))
    record.update(fields)
    try:
        STATUS.write_text(json.dumps(record, indent=1) + "\n")
    except OSError:
        pass


def done_units(journal):
    """Units a previous run finished, matched or exhausted."""
    if not journal.exists():
        return set()
    out = set()
    for line in journal.read_text(encoding="utf-8", errors="replace").splitlines():
        try:
            record = json.loads(line)
        except ValueError:
            continue
        if record.get("t") in ("unit", "match"):
            out.add((record.get("mod"), record.get("addr")))
    return out


def wall_units(modules):
    if not WALLS.exists():
        return []
    out = []
    for line in WALLS.read_text(encoding="utf-8", errors="replace").splitlines():
        m = WALL_RE.match(line)
        if not m:
            continue
        mod, addr, size, name = m.group(1), int(m.group(2), 16), int(m.group(3)), m.group(4)
        if modules and mod not in modules:
            continue
        out.append((size, mod, addr, size, name))
    return out


def queue_units(modules, lo, hi):
    mods = modules or fresh.modules()
    out = []
    for mod in mods:
        try:
            rows = fresh.unmatched(mod)
        except Exception:
            continue
        for _m, addr, size, name in rows:
            if lo <= size <= hi:
                out.append((size, mod, addr, size, name))
    return out


def near_miss_units(journal, max_slots, modules):
    """Units that finished a pass close to a match, cheapest residue first.

    A unit recorded at one or two differing slots is where a second, deeper
    sweep pays: the shape is nearly right, so a bigger budget or another base
    candidate is worth more there than anywhere in the untouched queue.
    """
    best = {}
    if not journal.exists():
        return []
    for line in journal.read_text(encoding="utf-8", errors="replace").splitlines():
        try:
            record = json.loads(line)
        except ValueError:
            continue
        if record.get("t") != "unit" or record.get("status") != "exhausted":
            continue
        slots = record.get("slots")
        if slots is None or slots > max_slots:
            continue
        if modules and record.get("mod") not in modules:
            continue
        key = (record.get("mod"), record.get("addr"))
        if key not in best or slots < best[key][0]:
            best[key] = (slots, record["mod"], record["addr"], record.get("name"),
                         record.get("size"))

    sizes = {}
    for mod in {row[1] for row in best.values()}:
        try:
            for _m, addr, size, _name in fresh.unmatched(mod):
                sizes[(mod, addr)] = size
        except Exception:
            continue
    out = []
    for slots, mod, addr, name, size in best.values():
        # The live delinked range wins: a wall line or an old journal record can
        # carry a size from before the unit boundaries were fixed, and scoring a
        # body against the wrong length hides its real residue.
        size = sizes.get((mod, addr)) or size
        if not size:
            continue
        if pathlib.Path("src", mod, name + ".c").exists():
            continue
        out.append((slots, mod, addr, size, name))
    return out


def todo_units(opts, finished):
    if opts["lane"] == "retry":
        rows = near_miss_units(JOURNAL, opts["retry_slots"], opts["modules"])
        return sorted(rows, key=lambda r: (r[0], r[3], r[2]))
    # The walls lane stays in front: those units have a written-down best shape,
    # so they are the cheapest ground.  Each lane is ordered smallest-first.
    units = []
    if opts["lane"] in ("walls", "all"):
        units += sorted(wall_units(opts["modules"]), key=lambda r: (r[0], r[1], r[2]))
    if opts["lane"] in ("queue", "all"):
        units += sorted(queue_units(opts["modules"], opts["lo"], opts["hi"]),
                        key=lambda r: (r[0], r[1], r[2]))
    seen, todo = set(), []
    for size, mod, addr, sz, name in units:
        if (mod, addr) in seen:
            continue
        seen.add((mod, addr))
        if pathlib.Path("src", mod, name + ".c").exists():
            continue
        if (mod, addr) in finished:
            continue
        todo.append((size, mod, addr, sz, name))
    return todo


def run_pass(opts, journal, number):
    """One walk of the lane.  Returns (visited, counts, variants, reason)."""
    # In retry mode the units are by definition already in the journal, so the
    # completed set must not filter them out again.
    finished = set() if (opts["dry"] or opts["lane"] == "retry") else done_units(journal)
    todo = todo_units(opts, finished)
    print("[grind] pass=%d lane=%s units=%d already-done=%d budget=%d/unit pad=%s%s"
          % (number, opts["lane"], len(todo), len(finished), opts["budget"],
             opts["pad"], " dry-run" if opts["dry"] else ""), flush=True)
    if not opts["dry"]:
        note(journal, dict(t="start", lane=opts["lane"], units=len(todo),
                           budget=opts["budget"], pad=opts["pad"],
                           modules=opts["modules"] or None, pass_=number,
                           when=time.strftime("%Y-%m-%dT%H:%M:%S")))

    started = time.monotonic()
    counts = {"matched": 0, "exhausted": 0, "no-candidate": 0, "dry": 0}
    variants = visited = 0
    reason = "queue-empty"
    for size, mod, addr, sz, name in todo:
        if opts["limit"] and visited >= opts["limit"]:
            reason = "limit"
            break
        if opts["hours"] and time.monotonic() - started >= opts["hours"] * 3600:
            reason = "hours"
            break
        if STOP.exists():
            reason = "stop-file"
            STOP.unlink(missing_ok=True)
            break
        if pathlib.Path("src", mod, name + ".c").exists():
            continue
        visited += 1
        try:
            status, tried, _best = grind_unit(mod, addr, sz, name, opts, journal)
        except KeyboardInterrupt:
            reason = "interrupted"
            break
        except (Exception, SystemExit) as exc:
            # SystemExit included: a bad candidate is a bad unit, not a reason to
            # end an overnight run.
            print("[grind] %-6s %#010x %-24s error: %s: %s"
                  % (mod, addr, name, type(exc).__name__, exc), flush=True)
            continue
        counts[status] = counts.get(status, 0) + 1
        variants += tried
        if not opts["dry"]:
            heartbeat(pass_=number, units=visited, matched=counts["matched"],
                      variants=variants, unit="%s %#x" % (mod, addr))

    elapsed = time.monotonic() - started
    print("[grind] stop reason=%s units=%d matched=%d exhausted=%d "
          "no-candidate=%d variants=%d in %.0fs"
          % (reason, visited, counts["matched"], counts["exhausted"],
             counts["no-candidate"], variants, elapsed), flush=True)
    if not opts["dry"]:
        note(journal, dict(t="stop", reason=reason, units=visited,
                           matched=counts["matched"], variants=variants,
                           seconds=round(elapsed), pass_=number))
        heartbeat(state="idle", last_reason=reason, units=visited,
                  matched=counts["matched"], variants=variants)
    return visited, counts, variants, reason


def show_status():
    """Everything a human needs to decide whether the daemon is healthy."""
    if STATUS.exists():
        try:
            beat = json.loads(STATUS.read_text())
        except ValueError:
            beat = {}
        print("heartbeat: %s" % json.dumps(beat))
    else:
        print("heartbeat: none (no run has started in this tree)")
    if not JOURNAL.exists():
        print("journal: none")
        return 0
    units = matches = variants = 0
    best = []
    for line in JOURNAL.read_text(encoding="utf-8", errors="replace").splitlines():
        try:
            record = json.loads(line)
        except ValueError:
            continue
        if record.get("t") == "unit":
            units += 1
            variants += record.get("variants") or 0
            if record.get("slots") is not None and record.get("status") != "matched":
                best.append((record["slots"], record.get("mod"), record.get("name"),
                             record.get("best")))
        elif record.get("t") == "match":
            matches += 1
    print("journal: %d units visited, %d matches, %d variants" % (units, matches, variants))
    best.sort()
    if best:
        print("closest exhausted units (slots, module, name, axis):")
        for slots, mod, name, label in best[:12]:
            print("  %2d  %-6s %-26s %s" % (slots, mod, name, label))
    return 0


def main(argv):
    if "-h" in argv or "--help" in argv:
        print(__doc__.strip())
        return 0
    if "--status" in argv:
        return show_status()

    opts = dict(lane="walls", hours=0.0, limit=0, budget=600, bases=3,
                lo=4, hi=0x200, pad=True, dry=False, reset=False,
                daemon=False, max_passes=0, sleep=60.0, commit=False,
                retry_slots=0, modules=[])
    for i, a in enumerate(argv):
        if a == "--lane":
            opts["lane"] = argv[i + 1]
        elif a == "--module":
            opts["modules"].append(argv[i + 1])
        elif a == "--hours":
            opts["hours"] = float(argv[i + 1])
        elif a == "--limit":
            opts["limit"] = int(argv[i + 1], 0)
        elif a == "--budget":
            opts["budget"] = int(argv[i + 1], 0)
        elif a == "--bases":
            opts["bases"] = int(argv[i + 1], 0)
        elif a == "--min":
            opts["lo"] = int(argv[i + 1], 0)
        elif a == "--max":
            opts["hi"] = int(argv[i + 1], 0)
        elif a == "--max-passes":
            opts["max_passes"] = int(argv[i + 1], 0)
        elif a == "--sleep":
            opts["sleep"] = float(argv[i + 1])
        elif a == "--no-pad":
            opts["pad"] = False
        elif a == "--dry-run":
            opts["dry"] = True
        elif a == "--reset":
            opts["reset"] = True
        elif a == "--daemon":
            opts["daemon"] = True
        elif a == "--commit":
            opts["commit"] = True
        elif a == "--retry-slots":
            opts["retry_slots"] = int(argv[i + 1], 0)
            opts["lane"] = "retry"

    journal = JOURNAL
    journal.parent.mkdir(parents=True, exist_ok=True)
    if opts["reset"] and journal.exists():
        journal.unlink()
    if STOP.exists() and not opts["daemon"]:
        STOP.unlink(missing_ok=True)

    started = time.monotonic()
    passes = 0
    while True:
        passes += 1
        try:
            visited, counts, _variants, reason = run_pass(opts, journal, passes)
        except KeyboardInterrupt:
            break
        except (Exception, SystemExit) as exc:
            # The pass itself fell over.  In daemon mode that is a log line and a
            # pause, never the end of the process.
            print("[grind] pass %d failed: %s: %s"
                  % (passes, type(exc).__name__, exc), flush=True)
            if not opts["daemon"]:
                raise
            visited = 0
            time.sleep(max(opts["sleep"], 60.0))
            continue
        if not opts["daemon"]:
            break
        if reason == "stop-file":
            # build/grind.stop means stop, not "finish this pass and carry on"
            print("[grind] stopped by %s" % STOP.as_posix(), flush=True)
            break
        if opts["max_passes"] and passes >= opts["max_passes"]:
            break
        if opts["hours"] and time.monotonic() - started >= opts["hours"] * 3600:
            break
        # A pass that had nothing left is unlikely to have anything in a minute:
        # only a new candidate or an edit changes that, so back off.
        pause = opts["sleep"] if visited else max(opts["sleep"], 1800.0)
        print("[grind] sleeping %.0fs before pass %d" % (pause, passes + 1), flush=True)
        # Poll the stop file while sleeping: an idle daemon waits half an hour
        # between passes, and a stop that is only noticed a pass later is not
        # the graceful stop this file promises.
        remaining = pause
        try:
            while remaining > 0 and not STOP.exists():
                chunk = min(15.0, remaining)
                time.sleep(chunk)
                remaining -= chunk
        except KeyboardInterrupt:
            break
        if STOP.exists():
            STOP.unlink(missing_ok=True)
            print("[grind] stopped by %s" % STOP.as_posix(), flush=True)
            break
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
