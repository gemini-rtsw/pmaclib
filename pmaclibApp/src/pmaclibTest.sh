#!/bin/csh -f

setenv EPICS_CA_ADDR_LIST 172.16.2.23

medm -x -macro "top=pmac0:" /gem_sw/work/R3.14.12.4/support/pmaclib/bin/linux-x86_64/pmaclibTest.adl

