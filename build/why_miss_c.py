"""Show why a generated unit misses: compile just its .o and diff against retail."""
import importlib.util, pathlib, subprocess, sys, struct
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s); s.loader.exec_module(m); return m

a = load('build/auto.py', 'auto')
t10 = load('build/t10.py', 't10')
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)

def text_of(path):
    d = pathlib.Path(path).read_bytes()
    if d[:4] != b'\x7fELF':
        return b''
    shoff = struct.unpack_from('<I', d, 0x20)[0]; shent = struct.unpack_from('<H', d, 0x2e)[0]
    shnum = struct.unpack_from('<H', d, 0x30)[0]; shstr = struct.unpack_from('<H', d, 0x32)[0]
    def sh(i):
        o = shoff + i*shent
        return struct.unpack_from('<II', d, o) + struct.unpack_from('<II', d, o+0x10)
    _,_,stroff,_ = sh(shstr)
    for i in range(shnum):
        nm_off, _, off, size = sh(i)
        nm = d[stroff+nm_off:d.index(b'\0', stroff+nm_off)].decode()
        if nm == '.text':
            return d[off:off+size]
    return b''

def show(tag, code, base):
    for ins in md.disasm(code, base):
        print(f'    {tag} {ins.address:#010x}  {ins.mnemonic:8s} {ins.op_str}')

mod = sys.argv[1]
a.set_module(mod)
for spec in sys.argv[2:]:
    name, cfile = spec.split('=', 1)
    addr = a.NAME2ADDR[name]
    size = a.FUNCS[addr][1]
    src_text = pathlib.Path(cfile).read_text()
    srcdir = pathlib.Path(t10.srcdir_for(mod))
    srcdir.mkdir(parents=True, exist_ok=True)
    src = srcdir / f'{name}.c'
    dl = pathlib.Path(t10.delinks_for(mod))
    before = dl.read_text()
    existed = src.exists()
    old = src.read_bytes() if existed else None
    try:
        src.write_text('#include "types.h"\n\n' + src_text)
        if name not in before:
            dl.write_text(before.rstrip('\n') + '\n\n' + t10.entry_for(srcdir.as_posix(), name, addr, size))
        subprocess.run([sys.executable, 'tools/configure.py', 'ATOJ'], capture_output=True)
        obj = f'build/ATOJ/src/{mod}/{name}.o'
        r = subprocess.run([sys.executable, '-m', 'ninja', obj], capture_output=True, text=True)
        built = text_of(obj)
        retail = text_of(f'build/ATOJ/delinks/src/{mod}/{name}.o')
        print(f'=== {name} ({mod}) addr={addr:#x} size={size:#x}  built={len(built)}B retail={len(retail)}B equal={built==retail}')
        if r.returncode != 0:
            tail = '\n'.join(r.stdout.strip().splitlines()[-4:])
            print('  compile failed:\n', tail)
            continue
        print('  --- retail ---'); show('R', retail, addr)
        print('  --- built  ---'); show('B', built, addr)
        print('  --- C ---')
        for line in src_text.splitlines():
            print('   ', line)
    finally:
        dl.write_text(before)
        if old is None:
            src.unlink(missing_ok=True)
        else:
            src.write_bytes(old)
        subprocess.run([sys.executable, 'tools/configure.py', 'ATOJ'], capture_output=True)
