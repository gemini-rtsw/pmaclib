/* loadRecord.c */

/* loadRecord.c - Record Support Routines for PMAC Load records */
/*
 * Author: 	Thomas A Coleman
 * Date:	3/11/97
 *
 *      Experimental Physics and Industrial Control System (EPICS)
 *
 *      Copyright 1991, the Regents of the University of California,
 *      and the University of Chicago Board of Governors.
 *
 *      This software was produced under  U.S. Government contracts:
 *      (W-7405-ENG-36) at the Los Alamos National Laboratory,
 *      and (W-31-109-ENG-38) at Argonne National Laboratory.
 *
 *      Initial development by:
 *              The Controls and Automation Group (AT-8)
 *              Ground Test Accelerator
 *              Accelerator Technology Division
 *              Los Alamos National Laboratory
 *
 *      Co-developed with
 *              The Controls and Computing Group
 *              Accelerator Systems Division
 *              Advanced Photon Source
 *              Argonne National Laboratory
 *
 * Modification Log:
 * -----------------
 * .01  10-24-91        jba     Removed unused code
 * .02  11-11-91        jba     Moved set and reset of alarm stat and sevr to macros
 * .03  02-05-92	jba	Changed function arguments from paddr to precord 
 * .04  02-28-92	jba	ANSI C changes
 * .05  04-10-92        jba     pact now used to test for asyn processing, not status
 * .06  04-18-92        jba     removed process from dev init_record parms
 * .07  07-15-92        jba     changed VALID_ALARM to INVALID alarm
 * .08  07-16-92        jba     added invalid alarm fwd link test and chngd fwd lnk to macro
 * .09  08-14-92        jba     Added simulation processing
 * .10  08-19-92        jba     Added code for invalid alarm output action
 * .11  10-10-92        jba     replaced code for get of VAL from DOL with recGblGetLinkValue call
 * .12  10-18-92        jba     pact now set in recGblGetLinkValue
 * .13  03-30-94        mcn     converted to fast links
 * .14  04-27-01	ajf     converted to 3.13
 * .15  07-05-04        ajf     converted to 3.14
 *
 *
 *  Note: The device support for this record is contained in "devPmacMbx.c".
 *
 */

#include        <stdio.h>
#include        <string.h>

#include        <alarm.h>
#include	<dbDefs.h>
#include	<dbEvent.h>
#include	<dbAccess.h>
#include	<dbFldTypes.h>
#include	<devSup.h>
#include	<errMdef.h>
#include	<recGbl.h>
#include	<recSup.h>
#include	<menuYesNo.h>
#define GEN_SIZE_OFFSET
#include	<loadRecord.h>
#undef  GEN_SIZE_OFFSET

#include "epicsExport.h"

/* Create RSET - Record Support Entry Table*/
static long init_record();
static long process();
static long get_value();
#define report             NULL
#define initialize         NULL
#define special            NULL
#define cvt_dbaddr         NULL
#define get_array_info     NULL
#define put_array_info     NULL
#define get_units          NULL
#define get_precision      NULL
#define get_enum_str       NULL
#define get_enum_strs      NULL
#define put_enum_str       NULL
#define get_graphic_double NULL
#define get_control_double NULL
#define get_alarm_double   NULL

rset loadRSET={
	RSETNUMBER,
	report,
	initialize,
	init_record,
	process,
	special,
	get_value,
	cvt_dbaddr,
	get_array_info,
	put_array_info,
	get_units,
	get_precision,
	get_enum_str,
	get_enum_strs,
	put_enum_str,
	get_graphic_double,
	get_control_double,
	get_alarm_double };
epicsExportAddress(rset, loadRSET);

struct loaddset { /* load dset */
	long		number;
	DEVSUPFUN	dev_report;
	DEVSUPFUN	init;
	DEVSUPFUN	init_record; /*returns: (-1,0)=>(failure,success)*/
	DEVSUPFUN	get_ioint_info;
	DEVSUPFUN	dev_load;/*(-1,0)=>(failure,success)*/
};
static void monitor();
static long download();


static long init_record( loadRecord *pRec, int pass )
{
    struct loaddset *pDset;
    long status=0;

    if (pass==0) return(0);

    if( pRec->siml.type == CONSTANT)
      recGblInitConstantLink(&pRec->siml,DBF_USHORT,&pRec->simm);

    if( pRec->siol.type == CONSTANT)
      recGblInitConstantLink(&pRec->siol,DBF_LONG,&pRec->sval);

    if( pRec->sdnl.type == CONSTANT)
      recGblInitConstantLink(&pRec->sdnl,DBF_STRING,&pRec->sdnv);

    if( pRec->supl.type == CONSTANT)
      recGblInitConstantLink(&pRec->supl,DBF_STRING,&pRec->supv);

    if(!(pDset = (struct loaddset *)(pRec->dset))) {
	recGblRecordError(S_dev_noDSET,(void *)pRec,"load: init_record");
	return(S_dev_noDSET);
    }

    /* must have dev_load functions defined */
    if( (pDset->number < 5) || (pDset->dev_load == NULL) ) {
	recGblRecordError(S_dev_missingSup,(void *)pRec,"load: init_record");
	return(S_dev_missingSup);
    }

    /* get the initial value dnl is a constant*/
    if( pRec->dnl.type == CONSTANT )
    {
      if(recGblInitConstantLink(&pRec->dnl,DBF_STRING,pRec->dnv))
        pRec->udf=FALSE;
    }

    /* get the initial value upl is a constant*/
    if( pRec->upl.type == CONSTANT )
    {
      if(recGblInitConstantLink(&pRec->upl,DBF_STRING,pRec->upv))
        pRec->udf=FALSE;
    }

    sprintf(pRec->msg," "); 
    pRec->val = 0;

    if( pDset->init_record ) {
	if((status=(*pDset->init_record)(pRec))) return(status);
    }
    return(0);
}


static long process( loadRecord *pRec )
{
    struct loaddset *pDset;
    long	    status;
    unsigned char   pact;

    status = 0;
    pDset  = (struct loaddset *)(pRec->dset);
    pact   = pRec->pact;

    if( (pDset==NULL) || (pDset->dev_load==NULL) )
    {
      pRec->pact=TRUE;
      recGblRecordError(S_dev_missingSup,(void *)pRec,"dev_load");
      return(S_dev_missingSup);
    }

    if( !pRec->pact )
    {
      if( pRec->dnl.type != CONSTANT )
      {
        status = dbGetLink(&(pRec->dnl),DBR_STRING,pRec->dnv,0,0);
        if(RTN_SUCCESS(status))
          pRec->udf=FALSE;
      }

      if( pRec->upl.type != CONSTANT )
      {
        status = dbGetLink(&(pRec->upl),DBR_STRING,pRec->upv,0,0);
        if(RTN_SUCCESS(status))
          pRec->udf=FALSE;
      }
    }

    if( pRec->udf == TRUE )
    {
      recGblSetSevr(pRec,UDF_ALARM,INVALID_ALARM);
      return(-1);
    }

    /* process the new value */
    if( pRec->nsev < INVALID_ALARM )
      status = download(pRec);

    /* check if device support set pact */
    if( !pact && pRec->pact )
      return(0);

    pRec->pact = TRUE;

    recGblGetTimeStamp(pRec);

    /* check event list */
    monitor(pRec);

    /* process the forward scan link record */
    recGblFwdLink(pRec);

    pRec->pact=FALSE;
    return(status);
}


static long get_value( loadRecord *pRec, struct valueDes *pvdes )
{
    pvdes->field_type  = DBF_LONG;
    pvdes->no_elements = 1;
    pvdes->pvalue      = (void *)(&pRec->val);
    return(0);
}


static void monitor( loadRecord *pRec )
{
    unsigned short monitor_mask;

    monitor_mask = recGblResetAlarms(pRec);

    if(strncmp(pRec->pdnv,pRec->dnv,sizeof(pRec->dnv)))
    {
      db_post_events(pRec,&(pRec->dnv[0]),monitor_mask|DBE_VALUE);
      strncpy(pRec->pdnv,pRec->dnv,sizeof(pRec->dnv));
    }

    if(strncmp(pRec->pupv,pRec->upv,sizeof(pRec->upv)))
    {
      db_post_events(pRec,&(pRec->upv[0]),monitor_mask|DBE_VALUE);
      strncpy(pRec->pupv,pRec->upv,sizeof(pRec->upv));
    }

    if(strncmp(pRec->pmsg,pRec->msg,sizeof(pRec->msg))) 
    {
      db_post_events(pRec,&(pRec->msg[0]),monitor_mask|DBE_VALUE);
      strncpy(pRec->pmsg,pRec->msg,sizeof(pRec->msg));
    }

    if(pRec->pval != pRec->val)
    {
      db_post_events(pRec,&pRec->val,monitor_mask|DBE_VALUE);
      pRec->pval=pRec->val;
    }

    return;
}


static long download( loadRecord *pRec )
{
    long            status;
    struct loaddset *pDset;

    pDset = (struct loaddset *)(pRec->dset);

    if( pRec->pact == TRUE )
    {
      status = (*pDset->dev_load)(pRec);
      return(status);
    }

    status = dbGetLink(&(pRec->siml), DBR_USHORT, &(pRec->simm), 0, 0);
    if(status)
      return(status);

    if(pRec->simm == menuYesNoNO)
    {
      status=(*pDset->dev_load)(pRec);
      return(status);
    }

    if(pRec->simm == menuYesNoYES)
    {
      status = dbGetLink(&(pRec->sdnl), DBR_STRING, &(pRec->sdnv), 0, 0);
      if(status == 0)
      {
        strncpy(pRec->dnv,pRec->sdnv,sizeof(pRec->sdnv));

        status = dbGetLink(&(pRec->supl), DBR_STRING, &(pRec->supv), 0, 0);
	if(status == 0)
        {
	  strncpy(pRec->upv,pRec->supv,sizeof(pRec->supv));

          status = dbGetLink(&(pRec->siol), DBR_LONG, &(pRec->sval), 0, 0);
	  if(status == 0)
          {
	    pRec->val = pRec->sval;
	    pRec->udf = FALSE;
	  }
        }
      }
    }
    else 
    {
      status = -1;
      recGblSetSevr(pRec,SOFT_ALARM,INVALID_ALARM);
      return(status);
    }
    recGblSetSevr(pRec,SIMM_ALARM,pRec->sims);

    return(status);
}
