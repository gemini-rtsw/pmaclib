TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
# Build a support module called <APPNAME>

LIBRARY_IOC += pmaclib

# <APPNAME>.dbd will be created and installed
DBD += pmaclib.dbd

# pmaclib.dbd will be made up from these files:
pmaclib_DBD += base.dbd
# List local .dbd files here
# pmaclib_DBD += xxx.dbd

# Include dbd files for external support modules:
# pmaclib_DBD += genSubRecord.dbd

# specify all source files to be compiled
# including sequencer (.st) source files
#pmaclib_SRCS += xxx.c
#pmaclib_SRCS += xxx.st

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

