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
| Code | 203,612 B | 1,841,700 B | 11.06% `███░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 2,051 | 3,806 | 53.89% `█████████████░░░░░░░░░░░` |
| Fuzzy match | 11.06% | n/a | `███░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,340 | 4,988 | 87.01% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 41.45% (93,104 / 224,616 B) | 1,138 / 1,377 (82.6%) | 41.45% | 13.20% (9,224 / 69,856 B) | 1,269 / 1,425 |
| OV002 | 8.03% (57,440 / 715,724 B) | 507 / 1,360 (37.3%) | 8.03% | 61.19% (20,904 / 34,164 B) | 1,063 / 1,305 |
| OV003 | 3.88% (22,752 / 586,712 B) | 166 / 547 (30.3%) | 3.88% | 82.87% (53,572 / 64,648 B) | 673 / 772 |
| OV004 | 9.30% (8,328 / 89,584 B) | 80 / 170 (47.1%) | 9.30% | 86.66% (23,748 / 27,404 B) | 693 / 749 |
| OV005 | 7.80% (11,152 / 142,964 B) | 69 / 166 (41.6%) | 7.80% | 87.59% (323,324 / 369,132 B) | 325 / 367 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 33.86% (3,704 / 10,940 B) | 38 / 55 (69.1%) | 33.86% | 93.33% (448 / 480 B) | 58 / 68 |
| OV009 | 18.20% (4,512 / 24,788 B) | 33 / 61 (54.1%) | 18.20% | 99.68% (9,940 / 9,972 B) | 70 / 85 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

2,051 of 3,806 functions (53.89%) and 203,612 of 1,841,700 code bytes (11.06%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
