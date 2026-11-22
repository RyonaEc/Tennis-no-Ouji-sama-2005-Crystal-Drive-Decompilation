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
| Code | 107,676 B | 1,841,700 B | 5.85% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,539 | 3,806 | 40.44% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 5.85% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 124 B | 801,936 B | 0.02% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,541 | 2,138 | 72.08% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 22.55% (50,656 / 224,616 B) | 882 / 1,377 (64.1%) | 22.55% | 0.18% (124 / 69,856 B) | 884 / 1,104 |
| OV002 | 5.02% (35,944 / 715,724 B) | 391 / 1,360 (28.8%) | 5.02% | 0.00% (0 / 34,164 B) | 391 / 595 |
| OV003 | 1.37% (8,056 / 586,712 B) | 102 / 547 (18.6%) | 1.37% | 0.00% (0 / 64,648 B) | 102 / 172 |
| OV004 | 4.26% (3,812 / 89,584 B) | 59 / 170 (34.7%) | 4.26% | 0.00% (0 / 27,404 B) | 59 / 88 |
| OV005 | 3.14% (4,484 / 142,964 B) | 46 / 166 (27.7%) | 3.14% | 0.00% (0 / 369,132 B) | 46 / 78 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 14.41% (1,576 / 10,940 B) | 26 / 55 (47.3%) | 14.41% | 0.00% (0 / 480 B) | 26 / 44 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 2.30% (320 / 13,904 B) | 3 / 21 (14.3%) | 2.30% | 0.00% (0 / 7,872 B) | 3 / 7 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,539 of 3,806 functions (40.44%) and 107,676 of 1,841,700 code bytes (5.85%) are matching C; the rest are still delinked assembly.

124 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
