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
| Code | 36,224 B | 1,841,700 B | 1.97% `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Functions | 880 | 3,806 | 23.12% `██████░░░░░░░░░░░░░░░░░░` |
| Fuzzy match | 1.99% | n/a | `█░░░░░░░░░░░░░░░░░░░░░░░` |
| Data | 0 B | 801,936 B | 0.00% `░░░░░░░░░░░░░░░░░░░░░░░░` |
| Complete units | 890 | 1,359 | 65.49% `████████████████░░░░░░░░` |

## By module

| Module | Code | Functions | Fuzzy | Data | Complete units |
|---|---:|---:|---:|---:|---:|
| ARM9 main | 9.05% (20,324 / 224,616 B) | 562 / 1,377 (40.8%) | 9.07% | 0.00% (0 / 69,856 B) | 563 / 795 |
| OV002 | 1.65% (11,804 / 715,724 B) | 184 / 1,360 (13.5%) | 1.66% | 0.00% (0 / 34,164 B) | 185 / 306 |
| OV003 | 0.22% (1,304 / 586,712 B) | 55 / 547 (10.1%) | 0.23% | 0.00% (0 / 64,648 B) | 56 / 103 |
| OV004 | 1.42% (1,272 / 89,584 B) | 32 / 170 (18.8%) | 1.42% | 0.00% (0 / 27,404 B) | 32 / 54 |
| OV005 | 0.37% (536 / 142,964 B) | 25 / 166 (15.1%) | 0.41% | 0.00% (0 / 369,132 B) | 26 / 48 |
| OV006 | 0.00% (0 / 7,324 B) | 0 / 16 (0.0%) | 0.00% | 0.00% (0 / 170,504 B) | 0 / 1 |
| OV008 | 2.38% (260 / 10,940 B) | 7 / 55 (12.7%) | 3.94% | 0.00% (0 / 480 B) | 12 / 22 |
| OV009 | 1.10% (272 / 24,788 B) | 9 / 61 (14.8%) | 1.32% | 0.00% (0 / 9,972 B) | 10 / 19 |
| OV001 | 0.00% (0 / 13,904 B) | 0 / 21 (0.0%) | 0.00% | 0.00% (0 / 7,872 B) | 0 / 1 |
| OV010 | 1.80% (452 / 25,144 B) | 6 / 33 (18.2%) | 1.80% | 0.00% (0 / 47,904 B) | 6 / 10 |

## Against the whole ROM

880 of 3,806 functions (23.12%) and 36,224 of 1,841,700 code bytes (1.97%) are matching C; the rest are still delinked assembly.

No `.data`, `.rodata` or `.bss` unit has a matching C definition yet, so the data figures stay at zero.
