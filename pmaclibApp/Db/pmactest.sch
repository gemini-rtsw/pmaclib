[schematic2]
uniq 11
[tools]
[detail]
w -933 1179 100 0 n#1 hwin.hwin#10.in -936 1176 -936 1176 eais.dprAi.INP
w -245 1115 100 0 n#2 hwout.hwout#17.outp -248 1112 -248 1112 eaos.dprAo.OUT
w -949 843 100 0 n#3 hwin.hwin#12.in -952 840 -952 840 ebis.dprBi.INP
w -253 795 100 0 n#4 hwout.hwout#19.outp -256 792 -256 792 ebos.dprBo.OUT
w -909 499 100 0 n#5 hwin.hwin#14.in -912 496 -912 496 elongins.dprLi.INP
w -221 451 100 0 n#6 hwout.hwout#21.outp -224 448 -224 448 elongouts.dprLo.OUT
w -197 59 100 0 n#7 hwout.hwout#23.outp -200 56 -200 56 embbos.pmac1mbbo.OUT
w -149 1723 100 0 n#8 hwin.hwin#28.in -152 1720 -152 1720 estringins.mbxSi.INP
w -765 1683 100 0 n#9 hwout.hwout#31.outp -768 1680 -768 1680 estringouts.mbxSo.OUT
w -901 83 100 0 n#10 embbis.dprMbbi.INP -904 80 -904 80 hwin.hwin#16.in
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
use hwin -1128 1160 100 0 hwin#10
xform 0 -1032 1176
p -1196 1169 100 0 -1 val(in):#C0 S0 @F:$DFFE
use hwin -1144 824 100 0 hwin#12
xform 0 -1048 840
p -1214 833 100 0 -1 val(in):#C0 S0 @DP:$DFFC
use hwin -1104 480 100 0 hwin#14
xform 0 -1008 496
p -1172 489 100 0 -1 val(in):#C0 S0 @DP:$DFFA
use hwin -1096 64 100 0 hwin#16
xform 0 -1000 80
p -1165 74 100 0 -1 val(in):#C0 S0 @DP:$DFF8
use hwout 24 1137 100 0 hwout#17
xform 0 -152 1112
p -126 1104 100 0 -1 val(outp):#C0 S0 @F:$DFFE
use hwout -256 776 100 0 hwout#19
xform 0 -160 792
p -160 783 100 0 -1 val(outp):#C0 S0 @DP:$DFFC
use hwout -224 432 100 0 hwout#21
xform 0 -128 448
p -101 439 100 0 -1 val(outp):#C0 S0 @DP:$DFFA
use hwout -200 40 100 0 hwout#23
xform 0 -104 56
p -81 50 100 0 -1 val(outp):#C0 S0 @DP:$DFF8
use estringouts -961 1625 100 0 mbxSo
xform 0 -896 1696
p -981 1755 100 0 1 DTYP:PMAC-VME ASCII
p -1045 1625 100 0 -1 PV:$(top)
use estringins -24 1614 100 0 mbxSi
xform 0 -24 1688
p -139 1756 100 0 1 DTYP:PMAC-VME ASCII
p -108 1614 100 0 -1 PV:$(top)
use hwin -344 1704 100 0 hwin#28
xform 0 -248 1720
p -341 1712 100 0 -1 val(in):#C0 S2
use hwout -768 1664 100 0 hwout#31
xform 0 -672 1680
p -672 1671 100 0 -1 val(outp):#C0 S2
use testPmacStatusRec -176 1368 100 0 testPmacStatusRec#32
xform 0 -48 1440
[comments]
