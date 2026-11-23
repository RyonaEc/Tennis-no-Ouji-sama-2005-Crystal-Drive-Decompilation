# Progress

The matching C sources are compiled with `mwccarm` 1.2/b56 and linked back into the original addresses. The rebuild is byte-identical to the retail ROM:

```
Target: 9eec3248d3569eba8abcb3244adcbdf01a5ea424
rebuilt: 9eec3248d3569eba8abcb3244adcbdf01a5ea424  -> OK
```

The figures come from `ninja report` (objdiff). A complete unit has its whole body in matching C; gap units are still delinked assembly.

## Overall

| Metric | Matched | Total | Progress |
|---|---:|---:|---|
| Code | 114,936 B | 1,841,700 B | 6.24% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,598 | 3,806 | 41.99% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 6.24% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 2,708 B | 801,936 B | 0.34% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,674 | 2,256 | 74.20% `██████████████████░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 25.36% (56,972 / 224,616 B) | 932 / 1,377 (67.7%) | 25.36% | 3.88% (2,708 / 69,856 B) | 1,008 / 1,214 |
| OV002 | 5.06% (36,192 / 715,724 B) | 394 / 1,360 (29.0%) | 5.06% | 0.00% (0 / 34,164 B) | 394 / 599 |
| OV003 | 1.48% (8,684 / 586,712 B) | 107 / 547 (19.6%) | 1.48% | 0.00% (0 / 64,648 B) | 107 / 176 |
| OV004 | 4.26% (3,812 / 89,584 B) | 59 / 170 (34.7%) | 4.26% | 0.00% (0 / 27,404 B) | 59 / 88 |
| OV005 | 3.14% (4,484 / 142,964 B) | 46 / 166 (27.7%) | 3.14% | 0.00% (0 / 369,132 B) | 46 / 78 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 15.03% (1,644 / 10,940 B) | 27 / 55 (49.1%) | 15.03% | 0.00% (0 / 480 B) | 27 / 44 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 2.30% (320 / 13,904 B) | 3 / 21 (14.3%) | 2.30% | 0.00% (0 / 7,872 B) | 3 / 7 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,598 of 3,806 functions (41.99%) and 114,936 of 1,841,700 code bytes (6.24%) are matching C; the rest are still delinked assembly.

2,708 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
