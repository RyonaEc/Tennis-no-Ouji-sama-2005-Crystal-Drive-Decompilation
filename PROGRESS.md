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
| Code | 67,984 B | 1,841,700 B | 3.69% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,160 | 3,806 | 30.48% `███████░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 3.72% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,172 | 1,733 | 67.63% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 12.04% (27,040 / 224,616 B) | 643 / 1,377 (46.7%) | 12.09% | 0.00% (0 / 69,856 B) | 646 / 879 |
| OV002 | 4.33% (31,012 / 715,724 B) | 338 / 1,360 (24.9%) | 4.34% | 0.00% (0 / 34,164 B) | 339 / 521 |
| OV003 | 0.63% (3,672 / 586,712 B) | 70 / 547 (12.8%) | 0.63% | 0.00% (0 / 64,648 B) | 71 / 130 |
| OV004 | 2.20% (1,968 / 89,584 B) | 42 / 170 (24.7%) | 2.20% | 0.00% (0 / 27,404 B) | 42 / 66 |
| OV005 | 1.54% (2,200 / 142,964 B) | 31 / 166 (18.7%) | 1.58% | 0.00% (0 / 369,132 B) | 32 / 59 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 6.91% (756 / 10,940 B) | 15 / 55 (27.3%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 3.36% (832 / 24,788 B) | 14 / 61 (23.0%) | 3.58% | 0.00% (0 / 9,972 B) | 15 / 29 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,160 of 3,806 functions (30.48%) and 67,984 of 1,841,700 code bytes (3.69%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
