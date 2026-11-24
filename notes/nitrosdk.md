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
the thumb build of the same sources. `libfx`, `libgx`, `libos`, `libmi`,
`libfs`, `libcard`, `libspi`, `libwm`, `libcp` and `libsyscall` are the ones this
ROM pulls from.

The sources are the useful part: they are what the original developers wrote and
Nintendo compiled, including hand-written `asm` blocks (`fx_mtx33.c`) that no C
spelling can reproduce.

## Measured: how much of arm9 is SDK code

`tools/match/sdkmap.py arm9` (searching `lib/ARM9-TEG/Rom` of both releases):

| | |
|---|---|
| unmatched arm9 functions | 1,365 (223,060 B) |
| byte-identical to a prebuilt SDK object | **164 (7,836 B)** |
| of those, 64 B or larger | 40 |
| present in both releases | 100 |
| present only in NitroSDK 2.0 RC3 | 49 (3,792 B) |
| present only in NitroSDK 1.0 | 15 (64 B: 4- and 6-byte stubs) |

By library: libos 42, libgx 42, libmi 16, libsyscall 15, libfx 10, libfs 8,
libcard 8, libext 7, libspi 4, libwm 3, libcp 2, libctrdg 2, and one each in
librtc, libpxi, libmb, libe3s, libdgt.

Two conclusions:

1. **The cartridge linked the NitroSDK 2.0 line.** 49 functions exist only in
   2.0 RC3 and match retail exactly; the 15 "1.0-only" hits total 64 bytes of
   4-6 byte stubs, which are identical-by-accident padding, not a version
   signal. A release that changed 3,792 bytes of library code and matches all of
   them is the release that was linked.
2. **These 164 units are not game code**, and treating them as mwccarm codegen
   problems is wasted effort.

Caveat on the map: retail's function boundaries are this project's, not
Nintendo's. In many rows the retail unit is larger than the SDK symbol it starts
at (`0x0201fe0c` 344 B starting at `VEC_Fx16Distance`, whose SDK size is 192 B),
because the linker placed several SDK functions contiguously and the retail
symbol analysis merged them or the neighbours follow without alignment. So a row
means "this unit starts at SDK symbol X and continues into SDK code", and the
real unit may need to be split, or written as the SDK source plus whatever game
code follows.

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

* an `asm` block in the SDK (`fx_mtx33.c`, `os_spinlock.c`, ...) is the original
  source of that unit: write it as `asm void` in `src/` and let `1.2/b56`
  assemble it, as `func_0201fcc8` does;
* a C function in the SDK is worth porting with its SDK types and then diffing
  with `var3.py`; if the bytes are not exact, the compiler that built the
  library differs from b56 and the unit stays parked. Do not install a
  stand-in override for a library unit: the reason it differs is provenance,
  which is a note, not a compiler tag.

None of this changes the definition of done. `reg.py` and `ninja sha1` still
decide, and both tools only report provenance.

## What would help next

* Work the 40 hits of 64 B or more: each needs its SDK source read (and
  sometimes a split), which is ordinary decompilation of documented code rather
  than guessing at compiler policy.
* Run `sdkmap.py` on the overlays: if the game links library code there too, the
  same map applies, and those modules have smaller units on average.
* The 304 partial hits (>= 12 leading bytes) are the compiled-C half of the same
  story: they identify routines that a SDK-source port can plausibly match.
* Pinning the exact SDK release (2.0 RC3 vs a later 2.0 patch) needs a function
  that changed between those releases; the 49 2.0-only hits already bound it to
  the 2.0 line, and no later archive predates the cartridge.
