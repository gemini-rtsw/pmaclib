/*
 * EPICS PMAC Device Driver support for continuous data gathering.
 *
 * Author: Andy Foster
 * Date:   12/04/98
 *
 */

/* vxWorks Includes */

#include <vxWorks.h>
#include <vxLib.h>
#include <stdio.h>
#include <sysLib.h>
#include <string.h>
#include <taskLib.h>
#include <tickLib.h>
#include <math.h>

/* EPICS Includes */

#include <errMdef.h>
#include <taskwd.h>
#include <callback.h>

#include <pmacVme.h>
#include <drvPmac.h>
#include <drvPmacGat.h>

#define DEBUG       0
#define DEBUG_WRITE 0

#define PMAC_GAT_READ       "pmacGatR"
#define PMAC_GAT_WRITE      "pmacGatW"
#define PMAC_GAT_PRI        45
#define PMAC_GAT_OPT        VX_FP_TASK
#define PMAC_GAT_STACK      8000
#define DPRAM_FACTOR        58          /* Allows Maximum DPRAM size to */
                                        /* not exceed 0xDCFF or 0xDD3F  */

#define WRITE_BOTTOM        0
#define WRITE_TOP           1

#define PMAC_GATTYP_Y       0x00
#define PMAC_GATTYP_X       0x40
#define PMAC_GATTYP_D       0x80
#define PMAC_GATTYP_L       0xC0
#define PMAC_GATTYP_F       0xF0
#define PMAC_GATTYP_P       0xE0

/* Global variables */

long prevDPRAMend[PMAC_MAX_CARDS];
long DPRAMsize[PMAC_MAX_CARDS];      /* The size of the DPRAM buffer     */
long pmac_buf_size[PMAC_MAX_CARDS];  /* The size of the in-memory buffer */
FILE *fd[PMAC_MAX_CARDS];
int  readingDPRAM[PMAC_MAX_CARDS];
int  writingFile[PMAC_MAX_CARDS];
double transferRate[PMAC_MAX_CARDS];
long   amountWritten[PMAC_MAX_CARDS];
long ticksBefore[PMAC_MAX_CARDS];
long ticksAfter[PMAC_MAX_CARDS];
int  writeFlag[PMAC_MAX_CARDS];
char *gatherBuffer[PMAC_MAX_CARDS];
long buf_pos[PMAC_MAX_CARDS];
char errorString[PMAC_MAX_CARDS][40];

int  drvPmacGatReadTask( int );
int  drvPmacGatWriteTask( int );
char drvPmacMbxWriteRead( int, char *, char *, char *);
void init_buffer( int card );
long write_buffer( int, char *, int );


PMAC_LOCAL void drvPmacGatScanInit( int	card )
{
  PMAC_CARD *pCard = &drvPmacCard[card];
	
  pCard->scanGatReadSem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);
  if( pCard->scanGatReadSem == NULL )
    errMessage (0, "drvPmacGatScanInit: read semBcreate failed");
  else
  {
    sprintf(pCard->scanGatReadName, "%s%d", PMAC_GAT_READ, pCard->card);
    pCard->scanGatReadId = taskSpawn( pCard->scanGatReadName,
			              PMAC_GAT_PRI, PMAC_GAT_OPT, PMAC_GAT_STACK,
			              (FUNCPTR)drvPmacGatReadTask,
			              pCard->card,0,0,0,0,0,0,0,0,0 );
    taskwdInsert(pCard->scanGatReadId, NULL, 0L);
  }

  pCard->scanGatWriteSem = semBCreate(SEM_Q_FIFO,SEM_EMPTY);
  if( pCard->scanGatWriteSem == NULL )
    errMessage (0, "drvPmacGatScanInit: write semBcreate failed");
  else
  {
    sprintf(pCard->scanGatWriteName, "%s%d", PMAC_GAT_WRITE, pCard->card);
    pCard->scanGatWriteId = taskSpawn( pCard->scanGatWriteName,
				       PMAC_GAT_PRI, PMAC_GAT_OPT, PMAC_GAT_STACK,
			               (FUNCPTR)drvPmacGatWriteTask,
			               pCard->card,0,0,0,0,0,0,0,0,0 );
    taskwdInsert (pCard->scanGatWriteId, NULL, 0L);
  }

  gatherBuffer[card] = NULL;

  return;
}


long drvPmacGatSources( int card )
{
  long      terminator;
  char      command[PMAC_MBX_OUT_BUFLEN];
  char      response[PMAC_MBX_IN_BUFLEN];
  char      errmsg[PMAC_MBX_ERR_BUFLEN];
  int       I20;
  int       source;
  int       totSinglewords;
  PMAC_CARD *pCard = &drvPmacCard[card];

  totSinglewords = 0;
  sprintf(command, "I20");
  terminator = drvPmacMbxWriteRead(card, command, response, errmsg);
  sscanf(response, "%d", &I20);

  for( source = 0; source < PMAC_MAX_GAT; source++ )
  {
    pCard->GatIo[source].srcEna = ((I20 >> source) && 0x1);
    sprintf (command, "I%d", (source + 21));
    terminator = drvPmacMbxWriteRead (card, command, response, errmsg);
    sscanf (response, "%d", &pCard->GatIo[source].srcIx);
    pCard->GatIo[source].srcType = ((0x00FF0000 & pCard->GatIo[source].srcIx) >> 16);

    if( pCard->GatIo[source].srcEna )
    {
      switch( pCard->GatIo[source].srcType )
      {
        case PMAC_GATTYP_D:
          totSinglewords += 2;
          break;

        case PMAC_GATTYP_L:
          totSinglewords += 2;
          break;

        case PMAC_GATTYP_F:
          totSinglewords += 2;
          break;

        case PMAC_GATTYP_P:
          totSinglewords += 2;
          break;

        case PMAC_GATTYP_X:
          totSinglewords += 1;
          break;

        case PMAC_GATTYP_Y:
          totSinglewords += 1;
          break;

        default:
          break;
      }
    }
  }

  sprintf(command, "DELETE GATHER");
  terminator = drvPmacMbxWriteRead(card, command, response, errmsg);
  if( errmsg[0] )
    printf("Error message returned from DELETE GATHER command: %s\n", errmsg);

  DPRAMsize[card] = DPRAM_FACTOR*totSinglewords;
  sprintf(command, "DEFINE GATHER %ld", DPRAMsize[card]);
  terminator = drvPmacMbxWriteRead(card, command, response, errmsg);
  if( errmsg[0] )
    printf("Error message returned from DEFINE GATHER %ld command: %s\n", DPRAMsize[card], errmsg);

#if DEBUG
  printf("drvPmacGatSources: DPRAMsize[%d] = %d, totSinglewords = %d\n", 
          card, DPRAMsize[card], totSinglewords);
#endif

  return(0);
}


int drvPmacGatReadTask( int card )
{
  int       ofs;
  char      *val;
  long      status;
  long      end;
  long      size;
  long      ret;
  long      terminator;
  long      pmac_gat_offset[PMAC_MAX_CARDS];
  long      pmac_gat_nextaddr[PMAC_MAX_CARDS];
  int       delay;
  char      response[PMAC_MBX_IN_BUFLEN];
  char      errmsg[PMAC_MBX_ERR_BUFLEN];
  PMAC_CARD *pCard = &drvPmacCard[card];
		
  delay = sysClkRateGet()/50;   /* 50 Hz */

  printf("Frequency of read = %d Hz\n", sysClkRateGet()/delay);

  terminator = drvPmacMbxWriteRead(card, "version", response, errmsg);
  if( errmsg[0] )
  {
    printf("Error message returned from VERSION command: %s\n", errmsg);
    return(1);
  }
  else
  {
    if( !strncmp(response, "1.15G", 5) || 
        !strncmp(response, "1.16A", 5) || 
        !strncmp(response, "1.16B", 5) )
    {
      pmac_gat_offset[card]   = 0x0800;
      pmac_gat_nextaddr[card] = 0x07FE;
      printf("PMAC Card %d - Firmware version %s\n", card, response);
    }
    else if( !strncmp(response, "1.16C", 5) || !strncmp(response, "1.16D", 5) )
    {
      pmac_gat_offset[card]   = 0x0900;
      pmac_gat_nextaddr[card] = 0x08FE;
      printf("PMAC Card %d - Firmware version %s\n", card, response);
    }
    else
    {
      printf("Unknown version of PMAC firmware: %s\n", response);
      return(1);
    }
  }

  FOREVER
  {
    if( semTake(pCard->scanGatReadSem, WAIT_FOREVER) != OK )
      errMessage(0,"drvPmacGatReadTask: read semTake returned error");
    else
    {
      while( pCard->logFlag )
      {
        taskDelay(delay);
        taskLock();
        readingDPRAM[card] = 1;
        ofs                = 4*prevDPRAMend[card] + pmac_gat_offset[card];
        status             = pmacRamGetH(pmacRamAddr(card, pmac_gat_nextaddr[card]), &end);
        val                = (char *)pmacRamAddr(card, ofs);

        if( end == prevDPRAMend[card] )
        {
          /* no new data - assuming we are running fast enough not to have 
             wrapped around an integer number of times.
             Note that this 'continue' jumps back to the 'taskDelay' statement
             above, which is why the 'taskDelay' is where it is. If a delay
             does not follow this at slow logging speeds, the system hangs */
          continue;
        }
        else if( end < prevDPRAMend[card] ) /* we have wrapped around the DPRAM buffer */
        {
          size = DPRAMsize[card] - prevDPRAMend[card];
          ret  = write_buffer(card, val, 4*size);
          if(ret)
          {
            if(writingFile[card])
            {
printf("Overwriting about to occur\n");
              strcpy(errorString[card], "Write file error");
              drvPmacGatStop(card);
              break;
            }
            else
              semGive(pCard->scanGatWriteSem);
          }

#if DEBUG
          printf("WRAPPED(%d), read (top) = %d, read (bottom) = %d, prevDPRAMend = %d\n",
                  DPRAMsize[card], size, end, prevDPRAMend[card]);
#endif

          val = (char *)pmacRamAddr(card, pmac_gat_offset[card]);
          ret = write_buffer(card, val, 4*end);
          if(ret)
          {
            if(writingFile[card])
            {
printf("Overwriting about to occur\n");
              strcpy(errorString[card], "Write file error");
              drvPmacGatStop(card);
              break;
            }
            else
              semGive(pCard->scanGatWriteSem);
          }
        }
        else                    /* we haven't wrapped around the DPRAM buffer */
        {
          size = end - prevDPRAMend[card];

#if DEBUG
          printf("NOT WRAPPED(%d), Amount read = %d, prevDPRAMend = %d\n", 
                  DPRAMsize[card], size, prevDPRAMend[card]);
#endif

          ret  = write_buffer(card, val, 4*size);
          if(ret)
          {
            if(writingFile[card])
            {
printf("Overwriting about to occur\n");
              strcpy(errorString[card], "Write file error");
              drvPmacGatStop(card);
              break;
            }
            else
              semGive(pCard->scanGatWriteSem);
          }
        }
        prevDPRAMend[card] = end;
        readingDPRAM[card] = 0;
        taskUnlock();
      }
    }
  }
}


int drvPmacGatWriteTask( int card )
{
  PMAC_CARD *pCard = &drvPmacCard[card];
  long      wflag;
  double    rate;

  FOREVER
  {
    if( semTake(pCard->scanGatWriteSem, WAIT_FOREVER) != OK )
      errMessage(0,"drvPmacWriteTask: semTake returned error");
    else
    {
#if 1
      printf("drvPmacGatWriteTask - writing to disk\n");
#endif
      writingFile[card] = 1;
      if( writeFlag[card] == WRITE_BOTTOM )
      {
        ticksBefore[card] = tickGet();
        fwrite( gatherBuffer[card], 1, pmac_buf_size[card]/2, fd[card] );
        ticksAfter[card]  = tickGet();
        writeFlag[card] = WRITE_TOP;
      }
      else
      {
        ticksBefore[card] = tickGet();
        fwrite( gatherBuffer[card]+pmac_buf_size[card]/2, 1, 
                pmac_buf_size[card]/2, fd[card] );
        ticksAfter[card]  = tickGet();
        writeFlag[card] = WRITE_BOTTOM;
      }
      writingFile[card]   = 0;
      amountWritten[card] = pmac_buf_size[card]/2;
#if 1
      drvPmacGatWriteMonitor( card, &wflag, &rate );
      if( wflag == 0 )
        printf("drvPmacGatWriteTask - finished writing to disk: Rate = %f\n", rate);
      else
        printf("drvPmacGatWriteTask - finished writing to disk\n");
#endif
    }
  }
}


int drvPmacGatInit( int card, char *axis, char *filename, char *header )
{
  char fname[32];

  /* First read of the DPRAM, so prev read was at 0 */
  /* Set the error string to blank */
  /* Set up some state variables */

  prevDPRAMend[card]   = 0;
  readingDPRAM[card]   = 0;
  writingFile[card]    = 0;
  ticksBefore[card]    = 0;
  ticksAfter[card]     = 0;
  transferRate[card]   = 0.0;
  writeFlag[card]      = WRITE_BOTTOM;
  errorString[card][0] = '\0';

  /* Open output file for this cards raw data */

  sprintf(fname, "%s%s.raw", filename, axis);
  fd[card] = fopen( fname, "w+");

  /* Initialise local memory buffer for this card */
  init_buffer( card );

  /* Find out which addresses should be logged */
  drvPmacGatSources( card );

  /* Write the header lines */
  fwrite( header, 1, strlen(header), fd[card] );

  return(0);
}


int drvPmacGatStart( int card, long size )
{
  PMAC_CARD *pCard;
  int       status = 0;

  pCard = &drvPmacCard[card];

  if(!gatherBuffer[card])
  {
    pmac_buf_size[card] = size;
    gatherBuffer[card]  = malloc(pmac_buf_size[card]);
    if(!gatherBuffer[card])
    {
      sprintf(errorString[card], 
        "Failed to allocate internal logging buffer for card %d", card);
      drvPmacGatStop(card);
      status = 1;
    }
  }

  if(!status)
  {
    /* Set the flag which starts continuous reading of DPRAM */
    pCard->logFlag = 1;
    semGive(pCard->scanGatReadSem);
  }
  return(0);
}


int drvPmacGatStop( int card )
{
  PMAC_CARD *pCard = &drvPmacCard[card];
 
#if DEBUG
  printf("Calling drvPmacGatStop: logging = %d, writingFile = %d\n", 
         pCard->logFlag, writingFile[card]);
#endif

  if( pCard->logFlag )
  {
    pCard->logFlag = 0;
    if( writingFile[card] )
    {                       /* wait for existing data to be written to disk */
      while( writingFile[card] )
        taskDelay(2.0);
      drvPmacGatFlush( card );
    }
    else
      drvPmacGatFlush( card );

    fclose(fd[card]);
    fd[card] = NULL;
  }

#if DEBUG
  printf("End of drvPmacGatStop\n");
#endif

  return(0);
}


int drvPmacGatFlush( int card )
{
  if( writeFlag[card] == WRITE_BOTTOM )
  {
#if DEBUG
    printf("drvPmacGatFlush(BOTTOM): buf_pos = %d\n", buf_pos[card]);
#endif
    ticksBefore[card] = tickGet();
    fwrite( gatherBuffer[card], 1, buf_pos[card], fd[card] );
    ticksAfter[card] = tickGet();
    amountWritten[card] = buf_pos[card];
  }
  else
  {
#if DEBUG
    printf("drvPmacGatFlush(TOP): buf_pos = %d\n", buf_pos[card]);
#endif
    if( buf_pos[card] < pmac_buf_size[card]/2 )
    {
      /* Even though we think we are filling up the TOP of the buffer so
         that buf_pos is somewhere in the top-half, what has happened is
         that is has already wrapped around to the bottom again. In this
         case, just flush the TOP */
      ticksBefore[card] = tickGet();
      fwrite( gatherBuffer[card]+pmac_buf_size[card]/2, 1, 
              pmac_buf_size[card]/2, fd[card] );
      ticksAfter[card] = tickGet();
      amountWritten[card] = pmac_buf_size[card]/2;
    }
    else
    {
      ticksBefore[card] = tickGet();
      fwrite( gatherBuffer[card]+pmac_buf_size[card]/2, 1, 
              (buf_pos[card] - pmac_buf_size[card]/2), fd[card] );
      ticksAfter[card] = tickGet();
      amountWritten[card] = (buf_pos[card] - pmac_buf_size[card]/2);
    }
  }
  return(0);
}


int drvPmacGatError( int card, char *error )
{
  strcpy(error, errorString[card]);
  return(0);
}


int drvPmacGatWriteMonitor( long card, long *wFlag, double *rate )
{
  double totalSecs;

  if( writingFile[card] )
    *wFlag = 1;
  else
  {
    *wFlag = 0;
    if( ticksBefore[card] )
    {
      totalSecs          = (ticksAfter[card]-ticksBefore[card])/(double)sysClkRateGet();
      transferRate[card] = (double)amountWritten[card]/(1024.0 * totalSecs);
    }
  }
  *rate = transferRate[card];

  return(0);
}


int drvPmacGatResetError( int card )
{
  errorString[card][0] = '\0';
  return(0);
}


void init_buffer( int card )
{
  buf_pos[card] = 0;
}


long write_buffer( int card, char *buf, int size)
{
  char *from;
  char *to;
  int  amount;
  long ret;
  long prevPos;

#if DEBUG_WRITE
  printf("Entering write_buffer with buf_pos = %d, size = %d\n", buf_pos[card], size);
#endif

  prevPos = buf_pos[card];

  if( size > pmac_buf_size[card]-buf_pos[card] )
  {
    /* we want to write more than there is space for, so wrap around */

    to     = gatherBuffer[card] + buf_pos[card];
    from   = buf;
    amount = pmac_buf_size[card] - buf_pos[card];
    memcpy( to, from, amount );

    to     = gatherBuffer[card];
    from   = buf  + pmac_buf_size[card] - buf_pos[card];
    amount = size - pmac_buf_size[card] + buf_pos[card];
    memcpy( to, from, amount );

    buf_pos[card] = amount;
  }
  else
  {
    to     = gatherBuffer[card] + buf_pos[card];
    from   = buf;
    amount = size;
    memcpy( to, from, amount );

    buf_pos[card] = (buf_pos[card] + amount)%pmac_buf_size[card];
  }

  if( (prevPos < pmac_buf_size[card]/2) && (buf_pos[card] >= pmac_buf_size[card]/2) )
    ret = 1;
  else if( buf_pos[card] < prevPos )
    ret = 1;
  else
    ret = 0;

#if DEBUG_WRITE
  printf("Leaving write_buffer with buf_pos = %d, size = %d\n", buf_pos[card], size);
#endif

  return(ret);
}
