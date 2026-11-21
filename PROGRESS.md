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
| Code | 98,360 B | 1,841,700 B | 5.34% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,433 | 3,806 | 37.65% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 5.34% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,433 | 2,019 | 70.98% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 21.17% (47,548 / 224,616 B) | 840 / 1,377 (61.0%) | 21.17% | 0.00% (0 / 69,856 B) | 840 / 1,070 |
| OV002 | 4.59% (32,820 / 715,724 B) | 351 / 1,360 (25.8%) | 4.59% | 0.00% (0 / 34,164 B) | 351 / 540 |
| OV003 | 1.17% (6,864 / 586,712 B) | 94 / 547 (17.2%) | 1.17% | 0.00% (0 / 64,648 B) | 94 / 165 |
| OV004 | 4.02% (3,600 / 89,584 B) | 56 / 170 (32.9%) | 4.02% | 0.00% (0 / 27,404 B) | 56 / 87 |
| OV005 | 2.64% (3,772 / 142,964 B) | 41 / 166 (24.7%) | 2.64% | 0.00% (0 / 369,132 B) | 41 / 70 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 10.60% (1,160 / 10,940 B) | 22 / 55 (40.0%) | 10.59% | 0.00% (0 / 480 B) | 22 / 37 |
| OV009 | 7.91% (1,960 / 24,788 B) | 21 / 61 (34.4%) | 7.90% | 0.00% (0 / 9,972 B) | 21 / 35 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,433 of 3,806 functions (37.65%) and 98,360 of 1,841,700 code bytes (5.34%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
