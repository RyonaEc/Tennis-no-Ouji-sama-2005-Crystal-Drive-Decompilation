# Progress

Recompiling the matching C sources with `mwccarm` 1.2/sp2 and linking them back into the original addresses reproduces the retail ROM:

```
Target: 8d555a80db47278475ffdfdf808fe80dbd7c48cf
rebuilt: 8d555a80db47278475ffdfdf808fe80dbd7c48cf  -> OK
```

The numbers below come from `ninja report` (objdiff) and are written by `python build/progress.py`. `Complete` means a unit whose whole body is matching C; gap units are still delinked assembly.

## Overall

| Metric | Matched | Total | Progress |
|---|---:|---:|---|
| Code | 29,480 B | 1,841,700 B | 1.60% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 757 | 3,806 | 19.89% `█████░░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.60% | — | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 757 | 1,152 | 65.71% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 7.97% (17,904 / 224,616 B) | 511 / 1,377 (37.1%) | 7.97% | 0.00% (0 / 69,856 B) | 511 / 723 |
| OV002 | 1.38% (9,892 / 715,724 B) | 165 / 1,360 (12.1%) | 1.38% | 0.00% (0 / 34,164 B) | 165 / 275 |
| OV003 | 0.16% (928 / 586,712 B) | 46 / 547 (8.4%) | 0.16% | 0.00% (0 / 64,648 B) | 46 / 85 |
| OV004 | 0.21% (184 / 89,584 B) | 9 / 170 (5.3%) | 0.21% | 0.00% (0 / 27,404 B) | 9 / 18 |
| OV005 | 0.22% (312 / 142,964 B) | 19 / 166 (11.4%) | 0.22% | 0.00% (0 / 369,132 B) | 19 / 35 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 1.83% (200 / 10,940 B) | 5 / 55 (9.1%) | 1.83% | 0.00% (0 / 480 B) | 5 / 8 |
| OV009 | 0.24% (60 / 24,788 B) | 2 / 61 (3.3%) | 0.24% | 0.00% (0 / 9,972 B) | 2 / 5 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 0.00% (0 / 25,144 B) | 0 / 33 (0.0%) | 0.00% | 0.00% (0 / 47,904 B) | 0 / 1 |

## Against the whole ROM

757 of 3,806 functions (19.89%) and 29,480 of 1,841,700 code bytes (1.60%) are matching C; the remaining functions are still delinked assembly.

Data is reported but not yet started: no `.data`, `.rodata` or `.bss` unit has a matching C definition, so the data figures stay at zero until those units are claimed.
