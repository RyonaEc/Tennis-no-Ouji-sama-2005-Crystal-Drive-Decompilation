# NitroSDK: what the archive gives this decompilation

A unit that no compiler build and no source spelling reproduces is usually not
game code. It is a prebuilt NitroSDK library routine, linked in from Nintendo's
own `.a` files. `func_0201fcc8` was parked in `walls.txt` as a "2.0-generation
constant materialisation" wall for a day; it is `MTX_RotX33_` from `libfx`, a
hand-written asm block in the SDK, and `1.2/b56` assembles it byte for byte.

That makes two questions worth answering from evidence rather than intuition:
*whose* code a stubborn unit is, and which SDK release the cartridge linked.

## Where the SDKs come from

<https://twlsdk.randommeaninglesscharacters.com/download/NitroSDK/> mirrors the
SDK releases; <https://twlsdk.randommeaninglesscharacters.com/download/CodeWarrior/>
has the matching `CodeWarrior_DS-*` installers, and `download/mwccarm.zip` is the
same compiler set this repo vendors under `tools/mwccarm/` (no build older than
b56 is in it).

Extract them under `build/cand/sdk/<release>/`, which is gitignored. They are
Nintendo material: never commit an archive, an extracted tree, or library bytes.

Two are extracted here and are the ones that matter for this ROM:

| Release | Archive | Date |
|---|---|---|
| NitroSDK 1.0 | `NitroSDK-1_0-20040416.zip` | 2004-04-16 |
| NitroSDK 2.0 RC3 + patch 1 | `NitroSDK-2_0_rc3-patch-plus1-20041210.zip` | 2004-12-10 |

2.0 RC3 is three weeks older than the 2004-12-30 cartridge, so it is the only
release whose date fits a retail build among the archives we have.

## Layout of an extracted SDK

```
lib/ARM9-TEG/{Debug,Release,Rom}/lib*.a      prebuilt libraries (+ .thumb.a)
build/libraries/<lib>/src/*.c                the source each library was built from
include/nitro/...                            headers (types, function declarations)
man/en_US/...                                function reference, one page per function
Readme-NitroSDK-*.txt, SrcTreeMap.txt        release notes and source map
```

`Rom` is the release configuration a cartridge would link; `Debug`/`Release`
carry assertions and are not what the retail image contains. `.thumb.a` holds
the thumb build of the same sources.

## Measured: how much of arm9 is SDK code

`tools/match/sdkmap.py arm9` maps every function with no `src/arm9/<name>.c`
against the `lib/ARM9-TEG/Rom` libraries of both releases:

| | |
|---|---|
| unmatched arm9 functions | 421 (164,224 B) |
| **byte-identical to a prebuilt SDK object** | **34 (2,624 B)** |
| ... of which the SDK defines as `asm` | 16 (1,444 B) |
| ... of which the SDK defines as C | 11 |
| ... Metrowerks syscall stubs (`libsyscall/_svc_mw.o`) | 7 |
| present only in NitroSDK 2.0 RC3 | 15 (1,764 B) |
| present only in NitroSDK 1.0 | 7 (32 B: 4- and 6-byte stubs) |

By library: libmi 11, libsyscall 7, libos 5, libfx 3, libgx 3, libfs 2, libcp 2,
libmb 1. Only one row has a size different from its SDK symbol, so these are
1:1 library functions, not fragments of merged units.

**The cartridge linked the NitroSDK 2.0 line.** 15 functions exist only in
2.0 RC3 and match retail exactly; the 7 "1.0-only" hits total 32 bytes of
4-6 byte stubs, which are identical by accident, not version evidence.

**Correction.** An earlier version of this note reported 164 functions and
7,836 B. That sweep treated every function inside a bare `.text` block as
unmatched, which included units this project has already matched
(`func_0201fcc8` among them). The project's own rule - unmatched means
`src/<module>/<name>.c` is absent - gives 34.

## Using it

Per unit, before spending iterations on a shape no build reproduces:

```
python tools/match/sdk.py arm9 0x0201fcc8 28        # one unit
python tools/match/sdkmap.py arm9                   # whole module -> build/sdkmap-arm9.tsv
```

`sdk.py` prints library, member and offset for a hit, exact or partial; partial
hits are the normal case for compiled (not asm) SDK functions, because retail has
its relocations applied while the archive object still carries placeholders.

Then work from the SDK source:

* an `asm` block in the SDK is the original source of that unit: write it as
  `asm void` in `src/`, keeping the SDK's parameter list so the registers land in
  `r0..`, and let `1.2/b56` assemble it. Regenerating the candidate is scripted
  in `build/cand/gen_asm_sdk.py` (scratch), and 14 of the 16 asm units were
  installed this way;
* a C function in the SDK needs the SDK headers and Metrowerks' MSL
  (`stdio.h`, `stdarg.h`, ...), which ship in `CodeWarrior_DS-1_2.zip`, not in
  the SDK. That harness is not built yet: stub headers get part way and then
  fail inside `printf.h`. Until it exists, the 11 C units stay parked.

None of this changes the definition of done: `reg.py` and `ninja sha1` decide,
and both tools only report provenance.

## Status and what is left

Installed from this map so far: the three `MTX_RotX33_`-family units, then all
16 SDK asm units (1,444 B) - `MIi_CpuClear32`, `MIi_CpuCopy32`,
`MIi_CpuClearFast`, `MIi_CpuCopyFast`, `MI_Copy36B/48B/64B`, `MI_CpuFill8`,
`MI_CpuCopy8`, `MI_UncompressLZ8`, `MI_UncompressLZ16`,
`GXi_NopClearFifo128_`, `GX_SendFifo48B`, `OS_SpinWait`, `OS_ReleaseLockID`
and `OS_InitContext`.

The last two failed at first with an empty object, which looked like a missing
compiler feature but was an unknown type: the SDK declares
`register OSContext* context`, and `types.h` has no `OSContext`, so mwcc errored
out. Declaring the parameters as `u32 *` and restating the `context.h` /
`armArch.h` offsets fixed it, and both then assembled byte-exactly. Useful facts
about mwccasm in `asm void`, all confirmed on b56: `ldr rX, =MACRO` and
`ldr rX, =Symbol` emit a pool word (relocated for a symbol), `//` comments are
kept, and `#ifdef/#else/#endif` inside the body is honoured, so an SDK body can
be transcribed literally; only the parameter *types* have to exist here.

Left on the map:

* 3 C units: `G3X_SetFog` (84 B), `OS_GetLockID` (88 B) and an 8-byte stub
  above `MBi_calc_sendblock`. `G3X_SetFog` is four `volatile` register
  writes (`DISP3DCNT` 0x04000060, `FOG_OFFSET` 0x0400035c) and its signature
  is in `nitro/gx/g3x.h`, but retail if-converts the disable path into
  `ldreq/andeq/strheq/bxeq lr` and falls through to the enable path; the
  if/else and early-return forms both emit `bne` and 92 B against retail 84 B.
  `OS_GetLockID` (0x02024b04) scans a lock bitmap at 0x027fffb0 with `clz`,
  base IDs 0x40/0x60 and a `0x80000000 >> z` clear; its source is not in the
  shipped library tree, so the `clz` spelling has to be chosen to match mwcc. `VEC_Normalize` (344 B) needed the whole CP div/sqrt sequence: DIVCNT
  0x04000280, DIV_NUMER 0x04000290, DIV_DENOM 0x04000298, DIV_RESULT 0x040002a0,
  SQRTCNT 0x040002b0, SQRT_RESULT 0x040002b4, SQRT_PARAM 0x040002b8, with IME
  0x04000208 masked around the sqrt launch. The one shape detail that mattered:
  the SDK's `OS_RestoreInterrupts` reads IME back before writing it, so a bare
  restore is one instruction short and only matches once that read is present.
  Eight C units were matched this
  way already (`VEC_CrossProduct`, `CP_SaveContext`, `CP_RestoreContext`,
  `FX_InvAsync`, `OS_DisableIrqMask`, `FSi_ReadFileCommand`,
  `FSi_WriteFileCommand`): the recipe is to restate the SDK header as plain C -
  `volatile` casts for the hardware registers, a struct with the offsets read
  off the disassembly - and compile the SDK body unchanged.

* 7 Metrowerks syscall stubs in `libsyscall/_svc_mw.o` (4-6 B). They are compiler
  runtime stubs, not game functions: write them from that object, and do not
  invent C for them.

## The libsyscall stubs: bytes found, placement still open

The ten unmatched thumb stubs in `libsyscall` are plain BIOS calls and their
bytes are now known exactly. Written the way the already-matched ones are
(`src/arm9/SoftReset.c` style - a normal C function with `asm { swi 0xNN }`, so
the compiler adds `bx lr`), all ten compile byte-exactly:

| unit | code | size |
|---|---|---|
| `Halt` | `swi 0x06` | 4 B |
| `Sqrt` | `swi 0x0d` | 4 B |
| `LZ77UnCompReadByCallbackWrite16bit` | `swi 0x12` | 4 B |
| `RLUnCompReadNormalWrite8bit` | `swi 0x14` | 4 B |
| `IsDebugger` | `swi 0x0f` | 4 B |
| `CpuSet` | `swi 0x0b` | 4 B |
| `GetCRC16` | `swi 0x0e` | 4 B |
| `CpuFastSet` | `swi 0x0c` | 4 B |
| `VBlankIntrWait` | `mov r2,#0; swi 0x05` | 6 B |
| `Mod` | `swi 0x09; mov r0,r1` | 6 B |

`reg.py` then reports `miss` for every one of them, and the reason is now
measured rather than guessed: **the linker inserts two bytes of padding before
the unit**. With `Halt` delinked and built, the image reads

```
built   0x020040e0: ... 8a 5c 53 28 00 00 06 df 70 47 ...
retail  0x020040e0: ... 8a 5c 53 28 06 df 70 47 ...
```

so the stub lands at 0x020040e8 instead of 0x020040e6 and everything after it
shifts. `Halt.o` and `SoftReset.o` both put their 4 bytes in `.text` with
`sh_addralign = 4`; `SoftReset` starts at 0x0200406c (4-aligned, no padding),
while all ten of these start at 2-mod-4. The generated script has no `ALIGN`
before the entry - the padding comes from the object's own section alignment -
and neither escape hatch exists: dsd rejects a per-unit `align:2` ("inherited
from this file's header"), and mwccarm has no alignment option (`-help` lists
none; `.text` is always 4-aligned here).

So these ten (and any other 2-mod-4 function) cannot be placed until either dsd
learns to honour a 2-aligned unit inside a 32-aligned header, or the build
carries a per-unit alignment attribute that does not exist today. Bodies are kept
in `build/cand/svc/` for whoever picks this up - the byte work is done.

## Which compiler built the prebuilt libraries

The SDK answers it in its own build rules. `build/buildtools/modulerules.cctype.CW`
carries the per-release compiler and linker builds:

```
IRC/V0.2 HF1  CC 0038  LD 0050      NITRO V0.6    CC 0050  LD 0062
NITRO V0.3    CC 0038  LD 0056      NITRO V0.6.1  CC 0050  LD 0063
NITRO V0.4.1  CC 0043  LD 0057      NITRO V0.7    CC 0050  LD 0063
NITRO V0.5    CC 0046  LD 0058      NITRO V1.0    CC 0056  LD 0067
NITRO V0.5 HF1 CC 0049 LD 0061
```

`1.2/b56` is therefore exactly NITRO V1.0's compiler - the one Konami's game code
matches across the whole ROM. SDK 2.0 RC3 postdates V1.0, so Nintendo built its
prebuilt libraries with a *later* CC build (57 and up; none of those are vendored,
which is why the missing builds 57-71 never mattered for game code). The library
`asm` blocks transcribe perfectly for a second reason on top of that: assembly
output does not depend on the compiler build at all.

The later CC does **not** stop the library C units from matching. `func_02028920`
and `func_02028958` are `FSi_ReadFileCommand` and `FSi_WriteFileCommand` from
`fs_command.c`, both byte-identical to `libfs.a(fs_command.o)` in ARM9-TS/Rom.
Written from the SDK source as ordinary C - with the `FSFile`/`FSArchive` layout
taken from the disassembly (`arc` at 0x08, `prop.file.pos` at 0x28, the arg
pointer at 0x2c, `len` at 0x34, `read_func` at 0x40 and `write_func` at 0x3c) -
both compile EXACT under b56. So for these wrappers the codegen is stable across
the CC builds, and "it is SDK code" is not a reason to skip a unit. The number is not hard-coded anywhere: the
makefile derives it at build time from the installed compiler's `-V` banner
(`modulerules.cctype.CW:347`), so no SDK tree records the build Nintendo used.

## What the reference projects do (advice, not yet used here)

* **KH Days** (`sdk/` harness): compiles pret/pokediamond's NitroSDK C with its
  own compiler and byte-matches against the ROM, 520 of 700 files, and reports
  matched C, library assembly and SDK identifications as *separate rows*. That
  is the reporting split to copy for our 34 library units. Its headers are a
  sparse clone of `arm9/lib` plus stubs: `code32.h`/`code16.h` are just
  `#pragma thumb off`/`on`, `function_target.h` is empty, and `nitro/types.h`
  needs `ALIGN(n)` and `ENUMS_ALWAYS_INT_*`.
* **pokediamond** flags, as a hint rather than Nintendo's own SDK Makefile
  flags: `-O4,p -sym on -gccext,on -proc arm946e -gccinc -ipa file -fp soft
  -lang c99 -inline on,noauto`, with the configuration baked into its headers
  rather than `-D`. Defines to try for the Rom build: `SDK_ARM9`, `SDK_TS`,
  `SDK_FINALROM`, `SDK_CODE_ARM`. `#pragma optimize_for_size on` is legitimate
  source, used by pokediamond's `OS_interrupt.c`.
* **MSL headers**: `sm64ds-decomp`'s `tools/recover_cw2004.py` splits an
  InstallShield CodeWarrior payload, and `unshield` (with both `data1.cab` and
  `data1.hdr`) is the usual route. That is the missing piece for the 11 C units
  here.
* **Permuter**: tangOS reports that `sm64ds-decomp` wires `decomp-permuter` to
  the compiler; no ready-made mwccarm + objdiff scorer was named.
* **SDK release**: a per-release `SDK_CW_BUILD_NUMBER_CC` exists in Nintendo's
  build rules, so it is worth grepping any SDK tree for it before guessing a
  compiler build.

## What would help next

* Finish the two asm stragglers, then build the SDK-header compile harness
  (CodeWarrior MSL + `include/nitro`) so the 11 C units can be ported and
  diffed; that is the reusable part.
* Run `sdkmap.py` on the overlays: if the game links library code there too, the
  same map applies, and those modules have smaller units on average.
* Partial hits (leading bytes agree, then diverge) are the compiled-C half of
  the same story: they identify routines that a SDK-source port can plausibly
  match.

## Near hits: the compiled half of the SDK map

`sdkmap.py` only reports *exact* byte matches, and for compiled (not asm)
library functions those do not exist: retail has its relocations applied, so its
constant-pool words and pc-relative displacements differ from the archive member
while every instruction is the same. `--near PCT` scores same-size archive
bodies by equal-byte ratio and names the function they came from, which turns
that difference into a work list instead of a dead end.

    python tools/match/sdkmap.py arm9 --near 0.90

arm9: 72 functions, 20,944 bytes at >= 90 % (`build/sdkmap-arm9-near.tsv`).
The top rows are the proof: `0x02028888` = `FSi_SeekDirCommand` at 99 %,
`0x0202bea8` = `PM_SetBackLightAsync` at 99 %, `0x02023df8`/`0x02023e58` =
`GXS_LoadOBJ`/`GX_LoadOBJ` at 92 % - the last pair is the "2x96 B family" that
sat in `walls.txt` as a register-allocation tie, and it is not a family at all,
it is two adjacent SDK entry points.

Matching them does not need a decompiler: read the function in
`build/libraries/<lib>/src/`, restate the hardware registers as `volatile`
accesses, and `var3` decides. `MI_StopDma` (80 B, libmi `mi_dma.c`) and
`PMi_CallCallbackAndUnlock` (84 B, libspi ARM9 `pm.c`) matched first try that
way; the second had a wall note claiming a register tie that the struct-typed
source resolves by itself.

The near pass is **arm9 only**: ov003 (424 unmatched functions, 573,680 B) and
ov002 both return zero rows at 92 %, so the overlays are pure game code and the
SDK work list is the 72 arm9 rows. A third match from it, `PM_GetBackLight`
(120 B, libspi ARM9 `pm.c`), confirms the shape: retail's `func_0202bfdc(0, &f)`
is `PMi_ReadRegister(REG_PMIC_CTL_ADDR, &reg)`, and the two out-parameters are
`PMBackLightSwitch*`, so the 32-bit stores and the `? 1 : 0` spelling both come
straight from the SDK source.

### `Entry` (arm9 `0x02004800`) is crt0's `_start`

`sdk.py` flags it against `crt0.o`, and the SDK source is
`build/libraries/init/ARM9/src/crt0.c` (`asm void _start(void)`) in both
releases. The retail unit is that function written as an `asm void` block
(`src/arm9/Entry.c`), with three differences from the 2.0 RC3 source worth
remembering:

- the BSS-clear loop (`cmp/strcc/bcc` over `_start_ModuleParams[12..16]`) is
  **not** in retail — the game's crt0 skips it;
- `_start_ModuleParams` is the arm9 `.data` symbol `BuildInfo`, but retail's
  object bakes the *number* `0x02004b00` into the pool (objcheck requires the
  number, not the symbol);
- `HW_REG_BASE`, `HW_PLTT`, `HW_OAM`, `HW_RESET_VECTOR` and
  `0x027fff9c` are plain numbers, while `INITi_HW_DTCM` is the dtcm symbol
  `data_027c0000` and `OS_IrqHandler` is the itcm symbol `func_01ff8000` — those
  two *do* carry retail relocations.

The unit's symbol name is `Entry` (not `func_02004800`), because the linker
line passes `-m Entry`; the source file and delinks entry must use `Entry`.
Nineteen of the 232 bytes are the trailing pool, so the literal order has to
follow first use exactly as the block is written.

### Porting an SDK C function works best as a struct, not byte offsets

arm9 `0x02025280` is `OS_CreateThread` (`libos.a`, `os_thread.o`, SDK20, ratio
0.92 in `sdkmap`). A first hand-written candidate that addressed the thread
block with `*(u32 *)(t + 0x88)` style offsets was 240 B against retail's 264:
six instructions short, with a different register assignment. Writing the SDK
source almost literally — an `OSThread` struct with named fields
(`state`/`id`/`priority`/`profiler`/`mutex`/`mutexQueue*`/`stackTop`/
`stackBottom`/`stackWarningOffset` plus `context` as `u8[0x64]`) — matched first
try. The field *types* drive the codegen: `stackBottom`/`stackTop` must be
`u32` so `*(u32 *)(thread->stackBottom - 4) = OSi_STACK_CHECKNUM_BOTTOM` reuses
the stored field, and the two `volatile u32 zero` uses (the value handed to
`MI_CpuClear32`) are what keep retail's redundant `str`/`ldr` pair on the stack.

Recipe for the next one: read the SDK source, keep its statement order, replace
SDK headers with plain numbers for `#define` constants and with byte offsets for
struct fields, and keep the local-variable types the SDK declares.

### Some near-hits are `asm void` blocks - copy the SDK body verbatim

`sdkmap` rank alone does not say whether a near-hit is compiled C or hand-written
asm. Check the SDK source before writing any C: a function declared `asm void`
in the SDK assembles byte for byte from the SDK's own body once the symbols are
renamed (`OSi_ExceptionHandler` at arm9 `0x020260b8` matched on the first try,
as did `RtcWaitBusy` at `0x0202c430`, a 24-byte spin on a bss lock). Script the
check: collect every `asm\s+void\s+(\w+)\s*\(` in `build/cand/sdk/SDK20` and
intersect it with the unmatched rows of `build/sdkmap-<mod>-near.tsv`.

### PXIi_HandlerRecvFifoNotEmpty ported from pxi_fifo.c (arm9 0x02027d74, 292 B)

Try these before writing any C by hand: PXI register access is a `volatile`
macro pair (`REG_PXI_FIFO_CNT` = `*(volatile u16 *)0x04000184`,
`REG_PXI_RECV_FIFO` = `*(volatile u32 *)0x04100000`, `REG_PXI_SEND_FIFO` =
`*(volatile u32 *)0x04000188`), the message is
`union { struct { u32 tag:5; u32 err:1; u32 data:26; } e; u32 raw; }`, and the
two FIFO helpers are `static inline` so `-inline on,noauto` expands them at the
call site exactly as retail has them. One trap: every inline helper keeps the SDK
body intact - dropping the ERR_MASK (`0x4000`) read-modify-write from
`PXIi_SetToFifo` because the caller ignores its result loses six instructions and
20 bytes, and using an `enabled` that was never assigned makes b56 reuse a stale
register for `OS_RestoreInterrupts`. With both restored the unit matched on the
first try at 292 B.

### Touch-panel port (arm9 0x0202aea8, TP_GetCalibratedPoint, 304 B)

`tp.c` from NitroSDK 2.0 RC3 compiles to the retail bytes once the struct layouts
come from the SDK headers rather than from the instruction order guessing:
`TPData` is `{ u16 x; u16 y; u16 touch; u16 validity; }` (touch is at +4, not +0,
which is why the calibrated path copies +4/+6 first), `TPiCalibrateParam` is six
`s32` (`x0, xDotSize, xDotSizeInv, y0, yDotSize, yDotSizeInv`) living at
`tpState+0x18` as its own symbol `data_0204ac80` with the flag at `data_0204ac68+0x30`,
and the clamps are `GX_LCD_SIZE_X-1 = 0xff` for X but `GX_LCD_SIZE_Y-1 = 0xbf` for Y.
The conversions are 64-bit: `(u16)(((u64)(raw->x << 2) - cal->x0) * cal->xDotSizeInv >> 22)`,
where the `(u64)` cast is what gives the `subs/sbc` sign-extended subtract and the
`umull + two mla` 64-bit multiply. `TP_SetCalibrateParam` (0x0202b43c) has the same
shape but is still a register-colouring tie: see its wall note.
