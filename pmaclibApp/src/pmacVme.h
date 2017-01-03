/* @(#) pmacVme.h 1.5 97/05/06 */

/* pmacVme.h -  PMAC-VME Device Driver */

/*
 * Author:      Thomas A. Coleman
 * Date:        97/05/06
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 */

/*
*****************************************************************
                          COPYRIGHT NOTIFICATION
*****************************************************************

THE FOLLOWING IS A NOTICE OF COPYRIGHT, AVAILABILITY OF THE CODE,
AND DISCLAIMER WHICH MUST BE INCLUDED IN THE PROLOGUE OF THE CODE
AND IN ALL SOURCE LISTINGS OF THE CODE.
 
(C)  COPYRIGHT 1995 UNIVERSITY OF CHICAGO
 
Argonne National Laboratory (ANL), with facilities in the States of 
Illinois and Idaho, is owned by the United States Government, and
operated by the University of Chicago under provision of a contract
with the Department of Energy.

Portions of this material resulted from work developed under a U.S.
Government contract and are subject to the following license:  For
a period of five years from March 30, 1993, the Government is
granted for itself and others acting on its behalf a paid-up,
nonexclusive, irrevocable worldwide license in this computer
software to reproduce, prepare derivative works, and perform
publicly and display publicly.  With the approval of DOE, this
period may be renewed for two additional five year periods. 
Following the expiration of this period or periods, the Government
is granted for itself and others acting on its behalf, a paid-up,
nonexclusive, irrevocable worldwide license in this computer
software to reproduce, prepare derivative works, distribute copies
to the public, perform publicly and display publicly, and to permit
others to do so.

*****************************************************************
                                DISCLAIMER
*****************************************************************

NEITHER THE UNITED STATES GOVERNMENT NOR ANY AGENCY THEREOF, NOR
THE UNIVERSITY OF CHICAGO, NOR ANY OF THEIR EMPLOYEES OR OFFICERS,
MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL
LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT, OR PROCESS
DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY
OWNED RIGHTS.  

*****************************************************************
LICENSING INQUIRIES MAY BE DIRECTED TO THE INDUSTRIAL TECHNOLOGY
DEVELOPMENT CENTER AT ARGONNE NATIONAL LABORATORY (708-252-2000).
*/

/*
 * Modification History:
 * ---------------------
 * .01  6-7-95        tac     initial
 */

#ifndef __INCpmacVmeH
#define __INCpmacVmeH

#include "pmacTypes.h"
#include <epicsEvent.h>

/* PMAC Hardware Constants */

#define PMAC_TERM_ACK        0x06
#define PMAC_TERM_BELL       0x07
#define PMAC_TERM_CR         0x0D
#define PMAC_ASC_OUT_BUFLEN  160
#define PMAC_ASC_IN_BUFLEN   256
#define MBX_TIMEOUT	     10.0

#define NO_ERR_STATUS           (-1)
#define PMAC_BUFSIZE            (80)
#define PMAC_MEM_SIZE_BASE      (0x200)         /* Size of base registers */
#define PMAC_MEM_SIZE_DPRAM     (0x4000)        /* Size of DPRAM */

#define PMAC_DPRAM_MEMBASE      (0xD000)        /* Base address of DPRAM */
#define PMAC_BASE_MBX_REGS      (16)
#define PMAC_BASE_MBX_REGS_OUT  (15)
#define PMAC_BASE_MBX_REGS_IN   (16)
#define PMAC_STRLEN_FWVER       (31)

#define MSB(x)	(((x) >> 8) & 0xff)	  /* most signif byte of 2-byte integer */
#define LSB(x)	((x) & 0xff)		  /* least signif byte of 2-byte integer*/
#define MSW(x) (((x) >> 16) & 0xffff) 	  /* most signif word of 2-word integer */
#define LSW(x) ((x) & 0xffff) 		  /* least signif byte of 2-word integer*/
#define BYTESWAP(x) (MSB(x) | (LSB(x) << 8))

typedef volatile char	PMAC_DPRAM_BASE;

typedef volatile struct  /* PMAC_MBX_BASE */
{
	struct pmacBaseMbxStruct
	{
		struct { unsigned char unused; unsigned char data; } MB[PMAC_BASE_MBX_REGS];
		
	} mailbox;
} PMAC_MBX_BASE;

typedef struct  /* PMAC_CTLR */
{
	int		ctlr;
	int		configured;
	int		present;
	int		enabled;
	int		active;
	int		presentBase;
	int		enabledBase;
	int		activeBase;
	int		presentDpram;
	int		enabledDpram;
	int		activeDpram;
	int		enabledGather;
	int		activeGather;
	PMAC_MBX_BASE   *pBase;
	PMAC_DPRAM_BASE *pDpramBase;
	unsigned	irqVector;
	unsigned	irqLevel;
	unsigned long	vmebusBase;
	unsigned long	vmebusDpram;
	epicsEventId	ioMbxLockSem;
	epicsEventId	ioMbxReceiptSem;
	epicsEventId	ioMbxReadmeSem;
	epicsEventId	ioAscLockSem;
	epicsEventId	ioAscReadmeSem;
	epicsEventId	ioGatBufferSem;
	char		firmwareVersion[PMAC_STRLEN_FWVER];
} PMAC_CTLR;

PMAC_LOCAL PMAC_CTLR    pmacVmeCtlr[PMAC_MAX_CARDS];

/*
 * FORWARD DECLARATIONS
 */

PMAC_LOCAL void pmacMbxReceiptISR (void *);
PMAC_LOCAL void pmacMbxReadmeISR (void *);
PMAC_LOCAL void pmacAscInISR (void *);
PMAC_LOCAL void pmacGatBufferISR (void *);

PMAC_LOCAL long pmacVmeInit (void);

long pmacMbxLock (int ctlr);
long pmacMbxUnlock (int	ctlr);

PMAC_LOCAL char pmacMbxOut (int ctlr, char * writebuf);
PMAC_LOCAL char pmacMbxIn (int ctlr, char * readbuf, char * errmsg);
PMAC_LOCAL char pmacMbxRead (int ctlr, char * readbuf, char * errmsg);
PMAC_LOCAL char pmacMbxWrite (int ctlr, char * writebuf);

long pmacAscLock (int ctlr);
long pmacAscUnlock (int	ctlr);

PMAC_LOCAL long pmacAscH2p (int ctlr, long off, int val);
PMAC_LOCAL long pmacAscP2h (int ctlr, long off, int * pVal);
PMAC_LOCAL long pmacAscOutStatus (int ctlr, int * pVal);
PMAC_LOCAL long pmacAscOutComplete (int ctlr);
PMAC_LOCAL long pmacAscOutCtrlChar (int ctlr, int ctrlchar);
PMAC_LOCAL long pmacAscOutString (int ctlr, char * string);
PMAC_LOCAL long pmacAscOutBuffer (int ctlr, char * string);
PMAC_LOCAL long pmacAscInStatus (int ctlr, int * pVal);
PMAC_LOCAL long pmacAscInEnable (int ctlr);
PMAC_LOCAL long pmacAscInCount (int ctlr, int * pVal);
PMAC_LOCAL long pmacAscInString (int ctlr, char * string);

PMAC_DPRAM * pmacRamAddr (int ctlr, int offset);

PMAC_LOCAL long pmacRamGetH (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetY (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetX (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetSY (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetSX (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetDP (PMAC_DPRAM * pRam, long * pVal);
PMAC_LOCAL long pmacRamGetF (PMAC_DPRAM * pRam, double * pVal);
PMAC_LOCAL long pmacRamGetD (PMAC_DPRAM * pRam, double * pVal);
PMAC_LOCAL long pmacRamGetL (PMAC_DPRAM * pRam, double * pVal);


PMAC_LOCAL long pmacRamPutH (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long pmacRamPutY (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long pmacRamPutX (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long	pmacRamPutSY (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long pmacRamPutSX (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long pmacRamPutDP (PMAC_DPRAM * pRam, long val);
PMAC_LOCAL long pmacRamPutF (PMAC_DPRAM * pRam, double val);

// PMAC_LOCAL void pmacGatBufferSem (int ctlr);

#endif /* __INCpmacVmeH */
