[schematic2]
uniq 11
[tools]
[detail]
w -245 1115 100 0 n#1 hwout.hwout#17.outp -248 1112 -248 1112 eaos.dprAo.OUT
w -253 795 100 0 n#2 hwout.hwout#19.outp -256 792 -256 792 ebos.dprBo.OUT
w -221 451 100 0 n#3 hwout.hwout#21.outp -224 448 -224 448 elongouts.dprLo.OUT
w -197 59 100 0 n#4 hwout.hwout#23.outp -200 56 -200 56 embbos.pmac1mbbo.OUT
w -926 1179 100 0 n#5 hwinl.hwinl#33.in -929 1176 -929 1176 eais.dprAi.INP
w -950 843 100 0 n#6 hwinl.hwinl#35.in -953 840 -953 840 ebis.dprBi.INP
w -910 499 100 0 n#7 hwinl.hwinl#37.in -913 496 -913 496 elongins.dprLi.INP
w -902 83 100 0 n#8 hwinl.hwinl#39.in -905 80 -905 80 embbis.dprMbbi.INP
w -958 1571 100 0 n#9 hwinm.hwinm#40.in -961 1568 -961 1568 estringins.mbxSi.INP
w -260 1530 -100 0 n#10 hwoutm.hwoutm#41.outp -270 1520 -270 1520 estringouts.mbxSo.OUT
[cell use]
use eais -847 1075 100 0 dprAi
xform 0 -808 1144
p -934 1215 100 0 1 DTYP:PMAC-VME DPRAM
p -923 1252 100 0 1 SCAN:I/O Intr
p -931 1075 100 0 -1 PV:$(top)
use eaos -429 1060 100 0 dprAo
xform 0 -376 1144
p -507 1221 100 0 1 DTYP:PMAC-VME DPRAM
p -513 1060 100 0 -1 PV:$(top)
use ebis -867 736 100 0 dprBi
xform 0 -824 808
p -944 879 100 0 1 DTYP:PMAC-VME DPRAM
p -908 916 100 0 1 SCAN:I/O Intr
p -951 736 100 0 -1 PV:$(top)
use ebos -432 740 100 0 dprBo
xform 0 -384 824
p -505 894 100 0 1 DTYP:PMAC-VME DPRAM
p -516 740 100 0 -1 PV:$(top)
use elongins -824 392 100 0 dprLi
xform 0 -784 464
p -894 534 100 0 1 DTYP:PMAC-VME DPRAM
p -891 591 100 0 1 SCAN:I/O Intr
p -908 392 100 0 -1 PV:$(top)
use elongouts -396 395 100 0 dprLo
xform 0 -352 480
p -479 550 100 0 1 DTYP:PMAC-VME DPRAM
p -480 395 100 0 -1 PV:$(top)
use embbis -832 -28 100 0 dprMbbi
xform 0 -776 48
p -906 116 100 0 1 DTYP:PMAC-VME DPRAM
p -776 62 100 0 0 ONST:State 1
p -968 62 100 0 0 ONVL:1
p -776 30 100 0 0 TWST:State 2
p -968 30 100 0 0 TWVL:2
p -776 94 100 0 0 ZRST:State 0
p -968 94 100 0 0 ZRVL:0
p -85 -66 100 0 0 EIST:State 8
p -968 -162 100 0 0 EIVL:8
p -776 -34 100 0 0 FRST:State 4
p -968 -34 100 0 0 FRVL:4
p -776 -66 100 0 0 FVST:State 5
p -968 -66 100 0 0 FVVL:5
p -776 -194 100 0 0 NIST:State 9
p -968 -194 100 0 0 NIVL:9
p -776 -130 100 0 0 SVST:State 7
p -968 -130 100 0 0 SVVL:7
p -776 -98 100 0 0 SXST:State 6
p -968 -98 100 0 0 SXVL:6
p -776 -2 100 0 0 THST:State 3
p -968 -2 100 0 0 THVL:3
p -904 164 100 0 0 SCAN:I/O Intr
p -916 -28 100 0 -1 PV:$(top)
p -776 -258 100 0 0 ELST:State 11
p -968 -258 100 0 0 ELVL:11
p -1192 30 100 0 0 EVNT:State 15
p -968 -386 100 0 0 FFVL:15
p -776 -354 100 0 0 FTST:State 14
p -968 -354 100 0 0 FTVL:14
p -1192 -2 100 0 0 NOBT:4
p -776 -226 100 0 0 TEST:State 10
p -968 -226 100 0 0 TEVL:10
p -776 -322 100 0 0 TTST:State 13
p -968 -322 100 0 0 TTVL:13
p -776 -290 100 0 0 TVST:State 12
use embbos -392 -35 100 0 pmac1mbbo
xform 0 -328 56
p -438 138 100 0 1 DTYP:PMAC-VME DPRAM
p -296 118 100 0 0 ONST:One
p -488 118 100 0 0 ONVL:1
p -296 86 100 0 0 TWST:Two
p -488 86 100 0 0 TWVL:2
p -296 150 100 0 0 ZRST:Zero
p -488 150 100 0 0 ZRVL:0
p -296 22 100 0 0 FRST:Four
p -488 22 100 0 0 FRVL:4
p -296 -298 100 0 0 FTST:Fourteen
p -488 -298 100 0 0 FTVL:14
p -296 -10 100 0 0 FVST:Five
p -488 -10 100 0 0 FVVL:5
p -296 -42 100 0 0 SXST:Six
p -488 -42 100 0 0 SXVL:6
p -296 54 100 0 0 THST:Three
p -488 54 100 0 0 THVL:3
p -296 -106 100 0 0 EIST:Eight
p -488 -106 100 0 0 EIVL:8
p -296 -202 100 0 0 ELST:Eleven
p -488 -202 100 0 0 ELVL:11
p -296 -330 100 0 0 FFST:Fifteen
p -488 -330 100 0 0 FFVL:15
p -296 -138 100 0 0 NIST:Nine
p -488 -138 100 0 0 NIVL:9
p -712 54 100 0 0 NOBT:4
p -104 -74 100 0 0 SVST:Seven
p -488 -74 100 0 0 SVVL:7
p -296 -170 100 0 0 TEST:Ten
p -488 -170 100 0 0 TEVL:10
p -296 -266 100 0 0 TTST:Thirteen
p -488 -266 100 0 0 TTVL:13
p -296 -234 100 0 0 TVST:Twelve
p -488 -234 100 0 0 TVVL:12
use hwout 24 1137 100 0 hwout#17
xform 0 -152 1112
p -126 1104 100 0 -1 val(outp):#C$(card) S0 @F:$DFFE
use hwout -256 776 100 0 hwout#19
xform 0 -160 792
p -160 783 100 0 -1 val(outp):#C$(card) S0 @DP:$DFFC
use hwout -224 432 100 0 hwout#21
xform 0 -128 448
p -101 439 100 0 -1 val(outp):#C$(card) S0 @DP:$DFFA
use hwout -200 40 100 0 hwout#23
xform 0 -104 56
p -81 50 100 0 -1 val(outp):#C$(card) S0 @DP:$DFF8
use estringouts -465 1465 100 0 mbxSo
xform 0 -400 1536
p -485 1595 100 0 1 DTYP:PMAC-VME ASCII
p -549 1465 100 0 -1 PV:$(top)
use estringins -832 1462 100 0 mbxSi
xform 0 -832 1536
p -947 1604 100 0 1 DTYP:PMAC-VME ASCII
p -916 1462 100 0 -1 PV:$(top)
use testPmacStatusRec -784 1784 100 0 testPmacStatusRec#32
xform 0 -656 1856
use hwinl -1320 1104 100 0 hwinl#33
xform 0 -1120 1152
p -1302 1168 100 0 -1 val(in):#C$(card) S0 @F:$DFFE
use hwinl -1344 768 100 0 hwinl#35
xform 0 -1144 816
p -1326 832 100 0 -1 val(in):#C$(card) S0 @F:$DFFC
use hwinl -1304 424 100 0 hwinl#37
xform 0 -1104 472
p -1286 488 100 0 -1 val(in):#C$(card) S0 @F:$DFFA
use hwinl -1296 8 100 0 hwinl#39
xform 0 -1096 56
p -1278 72 100 0 -1 val(in):#C$(card) S0 @F:$DFF8
use hwinm -1256 1496 100 0 hwinm#40
xform 0 -1104 1544
p -1246 1560 100 0 -1 val(in):#C$(card) S2
use hwoutm -280 1448 100 0 hwoutm#41
xform 0 -128 1496
p -174 1511 100 0 -1 val(outp):#C$(card) S2
[comments]
