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
| Code | 186,436 B | 1,841,700 B | 10.12% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,976 | 3,806 | 51.92% `████████████░░░░░░░░░░░░` |
| Fuzzy match | 10.12% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,265 | 4,931 | 86.49% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 38.18% (85,768 / 224,616 B) | 1,104 / 1,377 (80.2%) | 38.18% | 13.20% (9,224 / 69,856 B) | 1,235 / 1,411 |
| OV002 | 7.18% (51,388 / 715,724 B) | 481 / 1,360 (35.4%) | 7.18% | 61.19% (20,904 / 34,164 B) | 1,037 / 1,277 |
| OV003 | 3.40% (19,932 / 586,712 B) | 155 / 547 (28.3%) | 3.40% | 82.87% (53,572 / 64,648 B) | 662 / 758 |
| OV004 | 9.30% (8,328 / 89,584 B) | 80 / 170 (47.1%) | 9.30% | 86.66% (23,748 / 27,404 B) | 693 / 749 |
| OV005 | 7.63% (10,912 / 142,964 B) | 68 / 166 (41.0%) | 7.63% | 87.59% (323,324 / 369,132 B) | 324 / 366 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 27.20% (2,976 / 10,940 B) | 35 / 55 (63.6%) | 27.20% | 93.33% (448 / 480 B) | 55 / 68 |
| OV009 | 18.20% (4,512 / 24,788 B) | 33 / 61 (54.1%) | 18.20% | 99.68% (9,940 / 9,972 B) | 70 / 85 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,976 of 3,806 functions (51.92%) and 186,436 of 1,841,700 code bytes (10.12%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
