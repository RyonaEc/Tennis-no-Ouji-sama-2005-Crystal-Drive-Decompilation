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
| Code | 85,280 B | 1,841,700 B | 4.63% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,334 | 3,806 | 35.05% `████████░░░░░░░░░░░░░░░░` |
| Fuzzy match | 4.63% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,334 | 1,906 | 69.99% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 18.54% (41,652 / 224,616 B) | 787 / 1,377 (57.2%) | 18.54% | 0.00% (0 / 69,856 B) | 787 / 1,022 |
| OV002 | 4.38% (31,380 / 715,724 B) | 342 / 1,360 (25.1%) | 4.38% | 0.00% (0 / 34,164 B) | 342 / 525 |
| OV003 | 0.82% (4,816 / 586,712 B) | 81 / 547 (14.8%) | 0.82% | 0.00% (0 / 64,648 B) | 81 / 144 |
| OV004 | 3.08% (2,760 / 89,584 B) | 49 / 170 (28.8%) | 3.08% | 0.00% (0 / 27,404 B) | 49 / 77 |
| OV005 | 1.65% (2,352 / 142,964 B) | 33 / 166 (19.9%) | 1.65% | 0.00% (0 / 369,132 B) | 33 / 60 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 8.48% (928 / 10,940 B) | 20 / 55 (36.4%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 3.58% (888 / 24,788 B) | 15 / 61 (24.6%) | 3.58% | 0.00% (0 / 9,972 B) | 15 / 29 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,334 of 3,806 functions (35.05%) and 85,280 of 1,841,700 code bytes (4.63%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
