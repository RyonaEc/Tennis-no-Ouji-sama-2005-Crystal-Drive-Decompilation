# DS matching-decompilation prior art

Where the rest of the DS scene keeps its evidence, and which part of it answers
which question. Surveyed 2026-11-22. This file is a **pointer list**: the levers
ourselves live in [`mwccarm-codegen.md`](mwccarm-codegen.md), the dated evidence
in [`mwccarm-codegen-log.md`](mwccarm-codegen-log.md).

## What "100%" means in each project

Read the claim's denominator before quoting a number: "every function matched"
usually includes original hand-written assembly, and "byte-identical rebuild"
can still leave data coming from the ROM.

| Project | Game | Claim, and what it actually covers | Compiler |
|---|---|---|---|
| [Project-Holon/aletheia](https://github.com/Project-Holon/aletheia) | Pokémon Black/White | 100% matched, 35,197 functions, rebuilt ROM byte-identical; the same source builds several EU languages. The only strict all-C project found. | `dsi/1.1p1` plus `2.0/sp2`, `2.0/sp1p2`, `2.0/sp2p2`, `1.2/sp2`, `1.2` for overlays/SDK |
| [banatic/acww-decomp](https://github.com/banatic/acww-decomp) | Animal Crossing: Wild World (Korea) | 25,251 game functions in C, every module byte-exact; **game code only**, data still read from the ROM | `1.2/base` default, per-file `1.2/sp2` / `1.2/sp2p3` |
| [Yokimitsuro/khdays-decomp](https://github.com/Yokimitsuro/khdays-decomp) | KH 358/2 Days (EU) | 306/306 modules byte-identical, ~99.5% real C; the rest is verified original library assembly | `mwccarm` (shared with Re:coded, 3.0 build 139) |
| [kh-recoded-decomp](https://github.com/kh-recoded-decomp/kh-recoded-decomp) | KH Re:coded (EU) | ARM9 100% including libraries, all 108 modules byte-exact, **95.98% C-only** | 4.0 build 1036, 3.0 build 139 |
| [7ucina/acww-decomp](https://github.com/7ucina/acww-decomp) (was `6elphegor/`) | ACWW (USA Rev 1) | the `matching` tag rebuilds the ROM byte for byte; main still has six functions not built from source | 1.2 `base`/`b56`/`sp2`/`sp2p3`/`sp3` |
| [tangosdev/sm64ds-decomp](https://github.com/tangosdev/sm64ds-decomp) | Super Mario 64 DS | 11,389/11,389 functions matched but **113 are original assembly**; 52.9% data, 99.9% linked | `2004/b56` (build 56), `mwldarm` 1.2/sp2p3 |
| [fuddlesworth/pokebw2](https://github.com/fuddlesworth/pokebw2) | Pokémon Black 2/White 2 | 99.21%; both regions rebuild byte for byte | `dsi/1.1p1`, Thumb |
| [weaversworkshop/mlbis-decomp](https://github.com/weaversworkshop/mlbis-decomp) | Mario & Luigi: Bowser's Inside Story | 98.83%, main module still a few bytes off | `2.0/sp1p2`, `2.0/sp2p3` |
| [cntrl-alt-lenny/gx-spirit-caller](https://github.com/cntrl-alt-lenny/gx-spirit-caller) | Yu-Gi-Oh! GX Spirit Caller | all three regions byte-identical, but partly through shipped `.s` | `mwccarm`/`mwldarm` |
| [DQIX/dqix-decomp](https://github.com/DQIX/dqix-decomp) | Dragon Quest IX (EN+JP) | in progress, targets both versions | `mwccarm` |
| [ReGame-Labs/dwds_decomp](https://github.com/ReGame-Labs/dwds_decomp) | Digimon World DS | 65.1% of game functions in C; modules byte-exact via delinked bytes | `1.2/sp4` (build 87) |
| [tangOS-decomps/pictochat-decomp](https://github.com/tangOS-decomps/pictochat-decomp) | PictoChat (DSi title) | 1,002/1,551 functions | — |

Nothing qualifying exists on GitLab, Codeberg, Bitbucket or SourceForge as of
2026-11-22; the non-GitHub DS material is documentation and disassembly, not
rebuilds (see the last section).

## Documents to read, by problem

* **Which compiler build, and which build per file** —
  [acww `docs/matching.md`](https://github.com/7ucina/acww-decomp/blob/main/docs/matching.md)
  and [`docs/versions.md`](https://github.com/banatic/acww-decomp/blob/main/docs/versions.md):
  the only DS writeup that documents per-file 1.2 variant selection and why a
  unit needs it.
  [sm64ds `notes/mwccarm-version-archive-search.md`](https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/mwccarm-version-archive-search.md)
  maps Nintendo releases to compiler and linker build numbers; its CC-versus-LD
  correction is quoted in our playbook.
* **Object and file layout** —
  [acww `docs/matching.md`](https://github.com/7ucina/acww-decomp/blob/main/docs/matching.md)
  §4: functions emitted in reverse source order, data ordered by a size sort,
  and what `-inline noauto` still inlines. The ordering half is verified here.
* **Overlay (`ovNNN`) handling and relinking** —
  [acww `tools/pipeline/linking.md`](https://github.com/7ucina/acww-decomp/blob/main/tools/pipeline/linking.md)
  (link order, section boundaries, overlay BSS),
  [zeldaret/ph `docs/build_system.md`](https://github.com/zeldaret/ph/blob/main/docs/build_system.md)
  (per-module delinking),
  [pmdsky-debug `docs/overlays.md`](https://github.com/UsernameFodder/pmdsky-debug/blob/master/docs/overlays.md)
  (overlay table and load addresses), and
  [dsd `docs/relocs.md`](https://github.com/AetiasHax/ds-decomp/blob/6d315a3bc467d40e9c6b9c547366fdd9f178394a/docs/relocs.md)
  (overlay-ID relocations, target assignment).
* **C++ shapes: vtables, ctors/dtors, base adjusters** —
  [sm64ds `notes/objisolate.md`](https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/objisolate.md)
  and the C++ part of acww's `docs/matching.md`. Not needed for a C unit, but
  this ROM's `_ZThn`-style adjusters and destructor triples come from here.
* **Codegen lever catalogues** —
  [sm64ds `notes/mwccarm-codegen.md`](https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/mwccarm-codegen.md),
  the preserved [decomp.wtf MWCC guide](https://github.com/i12bp8/decomp.wtf/blob/main/src/decompnet/app/guide/mwcc.md),
  [gx-spirit-caller `docs/compiler-quirks.md`](https://github.com/cntrl-alt-lenny/gx-spirit-caller/blob/main/docs/compiler-quirks.md),
  and [pokebw2 `docs/decompiling.md`](https://github.com/fuddlesworth/pokebw2/blob/main/docs/decompiling.md)
  (automated compiler probing).
* **Workflow runbooks** —
  [mlbis `docs/RUNBOOK.md`](https://github.com/weaversworkshop/mlbis-decomp/blob/main/docs/RUNBOOK.md)
  and [kh `docs/PORTING_FROM_DAYS.md`](https://github.com/Yokimitsuro/khrecoded-decomp/blob/main/docs/PORTING_FROM_DAYS.md)
  (reusing a matched function across two titles whose compilers differ).
* **Progress accounting: real C versus library assembly** —
  [kh `docs/PROGRESS_POLICY.md`](https://github.com/Yokimitsuro/khdays-decomp/blob/main/docs/PROGRESS_POLICY.md),
  [sm64ds `notes/asm-policy.md`](https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/asm-policy.md)
  and [`notes/data-tracking.md`](https://github.com/tangosdev/sm64ds-decomp/blob/main/notes/data-tracking.md).
* **Hardware, header and overlay addresses** —
  [GBATEK](https://problemkaputt.de/gbatek.htm):
  [memory maps](http://problemkaputt.de/gbatek-ds-memory-maps.htm),
  [cartridge header](http://problemkaputt.de/gbatek-ds-cartridge-header.htm),
  [secure area](http://problemkaputt.de/gbatek-ds-cartridge-secure-area.htm).
* **Japanese-language material** — the
  [Tongari Boushi field guide](https://darkenedforest.github.io/tongari-boushi-to-oshare-na-mahou-tsukai-archive/field-guide/how-the-game-works/overlays)
  catalogues one DS title's ARM9 overlays, functions and strings; oupo's
  [overlay extraction and disassembly script](https://oupo.hatenadiary.com/entry/20171122/1511342170)
  and the [Qiita NTRGhidra walkthrough](https://qiita.com/DaisukeDaisuke/items/23ba8a13d4f8bdb63396)
  cover getting a ROM into a disassembler.

## Already absorbed - do not re-import

* `tools/match/slots.py` already does what pokebw2's `tools/decomp/locals.py`
  does: `-sym on` is in our flags, so every candidate object carries DWARF and
  the register or stack slot of every parameter and local can be read back.
* sm64ds' codegen levers are already distilled into
  [`mwccarm-codegen.md`](mwccarm-codegen.md); its `pictochat`, `pictochat-decomp`
  and `pmdsky` material adds nothing to that list.
* Their `compiler_search.py` / `asmdiff.py` surface is covered here by
  `vprobe.py`, `var3.py` and `reg.py`; there is nothing to port.
* Compiler-variant practice is already the `config/ATOJ/mwcc_versions.txt`
  mechanism, verified by the ROM digest rather than by an object diff.

## Caveats

* Project names move: `6elphegor/acww-decomp` is now `7ucina/acww-decomp`.
* `InusualZ/mhtri-decomp` is a **Wii** decompilation, not DS. Its
  subscript-versus-pointer-sum lever still applies, but it is not a DS peer.
* Percentages are not comparable across projects: denominators differ (game
  functions only, whole ARM9, functions plus library assembly, bytes).
