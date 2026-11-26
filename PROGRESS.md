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
| Code | 168,252 B | 1,841,700 B | 9.14% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,889 | 3,806 | 49.63% `████████████░░░░░░░░░░░░` |
| Fuzzy match | 9.14% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,178 | 4,844 | 86.25% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 36.81% (82,684 / 224,616 B) | 1,085 / 1,377 (78.8%) | 36.81% | 13.20% (9,224 / 69,856 B) | 1,216 / 1,403 |
| OV002 | 6.43% (46,056 / 715,724 B) | 456 / 1,360 (33.5%) | 6.43% | 61.19% (20,904 / 34,164 B) | 1,012 / 1,246 |
| OV003 | 2.48% (14,564 / 586,712 B) | 132 / 547 (24.1%) | 2.48% | 82.87% (53,572 / 64,648 B) | 639 / 730 |
| OV004 | 7.01% (6,276 / 89,584 B) | 70 / 170 (41.2%) | 7.01% | 86.66% (23,748 / 27,404 B) | 683 / 738 |
| OV005 | 6.57% (9,388 / 142,964 B) | 62 / 166 (37.3%) | 6.57% | 87.59% (323,324 / 369,132 B) | 318 / 360 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 25.56% (2,796 / 10,940 B) | 34 / 55 (61.8%) | 25.56% | 93.33% (448 / 480 B) | 54 / 67 |
| OV009 | 15.60% (3,868 / 24,788 B) | 30 / 61 (49.2%) | 15.60% | 99.68% (9,940 / 9,972 B) | 67 / 83 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,889 of 3,806 functions (49.63%) and 168,252 of 1,841,700 code bytes (9.14%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
