# Progress on bytematch.dev

[ByteMatch](https://bytematch.dev) tracks matching-decompilation progress. Everything
the site needs from this repository is already committed here; the remaining steps
are one-time settings on the project's Manage page, which need a signed-in
repository admin.

Project page: `https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation`

## Files in this repository

| Path | What it is |
|---|---|
| `assets/icon.png` | 256 × 256 game icon, taken from the retail banner |
| `assets/cover.png` | 1500 × 500 cover, generated from the same icon |
| `tools/extract_icon.py` | Regenerates `icon.png` from a ROM (`python tools/extract_icon.py <rom>`) |
| `tools/make_cover.py` | Regenerates `cover.png` from a ROM (`python tools/make_cover.py <rom>`) |
| `tools/upload_report.py` | Uploads `build/ATOJ/report.json` to ByteMatch |
| `build/ATOJ/report.json` | The objdiff report, produced by `ninja report` |

ByteMatch does not store images: it links to them, so both images are served from
this repository. They only load once the repository is public.

## One-time setup (Manage page)

1. **Add project** — Manage → Add project → pick this repository.
2. **Icon and cover** — under *Icon and cover*, paste:

   * icon: `https://raw.githubusercontent.com/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation/main/assets/icon.png`
   * cover: `https://raw.githubusercontent.com/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation/main/assets/cover.png`

3. **Game** — under Settings set game `Tennis no Ouji-sama 2005: Crystal Drive`,
   franchise `Tennis no Ouji-sama`, year `2004`. Naming the game is what puts the
   project on its game page in addition to the main list.
4. **Default version / category** — version `ATOJ`; leave the category default so
   the headline counts the whole main module.

## Getting reports in

ByteMatch reads reports either from a GitHub Actions artifact named `<VERSION>_report`
on a successful run on the default branch, or through its upload API. **Use the upload
API here.** The build needs the retail ROM, the ARM7 BIOS and the CodeWarrior
compilers, none of which can be committed or fetched in hosted CI, so
`.github/workflows/build.yml` targets a self-hosted runner and will not produce an
artifact on GitHub's runners.

Upload from a machine that can build:

```sh
python -m ninja report          # writes build/ATOJ/report.json
export BYTEMATCH_TOKEN=...      # Manage page -> upload token (shown once)
python tools/upload_report.py   # tags the upload with the current commit
```

`tools/upload_report.py --dry-run` prints what would be sent without uploading.

Upload after each batch of newly matched functions. The report is only meaningful
when the ROM still hashes to the retail dump, so run `ninja sha1` first.

## Badges

The README uses two:

```markdown
[![Code](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation.svg?label=Code&measure=matched_code_percent)](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation)
[![Functions](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation.svg?label=Functions&measure=matched_functions)](https://bytematch.dev/RyonaEc/Tennis-no-Ouji-sama-2005-Crystal-Drive-Decompilation)
```

`measure` accepts `matched_code_percent`, `matched_data_percent`,
`matched_functions`, `complete_units` and `fuzzy_match_percent`; add `&style=flat-square`
or `&version=ATOJ` as needed. Until the repository is public and the project is
registered, every badge renders as `ByteMatch: project not found`.

## Going public checklist

1. Make the repository public.
2. Register the project on bytematch.dev (Manage → Add project).
3. Paste the icon and cover links, set the game, franchise and year.
4. Create an upload token and post the first report with `tools/upload_report.py`.
5. Check that the badges in the README resolve.

If the reports do not appear, the Manage page shows the last ingest error.
