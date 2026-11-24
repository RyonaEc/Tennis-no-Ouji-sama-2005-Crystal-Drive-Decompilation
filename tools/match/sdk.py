"""Find a retail function inside prebuilt NitroSDK libraries.

A unit that no compiler build and no source spelling reproduces is often not
game code at all: it is a prebuilt NitroSDK library routine, hand-written asm in
the SDK (fx_mtx33.c and friends) or simply compiled for Nintendo by Nintendo.
`tools/match/sdk.py` searches the extracted SDK trees for the retail bytes and
says which library, member and offset they came from, so the answer is "this is
MTX_RotX33_ from libfx" instead of a wall note about compiler generations.

    python tools/match/sdk.py arm9 0x0201fcc8 28
    python tools/match/sdk.py --hex 01231b030360002343608360c3600261416183614942c16102627047
    python tools/match/sdk.py arm9 0x0201fce4 28 --root build/cand/sdk --top 5

Roots default to `build/cand/sdk`, where the archives are extracted. They are
Nintendo SDK downloads and are gitignored: fetch them from
<https://twlsdk.randommeaninglesscharacters.com/download/NitroSDK/> and extract
under `build/cand/sdk/<release>/`, never into the repository proper.

Partial hits are the normal case for compiled (not asm) library functions,
whose instructions carry relocations the retail image has already applied: the
tool reports how many leading bytes agree, and the report is evidence about
provenance, not a match. `reg.py` and the ROM digest still decide.
"""

import argparse
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))


def ar_members(data):
    """Yield (name, payload) for each member of a System V / GNU ar archive."""
    if not data.startswith(b"!<arch>\n"):
        return
    i = 8
    while i + 60 <= len(data):
        hdr = data[i:i + 60]
        name = hdr[:16].decode("latin-1").strip().rstrip("/")
        try:
            size = int(hdr[48:58].decode("latin-1").strip())
        except ValueError:
            return
        yield name, data[i + 60:i + 60 + size]
        i += 60 + size + (size & 1)


def common_prefix(a, b):
    n = min(len(a), len(b))
    i = 0
    while i < n and a[i] == b[i]:
        i += 1
    return i


def search_file(path, needle, size, min_prefix):
    """Return (score, where, offset) for the best hit inside one file."""
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError:
        return None

    prefix = needle[:min_prefix] if len(needle) >= min_prefix else needle
    best = None

    def check(blob, where, base=0):
        nonlocal best
        pos = blob.find(prefix)
        while pos != -1:
            window = blob[pos:pos + size]
            score = common_prefix(window, needle)
            if score >= min_prefix and (best is None or score > best[0]):
                best = (score, where, base + pos)
            pos = blob.find(prefix, pos + 1)

    members = list(ar_members(data) or ())
    if members:
        # Scanning the container as well would just repeat every member hit at
        # the container's own offsets; the member name is the useful one.
        for name, payload in members:
            check(payload, "%s(%s)" % (os.path.basename(path), name))
    else:
        check(data, os.path.basename(path))
    return best


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("module", nargs="?", help="module, e.g. arm9 or ov003")
    ap.add_argument("addr", nargs="?", help="retail address, e.g. 0x0201fcc8")
    ap.add_argument("size", nargs="?", type=int, help="function size in bytes")
    ap.add_argument("--hex", help="search this hex string instead of retail bytes")
    ap.add_argument("--root", action="append",
                    help="SDK tree to search (default build/cand/sdk, repeatable)")
    ap.add_argument("--top", type=int, default=10, help="hits to print (default 10)")
    ap.add_argument("--min-prefix", type=int, default=8,
                    help="leading bytes that must agree before a hit counts")
    args = ap.parse_args(argv)

    if args.hex:
        needle = bytes.fromhex(args.hex)
        size = args.size or len(needle)
        label = "--hex"
    else:
        if not (args.module and args.addr and args.size):
            ap.error("give <module> <addr> <size>, or --hex")
        import fastmatch as fm
        needle = fm.retail(args.module, int(args.addr, 0), args.size)
        size = args.size
        label = "%s 0x%08x (%d B)" % (args.module, int(args.addr, 0), args.size)

    roots = args.root or ["build/cand/sdk"]
    roots = [r for r in roots if os.path.isdir(r)]
    if not roots:
        print("no SDK tree found (looked in %s)" % ", ".join(args.root or ["build/cand/sdk"]))
        print("fetch an archive from https://twlsdk.randommeaninglesscharacters.com/download/NitroSDK/")
        print("and extract it under build/cand/sdk/<release>/ (gitignored)")
        return 2

    hits = []
    scanned = 0
    for root in roots:
        for dirpath, _dirnames, filenames in os.walk(root):
            for fn in filenames:
                if not fn.lower().endswith((".a", ".o", ".elf")):
                    continue
                path = os.path.join(dirpath, fn)
                scanned += 1
                got = search_file(path, needle, size, args.min_prefix)
                if got:
                    hits.append((got[0], got[1], got[2], path))

    if not hits:
        print("%s: not found in %d objects under %s" % (label, scanned, ", ".join(roots)))
        print("no leading %d bytes agree, so this is probably game code" % args.min_prefix)
        return 1

    hits.sort(key=lambda h: (-h[0], h[3]))
    print("%s: searched %d objects" % (label, scanned))
    for score, where, off, path in hits[:args.top]:
        mark = "exact" if score == size else "%d/%d bytes" % (score, size)
        rel = os.path.relpath(path, os.getcwd())
        print("  %-14s %s +0x%x  [%s]" % (mark, where, off, rel))
    return 0


if __name__ == "__main__":
    sys.exit(main())
