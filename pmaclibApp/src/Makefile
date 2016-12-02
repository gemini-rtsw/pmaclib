TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
#

DBDINC += loadRecord

# Build a support module called <APPNAME>

LIBRARY_IOC_RTEMS += pmaclib

# <APPNAME>.dbd will be created and installed
DBD += pmaclib.dbd

# pmaclib.dbd will be made up from these files:
pmaclib_DBD += base.dbd
pmaclib_DBD += loadRecord.dbd
pmaclib_DBD += genSubRecord.dbd
pmaclib_DBD += statusRecord.dbd
# List local .dbd files here
pmaclib_DBD += pmacDev.dbd

# specify all source files to be compiled
# including sequencer (.st) source files
pmaclib_SRCS += loadRecord.c
pmaclib_SRCS += devPmacMbx.c
pmaclib_SRCS += devPmacRam.c
pmaclib_SRCS += drvPmacVme.c
pmaclib_SRCS += drvPmac.c
pmaclib_SRCS += drvPmacGat.c
pmaclib_SRCS += pmacUpDown.c


INC	+= drvPmac.h
INC	+= drvPmacGat.h
INC	+= pmacVme.h
INC	+= pmacTypes.h


#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

