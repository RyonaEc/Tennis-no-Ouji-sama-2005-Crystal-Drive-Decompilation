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
| Code | 30,476 B | 1,841,700 B | 1.65% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 794 | 3,806 | 20.86% `█████░░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.66% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 796 | 1,219 | 65.30% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 8.07% (18,116 / 224,616 B) | 523 / 1,377 (38.0%) | 8.07% | 0.00% (0 / 69,856 B) | 523 / 744 |
| OV002 | 1.38% (9,892 / 715,724 B) | 165 / 1,360 (12.1%) | 1.38% | 0.00% (0 / 34,164 B) | 165 / 275 |
| OV003 | 0.19% (1,120 / 586,712 B) | 51 / 547 (9.3%) | 0.19% | 0.00% (0 / 64,648 B) | 51 / 94 |
| OV004 | 0.48% (432 / 89,584 B) | 17 / 170 (10.0%) | 0.48% | 0.00% (0 / 27,404 B) | 17 / 29 |
| OV005 | 0.34% (484 / 142,964 B) | 24 / 166 (14.5%) | 0.34% | 0.00% (0 / 369,132 B) | 24 / 44 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 1.94% (212 / 10,940 B) | 6 / 55 (10.9%) | 2.45% | 0.00% (0 / 480 B) | 8 / 14 |
| OV009 | 0.89% (220 / 24,788 B) | 8 / 61 (13.1%) | 0.89% | 0.00% (0 / 9,972 B) | 8 / 16 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 0.00% (0 / 25,144 B) | 0 / 33 (0.0%) | 0.00% | 0.00% (0 / 47,904 B) | 0 / 1 |

## Against the whole ROM

794 of 3,806 functions (20.86%) and 30,476 of 1,841,700 code bytes (1.65%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
