#include    <epicsStdioRedirect.h>
#include    <epicsString.h>
#include    <epicsExit.h>
#include    <epicsExport.h>
#include    <registryFunction.h>
#include    <epicsPrint.h>
#include    <epicsInterrupt.h>
#include    <epicsThread.h> /*For reboot handler sleep()*/
#include    <cantProceed.h>
#include    <errlog.h>
#include    <dbDefs.h>
#include    <dbScan.h>
#include    <drvSup.h>
#include    <devSup.h>
#include    <devLib.h>
#include    <iocsh.h>

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <recSup.h>
#include <subRecord.h>
#include <genSubRecord.h>
#include <cadRecord.h>
#include <cad.h>
#include <menuCarstates.h>

#define TIME_INT        0.005  /* 5 msec                             */
#define TRIGGER_LATENCY 0.1    /* Seconds before Bancomm trigger     */
#define JUMP            0.1    /* Degrees change considered a slew   */
#define AZ_JUMP         0.1    /* Degrees change considered a slew   */
#define EL_JUMP         0.1    /* Degrees change considered a slew   */
#define LOG_LIMIT       72000  /* One hour of TCS logging capability */
#define NUM_EXTRAP	20	/* number of points to extrapolate    */
#define	DOUBLE_BUFF	(NUM_EXTRAP>10)
#define	LOGGING_ON	TRUE

#define MIN(x,y)	((x<y)?x:y)
#define MIN3(a,b,c)	((a<b) ? ((a<c) ? a : c) : ((b<c) ? b : c))

long fillBuffer		(double *, double *, double *, double *, double *,
			 double, long, double *, double, double, double,
                         double, double, long, int);
long calc_coeffs	(double *, double *, double *, double *, double *,
			 double *);
int calc_linear		(double, double, double, double, double, double,
			 double, double *, double *, double *);
int calc_quadratic	(double, double, double, double, double, double,
			 double, double *, double *, double * );
int fit_new_AZ_demand   (double, double *, double, double *, double,
                         double *, double, double, double, long, int);
int fit_new_EL_demand   (double, double *, double, double *, double,
                         double *, double, double, double, long, int);


#if LOGGING_ON
/* -----------------------------------------------------------------*/

#define	LOG_NONE	0
#define	LOG_AZIMUTH	1
#define	LOG_ELEVATION	2

#define	LOG_FIT         3
#define	LOG_AZ_FIT      5
#define	LOG_EL_FIT      6
#define	LOG_PMAC        4
#define	LOG_AZ_PMAC     7
#define	LOG_EL_PMAC     8
#define	LOG_AZ_CALC     9
#define	LOG_EL_CALC     10

#define	LOG_TCS_MAX	1000
#define	LOG_AZEL_MAX	1000
#define	LOG_EXTRAP_MAX	6

int	logSingleFlag = TRUE;
int	logEnableFlag = LOG_NONE;
int	logFitFlag    = LOG_NONE;
int	logPmacFlag   = LOG_NONE;
int	logCalcFlag   = LOG_NONE;
int     loggingFlag   = LOG_NONE;

/* Time interrupt log structure.
 */
struct logTimeStruct {
    int		hour;
    int		min;
    int		sec;
    int		frac;
} logTimeData;

/* TCS demands log structure.
 */
struct logTcsStruct {
    double	t;
    double	az;
    double	el;
};

/* trajectory calculation log structure.
 */
struct logTrajStruct {
    double	prevVel;
    double	vel;
    double	velPos;
    double	accel;
    double	prevpa;
    double	prevpb;
    double	prevpc;
    double	pa;
    double	pb;
    double	pc;
    double	currentPos;
    double	newpos;
    double	targetPos;
};

/* Pmac demand and velocity log structure.
 */
struct logPmacStruct {
    double	pos[NUM_EXTRAP];
    double	vel[NUM_EXTRAP];
    double      lastPos;
    double      lastVel;
    double      acceleration;
};

/* Tracking log structure.
 */
struct logTrackStruct {
    int		word;
    double	toff;
    double      startT;
    double      least;
    int         ticks;
    double	t1, t2, t3;
    double	v1, v2, v3;
    double	pos[LOG_EXTRAP_MAX];
    double	vel[LOG_EXTRAP_MAX];
};

/* calc_linear log structure.
 */
struct logCalcStruct {
    double	toff;
    double	ta;
    double	tb;
    double	tc;
    double	pa;
    double	pb;
    double	pc;
    double	A;
    double	B;
    double	C;
    double	pos;
    double	vel;
};

/* TCS log array and counter.
 */
struct logTcsStruct	logTcsData[LOG_TCS_MAX];
int			logTcsCounter = 0;

/* Tracking log array and counter.
 */
struct logTrackStruct	logTrackData[LOG_AZEL_MAX];
int			logTrackCounter = 0;

/* Pmac log array and counter.
 */
struct logPmacStruct	logPmacData[LOG_AZEL_MAX];
int			logPmacCounter = 0;

/* Trajectory calculation log array and counter.
 */
struct logTrajStruct	logTrajData[LOG_AZEL_MAX];
int			logTrajCounter = 0;

/* calculation log array and counter.
 */
struct logCalcStruct	logCalcData[LOG_AZEL_MAX];
int			logCalcCounter = 0;

void logSingle ();
void logWrap ();
void logTracking ();
void logAzEnable ();
void logElEnable ();
void logDisable ();
void logShow ();
void logTime (int, int, int, int);
void logTcs (double, double, double);
extern void logTrack (double[], double[], double[], double[], double[], double, double, double, int, long, long, long, long);

void dumpTimeLog ();
void dumpTcsLog ();
void dumpTrackLog ();
void dumpTimeLog ();
void dumpLogs();
void logClean ();

void logPmacEnable();
void logPmacAzEnable();
void logPmacElEnable();
void logPmacDisable();
void logPmacClean();
void dumpPmac();
extern void logPmac(double[], double[], double, double, double);

void logFitEnable();
void logFitAzEnable();
void logFitElEnable();
void logFitDisable();
void logFitClean();
void dumpFit();

extern void logFit(double, double, double, double, double, double, double, double, double, double, double, double, double);

void logCalcAzEnable();
void logCalcElEnable();
void logCalcDisable();
void logCalcClean();
void dumpCalc();
extern void logCalc(double, double, double, double, double, double, double, double, double, double, double, double);


void logTrackingEnd();

/* -----------------------------------------------------------------*/
#endif

#ifdef OLD_LOGGING
long   tcsjj = 0;
double tcsTT[LOG_LIMIT];
double tcsAz[LOG_LIMIT];
double tcsEl[LOG_LIMIT];
long   azjj = 0;
long   eljj = 0;
double faztt[96000];
double fazpos[96000];
double fazvel[96000];
double feltt[96000];
double felpos[96000];
double felvel[96000];
long   tcsjj = 0;
double tcsTT[96000];
double tcsAz[96000];
double tcsEl[96000];
long   azCounter = 0;
double azAA[96000];
double azBB[96000];
double azCC[96000];
long   afCounter = 0;
double azFit1[96000];
double azFit2[96000];
double azFit3[96000];
long logging = 0;
#endif



/*
 * This routine is at the heart of tracking in the MCS.
 *
 * Inputs:
 * -------
 *    pgsub->a = Array of 5 doubles.
 *    pgsub->b = Array of 5 doubles.
 *    pgsub->c = Array of 5 doubles.
 *    pgsub->d = Which one of the above was most recent?    LONG.
 *    pgsub->e = Follow Flag                                LONG.
 *    pgsub->f = Current Track Id                           DOUBLE.
 *    pgsub->g = Azimuth Current Position                   DOUBLE.
 *    pgsub->h = Azimuth Current Velocity                   DOUBLE.
 *    pgsub->i = Internal use:				    LONG.
 *               Previous recent sample
 *    pgsub->j = Not used				    LONG.
 *    pgsub->k = Elevation Current Position                 DOUBLE.
 *    pgsub->l = Elevation Current Velocity                 DOUBLE.
 *    pgsub->m = Azimuth Current Max. Velocity              DOUBLE.
 *    pgsub->n = Elevation Current Max. Velocity            DOUBLE.
 *    pgsub->o = Azimuth Current Max. Acceleration          DOUBLE.
 *    pgsub->p = Elevation Current Max. Acceleration        DOUBLE.
 *    pgsub->q = Handshake bit from Az. PMAC motion program DOUBLE.
 *    pgsub->r = Handshake bit from El. PMAC motion program DOUBLE.
 *    pgsub->s = Trajectory Calculation Mode                LONG.
 *    pgsub->t = az Pmac Demand Position                    DOUBLE.
 *    pgsub->u = el Pmac Demand Position                    DOUBLE.
 *
 * Outputs:
 * --------
 *    pgsub->vala = Array of NUM_EXTRAP Azimuth Positions           DOUBLE.
 *    pgsub->valb = Array of NUM_EXTRAP Azimuth Velocities          DOUBLE.
 *    pgsub->valc = Array of NUM_EXTRAP Elevation Positions         DOUBLE.
 *    pgsub->vald = Array of NUM_EXTRAP Elevation Velocities        DOUBLE.
 *    pgsub->vale = Mask for fanout                         LONG.
 *                  1  (Link 1) = Set trackId.
 *                  2  (Link 2) = Fill Az. buffer.
 *                  4  (Link 3) = Fill El. buffer.  
 *                  8  (Link 4) = Set time interrupt.
 *                  16 (Link 5) = Set Car Error.
 *    pgsub->valf = Time for external Bancomm Trigger       DOUBLE.
 *    pgsub->valg = TrackId                                 DOUBLE.
 *    pgsub->valh = Current Network time delay (sec)        DOUBLE.
 *    pgsub->vali = Number of samples missed                LONG.
 *    pgsub->valj = Fill bottom/top of Az. buffer (1/2)     LONG.
 *    pgsub->valk = Fill bottom/top of El. buffer (1/2)     LONG.
 *    pgsub->vall = Error string                            LONG.
 *
*/

long Tracking (struct genSubRecord *pgsub)
{
    long   follow;
    double azPos[NUM_EXTRAP];
    long   mask;
    int    i, imax;
    long   azHandShake;
    long   error;

    // static long   prevAzHandShake = 0;	/* top half */
    static double  demand = 0;

    /* Get input values.
     */
    follow          = *(long *) pgsub->e;
    azHandShake     = (long)(*(double *) pgsub->q);
    *(long *)pgsub->valj = 1;

    /* Initialize variables.
     */
    error = 0;
    mask  = 0;

    /* Check if follow is enabled. Process only if that's the case.
     */
    if (follow) 
    {
                        imax = NUM_EXTRAP + 1;
                        for (i=1; i<imax; i++)
                        {
			    azPos[i-1] = demand;
			    demand = demand + 1;
    			}
    }

    /* Copy mask and buffers to output links.
     */
     
       memcpy (pgsub->vala, azPos, NUM_EXTRAP * sizeof(double));

    /* Ok.
     */
    return(0);
}

epicsRegisterFunction(Tracking);



/* calc_coeffs - Not used anymore.
 */
long calc_coeffs (double *aa, double *bb, double *cc, double *A,
		  double *B, double *C)
{
    double denom;
    long   ret;

    denom = (aa[0]-bb[0]) * (aa[0]-cc[0]) * (bb[0]-cc[0]);
    if (denom == 0)
        ret = 1;
    else
    {
	*A  = ((aa[1]-bb[1])*(aa[0]-cc[0]) - (aa[1]-cc[1])*(aa[0]-bb[0])) /
	denom;
	*B  = (aa[1]-cc[1])/(aa[0]-cc[0]) - (*A)*(aa[0]+cc[0]);
	*C  = aa[1] - (*A)*aa[0]*aa[0] - (*B)*aa[0];
	ret = 0;

    }
    return (ret);
}


/* trackingFO - Split an array coming in through B into NUM_EXTRAP single
 * elements. These will be written to PMAC DPRAM further down the
 * chain of processing.
 */
long trackingFO (struct genSubRecord *pgsub)
{
    double *dptr;
    int    i;

    dptr = (double *)pgsub->b;

    for (i=0; i<NUM_EXTRAP; i++)
    {
	switch(i)
	{
	case 0:
	    *(double *)pgsub->vala = *(dptr++);
	    break;

	case 1:
	    *(double *)pgsub->valb = *(dptr++);
	    break;

	case 2:
	    *(double *)pgsub->valc = *(dptr++);
	    break;

	case 3:
	    *(double *)pgsub->vald = *(dptr++);
	    break;

	case 4:
	    *(double *)pgsub->vale = *(dptr++);
	    break;

	case 5:
	    *(double *)pgsub->valf = *(dptr++);
	    break;

	case 6:
	    *(double *)pgsub->valg = *(dptr++);
	    break;

	case 7:
	    *(double *)pgsub->valh = *(dptr++);
	    break;

	case 8:
	    *(double *)pgsub->vali = *(dptr++);
	    break;

	case 9:
	    *(double *)pgsub->valj = *(dptr++);
	    break;

#if NUM_EXTRAP>10
	case 10:
	    *(double *)pgsub->valk = *(dptr++);
	    break;

	case 11:
	    *(double *)pgsub->vall = *(dptr++);
	    break;

	case 12:
	    *(double *)pgsub->valm = *(dptr++);
	    break;

	case 13:
	    *(double *)pgsub->valn = *(dptr++);
	    break;

	case 14:
	    *(double *)pgsub->valo = *(dptr++);
	    break;

	case 15:
	    *(double *)pgsub->valp = *(dptr++);
	    break;

	case 16:
	    *(double *)pgsub->valq = *(dptr++);
	    break;

	case 17:
	    *(double *)pgsub->valr = *(dptr++);
	    break;

	case 18:
	    *(double *)pgsub->vals = *(dptr++);
	    break;

	case 19:
	    *(double *)pgsub->valt = *(dptr++);
	    break;
#endif
	default:
	    printf ("trackingFO: Should never see this\n");
	    break;
        }
    }

    return(0);
}
epicsRegisterFunction(trackingFO);

