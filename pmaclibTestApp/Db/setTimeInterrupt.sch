[schematic2]
uniq 62
[tools]
[detail]
w 1944 1451 100 0 n#58 eaos.eaos#0.FLNK 1856 1440 2080 1440 2080 1792 junction
w 2136 1803 100 0 n#58 efanouts.efanouts#55.LNK3 1760 2656 1888 2656 1888 1792 2432 1792 esubs.esubs#27.SLNK
w 1464 1419 100 0 c#61 inhier.SLNK.P 1376 1408 1600 1408 eaos.eaos#0.SLNK
w 2152 2411 100 0 n#57 efanouts.efanouts#55.LNK2 1760 2688 1920 2688 1920 2400 2432 2400 estringouts.estringouts#42.SLNK
w 2072 2731 100 0 n#56 efanouts.efanouts#55.LNK1 1760 2720 2432 2720 estringouts.estringouts#37.SLNK
w 2312 2763 100 0 n#48 hwin.hwin#38.in 2160 2576 2240 2576 2240 2752 2432 2752 estringouts.estringouts#37.DOL
w 2312 2443 100 0 n#48 junction 2240 2576 2240 2432 2432 2432 estringouts.estringouts#42.DOL
w 2760 2395 100 0 n#44 estringouts.estringouts#42.OUT 2688 2384 2880 2384 hwout.hwout#43.outp
w 2760 2715 100 0 n#41 estringouts.estringouts#37.OUT 2688 2704 2880 2704 hwout.hwout#40.outp
w 1374 1547 100 0 c#5 inhier.TIME.P 1312 1536 1472 1536 1472 1440 1600 1440 eaos.eaos#0.DOL
w 1918 1387 100 0 n#2 eaos.eaos#0.OUT 1856 1376 2016 1376 hwout.hwout#1.outp
[cell use]
use inhier 1296 1495 100 0 TIME
xform 0 1312 1536
use inhier 1360 1367 100 0 SLNK
xform 0 1376 1408
use efanouts 1520 2503 100 0 efanouts#55
xform 0 1640 2656
p 1616 2816 100 0 1 SELM:All
p 1632 2496 100 1024 -1 name:$(top)TimeInterruptFO
p 1792 2720 75 1280 -1 pproc(LNK1):PP
p 1792 2688 75 1280 -1 pproc(LNK2):PP
p 1792 2656 75 1280 -1 pproc(LNK3):PP
use hwin 1968 2535 100 0 hwin#38
xform 0 2064 2576
p 1968 2608 100 0 -1 val(in):$(top)PmacCmd_Run
use hwout 2880 2663 100 0 hwout#40
xform 0 2976 2704
p 2976 2695 100 0 -1 val(outp):#C0 S1
use hwout 2016 1335 100 0 hwout#1
xform 0 2112 1376
p 2112 1367 100 0 -1 val(outp):#C0 S3
use hwout 2880 2343 100 0 hwout#43
xform 0 2976 2384
p 2976 2375 100 0 -1 val(outp):#C1 S1
use estringouts 2432 2647 100 0 estringouts#37
xform 0 2560 2720
p 2496 2832 100 0 1 DTYP:PMAC-VME ASCII
p 2496 2800 100 0 1 OMSL:closed_loop
p 2368 2686 100 0 0 VAL:
p 2544 2640 100 1024 -1 name:$(top)azPMAC_RunS
use estringouts 2432 2327 100 0 estringouts#42
xform 0 2560 2400
p 2496 2512 100 0 1 DTYP:PMAC-VME ASCII
p 2496 2480 100 0 1 OMSL:closed_loop
p 2368 2366 100 0 0 VAL:
p 2544 2320 100 1024 -1 name:$(top)elPMAC_RunS
use esubs 2432 1703 100 0 esubs#27
xform 0 2576 1968
p 2144 1822 100 0 0 INAM:nullSub
p 2480 2224 100 0 1 SNAM:readTime
p 2544 1696 100 1024 -1 name:$(top)readTime
use eaos 1600 1319 100 0 eaos#0
xform 0 1728 1408
p 1664 1504 100 0 1 DTYP:Bancomm 635
p 1664 1536 100 0 1 OMSL:closed_loop
p 1664 1568 100 0 1 PREC:12
p 1712 1312 100 1024 -1 name:$(top)TODinterrupt
use bc200tr 1072 888 -100 0 frame
xform 0 2752 2192
[comments]
