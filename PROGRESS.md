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
| Code | 34,236 B | 1,841,700 B | 1.86% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 855 | 3,806 | 22.46% `█████░░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.88% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 865 | 1,317 | 65.68% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 8.74% (19,636 / 224,616 B) | 552 / 1,377 (40.1%) | 8.77% | 0.00% (0 / 69,856 B) | 553 / 779 |
| OV002 | 1.59% (11,384 / 715,724 B) | 181 / 1,360 (13.3%) | 1.60% | 0.00% (0 / 34,164 B) | 182 / 300 |
| OV003 | 0.22% (1,304 / 586,712 B) | 55 / 547 (10.1%) | 0.23% | 0.00% (0 / 64,648 B) | 56 / 103 |
| OV004 | 0.83% (744 / 89,584 B) | 24 / 170 (14.1%) | 0.83% | 0.00% (0 / 27,404 B) | 24 / 40 |
| OV005 | 0.37% (536 / 142,964 B) | 25 / 166 (15.1%) | 0.41% | 0.00% (0 / 369,132 B) | 26 / 48 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 2.38% (260 / 10,940 B) | 7 / 55 (12.7%) | 3.94% | 0.00% (0 / 480 B) | 12 / 22 |
| OV009 | 1.10% (272 / 24,788 B) | 9 / 61 (14.8%) | 1.32% | 0.00% (0 / 9,972 B) | 10 / 19 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 0.40% (100 / 25,144 B) | 2 / 33 (6.1%) | 0.40% | 0.00% (0 / 47,904 B) | 2 / 4 |

## Against the whole ROM

855 of 3,806 functions (22.46%) and 34,236 of 1,841,700 code bytes (1.86%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
