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
| Code | 66,668 B | 1,841,700 B | 3.62% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 1,144 | 3,806 | 30.06% `███████░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 3.65% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 1,156 | 1,716 | 67.37% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 11.83% (26,576 / 224,616 B) | 637 / 1,377 (46.3%) | 11.88% | 0.00% (0 / 69,856 B) | 640 / 878 |
| OV002 | 4.33% (30,968 / 715,724 B) | 337 / 1,360 (24.8%) | 4.34% | 0.00% (0 / 34,164 B) | 338 / 519 |
| OV003 | 0.52% (3,076 / 586,712 B) | 65 / 547 (11.9%) | 0.53% | 0.00% (0 / 64,648 B) | 66 / 123 |
| OV004 | 2.16% (1,932 / 89,584 B) | 41 / 170 (24.1%) | 2.16% | 0.00% (0 / 27,404 B) | 41 / 65 |
| OV005 | 1.54% (2,200 / 142,964 B) | 31 / 166 (18.7%) | 1.58% | 0.00% (0 / 369,132 B) | 32 / 59 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 6.91% (756 / 10,940 B) | 15 / 55 (27.3%) | 8.47% | 0.00% (0 / 480 B) | 20 / 35 |
| OV009 | 2.86% (708 / 24,788 B) | 12 / 61 (19.7%) | 3.08% | 0.00% (0 / 9,972 B) | 13 / 25 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

1,144 of 3,806 functions (30.06%) and 66,668 of 1,841,700 code bytes (3.62%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
