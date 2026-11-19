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
| Code | 65,100 B | 1,841,700 B | 3.53% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,131 | 3,806 | 29.72% `███████░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 3.56% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,141 | 1,699 | 67.16% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 11.60% (26,048 / 224,616 B) | 630 / 1,377 (45.8%) | 11.62% | 0.00% (0 / 69,856 B) | 631 / 871 |
| OV002 | 4.33% (30,968 / 715,724 B) | 337 / 1,360 (24.8%) | 4.34% | 0.00% (0 / 34,164 B) | 338 / 519 |
| OV003 | 0.49% (2,864 / 586,712 B) | 64 / 547 (11.7%) | 0.49% | 0.00% (0 / 64,648 B) | 65 / 121 |
| OV004 | 1.71% (1,528 / 89,584 B) | 38 / 170 (22.4%) | 1.71% | 0.00% (0 / 27,404 B) | 38 / 61 |
| OV005 | 1.39% (1,988 / 142,964 B) | 30 / 166 (18.1%) | 1.43% | 0.00% (0 / 369,132 B) | 31 / 57 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 6.91% (756 / 10,940 B) | 15 / 55 (27.3%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 2.00% (496 / 24,788 B) | 11 / 61 (18.0%) | 2.23% | 0.00% (0 / 9,972 B) | 12 / 23 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,131 of 3,806 functions (29.72%) and 65,100 of 1,841,700 code bytes (3.53%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
