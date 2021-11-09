/* @(#) devPmacMbx.c 1.8 97/05/06 */

/* devPmacMbx.c -  EPICS Device Support for PMAC-VME Mailbox */

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
*/

/*
 * Modification History:
 * ---------------------
 * .01  6-7-95        tac     initial
 */

/*
 * DESCRIPTION:
 * ------------
 * This module implements EPICS Device Support for PMAC-VME Mailbox.
 *
 */

/*
 * INCLUDES
 */

/* OSI Includes */

#include	<string.h>
#include 	<epicsStdlib.h>
#include	<epicsStdioRedirect.h>
#include	<epicsPrint.h>
#include 	<epicsExport.h>

/* EPICS Includes */

#include	<alarm.h>
#include	<cvtTable.h>
#include	<dbAccess.h>
#include	<dbDefs.h>
#include        <recGbl.h>
#include        <recSup.h>
#include	<devSup.h>
#include	<dbScan.h>
#include	<link.h>

#include	<aiRecord.h>
#include	<biRecord.h>
#include	<longinRecord.h>
#include	<mbbiRecord.h>
#include	<statusRecord.h>
#include	<aoRecord.h>
#include	<boRecord.h>
#include	<longoutRecord.h>
#include	<mbboRecord.h>
#include	<stringinRecord.h>
#include	<stringoutRecord.h>
#include	<loadRecord.h>

/* local includes */

#include	"drvPmac.h"

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

#if PMAC_DIAGNOSTICS
#define PMAC_MESSAGE	epicsPrintf
#define PMAC_DEBUG(level,code)       { if (devPmacMbxDebug >= (level)) { code } fflush(stdout);}
#define PMAC_TRACE(level,code)       { if ( (pRec->tpro > (0)) || (devPmacMbxDebug >= (level)) ) { code } }
#else
#define PMAC_DEBUG(level,code)      ;
#define PMAC_TRACE(level,code)      ;
#endif

#define NO_ERR_STATUS   (-1)

/*
 * TYPEDEFS
 */

typedef struct  /* PMAC_DSET_AI */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
	DEVSUPFUN	special_linconv;
} PMAC_DSET_AI;

typedef struct  /* PMAC_DSET_AO */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write;
	DEVSUPFUN	special_linconv;
} PMAC_DSET_AO;

typedef struct  /* PMAC_DSET_BI */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
} PMAC_DSET_BI;

typedef struct  /* PMAC_DSET_BO */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write;
} PMAC_DSET_BO;

typedef struct  /* PMAC_DSET_LI */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
} PMAC_DSET_LI;

typedef struct  /* PMAC_DSET_LO */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write;
} PMAC_DSET_LO;

typedef struct  /* PMAC_DSET_MBBI */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
} PMAC_DSET_MBBI;

typedef struct  /* PMAC_DSET_MBBO */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write;
} PMAC_DSET_MBBO;

typedef struct  /* PMAC_DSET_STATUS */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
} PMAC_DSET_STATUS;

typedef struct  /* PMAC_DSET_SI */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	read;
} PMAC_DSET_SI;

typedef struct  /* PMAC_DSET_SO */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	write;
} PMAC_DSET_SO;

typedef struct  /* PMAC_DSET_LOAD */
{
	long		number;
	DEVSUPFUN	report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record;
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	proc;
} PMAC_DSET_LOAD;

typedef struct  /* PMAC_MBX_DPVT */
{
	PMAC_MBX_IO	MbxIo;
} PMAC_MBX_DPVT;

/*
 * FORWARD DECLARATIONS
 */

LOCAL long devPmacMbx_init();

LOCAL long devPmacMbxAi_init();
LOCAL long devPmacMbxAi_read();

LOCAL long devPmacMbxAo_init();
LOCAL long devPmacMbxAo_write();

LOCAL long devPmacMbxBi_init();
LOCAL long devPmacMbxBi_read();

LOCAL long devPmacMbxBo_init();
LOCAL long devPmacMbxBo_write();

LOCAL long devPmacMbxLi_init();
LOCAL long devPmacMbxLi_read();

LOCAL long devPmacMbxLo_init();
LOCAL long devPmacMbxLo_write();

LOCAL long devPmacMbxMbbi_init();
LOCAL long devPmacMbxMbbi_read();

LOCAL long devPmacMbxMbbo_init();
LOCAL long devPmacMbxMbbo_write();

LOCAL long devPmacMbxStatus_init();
LOCAL long devPmacMbxStatus_read();

LOCAL long devPmacMbxSi_init();
LOCAL long devPmacMbxSi_read();

LOCAL long devPmacMbxSo_init();
LOCAL long devPmacMbxSo_write();

LOCAL long devPmacMbxLoad_init();
LOCAL long devPmacMbxLoad_proc();

void drvPmacMbxScan (PMAC_MBX_IO * pMbxIo);
void drvPmacFldScan (PMAC_MBX_IO * pMbxIo);

LOCAL void devPmacMbxCallback (CALLBACK * pCallback);

/*
 * GLOBALS
 */

char * devPmacMbxVersion = "@(#) devPmacMbx.c 1.8 97/05/06";

#if PMAC_DIAGNOSTICS
volatile int devPmacMbxDebug = 0;
#endif

PMAC_DSET_AI devPmacMbxAi =
{
	6,
	NULL,
	devPmacMbx_init,
	devPmacMbxAi_init,
	NULL,
	devPmacMbxAi_read,
	NULL
};
epicsExportAddress(dset, devPmacMbxAi);

PMAC_DSET_AO devPmacMbxAo =
{
	6,
	NULL,
	devPmacMbx_init,
	devPmacMbxAo_init,
	NULL,
	devPmacMbxAo_write,
	NULL
};
epicsExportAddress(dset, devPmacMbxAo);

PMAC_DSET_BI devPmacMbxBi =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxBi_init,
	NULL,
	devPmacMbxBi_read
};
epicsExportAddress(dset, devPmacMbxBi);

PMAC_DSET_BO devPmacMbxBo =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxBo_init,
	NULL,
	devPmacMbxBo_write};
epicsExportAddress(dset, devPmacMbxBo);

PMAC_DSET_LI devPmacMbxLi =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxLi_init,
	NULL,
	devPmacMbxLi_read
};
epicsExportAddress(dset, devPmacMbxLi);

PMAC_DSET_LO devPmacMbxLo =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxLo_init,
	NULL,
	devPmacMbxLo_write
};
epicsExportAddress(dset, devPmacMbxLo);

PMAC_DSET_MBBI devPmacMbxMbbi =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxMbbi_init,
	NULL,
	devPmacMbxMbbi_read
};
epicsExportAddress(dset, devPmacMbxMbbi);

PMAC_DSET_MBBO devPmacMbxMbbo =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxMbbo_init,
	NULL,
	devPmacMbxMbbo_write
};
epicsExportAddress(dset, devPmacMbxMbbo);

PMAC_DSET_STATUS devPmacMbxStatus =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxStatus_init,
	NULL,
	devPmacMbxStatus_read
};
epicsExportAddress(dset, devPmacMbxStatus);

PMAC_DSET_SI devPmacMbxSi =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxSi_init,
	NULL,
	devPmacMbxSi_read
};
epicsExportAddress(dset, devPmacMbxSi);

PMAC_DSET_SO devPmacMbxSo =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxSo_init,
	NULL,
	devPmacMbxSo_write
};
epicsExportAddress(dset, devPmacMbxSo);

PMAC_DSET_LOAD devPmacMbxLoad =
{
	5,
	NULL,
	devPmacMbx_init,
	devPmacMbxLoad_init,
	NULL,
	devPmacMbxLoad_proc
};
epicsExportAddress(dset, devPmacMbxLoad);

/*
 * LOCALS
 */

/*******************************************************************************
 *
 * devPmacMbx_init - EPICS device support init function
 *
 */
LOCAL long devPmacMbx_init
(
	int	after
)
{
	long status = 0;

	if (after == 1)
	{
		status = drvPmacStartup ();
	}
	
	return (status);
}
		
/*******************************************************************************
 *
 * devPmacMbxDpvtInit - EPICS PMAC_MBX_DPVT init
 *
 */
PMAC_MBX_DPVT * devPmacMbxDpvtInit
(
	struct dbCommon *pRec,
	int		card
)
{
	PMAC_MBX_DPVT *pDpvt;

	pDpvt = (PMAC_MBX_DPVT *) malloc (sizeof(PMAC_MBX_DPVT));

	pDpvt->MbxIo.pRec = pRec;
	pDpvt->MbxIo.card = card;
	pDpvt->MbxIo.terminator = 0;
	pDpvt->MbxIo.command[0] = '\0';		/* MJR OSI work 20160312 removed cast to (char) NULL*/
	pDpvt->MbxIo.response[0] = '\0';	/* Worked with vxworks... do we care about strict byte alignment ??? */
						/* we don't want 4-bytes being allocated here as the command and 
 						 * response elements are char uint_8 padding in the structure.
 						 * 			***PARANOID***
 						 *
 						 * http://stackoverflow.com/questions/15954506/how-can-a-char-variable-accept-pointernull-as-its-value
 						 */
	
	callbackSetCallback (devPmacMbxCallback, &pDpvt->MbxIo.callback);	
	callbackSetPriority (pRec->prio, &pDpvt->MbxIo.callback);	
	callbackSetUser ( (void *) pRec, &pDpvt->MbxIo.callback);
	
	return (pDpvt);
}
		
/*******************************************************************************
 *
 * devPmacMbxAi_init - EPICS PMAC device support ai init
 *
 */
LOCAL long devPmacMbxAi_init
(
	struct aiRecord	*pRec
)
{
	char *MyName = "devPmacMbxAi_init";

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
				       pRec->inp.value.vmeio.card, pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
		"devPmacMbxAi_init: Illegal INP field");
		return(S_db_badField);
	}
	return(0);
}

/*******************************************************************************
 *
 * devPmacMbxBi_init - EPICS PMAC device support bi init
 *
 */
LOCAL long devPmacMbxBi_init
(
	struct biRecord	*pRec
)
{
	char *MyName = "devPmacMbxBi_init";

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->inp.value.vmeio.card, pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	 default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxBi_init: Illegal INP field");
		return(S_db_badField);
	}
	return(0);
}

/*******************************************************************************
 *
 * devPmacMbxLi_init - EPICS PMAC device support longin init
 *
 */
LOCAL long devPmacMbxLi_init
(
	struct longinRecord	*pRec
)
{
	char *MyName = "devPmacMbxLi_init";

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->inp.value.vmeio.card, pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxLi_init: Illegal INP field");
		return(S_db_badField);
	}
	return(0);
}

/*******************************************************************************
 *
 * devPmacMbxMbbi_init - EPICS PMAC device support mbbi init
 *
 */
LOCAL long devPmacMbxMbbi_init
(
	struct mbbiRecord	*pRec
)
{
	char *MyName = "devPmacMbxMbbi_init";

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->inp.value.vmeio.card, pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxMbbi_init: Illegal INP field");
		return(S_db_badField);
	}
	 return(0);
}

/*******************************************************************************
 *
 * devPmacMbxStatus_init - EPICS PMAC device support status init
 *
 */
LOCAL long devPmacMbxStatus_init
(
	struct statusRecord	*pRec
)
{
	char *MyName = "devPmacMbxStatus_init";

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
   			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->inp.value.vmeio.card, pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxStatus_init: Illegal INP field");
		return(S_db_badField);
	}
	return(0);
}

/*******************************************************************************
 *
 * devPmacMbxAo_init - EPICS PMAC device support ao init
 *
 */
LOCAL long devPmacMbxAo_init
(
	struct aoRecord *pRec
)
{
	char *MyName = "devPmacMbxAo_init";

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->out.value.vmeio.card, pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxAo_init: Illegal OUT field");
		return(S_db_badField);
	}

	return (0);

}

/*******************************************************************************
 *
 * devPmacMbxBo_init - EPICS PMAC device support bo init
 *
 */
LOCAL long devPmacMbxBo_init
(
	struct boRecord *pRec
)
{
	char *MyName = "devPmacMbxBo_init";

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->out.value.vmeio.card, pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxBo_init: Illegal OUT field");
		return(S_db_badField);
	}

    return (0);

}

/*******************************************************************************
 *
 * devPmacMbxLo_init - EPICS PMAC device support longout init
 *
 */
LOCAL long devPmacMbxLo_init
(
	struct longoutRecord *pRec
)
{
	char *MyName = "devPmacMbxLo_init";

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->out.value.vmeio.card, pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxLo_init: Illegal OUT field");
		return(S_db_badField);
	}

	return (0);

}

/*******************************************************************************
 *
 * devPmacMbxMbbo_init - EPICS PMAC device support mbbo init
 *
 */
LOCAL long devPmacMbxMbbo_init
(
	struct mbboRecord *pRec
)
{
	char *	MyName = "devPmacMbxMbbo_init";
	long	status = 0;

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
			PMAC_MESSAGE ("%s: card %d parm %s\n", MyName,
		pRec->out.value.vmeio.card, pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxMbbo_init: Illegal INP field");
		return(S_db_badField);
	}

	return (status);

}

/*******************************************************************************
 *
 * devPmacMbxSi_init - EPICS PMAC device support stringin init
 *
 */
LOCAL long devPmacMbxSi_init
(
	struct stringinRecord *pRec
)
{
	char *	MyName = "devPmacMbxSi_init";
	long status = 0;

	switch (pRec->inp.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
		PMAC_MESSAGE ("%s: name %s card %d parm %s\n", MyName, 
                pRec->name, pRec->inp.value.vmeio.card, 
                pRec->inp.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->inp.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxSi_init: Illegal INP field");
		return(S_db_badField);
	}

	return (status);

}

/*******************************************************************************
 *
 * devPmacMbxSo_init - EPICS PMAC device support stringout init
 *
 */
LOCAL long devPmacMbxSo_init
(
	struct stringoutRecord *pRec
)
{
	char *	MyName = "devPmacMbxSo_init";
	long status = 0;

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
		PMAC_MESSAGE ("%s: name %s card %d parm %s\n", MyName, 
                pRec->name, pRec->out.value.vmeio.card, 
                pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxSo_init: Illegal OUT field");
		return(S_db_badField);
	}

	return (status);

}

/*******************************************************************************
 *
 * devPmacMbxLoad_init - EPICS PMAC device support load init
 *
 */
LOCAL long devPmacMbxLoad_init
(
	struct loadRecord *pRec
)
{
	char *	MyName = "devPmacMbxLoad_init";
	long status = 0;

	switch (pRec->out.type) {
	case (VME_IO) :

		PMAC_DEBUG
		(	1,
		PMAC_MESSAGE ("%s: name %s card %d parm %s\n", MyName, 
                pRec->name, pRec->out.value.vmeio.card, 
                pRec->out.value.vmeio.parm);
		)

		pRec->dpvt = (void *) devPmacMbxDpvtInit ( (struct dbCommon *) pRec,
						(int) pRec->out.value.vmeio.card );
		break;

	default :
		recGblRecordError(S_db_badField,(void *)pRec,
			"devPmacMbxLoad_init: Illegal OUT field");
		return(S_db_badField);
	}

	return (status);

}

/*******************************************************************************
 *
 * devPmacMbxAi_read - EPICS PMAC device support ai read
 *
 */
static long devPmacMbxAi_read
(
	struct aiRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxAi_read";
	double	      valD;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
		PMAC_MESSAGE ("%s: %s response [%s]\n", MyName, 
                  pRec->name, pMbxIo->response);
		PMAC_MESSAGE ("%s: TPRO=%d\n", MyName, pRec->tpro);
		)
		
		sscanf (pMbxIo->response, "%lf", &valD);

		/* Adjust Slope And Offset */
		if (pRec->aslo != 0.0)
		{
			valD *= pRec->aslo;
		}
		if (pRec->aoff != 0.0)
		{
			valD += pRec->aoff;
		}

		/* pRec->linr Conversion Ignored */

		/* Apply Smoothing Algorithm */
		if (pRec->smoo != 0.0)
		{
	    		if (pRec->init == TRUE) pRec->val = valD;	/* initial condition */
	    		pRec->val = valD * (1.00 - pRec->smoo) + (pRec->val * pRec->smoo);
		}
		else
		{
	    		pRec->val = valD;
		}

		pRec->udf = FALSE;
		return (2);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->inp.value.vmeio.parm);

		PMAC_TRACE
		(	2,
		PMAC_MESSAGE ("%s: %s command [%s]\n", MyName, pRec->name,
                  pMbxIo->command);
		PMAC_MESSAGE ("%s: TPRO=%d\n", MyName, pRec->tpro);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxBi_read - EPICS PMAC device support bi read
 *
 */
LOCAL long devPmacMbxBi_read
(
	struct biRecord	*pRec
)
{
 	char *MyName = "devPmacMbxBi_read";
	
	long valL;
	
	PMAC_MBX_DPVT *	pDpvt = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO *	pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		sscanf (pMbxIo->response, "%ld", &valL);

		pRec->rval = (unsigned long) valL;
		pRec->udf = FALSE;

		return (0);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->inp.value.vmeio.parm);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}
	
}

/*******************************************************************************
 *
 * devPmacMbxLi_read - EPICS PMAC device support longin read
 *
 */
LOCAL long devPmacMbxLi_read
(
	struct longinRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxLi_read";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		sscanf (pMbxIo->response, "%ld", &valL);

		pRec->val = valL;
		pRec->udf = FALSE;

		return (0);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->inp.value.vmeio.parm);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxMbbi_read - EPICS PMAC device support mbbi read
 *
 */
LOCAL long devPmacMbxMbbi_read
(
	struct mbbiRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxMbbi_read";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		sscanf (pMbxIo->response, "%ld", &valL);

		pRec->rval = (unsigned long) valL;
		pRec->udf = FALSE;

		return (0);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->inp.value.vmeio.parm);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxStatus_read - EPICS PMAC device support status read
 *
 */
LOCAL long devPmacMbxStatus_read
(
	struct statusRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxStatus_read";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		sscanf (pMbxIo->response, "%ld", &valL);

		pRec->val = valL;
		pRec->udf = FALSE;
	
		return (0);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->inp.value.vmeio.parm);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}
	
}

/*******************************************************************************
 *
 * devPmacMbxAo_write - EPICS PMAC device support ao write
 *
 */
LOCAL long devPmacMbxAo_write
(
	struct aoRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxAo_write";
	double	      valD;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		return (0);
	}

	else
	{

		/* Output Value */
		valD = (double) pRec->oval;
		
		/* Adjust Slope And Offset */
		if (pRec->aoff != 0.0)
		{
			valD -= (double) pRec->aoff;
		}
		if (pRec->aslo != 0.0)
		{
			valD /= (double) pRec->aslo;
		}

		/* pRec->linr Conversion Ignored */

		sprintf (pMbxIo->command, "%s%f", pRec->out.value.vmeio.parm, valD);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxBo_write - EPICS PMAC device support bo write
 *
 */
LOCAL long devPmacMbxBo_write
(
	struct boRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxBo_write";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *) pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		return (0);
	}

	else
	{

		valL = (long) pRec->val;

		sprintf (pMbxIo->command,"%s%ld", pRec->out.value.vmeio.parm, valL);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}
}

/*******************************************************************************
 *
 * devPmacMbxLo_write - EPICS PMAC device support longout write
 *
 */
LOCAL long devPmacMbxLo_write
(
	struct longoutRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxLo_write";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		return (0);
	}

	else
	{

		valL = (long) pRec->val;

		sprintf (pMbxIo->command,"%s%ld", pRec->out.value.vmeio.parm, valL);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxMbbo_write - EPICS PMAC device support mbbo write
 *
 */
LOCAL long devPmacMbxMbbo_write
(
	struct mbboRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxMbbo_write";
	long          valL;
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		return (0);
	}

	else
	{

		valL = (long) pRec->rval;

		sprintf (pMbxIo->command,"%s%ld", pRec->out.value.vmeio.parm, valL);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}

/*******************************************************************************
 *
 * devPmacMbxSi_read - EPICS PMAC device support stringin read
 *
 */
LOCAL long devPmacMbxSi_read
(
	struct stringinRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxSi_read";
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		if ( memccpy(pRec->val, pMbxIo->response,  '\0', MAX_STRING_SIZE ))
                   pRec->val[MAX_STRING_SIZE -1] = '\0';
		pRec->udf = FALSE;
		return (0);
	}

	else
	{

		switch (pRec->inp.value.vmeio.signal)
		{
		case (1):
			sprintf (pMbxIo->command,"%s%s", pRec->inp.value.vmeio.parm, pRec->val);
			break;
		case (0):
		default:
			sprintf (pMbxIo->command,"%s", pRec->inp.value.vmeio.parm);
			break;
		case (2):
			sprintf (pMbxIo->command,"%s %s", pRec->val, pRec->inp.value.vmeio.parm);
			break;
		}

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}
/*******************************************************************************
 *
 * devPmacMbxSo_write - EPICS PMAC device support stringout write
 *
 */
LOCAL long devPmacMbxSo_write
(
	struct stringoutRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxSo_write";
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;

	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s response [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)
		
		switch (pRec->out.value.vmeio.signal)
		{
		case (1):
			if (memccpy (pRec->val, pMbxIo->response, '\0', MAX_STRING_SIZE) ==NULL)
                             pRec->val[MAX_STRING_SIZE -1] = '\0';
                        break;
		case (0):
		default:
			break;
		}
		return (0);
	}

	else
	{

		sprintf (pMbxIo->command,"%s%s", pRec->out.value.vmeio.parm, pRec->val);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s command [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
		)

		drvPmacMbxScan (pMbxIo);

		pRec->pact = TRUE;
		return (0);
	}

}


/*******************************************************************************
 *
 * devPmacMbxLoad_proc - EPICS PMAC device support stringout write
 *
 */
LOCAL long devPmacMbxLoad_proc
(
	struct loadRecord	*pRec
)
{
 	char          *MyName = "devPmacMbxLoad_proc";
	PMAC_MBX_DPVT *pDpvt  = (PMAC_MBX_DPVT *)pRec->dpvt;
	PMAC_MBX_IO   *pMbxIo = &pDpvt->MbxIo;
	
	if (pRec->pact)
	{

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s message [%s]\n", 
                  MyName, pRec->name, pMbxIo->errmsg);
		)
		
		pRec->val = pMbxIo->terminator;
		sprintf (pRec->msg, "%s", pMbxIo->errmsg);
		
		return (0);
	}

	else
	{

		sprintf (pMbxIo->command, "%s", pRec->dnv);
		sprintf (pMbxIo->response, "%s", pRec->upv);

		PMAC_TRACE
		(	2,
			PMAC_MESSAGE ("%s: %s download [%s]\n", 
                  MyName, pRec->name, pMbxIo->command);
			PMAC_MESSAGE ("%s: %s upload [%s]\n", 
                  MyName, pRec->name, pMbxIo->response);
		)

		drvPmacFldScan (pMbxIo);

		pRec->pact = TRUE;

 		return (0);
	}

}


/*******************************************************************************
 *
 * devPmacMbxCallback - EPICS device support Callback
 *
 */
LOCAL void devPmacMbxCallback
(
	CALLBACK *pCallback
)
{
	char            *MyName = "devPmacMbxCallback";
	struct dbCommon *pRec;

	callbackGetUser (pRec, pCallback);


	PMAC_TRACE
	(	2,
		PMAC_MESSAGE ("%s: CALLBACK [%s].\n",
          MyName, pRec->name);
	)

        dbScanLock (pRec);
        (*pRec->rset->process)(pRec);
        dbScanUnlock (pRec);
				
	return;
}
epicsExportAddress( int, devPmacMbxDebug);

