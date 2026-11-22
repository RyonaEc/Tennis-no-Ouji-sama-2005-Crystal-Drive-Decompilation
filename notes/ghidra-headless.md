# Ghidra decompiler, headless (no GUI, no prompts)

`dsd-ghidra`'s loader asks which modules to mount (arm7 / arm9 / overlays) with a
dialog, which is useless for batch work. Two ways around it, both used here:

1. Never launch the GUI at all — only `analyzeHeadless.bat`. Headless cannot
   display a dialog, so no input is ever required.
2. Do not import the `.nds` container (the dsd loader parses only the header and
   leaves no code). Import the module binary directly instead:

```
analyzeHeadless <projdir> <projname> -import build/ATOJ/build/arm9.bin \
  -processor ARM:LE:32:v5t -loader BinaryLoader -loader-baseAddr 0x02004000 \
  -noanalysis -scriptPath <scripts> -postScript dump_decomp.py
```

Re-running the script against the already-imported program (fast, ~10 s):

```
analyzeHeadless <projdir> <projname> -process arm9.bin -noanalysis \
  -scriptPath <scripts> -postScript dump_decomp.py
```

`-noanalysis` is deliberate: a full auto-analysis of a 64 MB ROM takes minutes and
finds the wrong boundaries anyway. The post-script disassembles a 0x400-byte
window at each address and creates the function itself.

## Local install

`<tools>` below is any directory outside this repository; the exact location does
not matter and nothing here is installed system-wide.

| Piece | Path |
|---|---|
| JDK 21 (Temurin, portable) | `<tools>\jdk` |
| Ghidra 11.2.1 (portable) | `<tools>\ghidra11` |
| dsd-ghidra v0.7.0 extension | `<tools>\ghidra11\Ghidra\Extensions\dsd-ghidra` |
| dump script | `<tools>\scripts\dump_decomp.py` |
| project | `<tools>\proj` (programs `ATOJ`, `ATOJ9`) |

All portable, nothing installed system-wide, no admin rights used. The extension
release is built against Ghidra **11.2.1** specifically — a newer Ghidra rejects
it, so do not upgrade Ghidra without rebuilding the extension from source
(`cargo` + Gradle, not installed here).

Import each overlay the same way with its own base address and project name.

## What it bought

`func_02033f40` (80 B), previously parked as "ambiguous by-value arg":

```
call: func_02033fd8(out, x, y, x, y, op)   /* 6 args, r0=out */
```

`func_02033fd8` forwards all six to a handler selected by opcode class
(`op & 0x7c000000` == `0x04000000`, `0x08000000`, `0x10000000`, `0x20000000`,
`0x40000000`), i.e. a dispatch shim, not a value-returning arithmetic helper.

Four formulations tried against the 80 retail bytes, none matched:

| Shape | Result |
|---|---|
| 6 scalar args, no locals | frame 0x14, args stay in registers (retail frame 0x20, params spilled to sp+8/sp+0xc/sp+4) |
| `V2` struct passed twice by value | frame 0x58, mwcc introduces a frame pointer — retail has none |
| scalars copied into a local `V2`, then passed | same frame-pointer code |
| scalars copied into `u32 t[2]`, members passed | identical to the scalar version, array folded away |

Retail spills the incoming parameters to stack slots and then reuses `sp+0`/`sp+4`
as the outgoing argument area. That is an mwcc allocation choice, so the unit
stays parked until a source shape provokes the spill — do not force it.

## Working loop

1. `python tools/match/leafq.py <lo> <hi> [--module=ovNNN]` for unmatched leaf candidates.
2. Add the addresses to a `.bat` beside `dump_decomp.py` and run
   `cmd //c "build\ghidra\dumpXX.bat" > build/ghidra/out.txt 2>&1`; the projects that
   exist are `ATOJ9` (arm9.bin) and `ATOJOV3` (ov003). Anything else needs a new import
   before it can be dumped.
3. Read the decompile as a *shape*, not as source: it gives the field offsets, the loop
   boundaries and the constant folding. Write the C with named symbols from
   `config/ATOJ/.../symbols.txt`, never raw addresses.
4. Probe with `fastmatch.compile_text` against `fastmatch.retail` (plain byte compare,
   no relocations involved when the body is leaf code and uses symbols resolved by the
   probe), then confirm with `t10.try_unit` so the digest and the delink entry are
   checked together.
5. When the byte count lands within 4 and the diff is one instruction, the residue is
   codegen, not the algorithm: record the exact pair of instructions in
   `tools/match/walls.txt` and move on rather than rewriting logic that already matches.
