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
| Code | 130,668 B | 1,841,700 B | 7.09% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,659 | 3,806 | 43.59% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 7.09% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 3,948 | 4,610 | 85.64% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 26.03% (58,476 / 224,616 B) | 941 / 1,377 (68.3%) | 26.03% | 13.20% (9,224 / 69,856 B) | 1,072 / 1,282 |
| OV002 | 5.40% (38,640 / 715,724 B) | 408 / 1,360 (30.0%) | 5.40% | 61.19% (20,904 / 34,164 B) | 964 / 1,184 |
| OV003 | 2.09% (12,240 / 586,712 B) | 119 / 547 (21.8%) | 2.09% | 82.87% (53,572 / 64,648 B) | 626 / 709 |
| OV004 | 5.89% (5,276 / 89,584 B) | 63 / 170 (37.1%) | 5.89% | 86.66% (23,748 / 27,404 B) | 676 / 729 |
| OV005 | 6.16% (8,804 / 142,964 B) | 58 / 166 (34.9%) | 6.16% | 87.59% (323,324 / 369,132 B) | 314 / 354 |
| OV006 | 5.57% (408 / 7,324 B) | 2 / 16 (12.5%) | 5.57% | 80.25% (136,836 / 170,504 B) | 99 / 109 |
| OV008 | 16.71% (1,828 / 10,940 B) | 28 / 55 (50.9%) | 16.71% | 93.33% (448 / 480 B) | 48 / 65 |
| OV009 | 14.46% (3,584 / 24,788 B) | 28 / 61 (45.9%) | 14.46% | 99.68% (9,940 / 9,972 B) | 65 / 79 |
| OV001 | 3.51% (488 / 13,904 B) | 4 / 21 (19.0%) | 3.51% | 61.79% (4,864 / 7,872 B) | 24 / 32 |
| OV010 | 3.67% (924 / 25,144 B) | 8 / 33 (24.2%) | 3.67% | 99.92% (47,864 / 47,904 B) | 60 / 67 |

## Against the whole ROM

1,659 of 3,806 functions (43.59%) and 130,668 of 1,841,700 code bytes (7.09%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
