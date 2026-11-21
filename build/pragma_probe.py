"""Does this mwcc honour a pragma at all? Compile one body under each."""
import importlib.util, pathlib, subprocess

def load(p, n):
    s = importlib.util.spec_from_file_location(n, p)
    m = importlib.util.module_from_spec(s)
    s.loader.exec_module(m)
    return m

fm = load("build/fastmatch.py", "fm")
body = "\n".join(l for l in pathlib.Path("build/cand/func_0202c3e0.c").read_text().splitlines()
                 if not l.lstrip().startswith("#include"))
for pre in ["", "#pragma optimization_level 0\n", "#pragma peephole off\n",
            "#pragma opt_propagation off\n", "#pragma totally_bogus on\n"]:
    src, obj = pathlib.Path("build/_pp.c"), pathlib.Path("build/_pp.o")
    src.write_text('#include "types.h"\n\n' + pre + body)
    r = subprocess.run(["./tools/sjiswrap.exe", "tools/mwccarm/1.2/sp2/mwccarm.exe",
                        *fm.MWCC_FLAGS, "-c", str(src), "-o", str(obj)],
                       capture_output=True, text=True)
    size = len(fm.text_of(obj)) if obj.exists() else -1
    msg = (r.stderr.strip().splitlines() or ["(no diagnostics)"])[0][:110]
    label = pre.splitlines()[0] if pre else "(bare)"
    print(f"{label:38} size={size:3d}  {msg}")
    src.unlink(missing_ok=True)
    obj.unlink(missing_ok=True)
