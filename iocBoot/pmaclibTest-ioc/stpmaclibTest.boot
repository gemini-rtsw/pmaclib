$(LINUX_ONLY)#!${INSTALL}/bin/${ARCH}/pmaclibTest-ioc

cd ${INSTALL}

## Register all support components
dbLoadDatabase("dbd/pmaclibTest.dbd")
pmaclibTest_registerRecordDeviceDriver(pdbbase)

## Load record instances
#dbLoadRecords("db/pmactest.db", "top=pmac0:,card=0")
#dbLoadRecords("db/pmactest.db", "top=pmac1:,card=1")
dbLoadRecords("db/pmacMbxTest.db", "top=pmac0:,card=0")
dbLoadRecords("db/pmacMbxTest.db", "top=pmac1:,card=1")

#dbLoadRecords("db/iocAdminRTEMS.db","IOC=pmac,TODFORMAT=%m/%d/%Y %H:%M:%S")


var drvPmacDebug 1
var devPmacMbxDebug 1
# configure pmac card:
# pmacConfig( cardNumber, addrBase, addrDpram, irqVector, irqLevel)
pmacConfig(0, 0x7fa000, 0x700000, 0xa1, 6)
pmacConfig(1, 0x7fa200, 0x704000, 0xa5, 6)


iocInit

