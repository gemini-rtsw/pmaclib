[schematic2]
uniq 50
[tools]
[detail]
w 1800 2459 100 0 n#49 estringinval.estringinval#48.VAL 1776 2448 1872 2448 egenSub.egenSub#46.INPA
w 1592 1787 100 0 n#47 efanouts.efanouts#16.LNK4 1264 2928 1360 2928 1360 1776 1872 1776 egenSub.egenSub#46.SLNK
w 1640 3371 100 0 n#45 efanouts.efanouts#16.LNK1 1264 3024 1456 3024 1456 3360 1872 3360 estringouts.estringouts#44.SLNK
w 2280 2987 100 0 n#38 estringouts.estringouts#9.OUT 2128 2976 2480 2976 hwout.hwout#25.outp
w 2200 3355 100 0 n#38 estringouts.estringouts#44.OUT 2128 3344 2320 3344 2320 2976 junction
w 1776 3403 100 0 n#40 hwin.hwin#43.in 1728 3392 1872 3392 estringouts.estringouts#44.DOL
w 1608 2715 100 0 n#39 efanouts.efanouts#16.LNK3 1264 2960 1392 2960 1392 2704 1872 2704 eaos.eaos#1.SLNK
w 912 2955 100 0 SLNK inhier.SLNK.P 848 2944 1024 2944 efanouts.efanouts#16.SLNK
w 1592 3547 100 0 FLNK efanouts.efanouts#16.FLNK 1264 3056 1392 3056 1392 3536 1840 3536 outhier.FLNK.p
w 1544 3003 100 0 n#17 efanouts.efanouts#16.LNK2 1264 2992 1872 2992 estringouts.estringouts#9.SLNK
w 1776 3035 100 0 n#15 hwin.hwin#14.in 1728 3024 1872 3024 estringouts.estringouts#9.DOL
w 2222 2683 100 0 n#4 eaos.eaos#1.OUT 2128 2672 2352 2672 hwout.hwout#2.outp
w 1782 2747 100 0 n#3 hwin.hwin#0.in 1728 2736 1872 2736 eaos.eaos#1.DOL
[cell use]
use estringinval 1520 2391 100 0 estringinval#48
xform 0 1648 2464
p 1520 2558 100 0 0 VAL:$(dev) Tracking OFF
p 1456 2028 100 0 0 def(INP):$(dev) Tracking OFF
p 1632 2384 100 1024 -1 name:$(top)$(dev)$(command)Msg
use egenSub 1872 1687 100 0 egenSub#46
xform 0 2016 2112
p 1968 2448 100 0 1 FTA:STRING
p 1936 2528 100 0 1 SNAM:log_console
p 1984 1680 100 1024 1 name:$(top)$(dev)$(command)logMsg
use estringouts 1872 2919 100 0 estringouts#9
xform 0 2000 2992
p 1936 3072 100 0 1 DTYP:PMAC-VME ASCII
p 1936 3104 100 0 1 OMSL:closed_loop
p 1808 2958 100 0 0 VAL:
p 1984 2912 100 1024 -1 name:$(top)$(dev)$(command)PMAC_AbortS
use estringouts 1872 3287 100 0 estringouts#44
xform 0 2000 3360
p 1936 3440 100 0 1 DTYP:PMAC-VME ASCII
p 1936 3472 100 0 1 OMSL:closed_loop
p 1808 3326 100 0 0 VAL:
p 1984 3280 100 1024 -1 name:$(top)$(dev)$(command)PMAC_P_102_OFFS
use hwin 1536 2695 100 0 hwin#0
xform 0 1632 2736
p 1539 2728 100 0 -1 val(in):0
use hwin 1536 2983 100 0 hwin#14
xform 0 1632 3024
p 1536 3056 100 0 -1 val(in):$(top)PmacCmd_Abort
use hwin 1536 3351 100 0 hwin#43
xform 0 1632 3392
p 1536 3424 100 0 -1 val(in):$(top)PmacCmd_P102_OFF
use hwout 2352 2631 100 0 hwout#2
xform 0 2448 2672
p 2400 2608 100 0 -1 val(outp):$(top)FollowL .PP
use hwout 2480 2935 100 0 hwout#25
xform 0 2576 2976
p 2560 3008 100 0 -1 val(outp):#C$(card) S1
use efanouts 1024 2807 100 0 efanouts#16
xform 0 1144 2960
p 1120 3104 100 0 1 SELM:All
p 1136 2800 100 1024 -1 name:$(top)$(dev)$(command)trackingOffFO
p 1296 3024 75 1280 -1 pproc(LNK1):PP
p 1296 2992 75 1280 -1 pproc(LNK2):PP
p 1296 2928 75 1280 -1 pproc(LNK4):PP
use outhier 1808 3495 100 0 FLNK
xform 0 1824 3536
use inhier 832 2903 100 0 SLNK
xform 0 848 2944
use eaos 1872 2615 100 0 eaos#1
xform 0 2000 2704
p 1616 2686 100 0 0 OMSL:closed_loop
p 1984 2608 100 1024 -1 name:$(top)$(dev)$(command)TrackingOff
use bc200tr 352 1192 -100 0 frame
xform 0 2032 2496
[comments]
