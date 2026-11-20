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
| Code | 84,224 B | 1,841,700 B | 4.57% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,326 | 3,806 | 34.84% `████████░░░░░░░░░░░░░░░░` |
| Fuzzy match | 4.57% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,326 | 1,895 | 69.97% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 18.54% (41,652 / 224,616 B) | 787 / 1,377 (57.2%) | 18.54% | 0.00% (0 / 69,856 B) | 787 / 1,022 |
| OV002 | 4.38% (31,380 / 715,724 B) | 342 / 1,360 (25.1%) | 4.38% | 0.00% (0 / 34,164 B) | 342 / 525 |
| OV003 | 0.69% (4,048 / 586,712 B) | 75 / 547 (13.7%) | 0.69% | 0.00% (0 / 64,648 B) | 75 / 135 |
| OV004 | 2.76% (2,472 / 89,584 B) | 47 / 170 (27.6%) | 2.76% | 0.00% (0 / 27,404 B) | 47 / 75 |
| OV005 | 1.65% (2,352 / 142,964 B) | 33 / 166 (19.9%) | 1.65% | 0.00% (0 / 369,132 B) | 33 / 60 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 8.48% (928 / 10,940 B) | 20 / 55 (36.4%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 3.58% (888 / 24,788 B) | 15 / 61 (24.6%) | 3.58% | 0.00% (0 / 9,972 B) | 15 / 29 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,326 of 3,806 functions (34.84%) and 84,224 of 1,841,700 code bytes (4.57%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
