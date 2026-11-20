# Token budget: how the four agents should work

Matching is cheap. Reading is expensive. Every command below prints only the
part that changes a decision, and nothing else.

## The loop

1. `python build/fastmatch.py brief <mod> <addr> <size>` - one task card:
   mnemonic histogram, calls, branches, stores, pool words, and whether the
   unit is already walled. ~6 lines. Enough to plan a body.
2. `python build/udis.py <mod> <addr>` **once**, if the brief is not enough.
   Do not re-dump the same function in a later turn; the card plus the earlier
   listing is usually all a rewrite needs.
3. Write the candidates into a JSON spec and score them:

   `python build/fastmatch.py variants <mod> <addr> <size> spec.json`

   Each body is a full translation unit minus `#include "types.h"`: put your
   `extern` lines in the body. Output is one line per candidate plus up to 8
   differing instructions for the closest one. Ten variants cost ~15 lines.
4. Only when close, dump the full diff of one file:
   `python build/fastmatch.py diffc <mod> <addr> <size> file.c`
5. Install with `build/reg.py` only after `MATCH` (or `WINNER`).

## Do not

- Do not `cat build/walls.txt`, `ready.py` or `frameless-*.txt` wholesale.
  `brief` reports the wall note for the address you are on; that is enough.
- Do not re-read `notes/*.md` every turn. `grep -n "<keyword>" notes/...` for
  the one lever you need.
- Do not paste a whole unified diff to reason about one instruction. The
  compact diff is aligned by instruction index and is what the eye needs.
- Do not run `t10 --check`, `ninja sha1` or `ninja report` per unit. Run them
  once at the end of a batch.
- Do not restate the plan in prose between tool calls. One short line when the
  direction changes, then the call.

## Reporting (helpers -> main tree)

Four lines, no more:

```
unit:<mod> <addr> <size> <name>
state:MATCH | <n>B/<size>B, <k> diff | walled (<class>)
key:<the one instruction or declaration that decided it>
next:<the one experiment worth trying, or "none">
```

The candidate body itself goes in the report file the inbox already scans
(`build/inbox.py` picks up new `*.md` in the helper trees); never inline it in
the message as well.

## What actually costs tokens

- Full 40-instruction disassembly: ~700 tokens. Usually needed once per unit.
- One unified diff: ~500 tokens. Needed rarely; the compact one is ~60.
- A per-variant heredoc harness: ~120 tokens each, every round trip. The
  `variants` command replaces it with a JSON spec written once.
- Re-deriving the same wall: unbounded. Read the wall note first.
