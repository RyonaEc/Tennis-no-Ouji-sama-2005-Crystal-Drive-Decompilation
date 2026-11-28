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
| Code | 191,552 B | 1,841,700 B | 10.40% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,998 | 3,806 | 52.50% `█████████████░░░░░░░░░░░` |
| Fuzzy match | 10.40% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,287 | 4,955 | 86.52% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 38.50% (86,484 / 224,616 B) | 1,107 / 1,377 (80.4%) | 38.50% | 13.20% (9,224 / 69,856 B) | 1,238 / 1,414 |
| OV002 | 7.66% (54,860 / 715,724 B) | 496 / 1,360 (36.5%) | 7.66% | 61.19% (20,904 / 34,164 B) | 1,052 / 1,295 |
| OV003 | 3.48% (20,392 / 586,712 B) | 157 / 547 (28.7%) | 3.48% | 82.87% (53,572 / 64,648 B) | 664 / 761 |
| OV004 | 9.30% (8,328 / 89,584 B) | 80 / 170 (47.1%) | 9.30% | 86.66% (23,748 / 27,404 B) | 693 / 749 |
| OV005 | 7.63% (10,912 / 142,964 B) | 68 / 166 (41.0%) | 7.63% | 87.59% (323,324 / 369,132 B) | 324 / 366 |
| OV006 | 9.56% (700 / 7,324 B) | 5 / 16 (31.2%) | 9.56% | 80.25% (136,836 / 170,504 B) | 102 / 114 |
| OV008 | 31.48% (3,444 / 10,940 B) | 37 / 55 (67.3%) | 31.48% | 93.33% (448 / 480 B) | 57 / 68 |
| OV009 | 18.20% (4,512 / 24,788 B) | 33 / 61 (54.1%) | 18.20% | 99.68% (9,940 / 9,972 B) | 70 / 85 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,998 of 3,806 functions (52.50%) and 191,552 of 1,841,700 code bytes (10.40%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
