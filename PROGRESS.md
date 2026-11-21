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
| Code | 103,192 B | 1,841,700 B | 5.60% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,495 | 3,806 | 39.28% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 5.60% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,495 | 2,082 | 71.81% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 21.81% (48,984 / 224,616 B) | 864 / 1,377 (62.7%) | 21.81% | 0.00% (0 / 69,856 B) | 864 / 1,087 |
| OV002 | 4.81% (34,400 / 715,724 B) | 376 / 1,360 (27.6%) | 4.81% | 0.00% (0 / 34,164 B) | 376 / 574 |
| OV003 | 1.34% (7,852 / 586,712 B) | 100 / 547 (18.3%) | 1.34% | 0.00% (0 / 64,648 B) | 100 / 169 |
| OV004 | 4.14% (3,708 / 89,584 B) | 58 / 170 (34.1%) | 4.14% | 0.00% (0 / 27,404 B) | 58 / 88 |
| OV005 | 2.87% (4,100 / 142,964 B) | 43 / 166 (25.9%) | 2.87% | 0.00% (0 / 369,132 B) | 43 / 72 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 11.33% (1,240 / 10,940 B) | 23 / 55 (41.8%) | 11.32% | 0.00% (0 / 480 B) | 23 / 39 |
| OV009 | 8.84% (2,192 / 24,788 B) | 22 / 61 (36.1%) | 8.84% | 0.00% (0 / 9,972 B) | 22 / 36 |
| OV001 | 0.58% (80 / 13,904 B) | 1 / 21 (4.8%) | 0.58% | 0.00% (0 / 7,872 B) | 1 / 3 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,495 of 3,806 functions (39.28%) and 103,192 of 1,841,700 code bytes (5.60%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
