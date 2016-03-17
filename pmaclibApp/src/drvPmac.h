/* @(#) drvPmac.h 1.6 97/05/06 */

/* drvPmac.h -  EPICS Device Driver Support for PMAC-VME */

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

#ifndef __INCdrvPmacH
#define __INCdrvPmacH

#include <pmacTypes.h>
#include <epicsEvent.h>
#include <epicsRingPointer.h>

#define PMAC_MBX_OUT_BUFLEN   80
#define PMAC_MBX_IN_BUFLEN    80
#define PMAC_MBX_ERR_BUFLEN   10
#define PMAC_TASKNAME_LEN     15
#define PMAC_MAX_SVO         100
#define PMAC_MAX_BKG         200
#define PMAC_MAX_VAR         128
#define PMAC_MAX_OPN         200
#define PMAC_MAX_GAT          24

typedef struct  /* PMAC_MBX_IO */
{
	struct dbCommon *	pRec;
	int			card;
	long			terminator;
	char			command[PMAC_MBX_OUT_BUFLEN];
	char			response[PMAC_MBX_IN_BUFLEN];
	char			errmsg[PMAC_MBX_ERR_BUFLEN];
	CALLBACK		callback;
} PMAC_MBX_IO;

typedef struct  /* PMAC_RAM_IO */
{
	int		memType;
	int		pmacAdr;
	int		hostOfs;
        int             inputRec;
  	PMAC_DPRAM *	pAddress;
	long		valLong;
	double		valDouble;
 	void		(*pFunc) ();		/*MJR OSI work 20160312*/
  	void *		pParm;
} PMAC_RAM_IO;


typedef struct  /* PMAC_GAT_IO */
{
        int    srcEna;
        int    srcIx;
        int    srcType;
        int    numVal;
        long   *pValL;
        double *pValD;
} PMAC_GAT_IO;


typedef struct  /* PMAC_CARD */
{
	int		card;
	int		ctlr;
	int		configured;
	int		present;
	int		enabled;

	int		enabledMbx;
	int		enabledAsc;
	int		enabledRam;
	int		enabledSvo;
	int		enabledBkg;
	int		enabledVar;
	int		enabledOpn;
	int		enabledFld;
	int		enabledGat;

	epicsEventId	scanMbxSem;			/*MJR OSI work 20160312  next 8 lines*/
	epicsEventId	scanAscSem;
	epicsEventId	scanFldSem;
	epicsEventId	scanGatReadSem;
	epicsEventId	scanGatWriteSem;

	epicsRingPointerId		scanMbxQ;
	epicsRingPointerId		scanAscQ;
	epicsRingPointerId		scanFldQ;

	volatile double	scanSvoRate;	/* MJR 20160314		OSI  */
	volatile double	scanBkgRate;	/* MJR 20160314		OSI  */
	volatile double	scanVarRate;	/* MJR 20160314		OSI  */
	volatile double	scanOpnRate;	/* MJR 20160314		OSI  */

	int		scanMbxTaskId;
	int		scanAscTaskId;
	int		scanSvoTaskId;
	int		scanBkgTaskId;
	int		scanVarTaskId;
	int		scanOpnTaskId;
	int		scanFldTaskId;
	int		scanGatReadId;
	int		scanGatWriteId;
	
	char		scanMbxTaskName[PMAC_TASKNAME_LEN];
	char		scanAscTaskName[PMAC_TASKNAME_LEN];
	char		scanSvoTaskName[PMAC_TASKNAME_LEN];
	char		scanBkgTaskName[PMAC_TASKNAME_LEN];
	char		scanVarTaskName[PMAC_TASKNAME_LEN];
	char		scanOpnTaskName[PMAC_TASKNAME_LEN];
	char		scanFldTaskName[PMAC_TASKNAME_LEN];
	char		scanGatReadName[PMAC_TASKNAME_LEN];
	char		scanGatWriteName[PMAC_TASKNAME_LEN];

	int		numSvoIo;
	int		numBkgIo;
	int		numVarIo;
	int		numOpnIo;

	int		logFlag;

	PMAC_RAM_IO	SvoIo[PMAC_MAX_SVO];
	PMAC_RAM_IO	BkgIo[PMAC_MAX_BKG];
	PMAC_RAM_IO	VarIo[PMAC_MAX_VAR];
	PMAC_RAM_IO	OpnIo[PMAC_MAX_OPN];

	PMAC_GAT_IO     GatIo[PMAC_MAX_GAT];

} PMAC_CARD;

PMAC_LOCAL PMAC_CARD    drvPmacCard[PMAC_MAX_CARDS];


/* Function Prototypes */

PMAC_LOCAL long drvPmacStartup(void);
PMAC_LOCAL long	drvPmacRamGetData(PMAC_RAM_IO *);
PMAC_LOCAL long	drvPmacRamPutData(PMAC_RAM_IO *);
PMAC_LOCAL long	drvPmac_report(int);
PMAC_LOCAL long	drvPmac_init(void);

PMAC_LOCAL int	drvPmacSvoScan(void);
PMAC_LOCAL int	drvPmacBkgScan(void);
PMAC_LOCAL int	drvPmacVarScan(void);

PMAC_LOCAL void	drvPmacSvoScanInit(int);
int		drvPmacSvoTask(int);

PMAC_LOCAL void	drvPmacBkgScanInit(int);
int		drvPmacBkgTask(int);

PMAC_LOCAL void	drvPmacVarScanInit(int);
int		drvPmacVarTask(int);

PMAC_LOCAL void	drvPmacOpnScanInit(int);
int		drvPmacOpnTask(int);
long            drvPmacOpnRead(int);

PMAC_LOCAL void	drvPmacMbxScanInit(int);
int		drvPmacMbxTask(int);
char		drvPmacMbxWriteRead(int, char *, char *, char *);

PMAC_LOCAL void	drvPmacAscScanInit(int);
int		drvPmacAscTask(int);

PMAC_LOCAL void	drvPmacFldScanInit(int);
int		drvPmacFldTask(int);
long		drvPmacFldLoop(int, char *, char *, char *);

PMAC_LOCAL void	drvPmacGatScanInit(int);

long drvPmacDpramRequest( short, short, char *, void (*)(), 
                          void *, PMAC_RAM_IO **, int);

#endif /* __INCdrvPmacH */
