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
| Code | 164,408 B | 1,841,700 B | 8.93% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,865 | 3,806 | 49.00% `████████████░░░░░░░░░░░░` |
| Fuzzy match | 8.93% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,154 | 4,807 | 86.42% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 36.56% (82,128 / 224,616 B) | 1,081 / 1,377 (78.5%) | 36.56% | 13.20% (9,224 / 69,856 B) | 1,212 / 1,399 |
| OV002 | 6.09% (43,576 / 715,724 B) | 441 / 1,360 (32.4%) | 6.09% | 61.19% (20,904 / 34,164 B) | 997 / 1,223 |
| OV003 | 2.34% (13,756 / 586,712 B) | 127 / 547 (23.2%) | 2.34% | 82.87% (53,572 / 64,648 B) | 634 / 720 |
| OV004 | 7.01% (6,276 / 89,584 B) | 70 / 170 (41.2%) | 7.01% | 86.66% (23,748 / 27,404 B) | 683 / 738 |
| OV005 | 6.57% (9,388 / 142,964 B) | 62 / 166 (37.3%) | 6.57% | 87.59% (323,324 / 369,132 B) | 318 / 360 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 25.56% (2,796 / 10,940 B) | 34 / 55 (61.8%) | 25.56% | 93.33% (448 / 480 B) | 54 / 67 |
| OV009 | 15.60% (3,868 / 24,788 B) | 30 / 61 (49.2%) | 15.60% | 99.68% (9,940 / 9,972 B) | 67 / 83 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,865 of 3,806 functions (49.00%) and 164,408 of 1,841,700 code bytes (8.93%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
