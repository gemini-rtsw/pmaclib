[schematic2]
uniq 12
[tools]
[detail]
w 699 1347 100 0 n#1 hwin.hwin#25.in 696 1344 696 1344 estatus.pmac1status.INP
w -933 1179 100 0 n#2 hwin.hwin#10.in -936 1176 -936 1176 eais.pmac1ai.INP
w -245 1115 100 0 n#3 hwout.hwout#17.outp -248 1112 -248 1112 eaos.pmac1ao.OUT
w -949 843 100 0 n#4 hwin.hwin#12.in -952 840 -952 840 ebis.pmac1bi.INP
w -253 795 100 0 n#5 hwout.hwout#19.outp -256 792 -256 792 ebos.pmac1bo.OUT
w -909 499 100 0 n#6 hwin.hwin#14.in -912 496 -912 496 elongins.pmac1li.INP
w -221 451 100 0 n#7 hwout.hwout#21.outp -224 448 -224 448 elongouts.pmac1lo.OUT
w -901 83 100 0 n#8 hwin.hwin#16.in -904 80 -904 80 embbis.pmac1mbbi.INP
w -197 59 100 0 n#9 hwout.hwout#23.outp -200 56 -200 56 embbos.pmac1mbbo.OUT
w -149 1723 100 0 n#10 hwin.hwin#28.in -152 1720 -152 1720 estringins.pmac1mbx.INP
w -758 1690 -100 0 n#11 hwout.hwout#31.outp -768 1680 -768 1680 estringouts.pmac1mbxin.OUT
[cell use]
use eais -847 1075 100 0 pmac1ai
xform 0 -808 1144
p -934 1215 100 0 1 DTYP:PMAC-VME DPRAM
p -923 1252 100 0 1 SCAN:I/O Intr
use eaos -429 1060 100 0 pmac1ao
xform 0 -376 1144
p -507 1221 100 0 1 DTYP:PMAC-VME DPRAM
use ebis -867 736 100 0 pmac1bi
xform 0 -824 808
p -944 879 100 0 1 DTYP:PMAC-VME DPRAM
p -908 916 100 0 1 SCAN:I/O Intr
use ebos -432 740 100 0 pmac1bo
xform 0 -384 824
p -505 894 100 0 1 DTYP:PMAC-VME DPRAM
use elongins -824 392 100 0 pmac1li
xform 0 -784 464
p -894 534 100 0 1 DTYP:PMAC-VME DPRAM
p -891 591 100 0 1 SCAN:I/O Intr
use elongouts -396 395 100 0 pmac1lo
xform 0 -352 480
p -479 550 100 0 1 DTYP:PMAC-VME DPRAM
use embbis -832 -28 100 0 pmac1mbbi
xform 0 -776 48
p -906 116 100 0 1 DTYP:PMAC-VME DPRAM
p -776 62 100 0 0 ONST:State 1
p -968 62 100 0 1 ONVL:1
p -776 30 100 0 0 TWST:State 2
p -968 30 100 0 1 TWVL:2
p -776 94 100 0 0 ZRST:State Zero
p -968 94 100 0 1 ZRVL:0
p -85 -66 100 0 1 EIST:State 8
p -968 -162 100 0 1 EIVL:8
p -776 -34 100 0 1 FRST:State 4
p -968 -34 100 0 1 FRVL:4
p -776 -66 100 0 1 FVST:State 5
p -968 -66 100 0 1 FVVL:5
p -776 -194 100 0 1 NIST:State 9
p -968 -194 100 0 1 NIVL:9
p -776 -130 100 0 1 SVST:State 7
p -968 -130 100 0 1 SVVL:7
p -776 -98 100 0 1 SXST:State 6
p -968 -98 100 0 1 SXVL:6
p -776 -2 100 0 1 THST:State
p -968 -2 100 0 1 THVL:3
p -904 164 100 0 1 SCAN:I/O Intr
use embbos -392 -35 100 0 pmac1mbbo
xform 0 -328 56
p -438 138 100 0 1 DTYP:PMAC-VME DPRAM
p -296 118 100 0 0 ONST:One
p -488 118 100 0 1 ONVL:1
p -296 86 100 0 0 TWST:Two
p -488 86 100 0 1 TWVL:2
p -296 150 100 0 0 ZRST:Zero
p -488 150 100 0 0 ZRVL:1
p -296 22 100 0 1 FRST:Four
p -488 22 100 0 1 FRVL:4
p -296 -298 100 0 1 FTST:Fourteen
p -488 -298 100 0 1 FTVL:14
p -296 -10 100 0 1 FVST:Five
p -488 -10 100 0 1 FVVL:5
p -296 -42 100 0 1 SXST:Six
p -488 -42 100 0 1 SXVL:6
p -296 54 100 0 1 THST:Three
p -488 54 100 0 1 THVL:3
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
use estatus 820 458 100 0 pmac1status
xform 0 896 304
p 880 1070 100 0 0 SCAN:I/O Intr
p 521 1413 100 0 1 DTYP:PMAC-VME DPRAM
use hwin 504 1328 100 0 hwin#25
xform 0 600 1344
p 427 1372 100 0 -1 val(in):#C0 S0 @X:$DFF4
use estringouts -961 1625 100 0 pmac1mbxin
xform 0 -896 1696
use estringins -152 1640 100 0 pmac1mbx
xform 0 -24 1688
use hwin -344 1704 100 0 hwin#28
xform 0 -248 1720
p -341 1712 100 0 -1 val(in):#C0 S2
use hwout -768 1664 100 0 hwout#31
xform 0 -672 1680
p -672 1671 100 0 -1 val(outp):$(top)pmac1mbx
[comments]
