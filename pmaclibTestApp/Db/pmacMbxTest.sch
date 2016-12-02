[schematic2]
uniq 8
[tools]
[detail]
w -174 347 100 0 n#1 hwinm.hwinm#6.in -177 344 -177 344 estringins.mbxSi.INP
w 517 299 100 0 n#2 hwoutm.hwoutm#8.outp 514 296 514 296 estringouts.mbxSoSig1.OUT
w -174 155 100 0 n#3 hwinm.hwinm#12.in -177 152 -177 152 estringins.mbxSiVer.INP
w 533 99 100 0 n#4 hwoutm.hwoutm#17.outp 530 96 530 96 estringouts.mbxSoSig0.OUT
w 1925 -261 100 0 n#5 hwoutl.hwoutl#19.outp 1922 -264 1922 -264 eloads.mbxLoad.OUT
w 517 -181 100 0 n#6 hwoutm.hwoutm#25.outp 514 -184 514 -184 eaos.mbxAo.OUT
w -175 -142 -100 0 n#7 hwinm.hwinm#23.in -185 -152 -185 -152 eais.mbxAi.INP
n -775 306 -532 450 100
The stringin record with
DTYP = PMAC-VME ASCII
and VMEIO Signal = S2
uses the record's current
VAL as the command to
send to the mailbox.
_
n -791 74 -502 218 100
The stringin record with
DTYP = PMAC-VME ASCII
and VMEIO Signal = S1
uses the VMEIO @parm
parameter as the command to
send to the mailbox.
_
s -731 870 250 0 PMAC Mailbox (DTYP = PMAC-VME ASCII) Test Records
n 872 154 1503 250 100
The stringout with DTYP = PMAC-VME ASCII and VMEIO signal = S1
Will read back the response from the mailbox and place that into
the record's VAL field. If the signal is not S1, the record's VAL
field will remain unchanged.
_
[cell use]
use estringouts 319 241 100 0 mbxSoSig1
xform 0 384 312
p 299 371 100 0 1 DTYP:PMAC-VME ASCII
p 235 241 100 0 -1 PV:$(top)
use estringins -48 238 100 0 mbxSi
xform 0 -48 312
p -163 380 100 0 1 DTYP:PMAC-VME ASCII
p -132 238 100 0 -1 PV:$(top)
use hwinm -472 272 100 0 hwinm#6
xform 0 -320 320
p -462 336 100 0 -1 val(in):#C$(card) S2
use hwoutm 504 224 100 0 hwoutm#8
xform 0 656 272
p 610 287 100 0 -1 val(outp):#C$(card) S1
use estringins -48 46 100 0 mbxSiVer
xform 0 -48 120
p -163 188 100 0 1 DTYP:PMAC-VME ASCII
p -132 46 100 0 -1 PV:$(top)
use hwinm -472 80 100 0 hwinm#12
xform 0 -320 128
p -462 144 100 0 -1 val(in):#C$(card) S1 @ver
use statusRecTest -208 576 100 0 statusRecTest#13
xform 0 -32 656
p -199 600 100 0 1 set2:dev mbx
p -203 648 100 0 1 set0:dtyp PMAC-VME ASCII
use estringouts 335 41 100 0 mbxSoSig0
xform 0 400 112
p 315 171 100 0 1 DTYP:PMAC-VME ASCII
p 251 41 100 0 -1 PV:$(top)
use hwoutm 520 24 100 0 hwoutm#17
xform 0 672 72
p 626 87 100 0 -1 val(outp):#C$(card) S0
use eloads 1817 -634 100 0 mbxLoad
xform 0 1808 -344
p 1919 -359 100 0 1 DTYP:PMAC-VME ASCII
p 1733 -634 100 0 -1 PV:$(top)
use hwoutl 1920 -336 100 0 hwoutl#19
xform 0 2112 -288
p 2018 -273 100 0 -1 val(outp):#C$(card) S2
use eais -49 -258 100 0 mbxAi
xform 0 -56 -184
p -133 -258 100 0 -1 PV:$(top)
p -160 -119 100 0 1 DTYP:PMAC-VME ASCII
use eaos 388 -246 100 0 mbxAo
xform 0 392 -152
p 304 -246 100 0 -1 PV:$(top)
p 303 -79 100 0 1 DTYP:PMAC-VME ASCII
use hwinm -480 -224 100 0 hwinm#23
xform 0 -328 -176
p -470 -160 100 0 -1 val(in):#C$(card) S0 @I99=
use hwoutm 504 -256 100 0 hwoutm#25
xform 0 656 -208
p 610 -193 100 0 -1 val(outp):#C$(card) S0 @I99
[comments]
