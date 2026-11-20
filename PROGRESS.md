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
| Code | 73,476 B | 1,841,700 B | 3.99% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,234 | 3,806 | 32.42% `████████░░░░░░░░░░░░░░░░` |
| Fuzzy match | 4.02% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,246 | 1,824 | 68.31% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 14.29% (32,100 / 224,616 B) | 713 / 1,377 (51.8%) | 14.34% | 0.00% (0 / 69,856 B) | 716 / 964 |
| OV002 | 4.35% (31,156 / 715,724 B) | 339 / 1,360 (24.9%) | 4.36% | 0.00% (0 / 34,164 B) | 340 / 522 |
| OV003 | 0.64% (3,768 / 586,712 B) | 71 / 547 (13.0%) | 0.65% | 0.00% (0 / 64,648 B) | 72 / 132 |
| OV004 | 2.30% (2,064 / 89,584 B) | 43 / 170 (25.3%) | 2.30% | 0.00% (0 / 27,404 B) | 43 / 68 |
| OV005 | 1.61% (2,296 / 142,964 B) | 32 / 166 (19.3%) | 1.65% | 0.00% (0 / 369,132 B) | 33 / 60 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 6.91% (756 / 10,940 B) | 15 / 55 (27.3%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 3.36% (832 / 24,788 B) | 14 / 61 (23.0%) | 3.58% | 0.00% (0 / 9,972 B) | 15 / 29 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,234 of 3,806 functions (32.42%) and 73,476 of 1,841,700 code bytes (3.99%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
