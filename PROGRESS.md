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
| Code | 136,432 B | 1,841,700 B | 7.41% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,718 | 3,806 | 45.14% `███████████░░░░░░░░░░░░░` |
| Fuzzy match | 7.41% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 630,724 B | 801,936 B | 78.65% `███████████████████░░░░░` |
| Complete units | 4,007 | 4,676 | 85.69% `█████████████████████░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 27.70% (62,228 / 224,616 B) | 991 / 1,377 (72.0%) | 27.70% | 13.20% (9,224 / 69,856 B) | 1,122 / 1,334 |
| OV002 | 5.49% (39,300 / 715,724 B) | 410 / 1,360 (30.1%) | 5.49% | 61.19% (20,904 / 34,164 B) | 966 / 1,188 |
| OV003 | 2.20% (12,928 / 586,712 B) | 121 / 547 (22.1%) | 2.20% | 82.87% (53,572 / 64,648 B) | 628 / 711 |
| OV004 | 5.89% (5,276 / 89,584 B) | 63 / 170 (37.1%) | 5.89% | 86.66% (23,748 / 27,404 B) | 676 / 729 |
| OV005 | 6.24% (8,920 / 142,964 B) | 59 / 166 (35.5%) | 6.24% | 87.59% (323,324 / 369,132 B) | 315 / 356 |
| OV006 | 9.39% (688 / 7,324 B) | 4 / 16 (25.0%) | 9.39% | 80.25% (136,836 / 170,504 B) | 101 / 112 |
| OV008 | 16.71% (1,828 / 10,940 B) | 28 / 55 (50.9%) | 16.71% | 93.33% (448 / 480 B) | 48 / 65 |
| OV009 | 14.97% (3,712 / 24,788 B) | 29 / 61 (47.5%) | 14.97% | 99.68% (9,940 / 9,972 B) | 66 / 81 |
| OV001 | 3.51% (488 / 13,904 B) | 4 / 21 (19.0%) | 3.51% | 61.79% (4,864 / 7,872 B) | 24 / 32 |
| OV010 | 4.23% (1,064 / 25,144 B) | 9 / 33 (27.3%) | 4.23% | 99.92% (47,864 / 47,904 B) | 61 / 68 |

## Against the whole ROM

1,718 of 3,806 functions (45.14%) and 136,432 of 1,841,700 code bytes (7.41%) are matching C; the rest are still delinked assembly.

630,724 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
