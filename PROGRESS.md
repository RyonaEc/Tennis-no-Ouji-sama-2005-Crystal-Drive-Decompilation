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
| Code | 94,056 B | 1,841,700 B | 5.11% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,399 | 3,806 | 36.76% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 5.11% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,399 | 1,988 | 70.37% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 20.18% (45,324 / 224,616 B) | 822 / 1,377 (59.7%) | 20.18% | 0.00% (0 / 69,856 B) | 822 / 1,056 |
| OV002 | 4.56% (32,664 / 715,724 B) | 350 / 1,360 (25.7%) | 4.56% | 0.00% (0 / 34,164 B) | 350 / 539 |
| OV003 | 1.14% (6,668 / 586,712 B) | 92 / 547 (16.8%) | 1.14% | 0.00% (0 / 64,648 B) | 92 / 163 |
| OV004 | 3.26% (2,920 / 89,584 B) | 50 / 170 (29.4%) | 3.26% | 0.00% (0 / 27,404 B) | 50 / 79 |
| OV005 | 2.09% (2,988 / 142,964 B) | 36 / 166 (21.7%) | 2.09% | 0.00% (0 / 369,132 B) | 36 / 65 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 10.60% (1,160 / 10,940 B) | 22 / 55 (40.0%) | 10.59% | 0.00% (0 / 480 B) | 22 / 37 |
| OV009 | 6.84% (1,696 / 24,788 B) | 19 / 61 (31.1%) | 6.84% | 0.00% (0 / 9,972 B) | 19 / 34 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,399 of 3,806 functions (36.76%) and 94,056 of 1,841,700 code bytes (5.11%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
