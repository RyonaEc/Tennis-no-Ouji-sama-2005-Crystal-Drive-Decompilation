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
| Code | 181,264 B | 1,841,700 B | 9.84% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,952 | 3,806 | 51.29% `████████████░░░░░░░░░░░░` |
| Fuzzy match | 9.84% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,241 | 4,906 | 86.45% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 37.90% (85,120 / 224,616 B) | 1,101 / 1,377 (80.0%) | 37.90% | 13.20% (9,224 / 69,856 B) | 1,232 / 1,409 |
| OV002 | 6.79% (48,576 / 715,724 B) | 468 / 1,360 (34.4%) | 6.79% | 61.19% (20,904 / 34,164 B) | 1,024 / 1,263 |
| OV003 | 3.14% (18,440 / 586,712 B) | 148 / 547 (27.1%) | 3.14% | 82.87% (53,572 / 64,648 B) | 655 / 751 |
| OV004 | 9.30% (8,328 / 89,584 B) | 80 / 170 (47.1%) | 9.30% | 86.66% (23,748 / 27,404 B) | 693 / 749 |
| OV005 | 7.63% (10,912 / 142,964 B) | 68 / 166 (41.0%) | 7.63% | 87.59% (323,324 / 369,132 B) | 324 / 366 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 27.20% (2,976 / 10,940 B) | 35 / 55 (63.6%) | 27.20% | 93.33% (448 / 480 B) | 55 / 68 |
| OV009 | 17.31% (4,292 / 24,788 B) | 32 / 61 (52.5%) | 17.31% | 99.68% (9,940 / 9,972 B) | 69 / 83 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,952 of 3,806 functions (51.29%) and 181,264 of 1,841,700 code bytes (9.84%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
