import importlib.util, sys, pathlib, subprocess, re
def load(p,n):
    s=importlib.util.spec_from_file_location(n,p); m=importlib.util.module_from_spec(s); s.loader.exec_module(m); return m
t10=load("tools/match/t10.py","t10")
mod,name,addr,size,src=sys.argv[1],sys.argv[2],int(sys.argv[3],0),int(sys.argv[4],0),sys.argv[5]
version=sys.argv[6] if len(sys.argv)>6 else None
if version:
    # The unit must be built with this compiler before try_unit links the ROM,
    # so record the override and regenerate the build graph first.
    mpath=pathlib.Path("config/ATOJ/mwcc_versions.txt")
    key=f"src/{mod}/{name}.c"
    text=mpath.read_text(encoding="utf-8")
    lines=[l for l in text.splitlines() if not l.strip().startswith(key)]
    lines.append(f"{key} = {version}")
    mpath.write_text("\n".join(lines)+"\n",encoding="utf-8")
    subprocess.run([sys.executable,"tools/configure.py","ATOJ"],check=True,capture_output=True)
lines=pathlib.Path(src).read_text().splitlines()
while lines and lines[0].lstrip().startswith('#include'):
    lines.pop(0)
body='\n'.join(lines).strip('\n')
# The delinks entry names a symbol, so a body that defines something else - a
# scratch `f` left over from testing spellings - installs nothing and reads as a
# digest miss after a full rebuild.  Catch it before paying for the rebuild.
if not re.search(r'^\s*(?:asm\s+)?[A-Za-z_][\w \t*]*\b%s\s*\(' % re.escape(name), body, re.M):
    print("the body does not define %s: name the function that, or the rebuild "
          "spends two minutes installing nothing" % name)
    raise SystemExit(2)
print(t10.try_unit(name,addr,size,body,mod=mod))
