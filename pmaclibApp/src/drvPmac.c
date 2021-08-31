/* @(#) drvPmac.c 1.7 97/05/06 */

/* drvPmac.c -  EPICS Device Driver Support for PMAC-VME */

/*
 * Author:      Thomas A. Coleman
 * Date:        97/05/06
 *
 *              20190705  mdw  Converted VxWorks priority levels to 
 *                             EPICS OSI priority levels
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 */



#include <epicsStdioRedirect.h>
#include <epicsString.h>
#include <epicsMath.h>
#include <epicsPrint.h>
#include <epicsExit.h>
#include <epicsRingPointer.h>
#include <epicsExport.h>
#include <string.h>
#include <recSup.h>
#include <devLib.h>
#include <devSup.h>
#include <drvSup.h>
#include <errMdef.h>
#include <taskwd.h>
#include <iocsh.h>
#include <dbDefs.h>

#include <stdint.h>

/* local includes */

#include "pmacVme.h"
#include "drvPmac.h"

/*
 * DEFINES
 */

#define PMAC_PRIVATE FALSE

#if PMAC_PRIVATE
#define PMAC_LOCAL LOCAL
#else
#define PMAC_LOCAL
#endif

#define PMAC_MESSAGE   epicsPrintf
#define PMAC_DEBUG(level,code)   { if (drvPmacDebug >= (level)) { code } fflush(stdout); }

#define NO_ERR_STATUS   (-1)

#define FILE_TEXT_BUFLEN   (256)

#define PMAC_MBX_QUEUE_SIZE 1000



/* The thread priorities were for VxWorks. Priorities 43, 44 and 45 are fairly high priority levels
 * In VxWorks, the lower the priority number, the higher the priority.
 * IN EPICS OSI, priorities are 0 thru 100, with higher numbers being higher priority.
 * We need to choose relatively high priorities for the following and reverse the order of the priorty numbers
 */
#define PMAC_MBX_SCAN      "pmacMbx"
//#define PMAC_MBX_PRI      (45)
#define PMAC_MBX_PRI        (90)   
#define PMAC_MBX_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_ASC_QUEUE_SIZE 1000

#define PMAC_ASC_SCAN      "pmacAsc"
// #define PMAC_ASC_PRI     (45)
#define PMAC_ASC_PRI        (90)
#define PMAC_ASC_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_SVO_SCAN      "pmacSvo"
//#define PMAC_SVO_PRI      (45)
#define PMAC_SVO_PRI        (90)
#define PMAC_SVO_RATE       0.1
#define PMAC_SVO_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_BKG_SCAN      "pmacBkg"
//#define PMAC_BKG_PRI      (44)
#define PMAC_BKG_PRI        (91)
#define PMAC_BKG_RATE       0.05
#define PMAC_BKG_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_VAR_SCAN      "pmacVar"
//#define PMAC_VAR_PRI      (44)
#define PMAC_VAR_PRI        (91)
#define PMAC_VAR_RATE       0.05
#define PMAC_VAR_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_OPN_SCAN      "pmacOpn"
//#define PMAC_OPN_PRI      (43)
#define PMAC_OPN_PRI        (92)
#define PMAC_OPN_RATE       0.025
#define PMAC_OPN_STACK      epicsThreadGetStackSize(epicsThreadStackBig)
#define PMAC_FLD_QUEUE_SIZE 100

#define PMAC_FLD_SCAN      "pmacFld"
//#define PMAC_FLD_PRI      (45)
#define PMAC_FLD_PRI        (90)
#define PMAC_FLD_STACK      epicsThreadGetStackSize(epicsThreadStackMedium)

#define PMAC_DPRAM_SVO      1
#define PMAC_DPRAM_BKG      2
#define PMAC_DPRAM_VAR      3
#define PMAC_DPRAM_OPN      4
#define PMAC_DPRAM_NONE      (-1)

#define PMAC_MEMTYP_Y      1
#define PMAC_MEMTYP_X      2
#define PMAC_MEMTYP_SY      3
#define PMAC_MEMTYP_SX      4
#define PMAC_MEMTYP_DP      5
#define PMAC_MEMTYP_D      6
#define PMAC_MEMTYP_F      7
#define PMAC_MEMTYP_L      8
#define PMAC_MEMTYP_HY      9
#define PMAC_MEMTYP_HX      10
#define PMAC_MEMTYP_NONE   (-1)

/* PMAC Hardware Constants */

#define PMAC_VARTYP_Y      0
#define PMAC_VARTYP_L      1
#define PMAC_VARTYP_X      2
#define PMAC_VARTYP_NONE   (-1)


int pmacCardsConfigured = 0;
int pmacConfigFirst     = 1;

/*
 * TYPEDEFS
 */

typedef struct  /* PMAC_DRVET */
{
   long      number;
   DRVSUPFUN   report;
   DRVSUPFUN   init;
} PMAC_DRVET;

/*
 * GLOBALS
 */
char * drvPmacVersion = "@(#) drvPmac.c 2017 CCB debug";
int   drvPmacDebug = 0;      /* must be > 0 to see messages */

/* EPICS Driver Support Entry Table */

PMAC_DRVET drvPmac =
{
   2,
   drvPmac_report,
   drvPmac_init,
};
epicsExportAddress(drvet, drvPmac);


/*******************************************************************************
 *
 * pmacConfig - Configure PMAC card
 *
 * This routine is to be called in the startup script in order to init the
 * card.
 *
 * By default there are no cards configured.
 *
 */
long pmacConfig
(
 int      cardNumber,
 unsigned long   addrBase,
 unsigned long   addrDpram,
 unsigned int   irqVector,
 unsigned int   irqLevel
 )
{
   char          *MyName = "pmacConfig";
   int         i;
   long         val;
   char         block;
   volatile char *pBlock;
   long          status;
   PMAC_CTLR     *pPmacCtlr;
   PMAC_CARD     *pCard;


   if( pmacConfigFirst == 1 ) {
      for( i=0; i < PMAC_MAX_CARDS; i++ ) {
         drvPmacCard[i].configured = FALSE;
         drvPmacCard[i].card       = i;
         drvPmacCard[i].ctlr       = i;
         pmacVmeCtlr[i].ctlr       = i;
      }
      pmacConfigFirst = 0;
   }

   if ( (cardNumber < 0) | (cardNumber >= PMAC_MAX_CARDS) )
   {
      epicsPrintf ("%s: Card number %d invalid -- must be 0 to %d.\n",
            MyName, cardNumber, PMAC_MAX_CARDS - 1);
      return(ERROR);
   }

   pPmacCtlr              = &pmacVmeCtlr[cardNumber];
   pPmacCtlr->vmebusBase  = addrBase;
   pPmacCtlr->irqVector   = irqVector;
   pPmacCtlr->irqLevel    = irqLevel;
   pPmacCtlr->vmebusDpram = addrDpram;

   if( addrDpram == 0 )
      pPmacCtlr->enabledDpram = FALSE;
   else
      pPmacCtlr->enabledDpram = TRUE;


   status = devRegisterAddress ("PMAC BASE", atVMEA24, pPmacCtlr->vmebusBase,
                PMAC_MEM_SIZE_BASE, (void *) &(pPmacCtlr->pBase));


   if (!RTN_SUCCESS(status)) {
      epicsPrintf ("%s: card %d: Failure registering controller %d base address A24 %#010lx.\n",
              MyName, cardNumber, pPmacCtlr->ctlr, pPmacCtlr->vmebusBase);
      return (status);
   }

   status = devReadProbe (sizeof(char), (void *) &pPmacCtlr->pBase->mailbox.MB[0].data, (char *)&val);
   if (status != OK)
   {
      epicsPrintf ("%s: card %d: Failure probing for base address.\n", MyName, cardNumber);
      return (status);
   }

   epicsPrintf("PMAC #%d: regs at vme addr 0x%lx (local addr %p)\n", 
               cardNumber, pPmacCtlr->vmebusBase, pPmacCtlr->pBase);

   pPmacCtlr->enabled       = FALSE;
   pPmacCtlr->present       = FALSE;
   pPmacCtlr->active        = FALSE;
   pPmacCtlr->enabledBase   = TRUE;
   pPmacCtlr->presentBase   = TRUE;
   pPmacCtlr->activeBase    = FALSE;
   pPmacCtlr->enabledDpram  = TRUE; /*MRIPPA TEST TRUE...*/
   pPmacCtlr->presentDpram  = FALSE;
   pPmacCtlr->activeDpram   = FALSE;
   pPmacCtlr->enabledGather = TRUE; /*MRIPPA TEST TRUE...*/
   pPmacCtlr->activeGather  = FALSE;

   if ( pPmacCtlr->enabledDpram ) {
      status = devRegisterAddress ("PMAC DPRAM", atVMEA24, pPmacCtlr->vmebusDpram,
                PMAC_MEM_SIZE_DPRAM, (void *) &(pPmacCtlr->pDpramBase));
      if (!RTN_SUCCESS(status))
      {
          errlogPrintf ("%s: card %d:Failure registering controller %d DPRAM address A24 %#010lx.\n",
                   MyName, cardNumber, pPmacCtlr->ctlr, pPmacCtlr->vmebusDpram);
          return (status);
      }

      block = (char) ((pPmacCtlr->vmebusDpram & 0x000fc000) >> 14);
      pBlock = ((char *) pPmacCtlr->pBase) + 0x121;

      PMAC_DEBUG (1, 
                  PMAC_MESSAGE ("%s: Setting DPRAM mapping addr %#010lx val %d\n", 
                                MyName, (long unsigned int)pBlock, block);
      )

      *pBlock = block;

      status = devReadProbe ( sizeof(char), (char *) pPmacCtlr->pDpramBase, (char*)&val);
      if (status != OK)
      {
         epicsPrintf ("%s: card %d: Failure probing for DPRAM address.\n", MyName, cardNumber);
         return (status);
      }
      pPmacCtlr->presentDpram = TRUE;

      epicsPrintf("PMAC #%d: DPRAM present at vme A24 addr 0x%lx (local addr %p)\n", 
               cardNumber, pPmacCtlr->vmebusDpram, pPmacCtlr->pDpramBase);

   }

   pPmacCtlr->ioMbxReceiptSem = epicsEventMustCreate (epicsEventEmpty);
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioMbxReceiptSem = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioMbxReceiptSem);
   )

   if ( pPmacCtlr->ioMbxReceiptSem == NULL)
   {
      status = S_dev_internal;
      epicsPrintf ("%s: card %d: Failure creating binary semaphore (ioMbxReceiptSem).\n", MyName, cardNumber);
      return (status);
   }

   pPmacCtlr->ioMbxReadmeSem = epicsEventMustCreate (epicsEventEmpty);
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioMbxReadmeSem = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioMbxReadmeSem);
   )
   if ( pPmacCtlr->ioMbxReceiptSem == NULL)
   {
      status = S_dev_internal;
      epicsPrintf ("%s: card %d: Failure creating binary semaphore (ioMbxReadmeSem).\n", 
                     MyName, cardNumber);
      return (status);
   }

   pPmacCtlr->ioMbxLockMtx = epicsMutexMustCreate();
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioMbxLockMtx = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioMbxLockMtx);
   )


   pPmacCtlr->ioAscReadmeSem = epicsEventMustCreate (epicsEventEmpty);
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioAscReadmeSem = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioAscReadmeSem);
   )

   if ( pPmacCtlr->ioAscReadmeSem == NULL)
   {
      status = S_dev_internal;
      epicsPrintf ("%s: card %d: Failure creating binary semaphore (ioAscReadmeSem).\n", 
              MyName, cardNumber);
      return (status);
   }

   pPmacCtlr->ioAscLockMtx = epicsMutexMustCreate();
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioAscLockMtx = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioAscLockMtx);
   )


   pPmacCtlr->ioGatBufferSem = epicsEventMustCreate (epicsEventEmpty);
   PMAC_DEBUG (1, 
              PMAC_MESSAGE ("%s: card %d, ioGatBufferSem = %p\n", 
                                MyName, cardNumber, pPmacCtlr->ioGatBufferSem);
   )

   if ( pPmacCtlr->ioGatBufferSem == NULL)
   {
      status = S_dev_internal;
      epicsPrintf ("%s: card %d: Failure creating binary semaphore (ioGatBufferSem).\n",
                    MyName, cardNumber);
      return (status);
   }

   status = devConnectInterruptVME (pPmacCtlr->irqVector-1, pmacMbxReceiptISR, (void *)pPmacCtlr);
   if (!RTN_SUCCESS(status))
   {
      epicsPrintf ("%s: card %d: Failure to connect interrupt vector 0x%02x for pmacMbxReceiptISR()\n", 
                   MyName, cardNumber, pPmacCtlr->irqVector-1);
      return (status);
   }
   PMAC_DEBUG(1,
        PMAC_MESSAGE ("%s: card %d: Connected to interrupt vector 0x%02x for pmacMbxReceiptISR()\n", 
                  MyName, cardNumber, pPmacCtlr->irqVector - 1);
   )


   status = devConnectInterruptVME (pPmacCtlr->irqVector, pmacMbxReadmeISR, (void *)pPmacCtlr);
   if (!RTN_SUCCESS(status))
   {
      epicsPrintf ("%s: card %d: Failure to connect interrupt vector 0x%02x for pmacMbxReadmeISR()\n", 
                      MyName, cardNumber, pPmacCtlr->irqVector);
      return (status);
   }
   PMAC_DEBUG(1,
        PMAC_MESSAGE ("%s: card %d: Connected to interrupt vector 0x%02x for pmacMbxReadmeISR()\n", 
                    MyName, cardNumber, pPmacCtlr->irqVector);
   )

   status = devConnectInterruptVME(pPmacCtlr->irqVector + 1, pmacAscInISR, (void *)pPmacCtlr);
   if (!RTN_SUCCESS(status))
   {
      epicsPrintf ("%s: card %d: Failure to connect interrupt vector 0x%02x for pmacAscInISR()\n", 
           MyName, cardNumber, pPmacCtlr->irqVector+1);
      return (status);
   }
   PMAC_DEBUG(1,
              PMAC_MESSAGE ("%s: card %d: Connected to interrupt vector 0x%02x for pmacAscInISR()\n", 
                    MyName, cardNumber, pPmacCtlr->irqVector + 1);
   )

   status = devConnectInterruptVME ( pPmacCtlr->irqVector + 2, pmacGatBufferISR, (void *)pPmacCtlr);
   if (!RTN_SUCCESS(status))
   {
      errlogPrintf ("%s: card %d: Failure to connect interrupt vector 0x%02x for pmacGatBufferISR()\n", 
                   MyName, cardNumber, pPmacCtlr->irqVector+2);
      return (status);
   }
   PMAC_DEBUG(1,
        PMAC_MESSAGE ("%s: card %d: Connected to interrupt vector 0x%02x for pmacGatBufferISR()\n", 
                MyName, cardNumber, pPmacCtlr->irqVector + 2);
   )


   status = devEnableInterruptLevelVME (pPmacCtlr->irqLevel);

   PMAC_DEBUG(1,
        PMAC_MESSAGE ("%s: card %d: Enabled interrupt level %d\n", MyName, cardNumber, pPmacCtlr->irqLevel);
   )

   if (!RTN_SUCCESS(status))
   {
      epicsPrintf ("%s: card %d: Failure to enable interrupt level %d.\n", 
                    MyName, cardNumber, pPmacCtlr->irqLevel);
      return (status);
   }

   pPmacCtlr->present    = pPmacCtlr->presentBase | pPmacCtlr->presentDpram;
   pPmacCtlr->enabled    = pPmacCtlr->enabledBase | pPmacCtlr->enabledDpram;
   pPmacCtlr->configured = TRUE;


   pCard              = &drvPmacCard[cardNumber];
   pCard->numSvoIo    = 0;
   pCard->numBkgIo    = 0;
   pCard->numVarIo    = 0;
   pCard->numOpnIo    = 0;

   pCard->scanSvoRate = PMAC_SVO_RATE;
   pCard->scanBkgRate = PMAC_BKG_RATE;
   pCard->scanVarRate = PMAC_VAR_RATE;
   pCard->scanOpnRate = PMAC_OPN_RATE;

   pCard->enabledMbx = TRUE;
   pCard->configured = TRUE;
   pCard->enabledAsc = FALSE;
   pCard->enabledRam = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledSvo = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledBkg = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledVar = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledOpn = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledFld = TRUE; /*MRIPPA TEST TRUE...*/
   pCard->enabledGat = TRUE; /*MRIPPA TEST TRUE...*/
   pmacCardsConfigured++;

   return(0);
}

/*******************************************************************************
 *
 * drvPmac_report - print driver report information
 *
 */
PMAC_LOCAL long drvPmac_report(int level)
{
   short int  i;
   PMAC_CARD  *pCard;

   if (level > 0)
   {
      printf ("PMAC-VME driver: %s\n", drvPmacVersion);
      printf ("Number of cards configured = %d.\n", pmacCardsConfigured);
      for (i = 0; i < PMAC_MAX_CARDS; i++)
      {
         if ( drvPmacCard[i].configured )
         {  
            pCard = &drvPmacCard[i];

            printf ("card = %d  ctlr = %d  present = %d  enabled = %d\n",
            pCard->card, pCard->ctlr, pCard->present, pCard->enabled);
            printf ("    enabledMbx = %d  enabledAsc = %d  enabledRam = %d\n",
            pCard->enabledMbx, pCard->enabledAsc, pCard->enabledRam);
            printf ("    enabledSvo = %d  enabledBkg = %d  enabledVar = %d\n",
            pCard->enabledSvo, pCard->enabledBkg, pCard->enabledVar);
            printf ("    enabledOpn = %d  enabledFld = %d  enabledGat = %d\n",
            pCard->enabledOpn, pCard->enabledFld, pCard->enabledGat);
            printf ("    numSvoIo = %d  numBkgIo = %d  numVarIo = %d  numOpnIo = %d\n",
            drvPmacCard[i].numSvoIo, drvPmacCard[i].numBkgIo,
            drvPmacCard[i].numVarIo, drvPmacCard[i].numOpnIo);
         }
      }
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmac_init - initialize PMAC driver
 *
 */
PMAC_LOCAL long drvPmac_init(void)
{
   int status;

   status = pmacVmeInit ();
   return (status);
}

/*******************************************************************************
 *
 * drvPmacStartup - startup PMAC driver
 *
 */
PMAC_LOCAL long drvPmacStartup(void)
{
   char        *MyName = "drvPmacStartup";
   int         i;

   static long  status = 0;
   static int   oneTimeOnly = 0;

   if (oneTimeOnly != 0) return (status);

   oneTimeOnly = -1;

   for (i = 0; i < PMAC_MAX_CARDS; i++)
   {
      if ( drvPmacCard[i].configured )
      {
         PMAC_DEBUG (1,
                     PMAC_MESSAGE ("%s: Starting tasks for card %d.\n", MyName, i);
         )

         if ( drvPmacCard[i].enabledMbx )
         {
            drvPmacMbxScanInit(i);
         }

         if ( drvPmacCard[i].enabledSvo )
         {
            drvPmacSvoScanInit(i);
         }

         if ( drvPmacCard[i].enabledBkg )
         {
            drvPmacBkgScanInit(i);
         }

         if ( drvPmacCard[i].enabledVar )
         {
            drvPmacVarScanInit(i);
         }

         if ( drvPmacCard[i].enabledOpn )
         {
            drvPmacOpnScanInit(i);
         }

         if ( drvPmacCard[i].enabledFld )
         {
            drvPmacFldScanInit(i);
         }

         if ( drvPmacCard[i].enabledGat )
         {
            drvPmacGatScanInit (i);
         }
      }
   }

   return (status);
}

/*******************************************************************************
 *
 * drvPmacRamDisable - Disable PMAC card DPRAM
 *
 */
long drvPmacRamDisable (int card)
{
   PMAC_CARD   *pCard;

   pCard = &drvPmacCard[card];
   pCard->enabledRam = FALSE;

   return (0);
}

/*******************************************************************************
 *
 * drvPmacRamEnable - Enable PMAC card DPRAM
 *
 */
long drvPmacRamEnable(int card)
{
   PMAC_CARD   *pCard;

   pCard = &drvPmacCard[card];
   pCard->enabledRam = pCard->configured;

   return (0);
}

/*******************************************************************************
 *
 * drvPmacMemSpecParse - parse a PMAC address specification into values
 *
 */
long drvPmacMemSpecParse(char *pmacAdrSpec,int *memType,int *pmacAdr)
{
   char  *MyName = "drvPmacMemSpecParse";
   int   parmCount;

   long   status;
   char   memTypeStr[11];   

   parmCount = 0;
   memTypeStr[0] = '\0';
   *memType = PMAC_MEMTYP_NONE;
   *pmacAdr = 0;

   parmCount = sscanf (pmacAdrSpec, "%[^:]:$%x", memTypeStr, pmacAdr);

   PMAC_DEBUG (2,
               PMAC_MESSAGE ("%s: parse '%s' results parmCount %d\n", MyName, pmacAdrSpec, parmCount);
               PMAC_MESSAGE ("%s: memTypeStr '%s' pmacAdr %x\n", MyName, memTypeStr, *pmacAdr);
   )
      

   if ( (parmCount != 2) )
   {
      status = S_dev_badInit;
      errlogPrintf("%s: Improper address specification '%s' status = %ld\n",
          MyName, pmacAdrSpec, status);
      return (status);
   }

   if ( strcmp (memTypeStr,"Y") == 0)
   {
      *memType = PMAC_MEMTYP_Y;
   }
   else if ( strcmp (memTypeStr,"X") == 0 )
   {
      *memType = PMAC_MEMTYP_X;
   }
   else if ( strcmp (memTypeStr,"SY") == 0 )
   {
      *memType = PMAC_MEMTYP_SY;
   }
   else if ( strcmp (memTypeStr,"SX") == 0 )
   {
      *memType = PMAC_MEMTYP_SX;
   }
   else if ( strcmp (memTypeStr,"HY") == 0 )
   {
      *memType = PMAC_MEMTYP_HY;
   }
   else if ( strcmp (memTypeStr,"HX") == 0 )
   {
      *memType = PMAC_MEMTYP_HX;
   }
   else if ( strcmp (memTypeStr,"DP") == 0 )
   {
      *memType = PMAC_MEMTYP_DP;
   }
   else if ( strcmp (memTypeStr,"F") == 0 )
   {
      *memType = PMAC_MEMTYP_F;
   }
   else if ( strcmp (memTypeStr,"D") == 0 )
   {
      *memType = PMAC_MEMTYP_D;
   }
   else if ( strcmp (memTypeStr,"L") == 0 )
   {
      *memType = PMAC_MEMTYP_L;
   }
   else
   {
      status = S_dev_badInit;
      errPrintf (status, __FILE__, __LINE__,
          "%s: Illegal address type '%s' for '%s'.",
          MyName, memTypeStr, pmacAdrSpec);
      return (status);
   }

   PMAC_DEBUG(2,
              PMAC_MESSAGE ("%s: memType %d\n", MyName, *memType);
   )

   if ( (*pmacAdr < 0) || (*pmacAdr > 0xFFFF) )
   {
      status = S_dev_badInit;
      errPrintf (status, __FILE__, __LINE__,
          "%s: Address %p out of range for '%s'.",
          MyName, pmacAdr, pmacAdrSpec);
      return (status);
   }

   return (0);   
}

/*******************************************************************************
 *
 * drvPmacDpramRequest - add PMAC DPRAM address to scan list
 *
 */
long drvPmacDpramRequest
(
   short   card,
   short   pmacAdrOfs,
   char   *pmacAdrSpec,
   void   (*pFunc)(),
   void   *pParm,
   PMAC_RAM_IO ** ppRamIo,
   int     inputRec
)
{
   char *   MyName = "drvPmacDpramRequest";
   int   i;
   long   status;
   int   hostOfs;

   PMAC_CARD *   pCard;
   PMAC_RAM_IO *   pSvoIo;
   PMAC_RAM_IO *   pBkgIo;
   PMAC_RAM_IO *   pVarIo;
   PMAC_RAM_IO *   pOpnIo;

   int   memType;
   int   pmacAdr;

   pCard = &drvPmacCard[card];

   /* Parse PMAC Address Specification */
   status = drvPmacMemSpecParse (pmacAdrSpec, &memType, &pmacAdr);
   if (!RTN_SUCCESS(status))
   {
      return (status);
   }

   /* Add PMAC Address Offset */
   pmacAdr += (long) pmacAdrOfs;

   /* If Servo Fixed Data Buffer */
   if ( (pmacAdr >= 0xD009) && (pmacAdr < 0xD08A) )
   {
      hostOfs = 4 * (pmacAdr - 0xD000);
      if (memType == PMAC_MEMTYP_HX)
      {
         hostOfs += 2;
      }

      i = pCard->numSvoIo;
      pSvoIo = &pCard->SvoIo[i];
      *ppRamIo = pSvoIo;

      pSvoIo->memType  = memType;
      pSvoIo->pmacAdr  = pmacAdr;
      pSvoIo->hostOfs  = hostOfs;
      pSvoIo->inputRec = inputRec;
      pSvoIo->pAddress = pmacRamAddr(card,pSvoIo->hostOfs);
      pSvoIo->pFunc    = pFunc;
      pSvoIo->pParm    = pParm;

      PMAC_DEBUG(1,
                 PMAC_MESSAGE ("%s: card %d: Servo    -- index %d memType %d hostOfs %x pAddress %p\n",
                                MyName, card, i, pSvoIo->memType, pSvoIo->hostOfs, pSvoIo->pAddress);
      )

      pCard->numSvoIo++;
   }

   /* If Background Fixed Data Buffer */
   else if ( (pmacAdr >= 0xD08A) && (pmacAdr < 0xD18B) )
   {
      hostOfs = 4 * (pmacAdr - 0xD000);
      if (memType == PMAC_MEMTYP_HX)
      {
         hostOfs += 2;
      }

      i = pCard->numBkgIo;
      pBkgIo = &pCard->BkgIo[i];
      *ppRamIo = pBkgIo;

      pBkgIo->memType  = memType;
      pBkgIo->pmacAdr  = pmacAdr;
      pBkgIo->hostOfs  = hostOfs;
      pBkgIo->inputRec = inputRec;
      pBkgIo->pAddress = pmacRamAddr(card,pBkgIo->hostOfs);
      pBkgIo->pFunc    = pFunc;
      pBkgIo->pParm    = pParm;

      PMAC_DEBUG(1,
                 PMAC_MESSAGE ("%s: card %d: BkgFixed -- index %d memType %d hostOfs %x pAddress %p\n",
                               MyName, card, i, pBkgIo->memType, pBkgIo->hostOfs, pBkgIo->pAddress);
      )

      pCard->numBkgIo++;
   }

   /* If Background Variable Data Buffer -- Outside of DPRAM */
   else if ( (pmacAdr < 0xD000) || (pmacAdr > 0xDFFF) )
   {
      i = pCard->numVarIo;
      pVarIo = &pCard->VarIo[i];
      *ppRamIo = pVarIo;

      pVarIo->memType  = memType;
      pVarIo->pmacAdr  = pmacAdr;
      pVarIo->hostOfs  = 0;      /* Unknown at this time */
      pVarIo->inputRec = inputRec;
      pVarIo->pAddress = pmacRamAddr(card,pVarIo->hostOfs);
      pVarIo->pFunc    = pFunc;
      pVarIo->pParm    = pParm;

      PMAC_DEBUG(1,
                 PMAC_MESSAGE ("%s: card %d: BkgVar   -- index %d memType %d\n",
                               MyName, card, i, pVarIo->memType);
      )

      pCard->numVarIo++;
   }

   /* If Control Panel Function Or Open Use DPRAM */
   else if ( ((pmacAdr >= 0xD000) && (pmacAdr < 0xD009)) ||
             ((pmacAdr >= 0xDF00) && (pmacAdr < 0xE000)) )
   {
      hostOfs = 4 * (pmacAdr - 0xD000);
      if (memType == PMAC_MEMTYP_HX)
      {
         hostOfs += 2;
      }

      i = pCard->numOpnIo;
      pOpnIo = &pCard->OpnIo[i];
      *ppRamIo = pOpnIo;

      pOpnIo->memType  = memType;
      pOpnIo->pmacAdr  = pmacAdr;
      pOpnIo->hostOfs  = hostOfs;
      pOpnIo->pAddress = pmacRamAddr(card,pOpnIo->hostOfs);
      pOpnIo->pFunc    = pFunc;
      pOpnIo->pParm    = pParm;

      PMAC_DEBUG(1,
                 PMAC_MESSAGE ("%s: card %d: OpenUse  -- index %d memType %d hostOfs %x pAddress %p\n",
                                MyName, card, i, pOpnIo->memType, pOpnIo->hostOfs, pOpnIo->pAddress);
      )

      if( inputRec )      /* Only interested in scanning input records */
         pOpnIo->inputRec = 1;
      else
         pOpnIo->inputRec = 0;

      pCard->numOpnIo++;
   }
   else
   {
      status = S_dev_badRequest;
      errPrintf (status, __FILE__, __LINE__,
                 "%s: card %d: Unsupported DPRAM address range %#06x.",
                  MyName, card, pmacAdr);
      return (status);
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmacVarSetup - enable operation of background variable data buffer
 *
 */
long drvPmacVarSetup(int card)
{
   char *   MyName = "drvPmacVarSetup";
   int   i;
   long   status;
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pVarIo;
   int   hostOfs;   
   int   configOfs;
   long   configFormat   = PMAC_VARTYP_NONE;

   /* Start Config Table At 0xDD00 */
   /* Maximum 128 addresses + 256 Data Words */
   /* Maximum end of data at 0xDE80 */

   /* Determine First Data Location */
   configOfs = 4 * (0xDD00 - 0xD000);
   hostOfs = 4 * (0xDD00 + pCard->numVarIo - 0xD000);

   /* Set Size Of Buffer To Zero Before Changing Buffer Configuration */
   status = pmacRamPutH ( pmacRamAddr(card,0x07ec), 0);

   /* Configure Starting Address Of Buffer */
   status = pmacRamPutH ( pmacRamAddr(card,0x07ee), 0xDD00);

   /* For Each Background Variable */
   for (i=0; i < pCard->numVarIo; i++)
   {
      /* Determine Location */
      pVarIo = &pCard->VarIo[i];
      pVarIo->hostOfs = hostOfs;
      pVarIo->pAddress = pmacRamAddr(card,pVarIo->hostOfs);

      /* Determine Memory Format */
      switch (pVarIo->memType)
      {
          case (PMAC_MEMTYP_Y) :
          case (PMAC_MEMTYP_SY) :
             configFormat = PMAC_VARTYP_Y;
             break;
          case (PMAC_MEMTYP_X) :
          case (PMAC_MEMTYP_SX) :
             configFormat = PMAC_VARTYP_X;
             break;
          case (PMAC_MEMTYP_D) :
          case (PMAC_MEMTYP_L) :
             configFormat = PMAC_VARTYP_L;
             break;
          default :
             status = ERROR;
             errPrintf (status, __FILE__, __LINE__,
                       "%s: Illegal address type '%d'.",
                        MyName, pVarIo->memType);
             return (status);
             break;
      }

      /* Configure PMAC Address To Be Copied Into Buffer */ 
      status = pmacRamPutH ( pmacRamAddr(card,configOfs), pVarIo->pmacAdr );
      status = pmacRamPutH ( pmacRamAddr(card,configOfs + 2), configFormat );

      PMAC_DEBUG(1,
                 PMAC_MESSAGE ("%s: configFormat %ld memType %d hostOfs %x pAddress %p\n",
                               MyName, configFormat, pVarIo->memType, pVarIo->hostOfs, pVarIo->pAddress);
      )

      /* Determine Location Of Next Variable */
      configOfs += 4;
      if (configFormat == PMAC_VARTYP_L)
      {
         hostOfs += 8;
      }
      else
      {
         hostOfs += 4;
      }
   }

   /* Clear Data Ready Bit For Next Data Fill */
   status = pmacRamPutH ( pmacRamAddr(card,0x07e8), 0);

   /* Configure Size Of Buffer */
   status = pmacRamPutH ( pmacRamAddr(card,0x07ec), pCard->numVarIo);

   return (0);
}

/*******************************************************************************
 *
 * drvPmacSvoRead - read fixed servo data buffer
 *
 */
long drvPmacSvoRead(int card)
{
   char *   MyName = "drvPmacSvoRead";
   int   i;
   //long   status;
   long   pmacStatus;
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pSvoIo;

   /* Set Host Busy Bit */
   //status = pmacRamPutH ( pmacRamAddr(card,0x024), 1 );
   pmacRamPutH ( pmacRamAddr(card,0x024), 1 );
   do
   {
      /* Check PMAC Busy Bit */
      //status = pmacRamGetH ( pmacRamAddr(card,0x026), &pmacStatus );
      pmacRamGetH ( pmacRamAddr(card,0x026), &pmacStatus );

      PMAC_DEBUG(5,
                 PMAC_MESSAGE ("%s: PMAC waiting status %ld\n", MyName, pmacStatus);
      )

   } while ( (pmacStatus & 0x00008000) != 0);

   /* Read PMAC Servo Fixed Data Buffer */
   for (i=0; i < pCard->numSvoIo; i++)
   {
      pSvoIo = &pCard->SvoIo[i];
      //status = drvPmacRamGetData (pSvoIo);
      drvPmacRamGetData (pSvoIo);
   }

   /* Clear Host Busy Bit */
   //status = pmacRamPutH ( pmacRamAddr(card,0x024), 0);
   pmacRamPutH ( pmacRamAddr(card,0x024), 0);

   /* Notify Requester Of New Data */
   for (i=0; i < pCard->numSvoIo; i++)
   {
      if ( pCard->SvoIo[i].pFunc != (void *) NULL )
      {
         (*pCard->SvoIo[i].pFunc)(pCard->SvoIo[i].pParm);
      }
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmacBkgRead - read PMAC card background fixed data buffer
 *
 */
long drvPmacBkgRead(int card)
{
   char *   MyName = "drvPmacBkgRead";
   int   i;
   //long   status;
   long   pmacStatus;
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pBkgIo;

   /* Check for PMAC Data Ready */
   //status = pmacRamGetH ( pmacRamAddr(card,0x0228), &pmacStatus );
   pmacRamGetH ( pmacRamAddr(card,0x0228), &pmacStatus );

   PMAC_DEBUG(5,
              PMAC_MESSAGE ("%s: PMAC status %ld\n", MyName, pmacStatus);
   )

   /* If No Data Ready Then Return Without Reading */
   if ( (pmacStatus & 0x00000001) != 1)
   {
      return (0);
   }

   PMAC_DEBUG(4,
              PMAC_MESSAGE ("%s: PMAC Background Read triggered, status = %ld\n", MyName, pmacStatus);
   )

   /* Read PMAC Background Fixed Data Buffer */
   for (i=0; i<pCard->numBkgIo; i++)
   {
      pBkgIo = &pCard->BkgIo[i];
      //status = drvPmacRamGetData (pBkgIo);
      drvPmacRamGetData (pBkgIo);
   }

   /* Clear Data Ready Bit For Next Data */
   //status = pmacRamPutH ( pmacRamAddr(card,0x0228), 0);
   pmacRamPutH ( pmacRamAddr(card,0x0228), 0);

   /* Notify Requester Of New Data */
   for (i=0; i < pCard->numBkgIo; i++)
   {
      if ( pCard->BkgIo[i].pFunc != (void *) NULL )
      {
         (*pCard->BkgIo[i].pFunc)(pCard->BkgIo[i].pParm);
      }
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmacVarRead - read PMAC card background fixed data buffer
 */
long drvPmacVarRead(int card)
{
   char *   MyName = "drvPmacVarRead";
   int   i;
   //long   status;    warning: variable 'status' set but not used [-Wunused-but-set-variable]
   long   pmacStatus;
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pVarIo;

   /* Check For PMAC Data Ready */
   //status = pmacRamGetH ( pmacRamAddr(card,0x07E8), &pmacStatus );
   pmacRamGetH ( pmacRamAddr(card,0x07E8), &pmacStatus );

   PMAC_DEBUG(5,
              PMAC_MESSAGE ("%s: PMAC status %ld\n", MyName, pmacStatus);
   )

   /* If No Data Ready Then Return Without Reading */
   if ( (pmacStatus & 0x0001) != 1)
   {
      return (0);
   }

   /* Read PMAC Background Variable Data Buffer */
   for (i=0; i<pCard->numVarIo; i++)
   {
      pVarIo = &pCard->VarIo[i];
      //status = drvPmacRamGetData (pVarIo);
      drvPmacRamGetData (pVarIo);
   }

   /* Clear PMAC Data Ready Bit For Next Data */
   //status = pmacRamPutH ( pmacRamAddr(card,0x07E8), 0);
   pmacRamPutH ( pmacRamAddr(card,0x07E8), 0);

   /* Notify Requester Of New Data */
   for (i=0; i < pCard->numVarIo; i++)
   {
      if ( pCard->VarIo[i].pFunc != (void *) NULL )
      {
         (*pCard->VarIo[i].pFunc)(pCard->VarIo[i].pParm);
      }
   }

   return (0);
}

/*******************************************************************************
 *
 * pmacSvoShow - print servo fixed data scan information
 *
 */
int pmacSvoShow(int card,int index)
{
   char *      MyName = "pmacSvoShow";
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pSvoIo   = &pCard->SvoIo[index];

   printf ("%s: memType %d pmacAdr %X \n",
            MyName,
            pSvoIo->memType,
            pSvoIo->pmacAdr);
   printf ("%s: hostOfs %#x pAddress %#010x\n", 
            MyName, 
            pSvoIo->hostOfs, 
            (unsigned int) (uintptr_t) pSvoIo->pAddress);
   printf ("%s: valLong %ld valDouble %f\n",
            MyName,
            pSvoIo->valLong,
            pSvoIo->valDouble );
   printf ("%s: pFunc %#010x pParm %#010x\n",
            MyName,
            (unsigned int) (uintptr_t) pSvoIo->pFunc,
            (unsigned int) (uintptr_t) pSvoIo->pParm );

   return (0);
}


/*******************************************************************************
 *
 * pmacBkgShow - print background scan information
 *
 */
int pmacBkgShow(int card,int index)
{
   char *      MyName = "pmacBkgShow";
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pBkgIo   = &pCard->BkgIo[index];

   printf ("%s: memType %d pmacAdr %X \n",
            MyName,
            pBkgIo->memType,
            pBkgIo->pmacAdr);
   printf ("%s: hostOfs %#x pAddress %#010x\n",
            MyName,
            pBkgIo->hostOfs,
            (unsigned int)(uintptr_t) pBkgIo->pAddress);
   printf ("%s: valLong %ld valDouble %f\n",
            MyName,
            (long) (intptr_t) pBkgIo->valLong,
            pBkgIo->valDouble );
   printf ("%s: pFunc %#010x pParm %#010x\n",
            MyName,
            (unsigned int) (uintptr_t) pBkgIo->pFunc,
            (unsigned int) (uintptr_t)pBkgIo->pParm );

   return (0);
}


/*******************************************************************************
 *
 * pmacVarShow - print background scan information
 *
 */
int pmacVarShow(int card,int index)
{
   char *      MyName = "pmacVarShow";
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pVarIo   = &pCard->VarIo[index];

   printf ("%s: memType %d pmacAdr %X \n",
            MyName,
            pVarIo->memType,
            pVarIo->pmacAdr);
   printf ("%s: hostOfs %#x pAddress %#010x\n",
            MyName,
            pVarIo->hostOfs,
            (unsigned int) (uintptr_t) pVarIo->pAddress);
   printf ("%s: valLong %ld valDouble %f\n",
           MyName,
           pVarIo->valLong,
           pVarIo->valDouble );
   printf ("%s: pFunc %#010x pParm %#010x\n",
           MyName,
           (unsigned int) (uintptr_t) pVarIo->pFunc,
           (unsigned int) (uintptr_t) pVarIo->pParm );

   return (0);
}

/*******************************************************************************
 *
 * pmacOpnShow - print background scan information
 *
 */
int pmacOpnShow(int card,int index)
{
   char *      MyName = "pmacOpnShow";
   PMAC_CARD *   pCard   = &drvPmacCard[card];
   PMAC_RAM_IO *   pOpnIo   = &pCard->OpnIo[index];

   printf ("%s: memType %d pmacAdr %X \n",
           MyName,
           pOpnIo->memType,
           pOpnIo->pmacAdr);
   printf ("%s: hostOfs %#x pAddress %#010x\n",
           MyName,
           pOpnIo->hostOfs,
           (unsigned int) (uintptr_t) pOpnIo->pAddress);
   printf ("%s: valLong %ld valDouble %f\n",
           MyName,
           pOpnIo->valLong,
           pOpnIo->valDouble );
   printf ("%s: pFunc %#010x pParm %#010x\n",
           MyName,
           (unsigned int) (uintptr_t) pOpnIo->pFunc,
           (unsigned int) (uintptr_t) pOpnIo->pParm );

   return (0);
}


/*******************************************************************************
 *
 * drvPmacRamGetData - read data from PMAC DPRAM
 *
 */
PMAC_LOCAL long drvPmacRamGetData(PMAC_RAM_IO *pRamIo)
{
   // long   status;  warning: variable 'status' set but not used [-Wunused-but-set-variable]

   switch (pRamIo->memType)
   {
      case (PMAC_MEMTYP_Y) :
      case (PMAC_MEMTYP_X) :
         //status = pmacRamGetY (pRamIo->pAddress, &pRamIo->valLong);
         pmacRamGetY (pRamIo->pAddress, &pRamIo->valLong);
         pRamIo->valDouble = (double) pRamIo->valLong;
         break;
      case (PMAC_MEMTYP_SY) :
      case (PMAC_MEMTYP_SX) :
         //status = pmacRamGetSY (pRamIo->pAddress, &pRamIo->valLong);
         pmacRamGetSY (pRamIo->pAddress, &pRamIo->valLong);
         pRamIo->valDouble = (double) pRamIo->valLong;
         break;
      case (PMAC_MEMTYP_HY) :
      case (PMAC_MEMTYP_HX) :
         //status = pmacRamGetH (pRamIo->pAddress, &pRamIo->valLong);
         pmacRamGetH (pRamIo->pAddress, &pRamIo->valLong);
         pRamIo->valDouble = (double) pRamIo->valLong;
         break;
      case (PMAC_MEMTYP_DP) :
         //status = pmacRamGetDP (pRamIo->pAddress, &pRamIo->valLong);
         pmacRamGetDP (pRamIo->pAddress, &pRamIo->valLong);
         pRamIo->valDouble = (double) pRamIo->valLong;
         break;
      case (PMAC_MEMTYP_F) :
         //status = pmacRamGetF (pRamIo->pAddress, &pRamIo->valDouble);
         pmacRamGetF (pRamIo->pAddress, &pRamIo->valDouble);
         pRamIo->valLong = 0;
         break;
      case (PMAC_MEMTYP_D) :
         //status = pmacRamGetD (pRamIo->pAddress, &pRamIo->valDouble);
         pmacRamGetD (pRamIo->pAddress, &pRamIo->valDouble);
         pRamIo->valLong = 0;
         break;
      case (PMAC_MEMTYP_L) :
         //status = pmacRamGetL (pRamIo->pAddress, &pRamIo->valDouble);
         pmacRamGetL (pRamIo->pAddress, &pRamIo->valDouble);
         pRamIo->valLong = 0;
         break;
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmacRamPutData - write data to PMAC DPRAM
 *
 */
PMAC_LOCAL long drvPmacRamPutData(PMAC_RAM_IO *pRamIo)
{
   //long   status;   warning: variable 'status' set but not used [-Wunused-but-set-variable]

   switch (pRamIo->memType)
   {
      case (PMAC_MEMTYP_Y) :
      case (PMAC_MEMTYP_X) :
         //status = pmacRamPutY (pRamIo->pAddress, pRamIo->valLong);
         pmacRamPutY (pRamIo->pAddress, pRamIo->valLong);
         break;
      case (PMAC_MEMTYP_SY) :
      case (PMAC_MEMTYP_SX) :
         //status = pmacRamPutSY (pRamIo->pAddress, pRamIo->valLong);
         pmacRamPutSY (pRamIo->pAddress, pRamIo->valLong);
         break;
      case (PMAC_MEMTYP_HY) :
      case (PMAC_MEMTYP_HX) :
         //status = pmacRamPutH (pRamIo->pAddress, pRamIo->valLong);
         pmacRamPutH (pRamIo->pAddress, pRamIo->valLong);
         break;
      case (PMAC_MEMTYP_DP) :
         //status = pmacRamPutDP (pRamIo->pAddress, pRamIo->valLong);
         pmacRamPutDP (pRamIo->pAddress, pRamIo->valLong);
         break;
      case (PMAC_MEMTYP_F) :
         //status = pmacRamPutF (pRamIo->pAddress, pRamIo->valDouble);
         pmacRamPutF (pRamIo->pAddress, pRamIo->valDouble);
         break;
   }

   return (0);
}

/*******************************************************************************
 *
 * drvPmacMbxWriteRead - write command and read response in PMAC mailbox
 *
 */
char drvPmacMbxWriteRead
(
   int    card,
   char   *writebuf,
   char   *readbuf,
   char   *errmsg
)
{
   char terminator;
   char buffer[PMAC_MBX_IN_BUFLEN];

   if( !drvPmacCard[card].configured )
   {
      errlogPrintf("drvPmacMbxWriteRead card %d is not configured\n", card);
      return 0;
   }

   
   PMAC_DEBUG( 1,
      PMAC_MESSAGE("drvPmacMbxWriteRead: card = %d, writebuf = %s\n", card, writebuf);
   )
   pmacMbxLock(card);
   terminator = pmacMbxWrite (card, writebuf);
   terminator = pmacMbxRead (card, readbuf, errmsg);
   while ( terminator == PMAC_TERM_CR )
   {
      terminator = pmacMbxRead (card, buffer, errmsg);
   }

   pmacMbxUnlock(card);
   PMAC_DEBUG(1, 
       PMAC_MESSAGE("drvPmacMbxWriteRead: card = %d, readbuf = %s, errmsg = %s\n", card, readbuf, errmsg);
   )

   return (terminator);
}

/*******************************************************************************
 *
 * drvPmacMbxScan - put PMAC MBX request on queue
 *
 */
void drvPmacMbxScan(PMAC_MBX_IO *pMbxIo)
{
   char *   MyName = "drvPmacMbxScan";
   PMAC_CARD *   pCard;

   pCard = &drvPmacCard[pMbxIo->card];

   if( !pCard->configured )
   {
      errlogPrintf("drvPmacMbxScan: card %d is not configured\n", pMbxIo->card);
      return;
   }

   if ( !epicsRingPointerPush(pCard->scanMbxQ,(void *)pMbxIo)  )
   {
      errlogPrintf("drvPmacMbxScan: epicsRingPointerPush overflow.");
   }

   PMAC_DEBUG(6,
              PMAC_MESSAGE ("%s: epicsRingPointerPush completed (card number = %d)\n", MyName, pCard->card);
   )

   epicsEventSignal (pCard->scanMbxSem);
   return;
}

/*******************************************************************************
 *
 * drvPmacMbxScanInit - initialize PMAC MBX scan task
 *
 */
PMAC_LOCAL void drvPmacMbxScanInit(int card)
{
   PMAC_CARD *pCard = &drvPmacCard[card];

   pCard->scanMbxQ = epicsRingPointerCreate(sizeof (void *) * PMAC_MBX_QUEUE_SIZE);

   if ( pCard->scanMbxQ == NULL )
   {
      errlogPrintf ("drvPmacMbxScanInit: epicsRingPointerCreate failed");
      epicsExit(1);
   }

   pCard->scanMbxSem = epicsEventMustCreate(epicsEventEmpty);

   sprintf ( pCard->scanMbxTaskName, "%s%d", PMAC_MBX_SCAN, pCard->card);
   pCard->scanMbxTaskId = epicsThreadCreate (pCard->scanMbxTaskName,
                                             PMAC_MBX_PRI, PMAC_MBX_STACK,
                                             (EPICSTHREADFUNC)drvPmacMbxTask,
                                             pCard);
   taskwdInsert (pCard->scanMbxTaskId, NULL, NULL);

   return;
}

/*******************************************************************************
 *
 * drvPmacMbxTask - task for PMAC MBX input/output
 *
 */
EPICSTHREADFUNC drvPmacMbxTask(PMAC_CARD *pCard)     /* MDW OSI work 20160321 */
{
   char *MyName = "drvPmacMbxTask";
   PMAC_MBX_IO *pMbxIo;

   FOREVER
   {
      PMAC_DEBUG(6,
                  PMAC_MESSAGE ("%s: Waiting on Semaphore\n", MyName);
      )

      epicsEventMustWait(pCard->scanMbxSem);

      while(epicsRingPointerGetUsed(pCard->scanMbxQ)) {
         pMbxIo = (PMAC_MBX_IO *)epicsRingPointerPop(pCard->scanMbxQ);
         PMAC_DEBUG(2,
                     PMAC_MESSAGE ("%s: rngBufGet completed.\n", MyName);
                     PMAC_MESSAGE ("%s: card=%d command=[%s]\n", 
                                   MyName, pMbxIo->card, pMbxIo->command);
         )

         pMbxIo->terminator = drvPmacMbxWriteRead (pMbxIo->card,
                                                   pMbxIo->command, pMbxIo->response,
                                                   pMbxIo->errmsg);

         if ( pMbxIo->terminator == PMAC_TERM_BELL )
         {
            PMAC_MESSAGE ("%s: PMAC Error=[%s]\n", 
                          MyName, pMbxIo->errmsg);
            PMAC_MESSAGE ("%s: card=%d command=[%s]\n",
                          MyName, pMbxIo->card, pMbxIo->command);
            PMAC_MESSAGE ("%s: response=[%s]\n", 
                           MyName, pMbxIo->response);
         }

         PMAC_DEBUG(2,
                    PMAC_MESSAGE ("%s: response=[%s]\n", MyName, pMbxIo->response);
         )

         callbackRequest (&pMbxIo->callback);

         PMAC_DEBUG(2,
                    PMAC_MESSAGE ("%s: Callback requested.\n", MyName);
         )
      } 
   }
}


/*******************************************************************************
 *
 * drvPmacFldScan - put PMAC file request on queue
 *
 */
void drvPmacFldScan(PMAC_MBX_IO *pMbxIo)
{
   char *   MyName = "drvPmacFldScan";
   PMAC_CARD *   pCard;

   pCard = &drvPmacCard[pMbxIo->card];

   if ( epicsRingPointerPush(pCard->scanFldQ,(void *)pMbxIo) != 1  )
   {
      errlogPrintf ("drvPmacFldScan: epicsRingPointerPush failure.");
   }

   PMAC_DEBUG(1,
              PMAC_MESSAGE ("%s: epicsRingPointerPush completed for card %d.\n", MyName, pCard->card);
              PMAC_MESSAGE ("%s: pMbxIo = %p\n", MyName, pMbxIo);
   )

   epicsEventSignal(pCard->scanFldSem);
   return;
}

/*******************************************************************************
 *
 * drvPmacFldScanInit - initialize PMAC file scan task
 *
 */
PMAC_LOCAL void drvPmacFldScanInit(int card)
{
   PMAC_CARD *   pCard = &drvPmacCard[card];

   pCard->scanFldQ = epicsRingPointerCreate(PMAC_FLD_QUEUE_SIZE);

   if ( pCard->scanFldQ == NULL )
   {
      errlogPrintf("drvPmacFldScanInit: epicsRingPointerCreate() failed");
      epicsExit(1);
   }

   pCard->scanFldSem = epicsEventMustCreate(epicsEventEmpty);

   sprintf ( pCard->scanFldTaskName, "%s%d", PMAC_FLD_SCAN, pCard->card);
   pCard->scanFldTaskId = epicsThreadCreate(pCard->scanFldTaskName,
                                             PMAC_FLD_PRI, PMAC_FLD_STACK,
                                             (EPICSTHREADFUNC)drvPmacFldTask,
                                             (void *)(&pCard->card) );
   taskwdInsert (pCard->scanFldTaskId, NULL, NULL);

   return;
}

/*******************************************************************************
 *
 * drvPmacFldLoop - write command and read response from files
 *
 */
long drvPmacFldLoop
(
   int    card,
   char   *download,
   char   *upload,
   char   *message
)
{
   int   status;
   char  terminator;
   long  err;
   int   exitNow;
   FILE *fpDownload;
   FILE *fpUpload;

   char  textline[FILE_TEXT_BUFLEN];
   char  command[PMAC_MBX_OUT_BUFLEN];
   char  response[PMAC_MBX_IN_BUFLEN];
   char  errmsg[PMAC_MBX_ERR_BUFLEN];
   char *MyName = "drvPmacFldLoop";

   /* Open Download File */
   fpDownload = fopen (download, "r");
   if (fpDownload == (FILE *) NULL)
   {
      sprintf (message, "FILEDOWN");
      errlogPrintf("%s: could not open download file %s\n", MyName, download);
      return (ERROR);
   }

   /* Open Upload File */
   fpUpload = fopen (upload, "w");
   if (fpUpload == (FILE *) NULL)
   {
      sprintf (message, "FILEUP");
      errlogPrintf("%s: could not open upload file %s\n", MyName, upload);
      return (ERROR);
   }

   exitNow    = FALSE;
   terminator = 0;
   err        = 0;

   //terminator = drvPmacMbxWriteRead( card, "CLOSE\n", response, errmsg );


   if( terminator == PMAC_TERM_BELL )
      return (ERROR);

   /* Get Command String */
   if(!(status = (int) (intptr_t) fgets (textline, FILE_TEXT_BUFLEN - 1, fpDownload)))
      exitNow = TRUE;
   /* Loop Until Exit */
   while( !exitNow )
   {
printf("FldLoop: in while() loop\n"); fflush(stdout);
      if( (textline[0] != ';') && (textline[0] != '\n') && (textline[0] != '\r') )
      {
         sscanf(textline, "%79[^\r;]", command);

         PMAC_DEBUG(1,
              PMAC_MESSAGE("drvPmacFldLoop: card %d: Sent to PMAC: command=[%s]\n", card, command);
         )

         terminator = drvPmacMbxWriteRead( card, command, response, errmsg );


/* 20170131 MDW -- There seems to be something missing here, namely writing the response (such as a program listing) to the upload file */
         PMAC_DEBUG( 1,
              PMAC_MESSAGE("drvPmacFldLoop: card %d: This is where I'm supposed to be  writing out to the upload file\n", card);)

         if( terminator == PMAC_TERM_BELL )
         {
            fprintf (fpUpload, "[%s]\n", errmsg);
            sprintf (message, "%s", errmsg);

            errlogPrintf("drvPmacFldLoop: card %d: Error message from PMAC: %s\n", card, errmsg);

            exitNow = TRUE;
            err     = 1;
         }
      }
      else
        errlogPrintf("drvPmacFldLoop: card %d: Command rejected...\n", card);

      if( !exitNow )
      {
         /* Get Next Command */
            if(!(status = (int) (intptr_t) fgets (textline, FILE_TEXT_BUFLEN - 1, fpDownload)))
               exitNow = TRUE;
      }
printf("FldLoop: next command - %s, status=%d, exitNow=%d\n", textline, status, exitNow);
   }

   /* Send Final String To PMAC */

   //terminator = drvPmacMbxWriteRead( card, "CLOSE\n", response, errmsg );
   if( terminator == PMAC_TERM_BELL )
      return (ERROR);

   status = fclose (fpDownload);
   if(status == ERROR)
   {
      sprintf (message, "FILEDOWN");
      return (ERROR);
   }
   status = fclose (fpUpload);
   if(status == ERROR)
   {
      sprintf (message, "FILEUP");
      return (ERROR);
   }

   if( !err )
   {
      sprintf(message, "OK");
      return(OK);
   }
   else
      return(ERROR);
}

/*******************************************************************************
 *
 * drvPmacFldTask - task for PMAC file input/output
 *
 */

EPICSTHREADFUNC drvPmacFldTask(void *c)              /* MDW OSI work 20160321 */
{
   int   card = *(int *)c;
   char *MyName = "drvPmacFldTask";
   int   status;

   PMAC_CARD *pCard = &drvPmacCard[card];

   PMAC_MBX_IO *pMbxIo;

   FOREVER
   {
      epicsEventMustWait(pCard->scanFldSem);

      while (epicsRingPointerGetUsed(pCard->scanFldQ) )
      {
         if( (pMbxIo = epicsRingPointerPop(pCard->scanFldQ)) == NULL )
         {
            errMessage (0,"drvPmacFldTask: epicsRingPointerPop() returned NULL.");
         }
         else
         {
            PMAC_DEBUG(1,
                       PMAC_MESSAGE ("%s: rngBufGet completed.\n", MyName);
                       PMAC_MESSAGE ("%s: pMbxIo= %p\n", MyName, pMbxIo);
                       PMAC_MESSAGE ("%s: card=%d download=[%s]\n", MyName, pMbxIo->card, pMbxIo->command);
                       PMAC_MESSAGE ("%s: upload=[%s]\n", MyName, pMbxIo->response);
            )
printf("before drvPmacFldLoop\n");
            status = drvPmacFldLoop (pMbxIo->card, pMbxIo->command, pMbxIo->response, pMbxIo->errmsg);
printf("after drvPmacFldLoop\n");

            PMAC_DEBUG(1,
                       PMAC_MESSAGE ("%s: status=%d\n", MyName, status);
                       PMAC_MESSAGE ("%s: message=[%s]\n", MyName, pMbxIo->errmsg);
            )
            pMbxIo->terminator = (char) status;

            callbackRequest (&pMbxIo->callback);

            PMAC_DEBUG(1,
                       PMAC_MESSAGE ("%s: Callback requested.\n", MyName);
            )
         }
      }
   }
}

/*******************************************************************************
 *
 * drvPmacSvoTask - perform servo fixed buffer scanning
 *
 */
EPICSTHREADFUNC drvPmacSvoTask(void *c)           /* MDW OSI work 20160321 */
{
   int card = *(int *)c;
   PMAC_CARD *pCard = &drvPmacCard[card];


   FOREVER
   {
      if ( pCard->enabledSvo )
      {
         drvPmacSvoRead (pCard->card);
      }
      epicsThreadSleep(pCard->scanSvoRate);     /* MDW OSI work 20160321 */
   }
}

/*******************************************************************************
 *
 * drvPmacSvoScanInit - spawn task to perform servo scanning
 *
 */
PMAC_LOCAL void drvPmacSvoScanInit(int card)
{
   PMAC_CARD *pCard = &drvPmacCard[card];

   sprintf ( pCard->scanSvoTaskName, "%s%d", PMAC_SVO_SCAN, pCard->card);
   pCard->scanSvoTaskId = epicsThreadCreate (pCard->scanSvoTaskName,
                                             PMAC_SVO_PRI, PMAC_SVO_STACK,
                                             (EPICSTHREADFUNC)drvPmacSvoTask,
                                             (void *)&pCard->card );
   taskwdInsert (pCard->scanSvoTaskId, NULL, NULL);

   return;
}

/*******************************************************************************
 *
 * drvPmacBkgTask - perform background scanning
 *
 */
EPICSTHREADFUNC drvPmacBkgTask (void *c)               /* MDW OSI work 20160321 */
{
   int card = *(int *)c;
   PMAC_CARD *pCard = &drvPmacCard[card];


   FOREVER
   {
      if ( pCard->enabledBkg )
      {
         drvPmacBkgRead (pCard->card);
      }
      epicsThreadSleep (pCard->scanBkgRate);   /* MDW OSI work 20160321 */
   }
}

/*******************************************************************************
 *
 * drvPmacBkgScanInit - spawn task to perform background scanning
 *
 */
PMAC_LOCAL void drvPmacBkgScanInit(int card)
{
   PMAC_CARD *pCard = &drvPmacCard[card];

   sprintf ( pCard->scanBkgTaskName, "%s%d", PMAC_BKG_SCAN, pCard->card);
   pCard->scanBkgTaskId = epicsThreadCreate (pCard->scanBkgTaskName,
                                             PMAC_BKG_PRI, PMAC_BKG_STACK,
                                             (EPICSTHREADFUNC)drvPmacBkgTask,
                                             (void *)&pCard->card );
   taskwdInsert (pCard->scanBkgTaskId, NULL, NULL);

   return;
}

/*******************************************************************************
 *
 * drvPmacVarTask - perform background variable scanning
 *
 */
EPICSTHREADFUNC  drvPmacVarTask(void *c) /* MDW OSI work 20160321 */
{
   int card = *(int *)c; 
   PMAC_CARD *pCard = &drvPmacCard[card];

   FOREVER
   {
      if ( pCard->enabledVar )
      {
         drvPmacVarRead (pCard->card);
      }
      epicsThreadSleep(pCard->scanVarRate);    /* MDW OSI work 20160321 */
   }
}

/*******************************************************************************
 *
 * drvPmacVarScanInit - spawn task to perform background scanning
 *
 */
PMAC_LOCAL void drvPmacVarScanInit(int card)
{
   //long  status;  warning: variable 'status' set but not used [-Wunused-but-set-variable]

   PMAC_CARD *pCard = &drvPmacCard[card];

   //status = drvPmacVarSetup (pCard->card);
   drvPmacVarSetup (pCard->card);

   sprintf ( pCard->scanVarTaskName, "%s%d", PMAC_VAR_SCAN, pCard->card);
   pCard->scanVarTaskId = epicsThreadCreate (pCard->scanVarTaskName,
                                             PMAC_VAR_PRI, PMAC_VAR_STACK,
                                             (EPICSTHREADFUNC)drvPmacVarTask,
                                             (void *)&pCard->card );
   taskwdInsert(pCard->scanVarTaskId, NULL, NULL);

   return;
}


/*******************************************************************************
 *
 * drvPmacOpnTask - perform DPRAM Open variable scanning
 *
 */
EPICSTHREADFUNC drvPmacOpnTask(void *c) /* MDW OSI work 20160321 */
{
   int card = *(int *)c;
   PMAC_CARD *pCard = &drvPmacCard[card];

   FOREVER
   {
      if ( pCard->enabledOpn )
      {
         drvPmacOpnRead (pCard->card);
      }
      epicsThreadSleep(pCard->scanOpnRate);           /* MDW OSI work 20160321 */
   }
}


/*******************************************************************************
 *
 * drvPmacOpnScanInit - spawn task to perform Open DPRAM scanning
 *
 */
PMAC_LOCAL void drvPmacOpnScanInit(int card)
{
   PMAC_CARD *pCard = &drvPmacCard[card];

   sprintf ( pCard->scanOpnTaskName, "%s%d", PMAC_OPN_SCAN, pCard->card);
   pCard->scanOpnTaskId = epicsThreadCreate (pCard->scanOpnTaskName,
                                             PMAC_OPN_PRI, PMAC_OPN_STACK,
                                             (EPICSTHREADFUNC)drvPmacOpnTask,
                                             (void *)&pCard->card );
   taskwdInsert(pCard->scanOpnTaskId, NULL, NULL);

   return;
}


/*******************************************************************************
 *
 * drvPmacOpnRead - read PMAC card OPEN area
 */
long drvPmacOpnRead(int card )
{
   int          i;
   //long         status;  warning: variable 'status' set but not used [-Wunused-but-set-variable]
   PMAC_CARD   *pCard = &drvPmacCard[card];
   PMAC_RAM_IO *pOpnIo;

   /* Read PMAC Open area */
   for (i=0; i<pCard->numOpnIo; i++)
   {
      pOpnIo = &pCard->OpnIo[i];
      if(pOpnIo->inputRec )
      {
         //status = drvPmacRamGetData(pOpnIo);
         drvPmacRamGetData(pOpnIo);

         /* Notify Requester Of New Data */

         if( pCard->OpnIo[i].pFunc != (void *) NULL )
            (*pCard->OpnIo[i].pFunc)(pCard->OpnIo[i].pParm);
      }
   }
   return(0);
}

/* Register these symbols for use by IOC code */
/* Information needed by iocsh */
static const iocshArg     pmacConfigArg0 = {"cardNumber", iocshArgInt};
static const iocshArg     pmacConfigArg1 = {"addrBase",   iocshArgInt};
static const iocshArg     pmacConfigArg2 = {"addrDpram",  iocshArgInt};
static const iocshArg     pmacConfigArg3 = {"irqVector",  iocshArgInt};
static const iocshArg     pmacConfigArg4 = {"irqLevel",   iocshArgInt};

static const iocshArg    *pmacConfigArgs[] = {
        &pmacConfigArg0, &pmacConfigArg1, &pmacConfigArg2, &pmacConfigArg3, &pmacConfigArg4
};

static const iocshFuncDef pmacConfigFuncDef = {"pmacConfig", 5, pmacConfigArgs};

/* Wrapper called by iocsh, selects the argument types that pmacConfig needs */
static void pmacConfigCallFunc(const iocshArgBuf *args) {
    pmacConfig(args[0].ival, args[1].ival, args[2].ival, args[3].ival, args[4].ival);
}

/* Registration routine, runs at startup */
static void pmacRegisterCommands(void) {
    iocshRegister(&pmacConfigFuncDef, pmacConfigCallFunc);
}
epicsExportRegistrar(pmacRegisterCommands);
epicsExportAddress(int, drvPmacDebug);
