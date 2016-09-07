[schematic2]
uniq 13
[tools]
[detail]
w -94 67 100 0 n#1 hwinl.hwinl#3.in -97 64 -97 64 eais.aiOpn.INP
w 645 19 100 0 n#2 hwoutl.hwoutl#4.outp 642 16 642 16 eaos.aoOpn.OUT
w -94 -165 100 0 n#3 hwinl.hwinl#10.in -97 -168 -97 -168 eais.aiSvo.INP
w 645 -213 100 0 n#4 hwoutl.hwoutl#12.outp 642 -216 642 -216 eaos.aoSvo.OUT
w -78 -445 100 0 n#5 hwinl.hwinl#18.in -81 -448 -81 -448 eais.aiSvo2.INP
w 661 -493 100 0 n#6 hwoutl.hwoutl#20.outp 658 -496 658 -496 eaos.aoSvo2.OUT
w -86 -701 100 0 n#7 hwinl.hwinl#26.in -89 -704 -89 -704 eais.aiSvo3.INP
w 653 -749 100 0 n#8 hwoutl.hwoutl#28.outp 650 -752 650 -752 eaos.aoSvo3.OUT
w -78 -941 100 0 n#9 hwinl.hwinl#34.in -81 -944 -81 -944 eais.aiBfd.INP
w 661 -1229 100 0 n#10 hwoutl.hwoutl#36.outp 658 -1232 658 -1232 elongouts.bfdTrig.OUT
w 653 -997 100 0 n#11 hwoutl.hwoutl#39.outp 650 -1000 650 -1000 eaos.aoBfd.OUT
w -55 -1398 -100 0 n#12 hwinl.hwinl#43.in -65 -1408 -65 -1408 eais.aiBvd.INP
[cell use]
use eais 38 -38 100 0 aiOpn
xform 0 32 32
p -97 100 100 0 1 DTYP:PMAC-VME DPRAM
p -95 121 100 0 1 SCAN:I/O Intr
p -46 -38 100 0 -1 PV:$(top)
use eaos 512 -40 100 0 aoOpn
xform 0 512 48
p 428 -40 100 0 -1 PV:$(top)
p 394 123 100 0 1 DTYP:PMAC-VME DPRAM
use hwinl -480 -8 100 0 hwinl#3
xform 0 -288 40
p -470 56 100 0 -1 val(in):#C$(card) S0 @F:$DF00
use hwoutl 640 -56 100 0 hwoutl#4
xform 0 832 -8
p 738 7 100 0 -1 val(outp):#C$(card) S0 @F:$DF00
use eais 38 -270 100 0 aiSvo
xform 0 32 -200
p -97 -132 100 0 1 DTYP:PMAC-VME DPRAM
p -95 -111 100 0 1 SCAN:I/O Intr
p -46 -270 100 0 -1 PV:$(top)
use eaos 512 -272 100 0 aoSvo
xform 0 512 -184
p 428 -272 100 0 -1 PV:$(top)
p 394 -109 100 0 1 DTYP:PMAC-VME DPRAM
use hwinl -480 -240 100 0 hwinl#10
xform 0 -288 -192
p -470 -176 100 0 -1 val(in):#C$(card) S0 @F:$D000
use hwoutl 640 -288 100 0 hwoutl#12
xform 0 832 -240
p 738 -225 100 0 -1 val(outp):#C$(card) S0 @F:$D000
use eais 54 -550 100 0 aiSvo2
xform 0 48 -480
p -81 -412 100 0 1 DTYP:PMAC-VME DPRAM
p -79 -391 100 0 1 SCAN:I/O Intr
p -30 -550 100 0 -1 PV:$(top)
use eaos 528 -552 100 0 aoSvo2
xform 0 528 -464
p 444 -552 100 0 -1 PV:$(top)
p 410 -389 100 0 1 DTYP:PMAC-VME DPRAM
use hwinl -464 -520 100 0 hwinl#18
xform 0 -272 -472
p -454 -456 100 0 -1 val(in):#C$(card) S0 @F:$D008
use hwoutl 656 -568 100 0 hwoutl#20
xform 0 848 -520
p 754 -505 100 0 -1 val(outp):#C$(card) S0 @F:$D008
use eais 46 -806 100 0 aiSvo3
xform 0 40 -736
p -89 -668 100 0 1 DTYP:PMAC-VME DPRAM
p -87 -647 100 0 1 SCAN:I/O Intr
p -38 -806 100 0 -1 PV:$(top)
use eaos 520 -808 100 0 aoSvo3
xform 0 520 -720
p 436 -808 100 0 -1 PV:$(top)
p 402 -645 100 0 1 DTYP:PMAC-VME DPRAM
use hwinl -472 -776 100 0 hwinl#26
xform 0 -280 -728
p -462 -712 100 0 -1 val(in):#C$(card) S0 @X:$D006
use hwoutl 648 -824 100 0 hwoutl#28
xform 0 840 -776
p 746 -761 100 0 -1 val(outp):#C$(card) S0 @X:$D006
use eais 54 -1046 100 0 aiBfd
xform 0 48 -976
p -81 -908 100 0 1 DTYP:PMAC-VME DPRAM
p -79 -887 100 0 1 SCAN:I/O Intr
p -30 -1046 100 0 -1 PV:$(top)
use eaos 528 -1056 100 0 aoBfd
xform 0 528 -968
p 444 -1056 100 0 -1 PV:$(top)
p 410 -893 100 0 1 DTYP:PMAC-VME DPRAM
use hwinl -464 -1016 100 0 hwinl#34
xform 0 -272 -968
p -454 -952 100 0 -1 val(in):#C$(card) S0 @F:$D08B
use hwoutl 656 -1304 100 0 hwoutl#36
xform 0 848 -1256
p 754 -1241 100 0 -1 val(outp):#C$(card) S0 @X:$D08A
use elongouts 408 -1264 100 0 bfdTrig
xform 0 536 -1200
p 416 -1126 100 0 1 DTYP:PMAC-VME DPRAM
p 324 -1264 100 0 -1 PV:$(top)
use hwoutl 648 -1072 100 0 hwoutl#39
xform 0 840 -1024
p 746 -1009 100 0 -1 val(outp):#C$(card) S0 @F:$D08B
use eais 70 -1510 100 0 aiBvd
xform 0 64 -1440
p -65 -1372 100 0 1 DTYP:PMAC-VME DPRAM
p -63 -1351 100 0 1 SCAN:I/O Intr
p -14 -1510 100 0 -1 PV:$(top)
use hwinl -448 -1480 100 0 hwinl#43
xform 0 -256 -1432
p -438 -1416 100 0 -1 val(in):#C$(card) S0 @D:$0B13
[comments]
