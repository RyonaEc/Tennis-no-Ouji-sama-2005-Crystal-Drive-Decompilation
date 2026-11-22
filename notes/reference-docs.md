# Game documentation, and where the game's own text lives

What documentation exists for *Tennis no Ouji-sama 2005 Crystal Drive*
(NTR-ATOJ-JPN), and — more useful for matching — what the ROM itself says.
Surveyed 2026-11-22. The second half is verified evidence; the first half is a
pointer list to sources we do not control, so re-check a URL before relying on it.

## Outside the ROM

There is no English release: the game shipped in Japan only on 2004-12-30, and
the interface language is Japanese. English *labels* do appear in the interface
(`SIDE BY SIDE` is an in-ROM example below), but that is not a localisation.

| Source | Language | What it has |
|---|---|---|
| [Nintendo product page](https://www.nintendo.co.jp/ds/software/atoj/index.html) | ja | official basics: publisher, date, action-tennis, 1-4 players, 64 characters, 無我の境地 |
| [Retail manual scan](https://archive.org/details/tennis-no-oujisama-2005_manual) | ja | the only complete primary document. **Its OCR is unusable** (the only recoverable part is the English menu labels `SOLO PLAY`, `MULTI PLAY`, `OPTION`, `TACTICS EDIT`, `TEAM EDIT`, `GALLERY`); read the page images |
| [GameFAQs index](https://gamefaqs.gamespot.com/ds/924790-tennis-no-oji-sama-2005-crystal-drive/faqs) | en | 6 English guides: FAQ (weirdaznpyro), Guide/Walkthrough (AirMcnair), Coach FAQ (neofan12s), Food Guide (AckPtoo), Food List (Archanch), Move List (shaggy420); plus a 104 KB Spanish FAQ (ryuzaki) |
| [uk415.blog.fc2.com entry-257](https://uk415.blog.fc2.com/blog-entry-257.html) | ja | character unlock conditions for every school |
| [uk415.blog.fc2.com category-103](https://uk415.blog.fc2.com/blog-category-103.html) | ja | per-character stats and special-move inputs |
| [otomegame-capture.com](https://otomegame-capture.com/other/crystaldrive/data2.html) | ja | unlock conditions organised by mode |
| [menokenkou.work](https://menokenkou.work/tenisuouziura) | ja | hidden elements: Smith's wigs, voice cards, post-game extras |
| [ja.wikipedia テニスの王子様 (ゲーム)](https://ja.wikipedia.org/wiki/%E3%83%86%E3%83%8B%E3%82%B9%E3%81%AE%E7%8E%8B%E5%AD%90%E6%A7%98_(%E3%82%B2%E3%83%BC%E3%83%A0)) | ja | series context, not this title's system |

Not found, and worth not assuming:

- **No English fan translation patch or project.** GBAtemp's fan-translation
  roundup covers other *Prince of Tennis* titles, not this one. This is "nothing
  indexed", not "nothing exists".
- **No confirmed strategy guide / mook ISBN**, no primary Konami product page,
  and the "Konami the Best" re-release date appears only in secondary listings.
- No game-specific wikiwiki/gamecat page; the Japanese material is blogs.

## Inside the ROM: the Shift-JIS string bank in ov004

The game's own text is the authority here, and it is easy to read:

- Text is **Shift-JIS, NUL-padded**. Not ASCII, not UTF-16. Both of those were
  checked: no `SOLO PLAY`, `MULTI PLAY`, `GALLERY`, `SMART SHOP`, `OPEN CUP`
  or `IRON PLATE` ASCII string exists anywhere in the 64 MiB image.
- The bank sits in **ov004**: file offsets `0x1a4f6` .. `0x1b3d0`, which is
  ROM `0x1b04f6` .. `0x1b73d0` (`rom = file + 0x6000` for this overlay) and
  RAM `0x02066cb6` .. `0x02067b90`
  (`ram = 0x0204c7c0 + file_offset`, the ov004 load base being the first
  function's address). Every overlay loads at that same base, so this RAM range
  is only the bank while ov004 is the resident overlay; the workbench's overlay
  table says which one is (`notes/workbench.md`).
- **ov006 holds a second copy.** `都大会` also appears at `ov006` file offset
  `0x62c4`, a shorter table rather than the whole bank, so a match that reads
  one of these strings has to check which overlay is loaded before assigning it
  an address.
- **No symbol covers it.** The nearest named object is
  `data_ov004_02068fac kind:data(any)`, so the bank is part of the
  delinked-data blob; nothing in `symbols.txt` claims it yet.

`extract/ATOJ/arm9_overlays/ov004.bin` is the copy to read. The table is the
decoded content of `0x1b1c8`..`0x1b3d0` (tournament and application-form names):

| ov004 offset | String |
|---|---|
| `0x1b1c8` | 都大会団体 |
| `0x1b1d4` | SIDE BY SIDE |
| `0x1b1e4` | 全国大会団体 |
| `0x1b1f4` | 関東大会団体 |
| `0x1b204` | 都大会 DOUBLES |
| `0x1b214` | 都大会 SINGLES |
| `0x1b224` | 青学ランキング |
| `0x1b234` | 関東大会 DOUBLES |
| `0x1b248` | 関東大会 SINGLES |
| `0x1b25c` | 全国大会 SINGLES |
| `0x1b270` | 全国大会 DOUBLES |
| `0x1b284` | Ｒ−１の参加申込書 |
| `0x1b298` | Ｉ−１の参加申込書 |
| `0x1b2ac` | ＯＣＳの参加申込書 |
| `0x1b2c0` | Ｍ−１の参加申込書 |
| `0x1b2d4` | Ｕ−１の参加申込書 |
| `0x1b2e8` | ＫＯＳの参加申込書 |
| `0x1b2fc` | ＯＣＴの参加申込書 |
| `0x1b310` | ＳＯＨの参加申込書 |
| `0x1b324` | ＯＣＤの参加申込書 |
| `0x1b338` | ＴＯＬの参加申込書 |
| `0x1b34c` | Ｔ−１の参加申込書 |
| `0x1b360` | Ｄ．Ｕ．Ｓ．の参加申込書 |
| `0x1b37c` | 爆庭闘球祭：孤の参加申込書 |
| `0x1b398` | 爆庭闘球祭：双の参加申込書 |
| `0x1b3b4` | 爆庭闘球祭：群の参加申込書 |

The acronyms are printed in the ROM only in that form. The expansions below are
**inference**, from where the fan guides describe the same events — not text the
game spells out, so do not cite them as in-ROM strings:

| acronym | inferred event |
|---|---|
| `Ｒ−１` / `Ｉ−１` / `Ｍ−１` / `Ｔ−１` / `Ｕ−１` | R-1 / I-1 / M-1 / T-1 / U-1 tournaments |
| `ＯＣＳ` / `ＯＣＤ` / `ＯＣＴ` | Open Cup Singles / Doubles / Teams |
| `ＫＯＳ` | King of Strokers |
| `ＳＯＨ` | Spirit of Hard Core |
| `ＴＯＬ` | Top of Lefty |
| `Ｄ．Ｕ．Ｓ．` | unresolved; the manual describes it as a high-risk, high-return rule set |

The surrounding bank (`0x1a4f6` and up) also holds coach/shop dialogue
(`スミスコーチ`, `竹内倫子`, ...) and character names — e.g. `アイアンプレート`,
`オープンカップ`, `アルティメット`, `トーナメント` are all Shift-JIS there. A
useful side effect: `extract/ATOJ/arm9_overlays/ov004.bin` can be searched
directly for any string a matched function is seen to load.

## How this feeds naming

`notes/naming/annotations.tsv` takes a row only when a unit has **matched** and
the evidence is fresh, and its `address` column is the retail address the unit
matched at. None of the strings above is pinned to a matched function yet, so no
rows were added for them — writing one now would record a guess as if it were a
match. When a unit turns out to read this bank, record it as

```
string at ov004 0x1bXXX (RAM 0x0206XXXX)
```

in the `evidence` column. Until then the table above is the reference, and
top-level title-menu labels (`SOLO PLAY`, `GALLERY`, ...) stay unclaimed: they
are not in this string bank, so they are most likely drawn graphics rather than
text, and a matched unit is the only thing that can say otherwise.
