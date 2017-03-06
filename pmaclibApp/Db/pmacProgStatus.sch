[schematic2]
uniq 22
[tools]
[detail]
w 2096 355 100 0 n#1 hwin.hwin#99.in 2048 352 2144 352 2144 720 2192 720 egenSub.pmacProgStatusEnd.INPI
w 1232 899 100 0 n#2 egenSub.pmacProgStatusStart.VALE 1120 896 1344 896 1344 608 1888 608 1888 784 2192 784 egenSub.pmacProgStatusEnd.INPH
w 1216 963 100 0 n#3 egenSub.pmacProgStatusStart.VALD 1120 960 1312 960 1312 576 1920 576 1920 848 2192 848 egenSub.pmacProgStatusEnd.INPG
w 1280 1091 100 0 n#4 egenSub.pmacProgStatusStart.VALB 1120 1088 1440 1088 eloads.pmacProgStatusload.UPL
w 1283 816 100 0 n#4 junction 1280 1088 1280 544 1952 544 1952 912 2192 912 egenSub.pmacProgStatusEnd.INPF
w 656 883 100 0 n#5 hwin.hwin#231.in 608 880 704 880 704 992 832 992 egenSub.pmacProgStatusStart.INPC
w 1888 819 100 0 n#6 eloads.pmacProgStatusload.FLNK 1664 816 2112 816 2112 560 2192 560 egenSub.pmacProgStatusEnd.SLNK
w 1251 832 100 0 n#7 junction 1248 1152 1248 512 1984 512 1984 976 2192 976 egenSub.pmacProgStatusEnd.INPE
w 1280 1155 100 0 n#7 egenSub.pmacProgStatusStart.VALA 1120 1152 1440 1152 eloads.pmacProgStatusload.DNL
w 1248 419 100 0 n#8 egenSub.pmacProgStatusStart.FLNK 1120 416 1376 416 1376 768 1440 768 eloads.pmacProgStatusload.SLNK
w 2536 1107 100 0 n#9 egenSub.pmacProgStatusEnd.OUTC 2480 1104 2592 1104 hwout.hwout#165.outp
w 2536 1171 100 0 n#10 egenSub.pmacProgStatusEnd.OUTB 2480 1168 2592 1168 hwout.hwout#131.outp
w 2536 1235 100 0 n#11 egenSub.pmacProgStatusEnd.OUTA 2480 1232 2592 1232 hwout.hwout#139.outp
w 1888 1075 100 0 n#12 eloads.pmacProgStatusload.MSG 1664 1072 2112 1072 2112 1040 2192 1040 egenSub.pmacProgStatusEnd.INPD
w 1928 1107 100 0 n#13 eloads.pmacProgStatusload.VAL 1664 1104 2192 1104 egenSub.pmacProgStatusEnd.INPC
w 1168 1027 100 0 n#14 egenSub.pmacProgStatusStart.VALC 1120 1024 1216 1024 1216 480 2016 480 2016 1168 2192 1168 egenSub.pmacProgStatusEnd.INPB
w 1584 451 100 0 n#15 egenSub.pmacProgStatusStart.VAL 1120 448 2048 448 2048 1232 2192 1232 egenSub.pmacProgStatusEnd.INPA
w 1704 1043 100 0 n#16 eloads.pmacProgStatusload.OUT 1664 1040 1744 1040 hwout.hwout#87.outp
w 720 1059 100 0 n#17 ebis.pmacProgStatusBi.VAL 608 1056 832 1056 egenSub.pmacProgStatusStart.INPB
w 1288 2139 100 0 n#18 eseqs.pmacProgStatusSeq.LNK2 1264 2144 1312 2144 1312 1312 256 1312 256 448 832 448 egenSub.pmacProgStatusStart.SLNK
w 1328 2171 100 0 n#19 eseqs.pmacProgStatusSeq.LNK1 1264 2176 1536 2176 eaos.pmacProgStatusAo.SLNK
w 808 2179 100 0 n#20 hwin.hwin#112.in 768 2176 944 2176 eseqs.pmacProgStatusSeq.DOL1
w 1880 2147 100 0 n#21 eaos.pmacProgStatusAo.OUT 1792 2144 1968 2144 hwout.hwout#157.outp
[cell use]
use bc200tr -96 -104 -100 0 frame
xform 0 1584 1200
use hwin 1880 312 100 0 hwin#99
xform 0 1952 352
p 1859 344 100 0 -1 val(in):$(card)
use hwin 600 2136 100 0 hwin#112
xform 0 672 2176
p 579 2168 100 0 -1 val(in):$(BUSY)
use hwin 440 840 100 0 hwin#231
xform 0 512 880
p 384 830 100 0 -1 val(in):$(top)savePath
use egenSub 2351 467 100 0 pmacProgStatusEnd
xform 0 2336 896
p 2288 1246 100 0 1 FTA:LONG
p 2304 464 100 1024 0 name:
p 2226 1323 100 0 1 SNAM:ProgStatusEnd
p 2272 878 100 0 1 FTVA:STRING
p 2272 846 100 0 1 FTVB:LONG
p 2272 814 100 0 1 FTVC:STRING
p 2288 1214 100 0 1 FTB:STRING
p 2288 1182 100 0 1 FTC:LONG
p 2288 1150 100 0 1 FTD:STRING
p 2288 1118 100 0 1 FTE:STRING
p 2288 1086 100 0 1 FTF:STRING
p 2288 1054 100 0 1 FTG:STRING
p 2288 1022 100 0 1 FTH:STRING
p 2272 782 100 0 1 FTVD:STRING
p 2288 990 100 0 1 FTI:DOUBLE
p 2233 466 100 0 -1 PV:$(top)$(axis)
use egenSub 969 357 100 0 pmacProgStatusStart
xform 0 976 784
p 912 766 100 0 1 FTVA:STRING
p 912 734 100 0 1 FTVB:STRING
p 944 352 100 1024 0 name:
p 864 1200 100 0 1 SNAM:ProgStatusStart
p 912 574 100 0 0 FTJ:DOUBLE
p 912 702 100 0 1 FTVC:STRING
p 912 1118 100 0 1 FTB:STRING
p 912 318 100 0 0 SCAN:Passive
p 912 286 100 0 0 EVNT:0
p 912 1150 100 0 0 FTA:DOUBLE
p 912 670 100 0 1 FTVD:STRING
p 912 1086 100 0 1 FTC:STRING
p 912 1054 100 0 0 FTD:DOUBLE
p 912 1022 100 0 0 FTE:DOUBLE
p 912 638 100 0 1 FTVE:STRING
p 912 990 100 0 0 FTF:DOUBLE
p 846 356 100 0 -1 PV:$(top)$(axis)
use hwout 2616 1064 100 0 hwout#165
xform 0 2688 1104
p 2800 1102 100 0 -1 val(outp):$(top)$(axis)pmacSir .PP
use hwout 2616 1192 100 0 hwout#139
xform 0 2688 1232
p 2800 1230 100 0 -1 val(outp):$(top)$(axis)pmacC.IMSS
use hwout 2616 1128 100 0 hwout#131
xform 0 2688 1168
p 2800 1166 100 0 -1 val(outp):$(top)$(axis)pmacC.IVAL .PP
use hwout 1768 1000 100 0 hwout#87
xform 0 1840 1040
p 1744 990 100 0 -1 val(outp):#C$(card) S0 @
use hwout 1992 2104 100 0 hwout#157
xform 0 2064 2144
p 1888 2094 100 0 -1 val(outp):$(top)$(axis)pmacC.IVAL .PP
use eseqs 1101 1761 100 0 pmacProgStatusSeq
xform 0 1104 2016
p 1056 1760 100 1024 0 name:
p 1056 2238 100 0 1 DLY2:0.25
p 1280 2176 75 1024 -1 pproc(LNK1):PP
p 1280 2144 75 1024 -1 pproc(LNK2):PP
p 1280 2112 75 1024 -1 pproc(LNK3):NPP
p 1056 2206 100 0 1 SELM:All
p 975 1760 100 0 -1 PV:$(top)$(axis)
use eaos 1656 2080 100 0 pmacProgStatusAo
xform 0 1664 2176
p 1280 2158 100 0 0 OMSL:supervisory
p 1648 2080 100 1024 0 name:
p 1536 2079 100 0 -1 PV:$(top)$(axis)
use ebis 456 1006 100 0 pmacProgStatusBi
xform 0 480 1072
p 464 992 100 1024 0 name:
p 432 926 100 0 1 ONAM:MOTION
p 432 958 100 0 1 ZNAM:PLC
p 339 1006 100 0 -1 PV:$(top)$(axis)
use eloads 1583 673 100 0 pmacProgStatusload
xform 0 1552 960
p 1440 1230 100 0 1 DTYP:PMAC-VME ASCII
p 1544 672 100 1024 0 name:
p 1472 638 100 0 0 SCAN:Passive
p 1472 606 100 0 0 EVNT:0
p 1458 673 100 0 -1 PV:$(top)$(axis)
[comments]
