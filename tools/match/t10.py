"""Accept a candidate unit only if the rebuilt ROM still matches retail.

Adds the source to the module's delinks, rebuilds the whole ROM and compares its
SHA-1 against the retail digest.  A miss restores the tree to exactly the state
it had before the attempt; that restore happens in a finally block, so an
interrupted or crashing run cannot leave a unit half-linked, a dangling delinks
entry, or an overwritten source behind.

`repair()` is the safety net for anything that went wrong outside a call to
`try_unit` (a killed process, a hand edit): it drops delinks entries whose
source is gone, relinks sources that lost their entry, and removes duplicate
entries.  `selfcheck()` reports the same conditions without changing anything.
"""
import pathlib
import re
import subprocess
import sys

EXPECT = "9eec3248d3569eba8abcb3244adcbdf01a5ea424"
ARM9_DELINKS = 'config/ATOJ/arm9/delinks.txt'
DIGEST_RE = re.compile(r'tennis_ATOJ\.nds:\s*([0-9a-f]{40})')
ENTRY_HEAD_RE = re.compile(r'^(src/[^\s:]+\.c):\s*$')


def delinks_for(mod):
    if mod == 'arm9':
        return ARM9_DELINKS
    return f'config/ATOJ/arm9/overlays/{mod}/delinks.txt'


def srcdir_for(mod):
    return 'src/arm9' if mod == 'arm9' else f'src/{mod}'


def _run(args):
    return subprocess.run([sys.executable] + args, capture_output=True, text=True)


def rom_digest():
    """Digest of the ROM the last build produced, or None."""
    r = _run(["-m", "ninja", "sha1"])
    m = DIGEST_RE.search(r.stdout + r.stderr)
    return m.group(1) if m else None


def sha1():
    return rom_digest() == EXPECT


def configure_build():
    _run(["tools/configure.py", "ATOJ"])
    # `rom` only: the default target also regenerates objdiff.json on every
    # attempt, which no gate reads.  The digest below is still checked.
    r = _run(["-m", "ninja", "rom"])
    return r.returncode == 0, (r.stdout + r.stderr)[-400:]


_FM = None


def _fm():
    """The shared compiler harness, loaded once and only when needed.

    `fastmatch` loads this module itself, so the import stays lazy.
    """
    global _FM
    if _FM is None:
        import importlib.util
        path = pathlib.Path(__file__).resolve().with_name('fastmatch.py')
        spec = importlib.util.spec_from_file_location('fastmatch_prefilter', path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        _FM = module
    return _FM


def unit_compiler(rel):
    """Per-unit mwccarm build from `config/ATOJ/mwcc_versions.txt`, or None."""
    path = pathlib.Path('config/ATOJ/mwcc_versions.txt')
    if not path.exists():
        return None
    for line in path.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        if not line or line.startswith('#') or '=' not in line:
            continue
        key, _, value = line.partition('=')
        if key.strip() == rel:
            return value.strip() or None
    return None


def reconfigure():
    _run(["tools/configure.py", "ATOJ"])


def entry_for(srcdir, name, addr, size):
    return (f"{srcdir}/{name}.c:\n    complete\n"
            f"    .text       start:{addr:#010x} end:{addr + size:#010x}\n")


def _entry_span(lines, rel):
    """(start, end) line indexes of the delinks block for `rel`, or None."""
    for i, ln in enumerate(lines):
        if ln.rstrip('\r\n') == rel + ':':
            j = i + 1
            while j < len(lines) and lines[j].startswith('    '):
                j += 1
            return i, j
    return None


def selfcheck(mod='arm9'):
    """Report the tree's unit bookkeeping without changing anything."""
    dl = pathlib.Path(delinks_for(mod))
    srcdir = pathlib.Path(srcdir_for(mod))
    lines = dl.read_text().splitlines(keepends=True)
    named = [m.group(1) for m in (ENTRY_HEAD_RE.match(ln) for ln in lines) if m]
    dangling = [n for n in named if not pathlib.Path(n).exists()]
    dupes = sorted({n for n in named if named.count(n) > 1})
    unlinked = sorted(p.as_posix() for p in srcdir.glob('*.c')
                      if p.as_posix() not in set(named))
    return dict(entries=len(named), sources=len(list(srcdir.glob('*.c'))),
                dangling=dangling, duplicates=dupes, unlinked=unlinked)


def _auto():
    import importlib.util
    spec = importlib.util.spec_from_file_location('auto', pathlib.Path(__file__).with_name('auto.py'))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def _relink(mod, text, srcdir):
    """Append entries for sources that lost theirs (an interrupted run, a hand edit)."""
    auto = _auto()
    auto.set_module(mod)
    added = []
    for p in sorted(srcdir.glob('*.c')):
        if f'{p.as_posix()}:' in text:
            continue
        addr = auto.NAME2ADDR.get(p.stem)
        if addr is None:
            continue
        text += entry_for(srcdir.as_posix(), p.stem, addr, auto.FUNCS[addr][1]) + '\n'
        added.append(p.stem)
    return text, added


def repair(mod='arm9'):
    """Restore the invariant: every source has exactly one entry, every entry a source."""
    dl = pathlib.Path(delinks_for(mod))
    srcdir = pathlib.Path(srcdir_for(mod))
    lines = dl.read_text().splitlines(keepends=True)

    # drop duplicate and dangling blocks, keep the first block per source
    out, seen, dropped = [], set(), []
    i = 0
    while i < len(lines):
        m = ENTRY_HEAD_RE.match(lines[i])
        if m:
            rel = m.group(1)
            span = _entry_span(lines, rel)
            start, end = span
            block = lines[start:end]
            if rel in seen or not pathlib.Path(rel).exists():
                dropped.append(rel)
                i = end
                continue
            seen.add(rel)
            out.extend(block)
            i = end
            continue
        out.append(lines[i])
        i += 1
    text, relinked = _relink(mod, ''.join(out), srcdir)
    if text != ''.join(out) and not text.endswith('\n'):
        text += '\n'
    dl.write_text(text)
    reconfigure()
    return dict(dropped=dropped, relinked=relinked, remaining=selfcheck(mod))


def try_unit(name, addr, size, body, variants=None, mod='arm9'):
    """Test one candidate; returns True on a match, and never leaves a partial unit."""
    if not body or not body.strip():
        print(f'  skip   {name:22} (empty body)')
        return False

    bodies = [body] + list(variants or [])
    dl = pathlib.Path(delinks_for(mod))
    srcdir = pathlib.Path(srcdir_for(mod))
    srcdir.mkdir(parents=True, exist_ok=True)
    src = srcdir / f'{name}.c'
    rel = src.as_posix()

    before_text = dl.read_text()
    before_src = src.read_bytes() if src.exists() else None
    had_entry = _entry_span(before_text.splitlines(keepends=True), rel) is not None

    # Sound, cheap rejection first.  The byte compare masks the words the
    # linker fills, so it can accept a body the digest later rejects, but it
    # cannot reject one that would match: any differing unmasked byte is a
    # differing byte in the linked ROM too.  A body it rejects therefore never
    # earns a relink, and the digest below still decides every acceptance.
    try:
        fm = _fm()
        compiler = unit_compiler(rel)
        keep = [i for i, b in enumerate(bodies)
                if fm.compare(b, mod, addr, size, compiler)]
    except Exception:
        keep = list(range(len(bodies)))
    for i in range(len(bodies)):
        if i not in keep:
            print(f'  pref   {name:22} variant #{i} (bytes differ, no build)')
    if not keep:
        print(f'  miss   {name}')
        return False

    def restore():
        dl.write_text(before_text)
        if before_src is None:
            src.unlink(missing_ok=True)
        else:
            src.write_bytes(before_src)
        # Only a delinks change needs the build graph regenerated; a re-test
        # writes the same text back and leaves build.ninja valid.
        if not had_entry:
            reconfigure()

    try:
        for i in keep:
            b = bodies[i]
            src.write_text('#include "types.h"\n\n' + b)
            if not had_entry:
                dl.write_text(before_text.rstrip('\n') + '\n\n'
                              + entry_for(srcdir.as_posix(), name, addr, size))
            ok, log = configure_build()
            if ok and sha1():
                print(f'  MATCH  {name:22} variant #{i}')
                return True
            restore()
            if not ok and i == keep[0]:
                last = log.strip().splitlines()[-1][:120] if log.strip() else '?'
                print(f'  link/compile issue for {name}: {last}')
        print(f'  miss   {name}')
        return False
    except BaseException:
        restore()
        raise
    finally:
        # whatever happened, the tree must satisfy the invariant
        state = selfcheck(mod)
        if state['dangling'] or state['unlinked'] or state['duplicates']:
            print(f'  repair {name}: dangling={state["dangling"][:2]} '
                  f'unlinked={state["unlinked"][:2]} dupes={state["duplicates"][:2]}')
            repair(mod)


if __name__ == '__main__':
    if len(sys.argv) > 1 and sys.argv[1] == '--check':
        mods = sys.argv[2:] or ['arm9']
        bad = 0
        for m in mods:
            state = selfcheck(m)
            flag = 'ok' if not (state['dangling'] or state['unlinked'] or state['duplicates']) else 'BROKEN'
            bad += flag == 'BROKEN'
            print(f'{m}: {state["entries"]} entries, {state["sources"]} sources, '
                  f'dangling={len(state["dangling"])}, unlinked={len(state["unlinked"])}, '
                  f'dupes={len(state["duplicates"])} -> {flag}')
        sys.exit(1 if bad else 0)
    if len(sys.argv) > 1 and sys.argv[1] == '--repair':
        for m in sys.argv[2:] or ['arm9']:
            print(m, repair(m))
