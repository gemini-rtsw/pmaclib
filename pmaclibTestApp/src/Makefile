TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#=============================
#

SCRIPTS_HOST += ../pmaclibTest.sh
SCRIPTS_HOST += ../pmaclibTest.adl

PROD_IOC_RTEMS = pmaclibTest
#LIBRARY_IOC_RTEMS = pmaclibTest

DBD += pmaclibTest.dbd

# pmaclibTest.dbd will be made up from these files:
pmaclibTest_DBD += base.dbd
pmaclibTest_DBD += geminiRecords.dbd
pmaclibTest_DBD += pmaclib.dbd
pmaclibTest_DBD += pmaclibTestFuncs.dbd

pmaclibTest_LIBS += geminiRecords
pmaclibTest_LIBS += pmaclib

pmaclibTest_SRCS += pmaclibTest_registerRecordDeviceDriver.cpp
pmaclibTest_SRCS += follow.c
pmaclibTest_SRCS += pmaclibTestMain.cpp

pmaclibTest_LIBS += $(EPICS_BASE_IOC_LIBS)




#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

