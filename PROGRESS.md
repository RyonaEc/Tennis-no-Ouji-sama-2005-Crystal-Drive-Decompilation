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
| Code | 88,308 B | 1,841,700 B | 4.79% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,363 | 3,806 | 35.81% `█████████░░░░░░░░░░░░░░░` |
| Fuzzy match | 4.79% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,363 | 1,943 | 70.15% `█████████████████░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 18.80% (42,232 / 224,616 B) | 799 / 1,377 (58.0%) | 18.80% | 0.00% (0 / 69,856 B) | 799 / 1,035 |
| OV002 | 4.40% (31,460 / 715,724 B) | 343 / 1,360 (25.2%) | 4.40% | 0.00% (0 / 34,164 B) | 343 / 527 |
| OV003 | 1.01% (5,912 / 586,712 B) | 88 / 547 (16.1%) | 1.01% | 0.00% (0 / 64,648 B) | 88 / 155 |
| OV004 | 3.26% (2,920 / 89,584 B) | 50 / 170 (29.4%) | 3.26% | 0.00% (0 / 27,404 B) | 50 / 79 |
| OV005 | 1.85% (2,640 / 142,964 B) | 35 / 166 (21.1%) | 1.85% | 0.00% (0 / 369,132 B) | 35 / 63 |
| OV006 | 0.71% (52 / 7,324 B) | 1 / 16 (6.2%) | 0.71% | 0.00% (0 / 170,504 B) | 1 / 3 |
| OV008 | 10.60% (1,160 / 10,940 B) | 22 / 55 (40.0%) | 10.59% | 0.00% (0 / 480 B) | 22 / 37 |
| OV009 | 5.44% (1,348 / 24,788 B) | 18 / 61 (29.5%) | 5.44% | 0.00% (0 / 9,972 B) | 18 / 32 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 2.32% (584 / 25,144 B) | 7 / 33 (21.2%) | 2.32% | 0.00% (0 / 47,904 B) | 7 / 11 |

## Against the whole ROM

1,363 of 3,806 functions (35.81%) and 88,308 of 1,841,700 code bytes (4.79%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
