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
| Code | 120,792 B | 1,841,700 B | 6.56% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,630 | 3,806 | 42.83% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 6.56% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 3,919 | 4,564 | 85.87% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 25.64% (57,596 / 224,616 B) | 937 / 1,377 (68.0%) | 25.64% | 13.20% (9,224 / 69,856 B) | 1,068 / 1,278 |
| OV002 | 5.32% (38,052 / 715,724 B) | 405 / 1,360 (29.8%) | 5.32% | 61.19% (20,904 / 34,164 B) | 961 / 1,181 |
| OV003 | 1.75% (10,244 / 586,712 B) | 112 / 547 (20.5%) | 1.75% | 82.87% (53,572 / 64,648 B) | 619 / 697 |
| OV004 | 4.29% (3,844 / 89,584 B) | 60 / 170 (35.3%) | 4.29% | 86.66% (23,748 / 27,404 B) | 673 / 724 |
| OV005 | 4.03% (5,760 / 142,964 B) | 52 / 166 (31.3%) | 4.03% | 87.59% (323,324 / 369,132 B) | 308 / 342 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 80.25% (136,836 / 170,504 B) | 98 / 108 |
| OV008 | 15.03% (1,644 / 10,940 B) | 27 / 55 (49.1%) | 15.03% | 93.33% (448 / 480 B) | 47 / 64 |
| OV009 | 10.88% (2,696 / 24,788 B) | 26 / 61 (42.6%) | 10.88% | 99.68% (9,940 / 9,972 B) | 63 / 75 |
| OV001 | 2.30% (320 / 13,904 B) | 3 / 21 (14.3%) | 2.30% | 61.79% (4,864 / 7,872 B) | 23 / 30 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 99.92% (47,864 / 47,904 B) | 59 / 65 |

## Against the whole ROM

1,630 of 3,806 functions (42.83%) and 120,792 of 1,841,700 code bytes (6.56%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
