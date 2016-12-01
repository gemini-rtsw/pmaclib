#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <epicsStdioRedirect.h>
#include <epicsStdlib.h>
#include <epicsTypes.h>
#include <epicsMath.h>
#include <epicsString.h>

#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <recSup.h>
#include <genSubRecord.h>
#include <menuCarstates.h>
#include <epicsExport.h>
#include <registryFunction.h>
#include <epicsPrint.h>

#define ERROR (-1)
#define OK      0


#define SMALL_BUF             16
#define MEDIUM_BUF            64
#define LARGE_BUF             6144
#define HEADER                "A\nCLOSE\nDELETE GATHER\nDELETE TRACE\n"
#define MASK                  0xFFFF
#define MOTION_PROG_BASEADDR  0x0E00
#define MOTION_PROG_TOTAL     256
#define MOTION_PROG_1         0x1800
#define PLC_PROG_BASEADDR     0x0F00
#define PLC_PROG_TOTAL        32
#define PLC_DISABLE_BIT       22
#define TEMPFILE              "tempFile"
#define RESULTS               "results"


#define pmacUpDownDebug 1
#define PMAC_DIAGNOSTICS TRUE
#if PMAC_DIAGNOSTICS
#define PMAC_MESSAGE    errlogPrintf
#define PMAC_DEBUG(level,code)       { if (pmacUpDownDebug >= (level)) { code } }
#else
#define PMAC_DEBUG(level,code)      ;
#endif

long createScript( char *, char *, char * );
long createMotionSummary( char *, char * );  
long createPlcSummary( char *, char * );

/* 
 * Triggers the PMAC UPLOAD/DOWNLOAD command
 *
 * INPUTS:
 * 	pgsub->j    = "UPLOAD" or "DOWNLOAD" (STRING).
 *
 * OUTPUTS:
 *      pgsub->vala = "UPLOAD" or "DOWNLOAD" (STRING).
 */

long UpDownTrigger( struct genSubRecord *pgsub )
{
  strncpy(pgsub->vala, pgsub->j, MAX_STRING_SIZE);
  return(0);
}


/* 
 * Sets up a file for downloading to PMAC.
 * This will either be one chosen by the user or one created internally which
 * causes the contents of a PMAC PLC or MOTION program to be uploaded.
 *
 * INPUTS:
 * 	pgsub->a = PMAC Card Number                                (DOUBLE).
 *      pgsub->b = PLC or MOTION                                   (STRING).
 *      pgsub->c = Program Number                                  (DOUBLE).
 *      pgsub->d = Path name of directory in which files are saved (STRING).
 *      pgsub->e = UPLOAD or DOWNLOAD                              (STRING).
 *      pgsub->f = User's choice of filename to download           (STRING).
 *
 * OUTPUTS:
 * 	pgsub->vala = Name of file to download                        (STRING).
 *      pgsub->valb = Name of file containing uploaded data           (STRING).
 *      pgsub->valc = Error message from this function                (STRING).
 *      pgsub->vald = Path name of directory in which files are saved (STRING).
 *      pgsub->vale = UPLOAD or DOWNLOAD                              (STRING).
 *      pgsub->valf = PLC or MOTION                                   (STRING).
 *      pgsub->valg = Program Number                                  (LONG).
 */

long UpDownStart( struct genSubRecord *pgsub )
{
  int  fd;
  long status;
  long progNum;
  long pmacNum;
  char downfile[MEDIUM_BUF];
  char buffer[MEDIUM_BUF];
  char bufCheck[MEDIUM_BUF];
  char progType[MAX_STRING_SIZE];
  char pmacString[SMALL_BUF];
  char saveDir[MAX_STRING_SIZE];

  status  = 0;
  pmacNum = (long)(*(double *)pgsub->a);
  strncpy(progType, pgsub->b, MAX_STRING_SIZE);
  progNum = (long)(*(double *)pgsub->c);
  strncpy(saveDir, pgsub->d, MAX_STRING_SIZE);

  if( !strcmp( pgsub->e, "UPLOAD") )
  {
    switch(pmacNum)
    {
      case 0:
        strcpy(pmacString, "AZ");
        break;

      case 1:
        strcpy(pmacString, "EL");
        break;

      case 2:
        strcpy(pmacString, "SWC");
        break;

      default:
        status = 1;
        break;
    }

    if( !status )
    {
      if( !strcmp(progType, "PLC") )
      {
        if( (progNum < 0) || (progNum > 31) )
        {
          sprintf(pgsub->valc, "PLC number %ld is invalid", progNum);
          status = 1;
        }
      }
      else
      {
        if( (progNum < 1) || (progNum > 32766) )
        {
          sprintf(pgsub->valc, "Motion Program number %ld is invalid", progNum);
          status = 1;
        }
      }

      if( !status )
      {
        sprintf(downfile, "%s/%s", saveDir, TEMPFILE);
        fd = open(downfile, O_CREAT | O_RDWR, 0644);
        if( fd == ERROR )
        {
          printf("UpDownStart: %s not opened\n", downfile);
          strncpy(pgsub->valc, "Down file not opened", MAX_STRING_SIZE);
          status = 1;
        }
      }

      if( !status )
      {
        if( !strcmp(progType, "PLC") )
        {
          sprintf(buffer, "%sOPEN PLC %ld\nLIST\nCLOSE\n", HEADER, progNum);
          sprintf(bufCheck, "%s/pmac%s_%ld.plc", saveDir, pmacString, progNum);
          if( strlen(bufCheck) > MAX_STRING_SIZE )
            printf("UpDownStart: %s\n", bufCheck);
          strncpy(pgsub->valb, bufCheck, MAX_STRING_SIZE);
        }
        else
        {
          sprintf(buffer, "%sOPEN PROG %ld\nLIST\nCLOSE\n", HEADER, progNum);
          sprintf(bufCheck, "%s/pmac%s_%ld.mpg", saveDir, pmacString, progNum);
          if( strlen(bufCheck) > MAX_STRING_SIZE )
            printf("UpDownStart: %s\n", bufCheck);
          strncpy(pgsub->valb, bufCheck, MAX_STRING_SIZE);
        }

        write( fd, buffer, strlen(buffer) );
        close(fd);
        strncpy(pgsub->valc, pgsub->valb, MAX_STRING_SIZE);
      }
    }
    strncpy(pgsub->vala, downfile, MAX_STRING_SIZE);
  }
  else  /* Case of Download */
  {
    strncpy(pgsub->vala, pgsub->f, MAX_STRING_SIZE);
    sprintf(bufCheck, "%s/errors.pmc", saveDir);
    if( strlen(bufCheck) > MAX_STRING_SIZE )
      printf("UpDownStart: %s\n", bufCheck);
    strncpy(pgsub->valb, bufCheck, MAX_STRING_SIZE);
    strncpy(pgsub->valc, pgsub->valb, MAX_STRING_SIZE);
  }

  strncpy(pgsub->vald, pgsub->d, MAX_STRING_SIZE);
  strncpy(pgsub->vale, pgsub->e, MAX_STRING_SIZE);
  strncpy(pgsub->valf, pgsub->b, MAX_STRING_SIZE);
  *(long *)pgsub->valg = progNum;
  return(status);
}


/* 
 * Cleans up after an UP/DOWN load from/to PMAC
 *
 * INPUTS:
 * 	pgsub->a = Error Number from "Start"      (LONG).
 *      pgsub->b = Error Message from "Start"     (STRING).
 *      pgsub->c = Error Number from "Load"       (LONG).
 *      pgsub->d = Error Message from "Load"      (STRING).
 *      pgsub->e = File which was downloaded      (STRING).
 *      pgsub->f = File which was uploaded        (STRING).
 *      pgsub->g = UPLOAD or DOWNLOAD             (STRING).
 *      pgsub->h = Path to save directory         (STRING).
 *      pgsub->i = MOTION or PLC                  (STRING).
 *      pgsub->j = Program Number                 (LONG).
 *
 * OUTPUTS:
 * 	pgsub->vala = Error message to PMAC CAR   (STRING).
 *      pgsub->valb = State to PMAC CAR           (LONG).
 *      pgsub->valc = Status message to PMAC SIR  (STRING).
 */

long UpDownEnd( struct genSubRecord *pgsub )
{
  int  error;
  int  fd;
  char *c;
  char *cprev;
  char str[MAX_STRING_SIZE];
  long startError;
  char startMess[MAX_STRING_SIZE];
  long loadError;
  char loadMess[MAX_STRING_SIZE];
  char saveDir[MAX_STRING_SIZE];
  char buf[MEDIUM_BUF];
  char *MyName = "UpDownEnd";

  error        = 0;
  startError   = *(long *)pgsub->a;
  strncpy(startMess, pgsub->b, MAX_STRING_SIZE);
  loadError    = *(long *)pgsub->c;
  strncpy(loadMess, pgsub->d, MAX_STRING_SIZE);
  strncpy(saveDir, pgsub->h, MAX_STRING_SIZE);

  if( !strcmp(pgsub->g, "UPLOAD") )
  {
    fd = open(pgsub->e, O_RDONLY, 0644);
    if(fd != ERROR)
    {
      close(fd);
      error = remove(pgsub->e);
      PMAC_DEBUG(1,
          PMAC_MESSAGE("%s: Could not open upload file %s\n", MyName, (char *)pgsub->e);
      )
    }

    if( startError != 0 )
    {
      strncpy(pgsub->vala, startMess, MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else if( loadError != 0 )
    {
      strncpy(pgsub->vala, loadMess, MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else if( error )
    {
      printf("UpDownEnd: Error trying to remove file: %s", (char *)pgsub->e);
      strncpy(pgsub->vala, "Error trying to remove file", MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else
    {
      strncpy(str, pgsub->f, MAX_STRING_SIZE);
      if(!(c   = (char *)strtok(str, "/"))) {
         printf("Bad file name in %s at line %d\n", __FILE__, __LINE__);
         return ERROR; 
      }
      
      while( c )
      {
        cprev = c;
        c     = (char *)strtok(NULL, "/");
      }
      if( *(cprev+4) == 'A' )
        sprintf(buf, 
          "Azimuth PMAC %s Program No. %ld", (char *)pgsub->i, *(long *)pgsub->j);
      else if( *(cprev+4) == 'E' )
        sprintf(buf, 
          "Elevation PMAC %s Program No. %ld", (char *)pgsub->i, *(long *)pgsub->j);
      else
        sprintf(buf, 
          "SWC PMAC %s Program No. %ld", (char *)pgsub->i, *(long *)pgsub->j);
      error = createScript( saveDir, pgsub->f, buf );
      if( !error )
      {
        strcpy(pgsub->vala, " ");
        *(long *)pgsub->valb = menuCarstatesIDLE;
        strncpy(pgsub->valc, startMess, MAX_STRING_SIZE);
      }
      else
      {
        strcpy(pgsub->vala, "Error creating display script");
        *(long *)pgsub->valb = menuCarstatesERROR;
        strcpy(pgsub->valc, " ");
      }
    }
  }
  else  /* Case of Download */
  {
    if( loadError != 0 )
    {
      strncpy(pgsub->vala, loadMess, MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else
    {
      error = createScript( saveDir, pgsub->f, "PMAC errors from downloading file" );
      if( !error )
      {
        strcpy(pgsub->vala, " ");
        *(long *)pgsub->valb = menuCarstatesIDLE;
        strncpy(pgsub->valc, startMess, MAX_STRING_SIZE);
      }
      else
      {
        strcpy(pgsub->vala, "Error creating display script");
        *(long *)pgsub->valb = menuCarstatesERROR;
        strcpy(pgsub->valc, " ");
      }
    }
  }
  return(0);
}


/* 
 * Sets up download file to read back information about loaded PMAC 
 * PLC or MOTION programs
 *
 * INPUTS:
 *      pgsub->b = PLC or MOTION    (STRING).
 *      pgsub->c = Save Path        (STRING).
 *
 * OUTPUTS:
 * 	pgsub->vala = Name of file to download            (STRING).
 *      pgsub->valb = Name of file which contains results (STRING).
 *      pgsub->valc = Error message from this function    (STRING).
 *      pgsub->vald = PLC or MOTION                       (STRING).
 *      pgsub->vale = Save Path                           (STRING).
 */

long ProgStatusStart( struct genSubRecord *pgsub )
{
  int  fd;
  long i;
  long base;
  long total;
  long status;
  char downfile[MEDIUM_BUF];
  char bufLarge[LARGE_BUF];
  char bufSmall[SMALL_BUF];
  char progType[MAX_STRING_SIZE];
  char saveDir[MAX_STRING_SIZE];

  status  = 0;
  strncpy(progType, pgsub->b, MAX_STRING_SIZE);
  strncpy(saveDir, pgsub->c, MAX_STRING_SIZE);

  sprintf(downfile, "%s/%s", saveDir, TEMPFILE);
  fd = open(downfile, O_CREAT | O_RDWR, 0644);
  if( fd == ERROR )
  {
    printf("ProgStatusStart: %s not opened\n", downfile);
    strncpy(pgsub->valc, "Down file not opened", MAX_STRING_SIZE);
    status = 1;
  }

  if( !status )
  {
    sprintf(bufLarge, "%s", HEADER);
    if( !strcmp(progType, "MOTION") )
    {
      total = MOTION_PROG_TOTAL;
      base  = MOTION_PROG_BASEADDR;
    }
    else
    {
      total = PLC_PROG_TOTAL;
      base  = PLC_PROG_BASEADDR;
      sprintf(bufSmall, "i5\n");
      strcat(bufLarge, bufSmall);
    }

    for( i=0; i<total; i++ )
    {
      sprintf(bufSmall, "rhx:$%lx\n", base);
      strcat(bufLarge, bufSmall);
      sprintf(bufSmall, "rhy:$%lx\n", base);
      strcat(bufLarge, bufSmall);
      base++;
    }
    write( fd, bufLarge, strlen(bufLarge) );
    close(fd);
  }

  strncpy(pgsub->vala, downfile, MAX_STRING_SIZE);
  sprintf(bufLarge, "%s/%s", saveDir, RESULTS);
  if( strlen(bufLarge) > MAX_STRING_SIZE )
    printf("ProgStatusStart: %s\n", bufLarge);
  strncpy(pgsub->valb, bufLarge, MAX_STRING_SIZE);
  strncpy(pgsub->vald, progType, MAX_STRING_SIZE);
  strncpy(pgsub->vale, pgsub->c, MAX_STRING_SIZE);

  return(status);
}


/* 
 * Cleans up after getting status information about loaded PLC
 * and MOTION programs. Creates final result file and Tcl/Tk display
 * script.
 *
 * INPUTS:
 * 	pgsub->a = Error Number from "Start"      (LONG).
 *      pgsub->b = Error Message from "Start"     (STRING).
 *      pgsub->c = Error Number from "Load"       (LONG).
 *      pgsub->d = Error Message from "Load"      (STRING).
 *      pgsub->e = File which was downloaded      (STRING).
 *      pgsub->f = File which was uploaded        (STRING).
 *      pgsub->g = PLC or MOTION                  (STRING).
 *      pgsub->h = Path to save directory         (STRING).
 *      pgsub->i = PMAC Card Number               (DOUBLE).
 *
 * OUTPUTS:
 * 	pgsub->vala = Error message to PMAC CAR   (STRING).
 *      pgsub->valb = State to PMAC CAR           (LONG).
 *      pgsub->valc = Status message to PMAC SIR  (STRING).
 */

long ProgStatusEnd( struct genSubRecord *pgsub )
{
  int  error;
  int  fd;
  long startError;
  char startMess[MAX_STRING_SIZE];
  long loadError;
  long pmacNum;
  char loadMess[MAX_STRING_SIZE];
  char saveDir[MAX_STRING_SIZE];
  char buffer[MEDIUM_BUF];
  char pmacString[SMALL_BUF];
  char summaryFile[MEDIUM_BUF];

  error        = 0;
  startError   = *(long *)pgsub->a;
  strncpy(startMess, pgsub->b, MAX_STRING_SIZE);
  loadError    = *(long *)pgsub->c;
  strncpy(loadMess, pgsub->d, MAX_STRING_SIZE);
  strncpy(saveDir, pgsub->h, MAX_STRING_SIZE);
  pmacNum      = (long)(*(double *)pgsub->i);

  switch(pmacNum)
  {
    case 0:
      strcpy(pmacString, "AZ");
      break;

    case 1:
      strcpy(pmacString, "EL");
      break;

    case 2:
      strcpy(pmacString, "SWC");
      break;

    default:
      error = 1;
      break;
  }

  if( !error )
  {
    fd = open(pgsub->e, O_RDONLY, 0644);
    if(fd != ERROR)
    {
      close(fd);
      error = remove(pgsub->e);
      if( error )
      {
        printf("ProgStatusEnd: Error trying to remove file: %s", (char *)pgsub->e);
        strncpy(pgsub->vala, "Error trying to remove file", MAX_STRING_SIZE);
        *(long *)pgsub->valb = menuCarstatesERROR;
        strcpy(pgsub->valc, " ");
      }
    }
  }
  else
  {
    sprintf(pgsub->vala, "ProgStatusEnd: Unknown PMAC card number %ld", pmacNum);
    *(long *)pgsub->valb = menuCarstatesERROR;
    strcpy(pgsub->valc, " ");
  }

  if( !error )
  {
    if( startError != 0 )
    {
      strncpy(pgsub->vala, startMess, MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else if( loadError != 0 )
    {
      strncpy(pgsub->vala, loadMess, MAX_STRING_SIZE);
      *(long *)pgsub->valb = menuCarstatesERROR;
      strcpy(pgsub->valc, " ");
    }
    else
    {
      if( !strcmp( pgsub->g, "MOTION" ) )
      {
        sprintf(summaryFile, "%s/%smotionProgs.sum", saveDir, pmacString);
        error = createMotionSummary( pgsub->f, summaryFile );
        if( error == 1 )
        {
          strncpy(pgsub->vala, "Error opening file for writing", MAX_STRING_SIZE);
          *(long *)pgsub->valb = menuCarstatesERROR;
          strcpy(pgsub->valc, " ");
        }
        else if( error == 2 )
        {
          strncpy(pgsub->vala, "Error opening file for reading", MAX_STRING_SIZE);
          *(long *)pgsub->valb = menuCarstatesERROR;
          strcpy(pgsub->valc, " ");
        }
        else
        { 
          if( pmacNum == 0 )
            strcpy(buffer, "Motion Programs stored in Azimuth PMAC's Memory\0");
          else if( pmacNum == 1 )
            strcpy(buffer, "Motion Programs stored in Elevation PMAC's Memory\0");
          else
            strcpy(buffer, "Motion Programs stored in SWC PMAC's Memory");

          error = createScript( saveDir, summaryFile, buffer );
          if( !error )
          {
            /* Remove unwanted original upload file */
            error = remove(pgsub->f);
            if( !error )
            {
              strcpy(pgsub->vala, " ");
              *(long *)pgsub->valb = menuCarstatesIDLE;
              sprintf(buffer, "Result is in %s", summaryFile);
              if( strlen(buffer) > MAX_STRING_SIZE )
                printf("%s\n", buffer);
              strncpy(pgsub->valc, buffer, MAX_STRING_SIZE);
            }
            else
            {
              sprintf(buffer, "Error removing file %s", (char *)pgsub->f);
              if( strlen(buffer) > MAX_STRING_SIZE )
                printf("%s\n", buffer);
              strncpy(pgsub->vala, buffer, MAX_STRING_SIZE);
              *(long *)pgsub->valb = menuCarstatesERROR;
              strcpy(pgsub->valc, " ");
            }
          }
          else
          {
            strcpy( pgsub->vala, "Error creating display script");
            *(long *)pgsub->valb = menuCarstatesERROR;
            strcpy( pgsub->valc, " " );
          }
        }
      }
      else  /* status of "PLC" programs */
      {
        sprintf(summaryFile, "%s/%splcProgs.sum", saveDir, pmacString);
        error = createPlcSummary( pgsub->f, summaryFile );
        if( error == 1 )
        {
          strncpy(pgsub->vala, "Error opening file for writing", MAX_STRING_SIZE);
          *(long *)pgsub->valb = menuCarstatesERROR;
          strcpy(pgsub->valc, " ");
        }
        else if( error == 2 )
        {
          strncpy(pgsub->vala, "Error opening file for reading", MAX_STRING_SIZE);
          *(long *)pgsub->valb = menuCarstatesERROR;
          strcpy(pgsub->valc, " ");
        }
        else
        { 
          if( pmacNum == 0 )
            strcpy(buffer, "PLC Programs stored in Azimuth PMAC's Memory\0");
          else if( pmacNum == 1 )
            strcpy(buffer, "PLC Programs stored in Elevation PMAC's Memory\0");
          else
            strcpy(buffer, "PLC Programs stored in SWC PMAC's Memory");

          error = createScript( saveDir, summaryFile, buffer );
          if( !error )
          {
            /* Remove unwanted original upload file */
            error = remove(pgsub->f);
            if( !error )
            {
              strcpy(pgsub->vala, " ");
              *(long *)pgsub->valb = menuCarstatesIDLE;
              sprintf(buffer, "Result is in %s", summaryFile);
              if( strlen(buffer) > MAX_STRING_SIZE )
                printf("%s\n", buffer);
              strncpy(pgsub->valc, buffer, MAX_STRING_SIZE);
            }
            else
            {
              sprintf(buffer, "Error removing file %s", (char *)pgsub->f);
              if( strlen(buffer) > MAX_STRING_SIZE )
                printf("%s\n", buffer);
              strncpy(pgsub->vala, buffer, MAX_STRING_SIZE);
              *(long *)pgsub->valb = menuCarstatesERROR;
              strcpy(pgsub->valc, " ");
            }
          }
          else
          {
            strcpy( pgsub->vala, "Error creating display script");
            *(long *)pgsub->valb = menuCarstatesERROR;
            strcpy( pgsub->valc, " " );
          }
        }
      }
    }
  }
  return(0);
}


/* 
   Creates a simple startup script for a Tcl/Tk application used to display
   the results file
*/

long createScript( char *saveDir, char *filename, char *header )
{
  static long prevLen = 0;
  long        len;
  long        error;
  int         fd;
  char        displayName[MEDIUM_BUF];
  char        buffer[LARGE_BUF];

  error = 0;
  sprintf(displayName, "%s/displayFile.sh", saveDir);
  fd = open( displayName, O_CREAT | O_RDWR, 0755 );
  if(fd == ERROR)
  {
    printf("createScript: %s not opened\n", displayName);
    error = 1;
  }
  else
  {
    sprintf(buffer, 
     "#!/bin/csh\n$EPICS/extensions/bin/solaris/et_wish -f %s/data/displayFile.tk %s \"%s\"\n", saveDir, filename, header);

    /* Padding because contents of old file are NOT removed */
    len = strlen(buffer);
    if( prevLen > len )
      memset(&buffer[len], ' ', prevLen-len);
    prevLen = len;

    write( fd, buffer, strlen(buffer) );
    close(fd);
  }
  return(error);
}


long createMotionSummary( char *readFile, char *writeFile )  
{
  FILE *fd_read;
  FILE *fd_write;
  long val;
  long add;
  long len;
  long oldadd;
  long totw;
  long totp;
  long error;

  error   = 0;
  totw    = 0;
  totp    = 0;
  fd_read = fopen( readFile, "r" );
  if( fd_read )
  {
    fd_write = fopen( writeFile, "w+" );
    if( fd_write )
    {
      oldadd = MOTION_PROG_1;
      fprintf( fd_write, "Program Number      Address      Length\n");
      fprintf( fd_write, "------------------------------------------\n");
      while( fscanf( fd_read, "%lx", &val ) != EOF )
      {
        val &= MASK;
        if( !val )
          break;
   
        fscanf( fd_read, "%lx", &add );
        len    = add - oldadd;
        oldadd = add;
        totw  += len;
        totp++;
        fprintf( fd_write, "   %6ld            0x%lx  %6ld words\n", val, oldadd, len);
      }
      fprintf( fd_write, 
             "\nTotal of %ld programs occupying %ld words in PMAC's Memory\n", totp, totw);
      fclose(fd_write);
    }
    else
    {
      printf("CreateMotionSummary: Error opening %s for writing\n", writeFile);
      error = 1;
    }
  }
  else
  {
    printf("CreateMotionSummary: Error opening %s for reading\n", readFile);
    error = 2;
  }
  return(error);
}


long createPlcSummary( char *readFile, char *writeFile )  
{
  FILE *fd_read;
  FILE *fd_write;
  int  i;
  long i5;
  long disableBit;
  long len;
  long startAdd;
  long endAdd;
  long totw;
  long totp;
  long error;
  char *str;
  char buf[SMALL_BUF];

  error   = 0;
  totw    = 0;
  totp    = 0;
  fd_read = fopen( readFile, "r" );
  if( fd_read )
  {
    fd_write = fopen( writeFile, "w+" );
    if( fd_write )
    {
      fscanf( fd_read, "%s", buf );
      str = (char *)strtok(buf, "=");
      str = (char *)strtok(NULL, "=");
      i5  = atol(str);
      if( i5 == 0 )
      {
        fprintf( fd_write, "      I5=0: PLC0    cannot be enabled.\n");
        fprintf( fd_write, "            PLC1-31 cannot be enabled.\n");
      }
      else if( i5 == 1 )
      {
        fprintf( fd_write, "      I5=1: PLC0       can be enabled.\n");
        fprintf( fd_write, "            PLC1-31 cannot be enabled.\n");
      }
      else if( i5 == 2 )
      {
        fprintf( fd_write, "      I5=2: PLC0    cannot be enabled.\n");
        fprintf( fd_write, "            PLC1-31 can    be enabled.\n");
      }
      else if( i5 == 3 )
      {
        fprintf( fd_write, "      I5=3: PLC0    can be enabled.\n");
        fprintf( fd_write, "            PLC1-31 can be enabled.\n");
      }
      fprintf( fd_write, "\n      PLC            Address      Length     Active\n");
      fprintf( fd_write, "      ---------------------------------------------\n");
      for( i=0; i<PLC_PROG_TOTAL; i++ )
      {
        fscanf( fd_read, "%lx", &startAdd );
        startAdd  &= MASK;
        fscanf( fd_read, "%lx", &endAdd );
        disableBit = (endAdd>>PLC_DISABLE_BIT)&1;
        endAdd    &= MASK;
        len        = endAdd - startAdd;
        if( len )
        {
          totw += len;
          totp++;
          if( disableBit )
            sprintf(buf, "NO");
          else
            sprintf(buf, "YES");
          fprintf( fd_write, "   %6d            0x%lx  %6ld words     %s\n",
            i, startAdd, len, buf);
        }
      }
      fprintf( fd_write, 
           "\n      Total of %ld PLC's occupying %ld words in PMAC's Memory\n", totp, totw);
      fclose(fd_write);
    }
    else
    {
      printf("CreatePlcSummary: Error opening %s for writing\n", writeFile);
      error = 1;
    }
  }
  else
  {
    printf("CreatePlcSummary: Error opening %s for reading\n", readFile);
    error = 2;
  }
  return(error);
}
epicsRegisterFunction(UpDownTrigger);
epicsRegisterFunction(UpDownStart);
epicsRegisterFunction(UpDownEnd);

