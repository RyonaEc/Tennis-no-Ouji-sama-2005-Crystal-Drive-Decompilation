import pathlib, subprocess, sys
from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
BASE = 0x02004000
DL = pathlib.Path('config/ATOJ/arm9/delinks.txt')
RETAIL = pathlib.Path('extract/ATOJ/arm9/arm9.bin')
md = Cs(CS_ARCH_ARM, CS_MODE_ARM)

def diag(name, addr, size, body):
    src = pathlib.Path(f'src/arm9/{name}.c')
    src.write_text('#include "types.h"\n\n' + body)
    original = DL.read_text()
    entry = f"src/arm9/{name}.c:\n    complete\n    .text       start:{addr:#010x} end:{addr+size:#010x}\n"
    if entry not in original:
        DL.write_text(original.rstrip('\n') + '\n\n' + entry)
    subprocess.run([sys.executable, "tools/configure.py", "ATOJ"], capture_output=True)
    r = subprocess.run([sys.executable, "-m", "ninja"], capture_output=True, text=True)
    if r.returncode != 0:
        print(f'{name}: BUILD FAILED'); print((r.stdout + r.stderr)[-600:])
        DL.write_text(original); src.unlink(missing_ok=True); return
    built = pathlib.Path('build/ATOJ/build/arm9.bin').read_bytes()
    off = addr - BASE
    a = built[off:off+size]; b = RETAIL.read_bytes()[off:off+size]
    if a == b:
        print(f'{name}: identical'); DL.write_text(original); src.unlink(missing_ok=True); return
    print(f'{name}: differs')
    ia = list(md.disasm(a, addr)); ib = list(md.disasm(b, addr))
    for i in range(max(len(ia), len(ib))):
        ta = f'{ia[i].mnemonic} {ia[i].op_str}' if i < len(ia) else '<none>'
        tb = f'{ib[i].mnemonic} {ib[i].op_str}' if i < len(ib) else '<none>'
        mark = '  ' if ta == tb else '!!'
        print(f'  {mark} ours: {ta:34} retail: {tb}')
    DL.write_text(original); src.unlink(missing_ok=True)
