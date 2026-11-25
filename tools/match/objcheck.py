"""Check that an installed unit's object equals retail's, relocations included.

`var3`, `fastmatch` and objdiff all mask the words a relocation fills, so a
source that spells a relocated address as a plain number compiles and links to
the right byte and still scores below 100%: the value is right but the object
carries no relocation where retail has one.  `ninja sha1` cannot see that
either, and the unit then silently drops out of the matched counts while still
counting as complete.

`src/arm9/func_02004a14` was exactly that - `ldr r0, =0x027c0000` instead of
`=data_027c0000` - and cost 232 bytes and one function in the published
numbers.  This tool compares the unit's object with the delinked retail object,
word for word and relocation for relocation, and names the missing relocation.

It is a diagnostic, not a gate: it is stricter than objdiff, which resolves
relocations to addresses and therefore accepts an equivalent symbol+addend (a
`data_0204acdc[num]` access that retail records against the next symbol) and the
branch placeholders a delink leaves behind.  When `progress.py` reports a unit
below 100%, run this on that unit to see why.

    python tools/match/objcheck.py [mod ...]      audit installed units
    python tools/match/objcheck.py --unit <mod> <name>

Exit status is non-zero when any unit is not perfect; expect a handful of
address-equivalent findings on a healthy tree.
"""
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
BUILD = ROOT / 'build' / 'ATOJ'

SHF_TEXT = 1


def _sections(data):
    e_shoff, = struct.unpack_from('<I', data, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from('<HHH', data, 0x2E)
    hdrs = [struct.unpack_from('<IIIIIIIIII', data, e_shoff + i * e_shentsize)
            for i in range(e_shnum)]
    stroff = hdrs[e_shstrndx][4]
    named = {}
    for h in hdrs:
        end = data.index(b'\0', stroff + h[0])
        named.setdefault(data[stroff + h[0]:end].decode(), h)
    return data, named


def _relocs(data, named, section):
    """{offset: (symbol name, addend)} for relocations against one section."""
    symtab = named.get('.symtab')
    if symtab is None:
        return {}
    out = {}
    for name, entsize in ((f'.rela{section}', 12), (f'.rel{section}', 8)):
        sec = named.get(name)
        if sec is None:
            continue
        strtab = named['.strtab']
        for i in range(sec[5] // entsize):
            at = sec[4] + i * entsize
            if entsize == 12:
                off, info, addend = struct.unpack_from('<IiI', data, at)
            else:
                off, info = struct.unpack_from('<II', data, at)
                addend = 0
            sym = info >> 8
            so = symtab[4] + sym * 16
            symname = data[strtab[4] + struct.unpack_from('<I', data, so)[0]:]
            symname = symname[:symname.index(b'\0')].decode()
            out[off] = (symname, addend)
    return out


SECTIONS = ('.text', '.data', '.rodata')


def object_info(path):
    """{section: (bytes, {offset: reloc})} for the sections a unit can own."""
    data, named = _sections(pathlib.Path(path).read_bytes())
    out = {}
    for section in SECTIONS:
        sec = named.get(section)
        if sec is None:
            continue
        out[section] = (data[sec[4]:sec[4] + sec[5]],
                        _relocs(data, named, section))
    return out


def check(module, name, src_dir=None):
    """(ok, detail) for one installed unit."""
    rel = src_dir or f'src/{module}'
    ours = BUILD / rel / f'{name}.o'
    theirs = BUILD / 'delinks' / rel / f'{name}.o'
    if not ours.exists() or not theirs.exists():
        return False, f'missing object ({ours if not ours.exists() else theirs})'
    got = object_info(ours)
    want = object_info(theirs)
    if set(got) != set(want):
        return False, f'sections {sorted(got)} vs retail {sorted(want)}'
    bytes_total = relocs_total = 0
    for section in sorted(want):
        got_body, got_rel = got[section]
        want_body, want_rel = want[section]
        if len(got_body) != len(want_body):
            return False, (f'{section} is {len(got_body)} bytes, retail '
                           f'{len(want_body)}')
        bytes_total += len(want_body)
        relocs_total += len(want_rel)
        for off in range(0, len(want_body), 4):
            # A relocated word is whatever the linker puts there, so retail may
            # hold a filler (a `bl` placeholder, a zeroed address) and only the
            # relocation itself is comparable.
            if off in want_rel and off in got_rel:
                continue
            if got_body[off:off + 4] == want_body[off:off + 4]:
                continue
            if off in want_rel and off not in got_rel:
                return False, (f'no relocation in {section} at +{off:#04x} where '
                               f'retail relocates {want_rel[off][0]}; spell the '
                               f'symbol, not the number')
            return False, (f'{section} word at +{off:#04x} differs '
                           f'({got_body[off:off + 4][::-1].hex()} vs '
                           f'{want_body[off:off + 4][::-1].hex()})')
        missing = sorted(set(want_rel) - set(got_rel))
        if missing:
            off = missing[0]
            return False, (f'no relocation in {section} at +{off:#04x} where retail '
                           f'relocates {want_rel[off][0]} (spell the symbol, not '
                           f'the number)')
        extra = sorted(set(got_rel) - set(want_rel))
        if extra:
            off = extra[0]
            return False, (f'unexpected relocation in {section} at +{off:#04x} '
                           f'({got_rel[off][0]})')
        for off in sorted(want_rel):
            if got_rel[off] != want_rel[off]:
                return False, (f'relocation in {section} at +{off:#04x} is '
                               f'{got_rel[off]} in ours, {want_rel[off]} in retail')
    return True, f'{bytes_total} bytes, {relocs_total} relocations'


def installed(module):
    """Unit names from the module's delinks, so a renamed path still resolves."""
    base = (ROOT / 'config' / 'ATOJ' / 'arm9' if module == 'arm9'
            else ROOT / 'config' / 'ATOJ' / 'arm9' / 'overlays' / module)
    delinks = base / 'delinks.txt'
    if not delinks.exists():
        return []
    out = []
    for line in delinks.read_text(encoding='utf-8', errors='ignore').splitlines():
        stripped = line.strip()
        if stripped.startswith('src/') and stripped.endswith(':'):
            out.append(stripped[:-1])
    return out


MODULES = ['arm9'] + [f'ov{i:03d}' for i in range(11)]


def audit(module):
    bad = 0
    total = 0
    for rel in installed(module):
        name = pathlib.PurePosixPath(rel).stem
        src_dir = str(pathlib.PurePosixPath(rel).parent)
        ok, why = check(module, name, src_dir)
        total += 1
        if not ok:
            bad += 1
            print(f'  NOT PERFECT  {rel}: {why}')
    print(f'{module}: {total - bad}/{total} units perfect')
    return bad


def main():
    args = sys.argv[1:]
    if args[:1] == ['--unit']:
        module, name = args[1], args[2]
        ok, why = check(module, name)
        print(('ok   ' if ok else 'BAD  ') + f'{name}: {why}')
        return 0 if ok else 1
    mods = args or MODULES
    bad = 0
    for mod in mods:
        bad += audit(mod)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
