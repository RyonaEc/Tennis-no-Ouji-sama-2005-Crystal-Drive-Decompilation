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
| Code | 104,248 B | 1,841,700 B | 5.66% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,505 | 3,806 | 39.54% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 5.66% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 124 B | 801,936 B | 0.02% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,507 | 2,095 | 71.93% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 22.15% (49,760 / 224,616 B) | 872 / 1,377 (63.3%) | 22.15% | 0.18% (124 / 69,856 B) | 874 / 1,096 |
| OV002 | 4.81% (34,400 / 715,724 B) | 376 / 1,360 (27.6%) | 4.81% | 0.00% (0 / 34,164 B) | 376 / 574 |
| OV003 | 1.34% (7,852 / 586,712 B) | 100 / 547 (18.3%) | 1.34% | 0.00% (0 / 64,648 B) | 100 / 169 |
| OV004 | 4.14% (3,708 / 89,584 B) | 58 / 170 (34.1%) | 4.14% | 0.00% (0 / 27,404 B) | 58 / 88 |
| OV005 | 3.06% (4,380 / 142,964 B) | 45 / 166 (27.1%) | 3.06% | 0.00% (0 / 369,132 B) | 45 / 76 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 11.33% (1,240 / 10,940 B) | 23 / 55 (41.8%) | 11.33% | 0.00% (0 / 480 B) | 23 / 39 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 0.58% (80 / 13,904 B) | 1 / 21 (4.8%) | 0.58% | 0.00% (0 / 7,872 B) | 1 / 3 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,505 of 3,806 functions (39.54%) and 104,248 of 1,841,700 code bytes (5.66%) are matching C; the rest are still delinked assembly.

124 bytes of `.data` are matching C definitions; the rest of the data sections are still raw bytes in the delinked image.
