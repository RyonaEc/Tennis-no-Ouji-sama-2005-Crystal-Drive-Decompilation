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
| Code | 117,212 B | 1,841,700 B | 6.36% `██░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,614 | 3,806 | 42.41% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 6.36% | n/a | `██░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 544,792 B | 801,936 B | 67.93% `████████████████░░░░░░░░` |
| Complete units | 3,849 | 4,518 | 85.19% `████████████████████░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 25.41% (57,080 / 224,616 B) | 933 / 1,377 (67.8%) | 25.41% | 9.59% (6,700 / 69,856 B) | 1,063 / 1,275 |
| OV002 | 5.22% (37,352 / 715,724 B) | 401 / 1,360 (29.5%) | 5.22% | 42.28% (14,444 / 34,164 B) | 953 / 1,174 |
| OV003 | 1.48% (8,684 / 586,712 B) | 107 / 547 (19.6%) | 1.48% | 79.62% (51,472 / 64,648 B) | 610 / 689 |
| OV004 | 4.26% (3,812 / 89,584 B) | 59 / 170 (34.7%) | 4.26% | 73.14% (20,044 / 27,404 B) | 670 / 722 |
| OV005 | 3.49% (4,988 / 142,964 B) | 50 / 166 (30.1%) | 3.49% | 79.56% (293,668 / 369,132 B) | 295 / 337 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 61.77% (105,312 / 170,504 B) | 92 / 108 |
| OV008 | 15.03% (1,644 / 10,940 B) | 27 / 55 (49.1%) | 15.03% | 0.00% (0 / 480 B) | 27 / 44 |
| OV009 | 10.88% (2,696 / 24,788 B) | 26 / 61 (42.6%) | 10.88% | 97.15% (9,688 / 9,972 B) | 61 / 75 |
| OV001 | 2.30% (320 / 13,904 B) | 3 / 21 (14.3%) | 2.30% | 6.55% (516 / 7,872 B) | 21 / 29 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 89.65% (42,948 / 47,904 B) | 57 / 65 |

## Against the whole ROM

1,614 of 3,806 functions (42.41%) and 117,212 of 1,841,700 code bytes (6.36%) are matching C; the rest are still delinked assembly.

544,792 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
