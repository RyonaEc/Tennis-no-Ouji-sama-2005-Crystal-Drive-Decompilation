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
| Code | 29,912 B | 1,841,700 B | 1.62% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 781 | 3,806 | 20.52% `█████░░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.63% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 783 | 1,197 | 65.41% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 7.99% (17,936 / 224,616 B) | 519 / 1,377 (37.7%) | 7.99% | 0.00% (0 / 69,856 B) | 519 / 739 |
| OV002 | 1.38% (9,892 / 715,724 B) | 165 / 1,360 (12.1%) | 1.38% | 0.00% (0 / 34,164 B) | 165 / 275 |
| OV003 | 0.16% (928 / 586,712 B) | 46 / 547 (8.4%) | 0.16% | 0.00% (0 / 64,648 B) | 46 / 85 |
| OV004 | 0.48% (432 / 89,584 B) | 17 / 170 (10.0%) | 0.48% | 0.00% (0 / 27,404 B) | 17 / 29 |
| OV005 | 0.27% (388 / 142,964 B) | 22 / 166 (13.3%) | 0.27% | 0.00% (0 / 369,132 B) | 22 / 40 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 1.94% (212 / 10,940 B) | 6 / 55 (10.9%) | 2.45% | 0.00% (0 / 480 B) | 8 / 14 |
| OV009 | 0.50% (124 / 24,788 B) | 6 / 61 (9.8%) | 0.50% | 0.00% (0 / 9,972 B) | 6 / 12 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 0.00% (0 / 25,144 B) | 0 / 33 (0.0%) | 0.00% | 0.00% (0 / 47,904 B) | 0 / 1 |

## Against the whole ROM

781 of 3,806 functions (20.52%) and 29,912 of 1,841,700 code bytes (1.62%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
