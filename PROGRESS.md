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
| Code | 87,204 B | 1,841,700 B | 4.73% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,350 | 3,806 | 35.47% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 4.73% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,350 | 1,927 | 70.06% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 18.60% (41,788 / 224,616 B) | 791 / 1,377 (57.4%) | 18.60% | 0.00% (0 / 69,856 B) | 791 / 1,026 |
| OV002 | 4.38% (31,380 / 715,724 B) | 342 / 1,360 (25.1%) | 4.38% | 0.00% (0 / 34,164 B) | 342 / 525 |
| OV003 | 0.99% (5,820 / 586,712 B) | 87 / 547 (15.9%) | 0.99% | 0.00% (0 / 64,648 B) | 87 / 153 |
| OV004 | 3.26% (2,920 / 89,584 B) | 50 / 170 (29.4%) | 3.26% | 0.00% (0 / 27,404 B) | 50 / 79 |
| OV005 | 1.85% (2,640 / 142,964 B) | 35 / 166 (21.1%) | 1.85% | 0.00% (0 / 369,132 B) | 35 / 63 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 9.47% (1,036 / 10,940 B) | 21 / 55 (38.2%) | 9.46% | 0.00% (0 / 480 B) | 21 / 36 |
| OV009 | 3.97% (984 / 24,788 B) | 16 / 61 (26.2%) | 3.97% | 0.00% (0 / 9,972 B) | 16 / 30 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,350 of 3,806 functions (35.47%) and 87,204 of 1,841,700 code bytes (4.73%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
