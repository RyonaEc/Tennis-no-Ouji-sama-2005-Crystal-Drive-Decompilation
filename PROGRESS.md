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
| Code | 134,844 B | 1,841,700 B | 7.32% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,693 | 3,806 | 44.48% `███████████░░░░░░░░░░░░░` |
| Fuzzy match | 7.32% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 3,982 | 4,637 | 85.87% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 27.29% (61,304 / 224,616 B) | 971 / 1,377 (70.5%) | 27.29% | 13.20% (9,224 / 69,856 B) | 1,102 / 1,303 |
| OV002 | 5.49% (39,300 / 715,724 B) | 410 / 1,360 (30.1%) | 5.49% | 61.19% (20,904 / 34,164 B) | 966 / 1,188 |
| OV003 | 2.20% (12,928 / 586,712 B) | 121 / 547 (22.1%) | 2.20% | 82.87% (53,572 / 64,648 B) | 628 / 711 |
| OV004 | 5.89% (5,276 / 89,584 B) | 63 / 170 (37.1%) | 5.89% | 86.66% (23,748 / 27,404 B) | 676 / 729 |
| OV005 | 6.16% (8,804 / 142,964 B) | 58 / 166 (34.9%) | 6.16% | 87.59% (323,324 / 369,132 B) | 314 / 354 |
| OV006 | 5.57% (408 / 7,324 B) | 2 / 16 (12.5%) | 5.57% | 80.25% (136,836 / 170,504 B) | 99 / 109 |
| OV008 | 16.71% (1,828 / 10,940 B) | 28 / 55 (50.9%) | 16.71% | 93.33% (448 / 480 B) | 48 / 65 |
| OV009 | 14.46% (3,584 / 24,788 B) | 28 / 61 (45.9%) | 14.46% | 99.68% (9,940 / 9,972 B) | 65 / 79 |
| OV001 | 3.51% (488 / 13,904 B) | 4 / 21 (19.0%) | 3.51% | 61.79% (4,864 / 7,872 B) | 24 / 32 |
| OV010 | 3.67% (924 / 25,144 B) | 8 / 33 (24.2%) | 3.67% | 99.92% (47,864 / 47,904 B) | 60 / 67 |

## Against the whole ROM

1,693 of 3,806 functions (44.48%) and 134,844 of 1,841,700 code bytes (7.32%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
