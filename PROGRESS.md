# Progress

The matching C sources are compiled with `mwccarm` 1.2/sp2 and linked back into the original addresses. The rebuild is byte-identical to the retail ROM:

```
Target: 8d555a80db47278475ffdfdf808fe80dbd7c48cf
rebuilt: 8d555a80db47278475ffdfdf808fe80dbd7c48cf  -> OK
```

The figures come from `ninja report` (objdiff). A complete unit has its whole body in matching C; gap units are still delinked assembly.

## Overall

| Metric | Matched | Total | Progress |
|---|---:|---:|---|
| Code | 42,456 B | 1,841,700 B | 2.31% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 957 | 3,806 | 25.14% `██████░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 2.33% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 967 | 1,476 | 65.51% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 9.81% (22,028 / 224,616 B) | 586 / 1,377 (42.6%) | 9.83% | 0.00% (0 / 69,856 B) | 587 / 824 |
| OV002 | 2.18% (15,580 / 715,724 B) | 223 / 1,360 (16.4%) | 2.19% | 0.00% (0 / 34,164 B) | 224 / 374 |
| OV003 | 0.22% (1,304 / 586,712 B) | 55 / 547 (10.1%) | 0.23% | 0.00% (0 / 64,648 B) | 56 / 103 |
| OV004 | 1.71% (1,528 / 89,584 B) | 38 / 170 (22.4%) | 1.71% | 0.00% (0 / 27,404 B) | 38 / 61 |
| OV005 | 0.37% (536 / 142,964 B) | 25 / 166 (15.1%) | 0.41% | 0.00% (0 / 369,132 B) | 26 / 48 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 6.91% (756 / 10,940 B) | 15 / 55 (27.3%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 1.10% (272 / 24,788 B) | 9 / 61 (14.8%) | 1.32% | 0.00% (0 / 9,972 B) | 10 / 19 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

957 of 3,806 functions (25.14%) and 42,456 of 1,841,700 code bytes (2.31%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
