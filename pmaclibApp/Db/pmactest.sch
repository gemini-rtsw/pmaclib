[schematic2]
uniq 1
[tools]
[detail]
s -931 2267 250 0 PMAC Records Test Harness
s -995 1931 100 0 Records for DTYP = PMAC-VME DPRAM
s -523 1931 100 0 Records for DTYP = PMAC-VME ASCII
n -1327 1771 -1026 2299 100
List of PMAC record types
actually used at Gemini:

A&G
o    load "PMAC-VME ASCII"
o    stringin "PMAC-VME ASCII"

CRCS
o    status "PMAC-VME DPRAM"
o    load "PMAC-VME ASCII"
o    stringin "PMAC-VME ASCII"
o    stringout "PMAC-VME ASCII"
o    ao "PMAC-VME DPRAM"
o    ai "PMAC-VME DPRAM"

MCS
o    status "PMAC-VME DPRAM"
o    load "PMAC-VME ASCII"
o    stringin "PMAC-VME ASCII"
o    stringout "PMAC-VME ASCII"
o    ao "PMAC-VME DPRAM"
o    ai "PMAC-VME DPRAM"
_
[cell use]
use pmacMbxTest -528 1968 100 0 pmacMbxTest#49
xform 0 -352 2072
use pmacDprTest -984 1968 100 0 pmacDprTest#50
xform 0 -808 2072
[comments]
