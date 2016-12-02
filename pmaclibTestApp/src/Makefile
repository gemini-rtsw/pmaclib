TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
#

SCRIPTS_HOST += ../pmaclibTest.sh
SCRIPTS_HOST += ../pmaclibTest.adl


LIBRARY_IOC_RTEMS += pmaclibTest
pmaclibTest_SRCS += follow.c
pmaclibTest_LIBS += pmaclib


# <APPNAME>.dbd will be created and installed
DBD += pmaclibTest.dbd

# pmaclib.dbd will be made up from these files:
pmaclibTest_DBD += base.dbd
pmaclibTest_DBD += loadRecord.dbd
pmaclibTest_DBD += genSubRecord.dbd
pmaclibTest_DBD += statusRecord.dbd
pmaclibTest_DBD += pmaclib.dbd
pmaclibTest_DBD += pmaclibTestFuncs.dbd



#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

