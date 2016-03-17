/*
 * Header file for PMAC continuous data gathering.
 *
 * Author: Andy Foster
 * Date:   12/04/98
 *
 */

int drvPmacGatInit( int, char *, char *, char * );
int drvPmacGatStart( int, long );
int drvPmacGatStop( int );
int drvPmacGatFlush( int );
int drvPmacGatError( int, char * );
int drvPmacGatResetError( int );
int drvPmacGatWriteMonitor( long, long *, double * );
