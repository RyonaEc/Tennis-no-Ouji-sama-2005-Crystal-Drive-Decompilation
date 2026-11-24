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

Installed from this map so far: the three `MTX_RotX33_`-family units, then 14
SDK asm units (1,296 B) - `MIi_CpuClear32`, `MIi_CpuCopy32`, `MIi_CpuClearFast`,
`MIi_CpuCopyFast`, `MI_Copy36B/48B/64B`, `MI_CpuFill8`, `MI_CpuCopy8`,
`MI_UncompressLZ8`, `MI_UncompressLZ16`, `GXi_NopClearFifo128_`,
`GX_SendFifo48B`, `OS_SpinWait`.

Left on the map:

* `OS_ReleaseLockID` (0x02024b5c, 48 B) and `OS_InitContext` (0x020255e8, 100 B):
  SDK asm, but the transcribed block did not compile to retail bytes in one
  pass; check the source's `#include <nitro/code32.h>` context and any macro it
  uses.
* 11 C units: `VEC_Normalize` (344 B), `VEC_CrossProduct` (156 B),
  `CP_SaveContext` (116 B), `CP_RestoreContext` (108 B), `OS_GetLockID` (88 B),
  `G3X_SetFog` (84 B), `OS_DisableIrqMask` (72 B), `FX_InvAsync` (60 B),
  `FSi_WriteFileCommand` / `FSi_ReadFileCommand` (56 B each),
  `MBi_calc_sendblock` (8 B). These need the MSL/SDK header harness first.
* 7 Metrowerks syscall stubs in `libsyscall/_svc_mw.o` (4-6 B). They are compiler
  runtime stubs, not game functions: write them from that object, and do not
  invent C for them.

## What would help next

* Finish the two asm stragglers, then build the SDK-header compile harness
  (CodeWarrior MSL + `include/nitro`) so the 11 C units can be ported and
  diffed; that is the reusable part.
* Run `sdkmap.py` on the overlays: if the game links library code there too, the
  same map applies, and those modules have smaller units on average.
* Partial hits (leading bytes agree, then diverge) are the compiled-C half of
  the same story: they identify routines that a SDK-source port can plausibly
  match.
