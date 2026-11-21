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
| Code | 101,376 B | 1,841,700 B | 5.50% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,475 | 3,806 | 38.75% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 5.50% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,475 | 2,059 | 71.64% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 21.63% (48,588 / 224,616 B) | 858 / 1,377 (62.3%) | 21.63% | 0.00% (0 / 69,856 B) | 858 / 1,081 |
| OV002 | 4.76% (34,056 / 715,724 B) | 370 / 1,360 (27.2%) | 4.76% | 0.00% (0 / 34,164 B) | 370 / 565 |
| OV003 | 1.19% (7,004 / 586,712 B) | 95 / 547 (17.4%) | 1.19% | 0.00% (0 / 64,648 B) | 95 / 165 |
| OV004 | 4.06% (3,640 / 89,584 B) | 57 / 170 (33.5%) | 4.06% | 0.00% (0 / 27,404 B) | 57 / 88 |
| OV005 | 2.87% (4,100 / 142,964 B) | 43 / 166 (25.9%) | 2.87% | 0.00% (0 / 369,132 B) | 43 / 72 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 10.60% (1,160 / 10,940 B) | 22 / 55 (40.0%) | 10.59% | 0.00% (0 / 480 B) | 22 / 37 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,475 of 3,806 functions (38.75%) and 101,376 of 1,841,700 code bytes (5.50%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
