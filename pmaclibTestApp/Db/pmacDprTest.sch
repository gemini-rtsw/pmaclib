[schematic2]
uniq 7
[tools]
[detail]
w 154 1387 100 0 n#1 hwinl.hwinl#1.in 151 1384 151 1384 eais.dprAi.INP
w 130 1051 100 0 n#2 hwinl.hwinl#2.in 127 1048 127 1048 ebis.dprBi.INP
w 170 707 100 0 n#3 hwinl.hwinl#3.in 167 704 167 704 elongins.dprLi.INP
w 829 1323 100 0 n#4 hwoutl.hwoutl#5.outp 826 1320 826 1320 eaos.dprAo.OUT
w 829 1003 100 0 n#5 hwoutl.hwoutl#6.outp 826 1000 826 1000 ebos.dprBo.OUT
w 861 659 100 0 n#6 hwoutl.hwoutl#7.outp 858 656 858 656 elongouts.dprLo.OUT
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
use hwinl -240 1312 100 0 hwinl#1
xform 0 -40 1360
p -222 1376 100 0 -1 val(in):#C$(card) S0 @F:$DFFE
use hwinl -264 976 100 0 hwinl#2
xform 0 -64 1024
p -246 1040 100 0 -1 val(in):#C$(card) S0 @F:$DFFC
use hwinl -224 632 100 0 hwinl#3
xform 0 -24 680
p -206 696 100 0 -1 val(in):#C$(card) S0 @F:$DFFA
use hwoutl 824 1248 100 0 hwoutl#5
xform 0 1016 1296
p 922 1311 100 0 -1 val(outp):#C$(card) S0 @F:$DFFE
use hwoutl 824 928 100 0 hwoutl#6
xform 0 1016 976
p 922 991 100 0 -1 val(outp):#C$(card) S0 @F:$DFFC
use hwoutl 856 584 100 0 hwoutl#7
xform 0 1048 632
p 954 647 100 0 -1 val(outp):#C$(card) S0 @X:$DFFA
use statusRecTest -96 1576 100 0 statusRecTest#10
xform 0 80 1656
p -87 1600 100 0 1 set2:dev
p -91 1624 100 0 1 set1:inplink #C0 S0 @X:$DFFA
p -91 1648 100 0 1 set0:dtyp PMAC-VME DPRAM
use bkgThrTest 432 1576 100 0 bkgThrTest#11
xform 0 608 1656
[comments]
