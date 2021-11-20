TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
#

# <APPNAME>.dbd will be created and installed
DBDINC += loadRecord.h
DBD += pmacDev.dbd

# Build a support module called <APPNAME>
LIBRARY_IOC += pmaclib

#pmaclib_LIBS += electricFence
#USR_LDFLAGS += -Wl,--wrap,malloc -Wl,--wrap,realloc -Wl,--wrap,calloc -Wl,--wrap,free
#USR_LDFLAGS += -Wl,--wrap,_malloc_r -Wl,--wrap,_realloc_r -Wl,--wrap,_calloc_r -Wl,--wrap,_free_r


# specify all source files to be compiled
# including sequencer (.st) source files
pmaclib_SRCS += loadRecord.c
pmaclib_SRCS += devPmacMbx.c
pmaclib_SRCS += devPmacRam.c
pmaclib_SRCS += drvPmacVme.c
pmaclib_SRCS += drvPmac.c
pmaclib_SRCS += drvPmacGat.c
pmaclib_SRCS += pmacUpDown.c

pmaclib_LIB += $(EPICS_BASE_IOC_LIBS)

INC	+= drvPmac.h
INC	+= drvPmacGat.h
INC	+= pmacVme.h
INC	+= pmacTypes.h

#=============================
include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

