[schematic2]
uniq 31
[tools]
[detail]
w 1928 2667 100 0 SELECT inhier.SELECT.P 1856 2656 2048 2656 efanouts.efanouts#24.SELL
w 2344 2635 100 0 n#28 efanouts.efanouts#24.LNK2 2240 2624 2496 2624 2496 2272 2688 2272 pmacMABTopPos.pmacMABTopPos#4.SLNK
w 2344 2667 100 0 n#27 efanouts.efanouts#24.LNK1 2240 2656 2496 2656 2496 2912 2688 2912 pmacMABBotPos.pmacMABBotPos#0.SLNK
w 1864 2587 100 0 TOP inhier.SLNK.P 1776 2576 2000 2576 efanouts.efanouts#24.SLNK
w 2622 2379 100 0 POS junction 2592 3008 2592 2368 2688 2368 pmacMABTopPos.pmacMABTopPos#4.POS
w 2520 3019 100 0 POS inhier.POS.P 2400 3008 2688 3008 pmacMABBotPos.pmacMABBotPos#0.POS
w 3656 3083 100 0 VEL inhier.VEL.P 3552 3072 3808 3072 pmacMABBotVel.pmacMABBotVel#1.VEL
w 3726 2443 100 0 VEL junction 3680 3072 3680 2432 3808 2432 pmacMABTopVel.pmacMABTopVel#5.VEL
w 3614 2347 100 0 n#6 pmacMABTopPos.pmacMABTopPos#4.FLNK 3456 2336 3808 2336 pmacMABTopVel.pmacMABTopVel#5.SLNK
w 3614 2987 100 0 n#3 pmacMABBotPos.pmacMABBotPos#0.FLNK 3456 2976 3808 2976 pmacMABBotVel.pmacMABBotVel#1.SLNK
[cell use]
use inhier 1840 2615 100 0 SELECT
xform 0 1856 2656
use inhier 1760 2535 100 0 SLNK
xform 0 1776 2576
use inhier 2384 2967 100 0 POS
xform 0 2400 3008
use inhier 3536 3031 100 0 VEL
xform 0 3552 3072
use efanouts 2000 2439 100 0 efanouts#24
xform 0 2120 2592
p 2096 2752 100 0 1 SELM:Mask
p 2112 2432 100 1024 -1 name:$(top)$(axis)MABswitch
p 2272 2656 75 1280 -1 pproc(LNK1):PP
p 2272 2624 75 1280 -1 pproc(LNK2):PP
use pmacMABTopVel 3808 2279 100 0 pmacMABTopVel#5
xform 0 4192 2384
use pmacMABTopPos 2688 2215 100 0 pmacMABTopPos#4
xform 0 3072 2320
use pmacMABBotVel 3808 2919 100 0 pmacMABBotVel#1
xform 0 4192 3024
use pmacMABBotPos 2688 2855 100 0 pmacMABBotPos#0
xform 0 3072 2960
use bc200tr 1504 1288 -100 0 frame
xform 0 3184 2592
[comments]
