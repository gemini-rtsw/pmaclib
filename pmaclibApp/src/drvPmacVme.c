/* drvPmacVme.c - PMAC-VME Device Driver Library */

/*
 * Author:      Thomas A. Coleman
 * Date:        97/05/06
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 */


/* EPICS Includes */
#include <epicsStdioRedirect.h>
#include <epicsThread.h>
#include <epicsPrint.h>
#include <math.h>
#include <string.h>
#include <epicsEvent.h>
#include <devLib.h>
#include <errMdef.h>
#include <epicsExport.h>
#include <dbDefs.h>
#include <unistd.h>

/* Local Includes */

#include "pmacVme.h"

/*
 * DEFINES
 */

#define PMAC_DIAGNOSTICS TRUE
#define PMAC_PRIVATE FALSE

#if PMAC_PRIVATE
#define PMAC_LOCAL LOCAL
#else
#define PMAC_LOCAL
#endif

PMAC_LOCAL PMAC_CTLR    pmacVmeCtlr[PMAC_MAX_CARDS];

#if PMAC_DIAGNOSTICS
#define PMAC_MESSAGE   epicsPrintf
#define PMAC_DEBUG(level,code)       { if (drvPmacVmeDebug >= (level)) { code } fflush(stdout);}
#else
#define PMAC_DEBUG(level,code)      ;
#endif


/*
 * GLOBALS
 */

char * pmacVmeVersion = "@(#) drvPmacVme.c 1.6 97/05/06";

#if PMAC_DIAGNOSTICS
volatile int   drvPmacVmeDebug = 0;      /* must be > 0 to see messages */
#endif


/*******************************************************************************
 *
 * pmacVmeInit - Initialize PMAC-VME Hardware Configuration
 *
 */
PMAC_LOCAL long pmacVmeInit (void)
{
   int       i;
   PMAC_CTLR *pPmacCtlr;
   char *   MyName = "pmacVmeInit";
   
   for ( i=0; i < PMAC_MAX_CARDS; i++)
   {
      pPmacCtlr = &pmacVmeCtlr[i];

   PMAC_DEBUG
   (   7,
      PMAC_MESSAGE ("%s: PMAC VME INIT for controller %d:\n"
                              "configured = %d, presentBase = %d, enabledBase = %d\n\n", 
                               MyName, pPmacCtlr->ctlr, pPmacCtlr->configured, 
                               pPmacCtlr->presentBase, pPmacCtlr->enabledBase);
   )


      if ( pPmacCtlr->configured )
      {
         if ( pPmacCtlr->presentBase & pPmacCtlr->enabledBase )
         {
            //epicsEventSignal (pPmacCtlr->ioMbxLockSem);   
            //epicsEventSignal (pPmacCtlr->ioAscLockSem);
            //epicsMutexUnlock (pPmacCtlr->ioMbxLockMtx);   
            //epicsMutexUnlock (pPmacCtlr->ioAscLockMtx);
         
            pPmacCtlr->activeBase = TRUE;
         }
         
         if ( pPmacCtlr->presentDpram & pPmacCtlr->enabledDpram )
         {
            pPmacCtlr->activeDpram = TRUE;
         }

         if ( pPmacCtlr->activeDpram & pPmacCtlr->enabledGather )
         {
            pPmacCtlr->activeGather = TRUE;
         }

         pPmacCtlr->active = pPmacCtlr->activeBase | pPmacCtlr->activeDpram;
      }
   }   

   return (0);
}

static int cntRxISR = 0;
static int cntReadmeISR = 0;
static int cntAscinISR = 0;
static int cntGatbufISR = 0;

/*******************************************************************************
 *
 * pmacMbxReceiptISR - interrupt service routine for mailbox receipt acknowledge
 *
 */
static int mbxRcptISRcnt = 0;
PMAC_LOCAL void pmacMbxReceiptISR(void *p)
{
   PMAC_CTLR   *pPmacCtlr = (PMAC_CTLR *)p;
   
   cntRxISR++;
   epicsEventSignal (pPmacCtlr->ioMbxReceiptSem);
   mbxRcptISRcnt++;
   return;
}

/*******************************************************************************
 *
 * pmacMbxReadmeISR - interrupt service routine for mailbox message arrival
 *
 */
static int mbxReadmeISRcnt = 0;
PMAC_LOCAL void pmacMbxReadmeISR(void *p)
{
   PMAC_CTLR *pPmacCtlr = (PMAC_CTLR *)p;
   
   cntReadmeISR++;
   epicsEventSignal (pPmacCtlr->ioMbxReadmeSem);
   mbxReadmeISRcnt++;
   return;
}

static int mbxoutA = 0;
static int mbxoutB = 0;
static int mbxoutC = 0;
static int mbxoutD = 0;

/*******************************************************************************
 *
 * pmacMbxOut - put characters in PMAC mailbox
 *
 */
PMAC_LOCAL char pmacMbxOut
(
   int   ctlr,
   char   *writebuf
)
{
    int      i;
    int      length;
    char   firstcharacter;
    char   termination;
    PMAC_CTLR   *pPmacCtlr;

    pPmacCtlr = &pmacVmeCtlr[ctlr];
    termination = 0;
    length = strlen (writebuf);

    PMAC_DEBUG(1, 
         PMAC_MESSAGE("pmacMbxOut: card %d: writebuf=%s,length=%d\n", ctlr, writebuf, length);
    )

    firstcharacter = writebuf[0];

    for (i = 1; (i < length) && (i < PMAC_BASE_MBX_REGS_OUT); i++)
    {
        mbxoutA++;
        pPmacCtlr->pBase->mailbox.MB[i+1].data = writebuf[i];
    }

    if ((i == length) && (i < PMAC_BASE_MBX_REGS_OUT))
    {
        mbxoutB++;
        termination = PMAC_TERM_CR;
        pPmacCtlr->pBase->mailbox.MB[i+1].data = PMAC_TERM_CR;
    }
    else if (i > length)
    {
        mbxoutC++;
        termination = PMAC_TERM_CR;
        firstcharacter = PMAC_TERM_CR;
    }
    mbxoutD++;
    pPmacCtlr->pBase->mailbox.MB[0].data = firstcharacter;

    return (termination);

}   

/*******************************************************************************
 *
 * pmacMbxIn - get characters from PMAC mailbox
 *
 */
PMAC_LOCAL char pmacMbxIn
(
   int   ctlr,
   char   *readbuf,
   char   *errmsg
)
{
   int      i;
   int      j;
   char   chr;
   char   terminator;
   char   terminext;
   PMAC_CTLR   *pPmacCtlr;

   pPmacCtlr = &pmacVmeCtlr[ctlr];
   
   terminator = 0;
   terminext = 0;
   errmsg[0] = '\0';      /* MJR 20160312 OSI work. this used to assign to NULL*/

   for (i = 0; (i < PMAC_BASE_MBX_REGS_IN) && (terminator == 0); i++) {
      chr = pPmacCtlr->pBase->mailbox.MB[i].data;
      if (chr == PMAC_TERM_CR || chr == PMAC_TERM_ACK || chr == PMAC_TERM_BELL) {
         terminator = chr;
         readbuf[i] = '\0';   /* MJR 20160312 OSI work. this used to assign to NULL*/

         PMAC_DEBUG( 1,
             PMAC_MESSAGE("pmacMbxIn: card %d, terminator 0x%02x, readbuf=%s\n", ctlr, terminator, readbuf);
         )

         if (terminator == PMAC_TERM_BELL) {
            for (j=0, i++; (i < PMAC_BASE_MBX_REGS_IN) && (terminext == 0); j++, i++) {
               chr = pPmacCtlr->pBase->mailbox.MB[i].data;
               if (chr == PMAC_TERM_CR) {
                  terminext = chr;
                  errmsg[j] = '\0';   /* MJR 20160312 OSI work. this used to assign to NULL*/
               }
               else {
                  errmsg[j] = chr;
               }
            }
         }
         //else if(chr == PMAC_TERM_CR) { pPmacCtlr->pBase->mailbox.MB[1].data = 0; }
      }
      else {
         readbuf[i] = chr;
      }
   }
   return (terminator);
}



/*******************************************************************************
 *
 * pmacMbxRead - read response from PMAC mailbox
 *
 */
PMAC_LOCAL char pmacMbxRead
(
   int   ctlr,
   char   *readbuf,
   char   *errmsg
)
{
   int                  i = 0;
   char                 terminator = 0;
   epicsEventWaitStatus status = epicsEventWaitOK;
   PMAC_CTLR            *pPmacCtlr;
    
   pPmacCtlr = &pmacVmeCtlr[ctlr];

   while( (terminator == 0) && (status == epicsEventWaitOK) ) {
      pPmacCtlr->pBase->mailbox.MB[1].data = 0;
      status      = epicsEventWaitWithTimeout( pPmacCtlr->ioMbxReadmeSem, MBX_TIMEOUT);
      terminator = pmacMbxIn (ctlr, &readbuf[i], errmsg);
      i += PMAC_BASE_MBX_REGS_IN;
   }

 
   if( status == epicsEventWaitTimeout )
      epicsPrintf("pmacMbxRead: card %d: Read from PMAC Mailbox timed out: waiting for readme semaphore\n", ctlr);
   else if( status == epicsEventWaitError )
      epicsPrintf("pmacMbxRead: card %d: Read from PMAC Mailbox error: waiting for readme semaphore\n", ctlr);

PMAC_DEBUG( 1,
    PMAC_MESSAGE("pmacMbxRead:  readbuf=%s, terminator=%02x\n", readbuf, terminator);
)

   return (terminator);
}


/*******************************************************************************
 *
 * pmacMbxWrite - write command to PMAC mailbox
 *
 */
PMAC_LOCAL char pmacMbxWrite
(
   int   ctlr,
   char   *writebuf
)
{
   int       i = 0;
   STATUS    error = 0;
   char      terminator = 0;
   PMAC_CTLR *pPmacCtlr;
    
   pPmacCtlr = &pmacVmeCtlr[ctlr];

   PMAC_DEBUG(1,
      PMAC_MESSAGE("pmacMbxWrite: card %d: input=%s\n", ctlr, writebuf);
   )

   while ( (terminator == 0) && (error != ERROR) ) {
       terminator = pmacMbxOut (ctlr, &writebuf[i]);
       error      = epicsEventWaitWithTimeout(pPmacCtlr->ioMbxReceiptSem, MBX_TIMEOUT);
       i         += PMAC_BASE_MBX_REGS_OUT;
       if (i>100) break;
   }

   if( error == ERROR )
      epicsPrintf("pmacMbxWrite: card %d: Write to PMAC Mailbox timed out waiting for ioMbxReceiptSem\n", 
         ctlr);

   return (terminator);
}   

/*******************************************************************************
 *
 * pmacMbxLock - Lock PMAC mailbox for ctlr
 *
 */
long pmacMbxLock
(
   int   ctlr
)
{
   char *   MyName = "pmacMbxLock";
   PMAC_CTLR   *pPmacCtlr;

   pPmacCtlr = &pmacVmeCtlr[ctlr];
   if (epicsMutexTryLock(pPmacCtlr->ioMbxLockMtx) != epicsMutexLockOK ) { 
      epicsMutexLock (pPmacCtlr->ioMbxLockMtx);
   }

   PMAC_DEBUG
   (  1,
      PMAC_MESSAGE ("%s: card %d: PMAC MBX Mutex Locked\n", 
                  MyName, pPmacCtlr->ctlr);
   )
    
   return (0);
}

/*******************************************************************************
 *
 * pmacMbxUnlock - Unlock PMAC mailbox for ctlr
 *
 */
long pmacMbxUnlock
(
   int   ctlr
)
{
   char *   MyName = "pmacMbxUnlock";
   PMAC_CTLR   *pPmacCtlr;

   pPmacCtlr = &pmacVmeCtlr[ctlr];
   
   epicsMutexUnlock(pPmacCtlr->ioMbxLockMtx);
   PMAC_DEBUG
   (  1,
      PMAC_MESSAGE ("%s: card %d: MBX Mutex Unlocked\n", 
                  MyName, pPmacCtlr->ctlr);
   )
    
   return (0);
}

/*******************************************************************************
 *
 * pmacAscLock - Lock PMAC ASCII buffer for ctlr
 *
 */
long pmacAscLock
(
   int   ctlr
)
{
   PMAC_CTLR   *pPmacCtlr;

   pPmacCtlr = &pmacVmeCtlr[ctlr];
    
   //epicsEventMustWait (pPmacCtlr->ioAscLockSem);

   if (epicsMutexTryLock(pPmacCtlr->ioAscLockMtx) != epicsMutexLockOK ) { 
      epicsMutexLock (pPmacCtlr->ioAscLockMtx);
   }
   return (0);
}

/*******************************************************************************
 *
 * pmacAscUnlock - Unlock PMAC ASCII buffer for ctlr
 *
 */
long pmacAscUnlock
(
   int   ctlr
)
{
   PMAC_CTLR   *pPmacCtlr;

   pPmacCtlr = &pmacVmeCtlr[ctlr];
    
   //epicsEventSignal (pPmacCtlr->ioAscLockSem);
   epicsMutexUnlock (pPmacCtlr->ioAscLockMtx);
   return (0);
}


/*******************************************************************************
 *
 * pmacRamAddr - get DPRAM address
 *
 */
PMAC_DPRAM * pmacRamAddr
(
   int   ctlr,
   int   offset
)
{
   PMAC_CTLR *   pCtlr = &pmacVmeCtlr[ctlr];
   PMAC_DPRAM *   pDpram = (PMAC_DPRAM *) (pCtlr->pDpramBase + offset);
   
   return (pDpram);
}
   
/*******************************************************************************
 *
 * pmacRamGetH - get DPRAM 16 bits
 *
 */
PMAC_LOCAL long pmacRamGetH
(
   PMAC_DPRAM *   pDpram,
   long *      pVal
)
{
   short   ram0;
   long   val0;
   
   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   
   /* Convert unsigned 16 bit fixed-point format */
   val0 = (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutH - put DPRAM 16 bits
 *
 */
PMAC_LOCAL long pmacRamPutH
(
   PMAC_DPRAM *   pDpram,
   long       val
)
{
   short   ram0;
   long   val0;
   
   /* User value */
   val0 = val;
   
   /* Convert to unsigned 16 bit fixed-point format */
   ram0 = (short) (0x0000ffff & val0);

   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetY - get DPRAM Y word
 *
 */
PMAC_LOCAL long pmacRamGetY
(
   PMAC_DPRAM *   pDpram,
   long *       pVal
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   
   /* Convert unsigned 24 bit fixed-point format */
   val0 = ((0x000000ff & (long) ram1) << 16) | (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutY - put DPRAM Y word
 *
 */
PMAC_LOCAL long pmacRamPutY
(
   PMAC_DPRAM *   pDpram,
   long       val
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* User value */
   val0 = val;   

   /* Convert to unsigned 24 bit fixed-point format */
   ram0 = (short)  (0x0000ffff & val0);
   ram1 = (short) ((0x00ff0000 & val0) >> 16);
   
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetX - get DPRAM X word
 *
 */
PMAC_LOCAL long pmacRamGetX
(
   PMAC_DPRAM *   pDpram,
   long *      pVal
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   
   /* Convert unsigned 24 bit fixed-point format */
   val0 = ((0x000000ff & (long) ram1) << 16) | (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutX - put DPRAM X word
 *
 */
PMAC_LOCAL long pmacRamPutX
(
   PMAC_DPRAM *   pDpram,
   long      val
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* User value */
   val0 = val;   

   /* Convert to unsigned 24 bit fixed-point format */
   ram0 = (short)  (0x0000ffff & val0);
   ram1 = (short) ((0x00ff0000 & val0) >> 16);
   
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetSY - get DPRAM signed Y word
 *
 */
PMAC_LOCAL long pmacRamGetSY
(
   PMAC_DPRAM *   pDpram,
   long *      pVal
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   
   /* Convert signed 24 bit fixed-point format */
   val0 = (((long) ram1) << 16) | (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutSY - put signed DPRAM Y word
 *
 */
PMAC_LOCAL long pmacRamPutSY
(
   PMAC_DPRAM *   pDpram,
   long      val
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* User value */
   val0 = val;   

   /* Convert to unsigned 24 bit fixed-point format */
   ram0 = (short)  (0x0000ffff & val0);
   ram1 = (short) (val0 >> 16);
   
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetSX - get DPRAM signed X word
 *
 */
PMAC_LOCAL long pmacRamGetSX
(
   PMAC_DPRAM *   pDpram,
   long *      pVal
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   
   /* Convert signed 24 bit fixed-point format */
   val0 = (((long) ram1) << 16) | (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;
   
   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutSX - put signed DPRAM X word
 *
 */
PMAC_LOCAL long pmacRamPutSX
(
   PMAC_DPRAM *   pDpram,
   long      val
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* User value */
   val0 = val;   

   /* Convert to unsigned 24 bit fixed-point format */
   ram0 = (short)  (0x0000ffff & val0);
   ram1 = (short) (val0 >> 16);
   
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetDP - get DPRAM DP word
 *
 */
PMAC_LOCAL long pmacRamGetDP
(
   PMAC_DPRAM *   pDpram,
   long *      pVal
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];

   /* Convert 32 bit fixed-point format */
   val0 = (((long) ram1) << 16) | (0x0000ffff & (long) ram0);
   
   /* Return value */
   *pVal = val0;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutDP - put DPRAM DP word
 *
 */
PMAC_LOCAL long pmacRamPutDP
(
   PMAC_DPRAM *   pDpram,
   long      val
)
{
   short   ram0;
   short   ram1;
   long   val0;

   /* User value */
   val0 = val;

   /* Convert to 32 bit fixed-point format */
   ram0 = (short) (0x0000ffff & val0);
   ram1 = (short) (val0 >> 16);
   
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetF - get DPRAM F word
 *
 */
PMAC_LOCAL long pmacRamGetF
(
   PMAC_DPRAM *   pDpram,
   double *   pVal
)
{
   short   ram0;
   short   ram1;
   double   valD;
   union
   {
      long   val0;      
      float   valF;
   } valU;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];

   /* Convert 32 bit floating point format */
   valU.val0 = (((long) ram1) << 16) | (0x0000ffff & (long) ram0);
   valD = (double) valU.valF;
   
   /* Return value */
   *pVal = valD;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamPutF - put DPRAM F word
 *
 */
PMAC_LOCAL long pmacRamPutF
(
   PMAC_DPRAM *   pDpram,
   double      val
)
{
   short   ram0;
   short   ram1;
   union
   {
      long   val0;      
      float   valF;
   } valU;

   /* User value */
   valU.valF = (float) val;
   
   /* Convert to 32 bit floating point format */
   ram0 = (short) (0x0000ffff & valU.val0);
   ram1 = (short) (valU.val0 >> 16);
      
   /* Write PMAC DPRAM */
   pDpram[0] = ram0;
   pDpram[1] = ram1;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetD - get DPRAM D word
 *
 */
PMAC_LOCAL long pmacRamGetD
(
   PMAC_DPRAM *   pDpram,
   double *   pVal
)
{

   short   ram0;
   short   ram1;
   short   ram2;
   short   ram3;
   long   val0;
   long   val1;
   double   valD;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   ram2 = pDpram[2];
   ram3 = pDpram[3];

   /* Convert 48 bit fixed-point format */
   val0 = ((0x000000ff & (long) ram1) << 16) | (0x0000ffff & (long) ram0);
   val1 =              (((long) ram3) << 16) | (0x0000ffff & (long) ram2);
   valD = (double) val1 * 16777216.0 + (double) val0;
   
   /* Return value */
   *pVal = valD;

   return (0);
}

/*******************************************************************************
 *
 * pmacRamGetL - get DPRAM L word
 *
 * mantissa/2^35 * 2^(exp-$7ff)
 */
PMAC_LOCAL long pmacRamGetL
(
   PMAC_DPRAM *   pDpram,
   double *   pVal
)
{
   short   ram0;
   short   ram1;
   short   ram2;
   short   ram3;
   long   val0;
   long   val1;
   double      mantissa   = 0.0;
   long int   exponent   = 0;
   double   valD;

   /* Read PMAC DPRAM */
   ram0 = pDpram[0];
   ram1 = pDpram[1];
   ram2 = pDpram[2];
   ram3 = pDpram[3];

   /* Convert 48 bit floating point format */
   val0 = (((long) ram1) << 16) | (0x0000ffff & (long) ram0);
   val1 = (((long) ram3) << 16) | (0x0000ffff & (long) ram2);
   mantissa = ((double) val1) * 4096.0 + (double) ((val0 >> 12) & 0x00000fff);
   exponent = (val0 & 0x00000fff) - 2082;  /* 0x7ff + 35 */
   
   if (mantissa == 0.0)
   {
      valD = 0.0;
   }
   else
   {
      valD = mantissa * pow (2.0, (double) exponent);
   }

   /* Return value */
   *pVal = valD;

   return (0);
}


/*******************************************************************************
 *
 * pmacAscInISR - ASCII Host-Input Buffer Ready Interrupt Service Routine
 *
 */
static int ascInISRcnt = 0;
PMAC_LOCAL void pmacAscInISR(void *p)
{
   PMAC_CTLR   *pPmacCtlr = (PMAC_CTLR *)p;
        
   cntAscinISR++;
   epicsEventSignal (pPmacCtlr->ioAscReadmeSem);
   ascInISRcnt++;
   return;
}


/*******************************************************************************
 *
 * pmacAscH2p - Put ASCII Word
 *
 */
PMAC_LOCAL long pmacAscH2p
(
   int   ctlr,
   long   off,
   int   val
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + off);
   *pRam = (short) val;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscP2h - Get Ascii word
 *
 */
PMAC_LOCAL long pmacAscP2h
(
   int   ctlr,
   long   off,
   int *   pVal
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + off);
   *pVal = (int) *pRam;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscOutStatus - Read ASCII Host-Output-Control Flag
 *
 */
PMAC_LOCAL long pmacAscOutStatus
(
   int   ctlr,
   int *   pVal
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x062c);
   *pVal = (int) ((*pRam) & 0x0001);
   return (0);
}

/*******************************************************************************
 *
 * pmacAscOutComplete - Set ASCII Host Output Complete Flag
 *
 */
PMAC_LOCAL long pmacAscOutComplete
(
   int   ctlr
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x062c);
   *pRam = (short) 0x0001;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscOutCtrlChar - Send ASCII Host-Output Control Character
 *
 */
PMAC_LOCAL long pmacAscOutCtrlChar
(
   int   ctlr,
   int   ctrlchar
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x062e);
   *pRam = ((short) ctrlchar) & 0x00ff;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscOutString - Write ASCII Host-Output Transfer Buffer
 *
 */
PMAC_LOCAL long pmacAscOutString
(
   int   ctlr,
   char *   string
)
{
   PMAC_DPRAM *   pRam;
   short      charPair;
   short      buffer[PMAC_ASC_IN_BUFLEN/2];
   int      terminated;
   int      i;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x0630);

   buffer[(PMAC_ASC_OUT_BUFLEN/2)-1] = 0x0000;
   strncpy ( (char *) buffer, string, PMAC_ASC_OUT_BUFLEN - 1);

   terminated = FALSE;
   for (i=0; !terminated; i++)
   {
      charPair = buffer[i];
      pRam[i] = BYTESWAP (charPair);

      /*MJR 20160312 Moved away from NULL defined as (void *)0    */
      if ( (LSB(charPair) == '\0') || (MSB(charPair) == '\0') )       {
         terminated = TRUE;
      }
   }

   return (0);
}

/*******************************************************************************
 *
 * pmacAscOutBuffer - Read ASCII Host-Output Transfer Buffer
 *
 */

PMAC_LOCAL long pmacAscOutBuffer
(
   int   ctlr,
   char *   string
)
{
   int      i;
   PMAC_DPRAM *   pRam;
   short      charPair;
   short      buffer[PMAC_ASC_OUT_BUFLEN/2];
   int      terminated;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x630);
   
   terminated = FALSE;
   for (i=0; !terminated; i++)
   {
      charPair = pRam[i];
      buffer[i] = BYTESWAP (charPair);
      /*MJR 20160312 Moved away from NULL defined as (void *)0    */
      if ( (LSB(charPair) == '\0') || (MSB(charPair) == '\0') )
      {
         terminated = TRUE;
      }
   }
   strcpy (string, (char *) buffer);
   return (0);
}

/*******************************************************************************
 *
 * pmacAscInStatus - Read ASCII Host-Input Control Word
 *
 */
PMAC_LOCAL long pmacAscInStatus
(
   int   ctlr,
   int *   pVal
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x6d0);
   *pVal = (int) *pRam;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscInEnable - Set ASCII Host-Input Enable
 *
 */
PMAC_LOCAL long pmacAscInEnable
(
   int   ctlr
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x6d0);
   *pRam = 0;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscInCount - Determine Number Of ASCII Characters In Host-Input Buffer
 *
 */
PMAC_LOCAL long pmacAscInCount
(
   int   ctlr,
   int *   pVal
)
{
   PMAC_DPRAM * pRam;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x6d2);
   *pVal = (int) *pRam;
   return (0);
}

/*******************************************************************************
 *
 * pmacAscInString - Read ASCII Host-Input Transfer Buffer
 *
 */
PMAC_LOCAL long pmacAscInString
(
   int   ctlr,
   char *   string
)
{
   int      i;
   PMAC_DPRAM *   pRam;
   short      charPair;
   short      buffer[PMAC_ASC_IN_BUFLEN/2];
   int      terminated;
   
   pRam = (PMAC_DPRAM *) ((long) pmacVmeCtlr[ctlr].pDpramBase + 0x6d4);
   
   terminated = FALSE;
   for (i=0; !terminated; i++)
   {
      charPair = pRam[i];
      buffer[i] = BYTESWAP (charPair);
      /*MJR 20160312 Moved away from NULL defined as (void *)0    */
      if ( (LSB(charPair) == '\0') || (MSB(charPair) == '\0') )
      {
         terminated = TRUE;
      }
   }
   strcpy (string, (char *) buffer);
   return (0);
}

/*******************************************************************************
 *
 * pmacGatBufferISR - interrupt service routine for DPRAM Data Gathering Buffer
 *
 */
static int gatBufISRcnt = 0;
PMAC_LOCAL void pmacGatBufferISR(void *p)
{
   PMAC_CTLR   *pPmacCtlr = (PMAC_CTLR *)p;

   cntGatbufISR++;
   epicsEventSignal (pPmacCtlr->ioGatBufferSem);
   gatBufISRcnt++;
   return;
}
epicsExportAddress(int, drvPmacVmeDebug); 
epicsExportAddress(int, mbxoutA); 
epicsExportAddress(int, mbxoutB); 
epicsExportAddress(int, mbxoutC); 
epicsExportAddress(int, mbxoutD);
epicsExportAddress(int, gatBufISRcnt); 
epicsExportAddress(int, mbxReadmeISRcnt); 
epicsExportAddress(int, mbxRcptISRcnt); 
epicsExportAddress(int, ascInISRcnt); 

epicsExportAddress(int, cntRxISR);
epicsExportAddress(int, cntReadmeISR);
epicsExportAddress(int, cntAscinISR);
epicsExportAddress(int, cntGatbufISR);


