[schematic2]
uniq 8
[tools]
[detail]
w 154 1387 100 0 n#1 hwinl.hwinl#1.in 151 1384 151 1384 eais.dprAi.INP
w 130 1051 100 0 n#2 hwinl.hwinl#2.in 127 1048 127 1048 ebis.dprBi.INP
w 170 707 100 0 n#3 hwinl.hwinl#3.in 167 704 167 704 elongins.dprLi.INP
w 178 291 100 0 n#4 hwinl.hwinl#4.in 175 288 175 288 embbis.dprMbbi.INP
w 829 1323 100 0 n#5 hwoutl.hwoutl#5.outp 826 1320 826 1320 eaos.dprAo.OUT
w 829 1003 100 0 n#6 hwoutl.hwoutl#6.outp 826 1000 826 1000 ebos.dprBo.OUT
w 861 659 100 0 n#7 hwoutl.hwoutl#7.outp 858 656 858 656 elongouts.dprLo.OUT
[cell use]
use eais 233 1283 100 0 dprAi
xform 0 272 1352
p 146 1423 100 0 1 DTYP:PMAC-VME DPRAM
p 157 1460 100 0 1 SCAN:I/O Intr
p 149 1283 100 0 -1 PV:$(top)
use eaos 651 1268 100 0 dprAo
xform 0 704 1352
p 573 1429 100 0 1 DTYP:PMAC-VME DPRAM
p 567 1268 100 0 -1 PV:$(top)
use ebis 213 944 100 0 dprBi
xform 0 256 1016
p 136 1087 100 0 1 DTYP:PMAC-VME DPRAM
p 172 1124 100 0 1 SCAN:I/O Intr
p 129 944 100 0 -1 PV:$(top)
use ebos 648 948 100 0 dprBo
xform 0 696 1032
p 575 1102 100 0 1 DTYP:PMAC-VME DPRAM
p 564 948 100 0 -1 PV:$(top)
use elongins 256 600 100 0 dprLi
xform 0 296 672
p 186 742 100 0 1 DTYP:PMAC-VME DPRAM
p 189 799 100 0 1 SCAN:I/O Intr
p 172 600 100 0 -1 PV:$(top)
use elongouts 684 603 100 0 dprLo
xform 0 728 688
p 601 758 100 0 1 DTYP:PMAC-VME DPRAM
p 600 603 100 0 -1 PV:$(top)
use embbis 248 180 100 0 dprMbbi
xform 0 304 256
p 174 324 100 0 1 DTYP:PMAC-VME DPRAM
p 304 270 100 0 0 ONST:State 1
p 112 270 100 0 0 ONVL:1
p 304 238 100 0 0 TWST:State 2
p 112 238 100 0 0 TWVL:2
p 304 302 100 0 0 ZRST:State 0
p 112 302 100 0 0 ZRVL:0
p 995 142 100 0 0 EIST:State 8
p 112 46 100 0 0 EIVL:8
p 304 174 100 0 0 FRST:State 4
p 112 174 100 0 0 FRVL:4
p 304 142 100 0 0 FVST:State 5
p 112 142 100 0 0 FVVL:5
p 304 14 100 0 0 NIST:State 9
p 112 14 100 0 0 NIVL:9
p 304 78 100 0 0 SVST:State 7
p 112 78 100 0 0 SVVL:7
p 304 110 100 0 0 SXST:State 6
p 112 110 100 0 0 SXVL:6
p 304 206 100 0 0 THST:State 3
p 112 206 100 0 0 THVL:3
p 176 372 100 0 0 SCAN:I/O Intr
p 164 180 100 0 -1 PV:$(top)
p 304 -50 100 0 0 ELST:State 11
p 112 -50 100 0 0 ELVL:11
p -112 238 100 0 0 EVNT:State 15
p 112 -178 100 0 0 FFVL:15
p 304 -146 100 0 0 FTST:State 14
p 112 -146 100 0 0 FTVL:14
p -112 206 100 0 0 NOBT:4
p 304 -18 100 0 0 TEST:State 10
p 112 -18 100 0 0 TEVL:10
p 304 -114 100 0 0 TTST:State 13
p 112 -114 100 0 0 TTVL:13
p 304 -82 100 0 0 TVST:State 12
use embbos 688 173 100 0 pmac1mbbo
xform 0 752 264
p 642 346 100 0 1 DTYP:PMAC-VME DPRAM
p 784 326 100 0 0 ONST:One
p 592 326 100 0 0 ONVL:1
p 784 294 100 0 0 TWST:Two
p 592 294 100 0 0 TWVL:2
p 784 358 100 0 0 ZRST:Zero
p 592 358 100 0 0 ZRVL:0
p 784 230 100 0 0 FRST:Four
p 592 230 100 0 0 FRVL:4
p 784 -90 100 0 0 FTST:Fourteen
p 592 -90 100 0 0 FTVL:14
p 784 198 100 0 0 FVST:Five
p 592 198 100 0 0 FVVL:5
p 784 166 100 0 0 SXST:Six
p 592 166 100 0 0 SXVL:6
p 784 262 100 0 0 THST:Three
p 592 262 100 0 0 THVL:3
p 784 102 100 0 0 EIST:Eight
p 592 102 100 0 0 EIVL:8
p 784 6 100 0 0 ELST:Eleven
p 592 6 100 0 0 ELVL:11
p 784 -122 100 0 0 FFST:Fifteen
p 592 -122 100 0 0 FFVL:15
p 784 70 100 0 0 NIST:Nine
p 592 70 100 0 0 NIVL:9
p 368 262 100 0 0 NOBT:4
p 976 134 100 0 0 SVST:Seven
p 592 134 100 0 0 SVVL:7
p 784 38 100 0 0 TEST:Ten
p 592 38 100 0 0 TEVL:10
p 784 -58 100 0 0 TTST:Thirteen
p 592 -58 100 0 0 TTVL:13
p 784 -26 100 0 0 TVST:Twelve
p 592 -26 100 0 0 TVVL:12
use hwinl -240 1312 100 0 hwinl#1
xform 0 -40 1360
p -222 1376 100 0 -1 val(in):#C$(card) S0 @F:$DFFE
use hwinl -264 976 100 0 hwinl#2
xform 0 -64 1024
p -246 1040 100 0 -1 val(in):#C$(card) S0 @F:$DFFC
use hwinl -224 632 100 0 hwinl#3
xform 0 -24 680
p -206 696 100 0 -1 val(in):#C$(card) S0 @F:$DFFA
use hwinl -216 216 100 0 hwinl#4
xform 0 -16 264
p -198 280 100 0 -1 val(in):#C$(card) S0 @F:$DFF8
use hwoutl 824 1248 100 0 hwoutl#5
xform 0 1016 1296
p 922 1311 100 0 -1 val(outp):#C$(card) S0 @F:$DFFE
use hwoutl 824 928 100 0 hwoutl#6
xform 0 1016 976
p 922 991 100 0 -1 val(outp):#C$(card) S0 @F:$DFFC
use hwoutl 856 584 100 0 hwoutl#7
xform 0 1048 632
p 954 647 100 0 -1 val(outp):#C$(card) S0 @X:$DFFA
use hwoutl 1504 -1256 100 0 hwoutl#8
xform 0 1072 208
p 996 226 100 0 -1 val(outp):#C$(card) S0 @F:$DFF8
use statusRecTest -96 1576 100 0 statusRecTest#10
xform 0 80 1656
p -87 1600 100 0 1 set2:dev
p -91 1624 100 0 1 set1:inplink #C0 S0 @X:$DFFA
p -91 1648 100 0 1 set0:dtyp PMAC-VME DPRAM
use bkgThrTest 432 1576 100 0 bkgThrTest#11
xform 0 608 1656
[comments]
