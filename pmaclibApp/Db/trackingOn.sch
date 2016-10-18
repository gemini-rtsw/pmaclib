[schematic2]
uniq 25
[tools]
[detail]
w 2776 2395 100 0 n#24 estringinval.estringinval#23.VAL 2768 2384 2832 2384 egenSub.egenSub#19.INPA
w 2608 1723 100 0 n#21 efanouts.efanouts#15.LNK2 2336 2656 2432 2656 2432 1712 2832 1712 egenSub.egenSub#19.SLNK
w 2432 2699 100 0 n#18 efanouts.efanouts#15.LNK1 2336 2688 2576 2688 motionProg.motionProg#17.SLNK
w 1944 3179 100 0 n#16 eseqs.eseqs#9.LNK2 1920 3168 2016 3168 2016 2608 2096 2608 efanouts.efanouts#15.SLNK
w 2040 3211 100 0 n#14 eseqs.eseqs#9.LNK1 1920 3200 2208 3200 hwout.hwout#2.outp
w 2120 2891 100 0 FLNK eseqs.eseqs#9.FLNK 1920 2880 2368 2880 outhier.FLNK.p
w 1400 2891 100 0 SLNK inhier.SLNK.P 1248 2880 1600 2880 eseqs.eseqs#9.SLNK
w 1518 3211 100 0 n#3 hwin.hwin#0.in 1472 3200 1600 3200 eseqs.eseqs#9.DOL1
[cell use]
use estringinval 2512 2327 100 0 estringinval#23
xform 0 2640 2400
p 2512 2494 100 0 0 VAL:Tracking ON
p 2448 1964 100 0 0 def(INP):Tracking ON
p 2624 2320 100 1024 -1 name:$(top)$(command)Message
use egenSub 2832 1623 100 0 egenSub#19
xform 0 2976 2048
p 2912 2336 100 0 1 FTA:STRING
p 2912 2480 100 0 1 SNAM:log_console
p 2944 1616 100 1024 1 name:$(top)$(command)LogMsg
use motionProg 2576 2535 100 0 motionProg#17
xform 0 2848 2672
use efanouts 2096 2471 100 0 efanouts#15
xform 0 2216 2624
p 2176 2768 100 0 1 SELM:All
p 2208 2464 100 1024 -1 name:$(top)$(command)TrackingOnFO
p 2368 2688 75 1280 -1 pproc(LNK1):PP
p 2368 2656 75 1280 -1 pproc(LNK2):PP
use eseqs 1600 2791 100 0 eseqs#9
xform 0 1760 3040
p 1712 3264 100 0 0 DLY2:0.0
p 1712 3248 100 0 1 DLY3:0.1
p 1712 2784 100 1024 -1 name:$(top)$(command)TrackingOn
p 1936 3168 75 1024 -1 pproc(LNK2):PP
use outhier 2336 2839 100 0 FLNK
xform 0 2352 2880
use inhier 1232 2839 100 0 SLNK
xform 0 1248 2880
use hwout 2208 3159 100 0 hwout#2
xform 0 2304 3200
p 2256 3136 100 0 -1 val(outp):$(top)FollowL .PP
use hwin 1280 3159 100 0 hwin#0
xform 0 1376 3200
p 1283 3192 100 0 -1 val(in):1
use bc200tr 352 1192 -100 0 frame
xform 0 2032 2496
[comments]
