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
| Code | 31,924 B | 1,841,700 B | 1.73% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 821 | 3,806 | 21.57% `█████░░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.76% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 831 | 1,274 | 65.23% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 8.42% (18,908 / 224,616 B) | 538 / 1,377 (39.1%) | 8.44% | 0.00% (0 / 69,856 B) | 539 / 767 |
| OV002 | 1.41% (10,112 / 715,724 B) | 168 / 1,360 (12.4%) | 1.42% | 0.00% (0 / 34,164 B) | 169 / 282 |
| OV003 | 0.21% (1,216 / 586,712 B) | 53 / 547 (9.7%) | 0.21% | 0.00% (0 / 64,648 B) | 54 / 99 |
| OV004 | 0.75% (668 / 89,584 B) | 22 / 170 (12.9%) | 0.75% | 0.00% (0 / 27,404 B) | 22 / 36 |
| OV005 | 0.37% (536 / 142,964 B) | 25 / 166 (15.1%) | 0.41% | 0.00% (0 / 369,132 B) | 26 / 48 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 1.94% (212 / 10,940 B) | 6 / 55 (10.9%) | 3.50% | 0.00% (0 / 480 B) | 11 / 20 |
| OV009 | 1.10% (272 / 24,788 B) | 9 / 61 (14.8%) | 1.32% | 0.00% (0 / 9,972 B) | 10 / 19 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 0.00% (0 / 25,144 B) | 0 / 33 (0.0%) | 0.00% | 0.00% (0 / 47,904 B) | 0 / 1 |

## Against the whole ROM

821 of 3,806 functions (21.57%) and 31,924 of 1,841,700 code bytes (1.73%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
