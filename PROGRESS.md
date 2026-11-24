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
| Code | 137,468 B | 1,841,700 B | 7.46% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,727 | 3,806 | 45.38% `███████████░░░░░░░░░░░░░` |
| Fuzzy match | 7.46% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,016 | 4,686 | 85.70% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 27.80% (62,440 / 224,616 B) | 993 / 1,377 (72.1%) | 27.80% | 13.20% (9,224 / 69,856 B) | 1,124 / 1,335 |
| OV002 | 5.50% (39,360 / 715,724 B) | 411 / 1,360 (30.2%) | 5.50% | 61.19% (20,904 / 34,164 B) | 967 / 1,189 |
| OV003 | 2.22% (13,032 / 586,712 B) | 123 / 547 (22.5%) | 2.22% | 82.87% (53,572 / 64,648 B) | 630 / 714 |
| OV004 | 5.89% (5,276 / 89,584 B) | 63 / 170 (37.1%) | 5.89% | 86.66% (23,748 / 27,404 B) | 676 / 729 |
| OV005 | 6.24% (8,920 / 142,964 B) | 59 / 166 (35.5%) | 6.24% | 87.59% (323,324 / 369,132 B) | 315 / 356 |
| OV006 | 9.39% (688 / 7,324 B) | 4 / 16 (25.0%) | 9.39% | 80.25% (136,836 / 170,504 B) | 101 / 112 |
| OV008 | 17.95% (1,964 / 10,940 B) | 29 / 55 (52.7%) | 17.95% | 93.33% (448 / 480 B) | 49 / 65 |
| OV009 | 15.60% (3,868 / 24,788 B) | 30 / 61 (49.2%) | 15.60% | 99.68% (9,940 / 9,972 B) | 67 / 83 |
| OV001 | 4.60% (640 / 13,904 B) | 5 / 21 (23.8%) | 4.60% | 61.79% (4,864 / 7,872 B) | 25 / 33 |
| OV010 | 5.09% (1,280 / 25,144 B) | 10 / 33 (30.3%) | 5.09% | 99.92% (47,864 / 47,904 B) | 62 / 70 |

## Against the whole ROM

1,727 of 3,806 functions (45.38%) and 137,468 of 1,841,700 code bytes (7.46%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
