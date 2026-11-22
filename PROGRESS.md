# Progress

The matching C sources are compiled with `mwccarm` 1.2/sp2 and linked back into the original addresses. The rebuild is byte-identical to the retail ROM:

```
Target: 8d555a80db47278475ffdfdf808fe80dbd7c48cf
rebuilt: 8d555a80db47278475ffdfdf808fe80dbd7c48cf  -> OK
```

The figures come from `ninja report` (objdiff). A complete unit has its whole body in matching C; gap units are still delinked assembly.

## Overall

| Metric | Matched | Total | Progress |
|---|---:|---:|---|
| Code | 105,068 B | 1,841,700 B | 5.70% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,513 | 3,806 | 39.75% `██████████░░░░░░░░░░░░░░` |
| Fuzzy match | 5.70% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 124 B | 801,936 B | 0.02% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,515 | 2,107 | 71.90% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 22.15% (49,760 / 224,616 B) | 872 / 1,377 (63.3%) | 22.15% | 0.18% (124 / 69,856 B) | 874 / 1,096 |
| OV002 | 4.83% (34,592 / 715,724 B) | 378 / 1,360 (27.8%) | 4.83% | 0.00% (0 / 34,164 B) | 378 / 576 |
| OV003 | 1.36% (7,956 / 586,712 B) | 101 / 547 (18.5%) | 1.36% | 0.00% (0 / 64,648 B) | 101 / 171 |
| OV004 | 4.26% (3,812 / 89,584 B) | 59 / 170 (34.7%) | 4.26% | 0.00% (0 / 27,404 B) | 59 / 88 |
| OV005 | 3.14% (4,484 / 142,964 B) | 46 / 166 (27.7%) | 3.14% | 0.00% (0 / 369,132 B) | 46 / 78 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 12.03% (1,316 / 10,940 B) | 24 / 55 (43.6%) | 12.03% | 0.00% (0 / 480 B) | 24 / 41 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 2.30% (320 / 13,904 B) | 3 / 21 (14.3%) | 2.30% | 0.00% (0 / 7,872 B) | 3 / 7 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,513 of 3,806 functions (39.75%) and 105,068 of 1,841,700 code bytes (5.70%) are matching C; the rest are still delinked assembly.

124 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
