/*****************************************************************************
 Copyright (c) PMC-Sierra, Inc. 2004-2016. All rights reserved.
*--------------------------------------------------------------------------
* This software embodies materials and concepts which are proprietary and
* confidential to PMC-Sierra, Inc.
* PMC-Sierra distributes this software to its customers pursuant to the
* terms and conditions of the Software License Agreement
* contained in the text file software.lic that is distributed along with
* the software. This software can only be utilized if all
* terms and conditions of the Software License Agreement are
* accepted. If there are any questions, concerns, or if the
* Software License Agreement text file, software.lic, is missing please
* contact PMC-Sierra for assistance.
*--------------------------------------------------------------------------

******************************************************************************

  Program Name:    psxapp
  Abstract:       Program entry point
  Notes:


*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include "unixostypes.h"
#include "psxapp.h"

/* Globals */
FILE    *fptr = NULL;
sem_t mutex;

Command_t CommandTable[] =
{
  {"test01",         PSXAPP_OPCODE_TEST01},
  {"test02",         PSXAPP_OPCODE_TEST02},
  {"wake_up",        WAKE_UP},
  {"sleeptime",      SLEEPTEST},
  {"help",           PSXAPP_OPCODE_HELP},
  {"info",           PSXAPP_OPCODE_GET_INFO},
  {"userdefn",       PSXAPP_OPCODE_USERDEFN}
};

#ifdef DEBUG 
#define DPRINTF(fmt, ...) \
    do { printf(fmt, ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) \
    do { } while (0)
#endif

/*****************************************************************************

  Purpose:
  main program entry point

  Parameters:
  argc argv   - STD ARG input

  Return:
  none

*****************************************************************************/
int main(int argc, char* argv[])
{
  bit32       ret=0;
  void       *buffer = NULL;
  bit32       count=0;
  char *       token;
  int       Opcode = PSXAPP_OPCODE_UNKNOWN;
  int       index;
  int       i;
  char        commandLine[80];

  DPRINTF("DEBUGGING ENABLED\n");
  DPRINTF("%s: DEBUGGING ENABLED\n", __FUNCTION__);

  for(i=0;i < argc; i++)
  {
    printf("main %lu %s\n",i,argv[i]);
  }

  if (argc > 1)
  {
//    printf("\n");
//    printf("%s\n", gHeader);
//    printf("%s\n", gCpright);
//    printf("%s\n", gHeader);
//    printf("\n");
//    printf("SPCAPP for PMC SAS Host Bus Controller. Version %s\n\n", VERSION);

    /* Get the first parameter which specify which function to execute*/
    token = argv[1];

    if (token != NULL)
    {
      convertStringLower(token);
      for (index = 0;
           index < (sizeof (CommandTable)/sizeof(CommandTable[0]));
           index++)
      {
        if (!strcmp(CommandTable[index].commandString, token))
        {
          Opcode = CommandTable[index].commandOpcode;
          break;
        }
      }
    }

    switch (Opcode)
    {
    case PSXAPP_OPCODE_TEST01:
      test01(argc - 2, 0x5757);
      break;
    case PSXAPP_OPCODE_TEST02:
      test02(argc);
      break;
    case WAKE_UP:
      wake_up(argc);
      break;
    case SLEEPTEST:
      sleeptest(argc);
      break;
    case PSXAPP_OPCODE_HELP:
      help();
      break;
    case PSXAPP_OPCODE_GET_INFO:
      getInfo();
      break;
    case PSXAPP_OPCODE_USERDEFN:
      userDefn(argc, argv);
      break;
    default:
      printf("\nUnknown command: %d\n", Opcode);
      PrintUsage();
      break;
    }
  }
  else
  {
    ret=1;
    PrintUsage();
  }

  DPRINTF("%s: Exit\n", __FUNCTION__);

  return ret;
}

typedef struct psxIoctl_s
{
    bit32 signature;
    bit16 majorFunction;
    bit16 minorFunction;
    bit32 length;
    bit32 status;
    bit8  functionSpecificArea[1];
}psxIoctl_t;

int test01(int numArg, bit32 cmd)
{
  printf("test01: enter\n");  

  bit32 status = 0;
  bit32 x = 0;
  bit32 myPid = 0;
  bit32 *ptr= NULL;
  ioctl64_t ioctl64;

  if (numArg > 2) // Get next token which should specify card
  {
    printf( "Invalid number of parameters!\n" );
    PrintUsage();
    return 1;
  }

  memset(&ioctl64, 0, 64);

  ioctl64.signature = PMC_IOCTL_SIGNATURE;
  ioctl64.majorFunction = 0x5757;
  ioctl64.minorFunction = 0x000A;
  ioctl64.length = 64;
  ioctl64.status = 0;
  ioctl64.functionSpecificArea[0] = getpid();

  myPid = getpid();
  printf("pid %d 0x%08x\n", myPid, myPid);

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  status = agSendIOCTL( 0x01,
                        &ioctl64,
                        ioctl64.length);

  printf("test01: return from agSendIOCTL\n");
  if ( status )
  {
    printf( "test01: status = %x\n",
            status);
  }

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  printf("test01: exit\n");  

  return status;
}

int test02(int numArg)
{
  printf("test02: enter\n");
  printf("WAKE_UP: enter\n");
  bit32 status = 0;
  bit32 x = 0;
  bit32 myPid = 0;
  bit32 *ptr= NULL;
  ioctl64_t ioctl64;

  memset(&ioctl64, 0, 64);

  ioctl64.signature = PMC_IOCTL_SIGNATURE;
  ioctl64.majorFunction = 0x5759;
  ioctl64.minorFunction = 0x000C;
  ioctl64.length = 64;
  ioctl64.status = 0;
  ioctl64.functionSpecificArea[0] = getpid();
  myPid = getpid();
  printf("pid %d 0x%08x\n", myPid, myPid);

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  status = agSendIOCTL( 0x01,
                        &ioctl64,
                        ioctl64.length);

  printf("test02: return from agSendIOCTL\n");
  if ( status )
  {
    printf( "test02: status = %x\n",
            status);
  }

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  printf("test02: exit %d\n", status);
  return status;


}

int wake_up(int numArg)
{
  printf("WAKE_UP: enter\n");
  bit32 status = 0;
  bit32 x = 0;
  bit32 myPid = 0;
  bit32 *ptr= NULL;
  ioctl64_t ioctl64;

  memset(&ioctl64, 0, 64);

  ioctl64.signature = PMC_IOCTL_SIGNATURE;
  ioctl64.majorFunction = 0x5758;
  ioctl64.minorFunction = 0x000A;
  ioctl64.length = 64;
  ioctl64.status = 0;
  ioctl64.functionSpecificArea[0] = getpid();
  myPid = getpid();
  printf("pid %d 0x%08x\n", myPid, myPid);

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  status = agSendIOCTL( 0x01,
                        &ioctl64,
                        ioctl64.length);

  printf("test01: return from agSendIOCTL\n");
  if ( status )
  {
    printf( "test01: status = %x\n",
            status);
  }

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "%2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }


  printf("WAKE_UP: exit %d\n", status);
  return status;
}

int sleeptest(int numArg)
{
  printf("sleeptest: enter\n");

  bit32 status = 0;
  bit32 x = 0;
  bit32 myPid = 0;
  bit32 *ptr= NULL;
  ioctl64_t ioctl64;

  // timer
  struct timeval my_val = {3, 0};
  struct timeval my_interval = {3, 0};
  struct itimerval my_timer = {my_interval, my_val};


  signal(SIGIO, signal_handler);
  sem_init(&mutex, 0, 0);

  signal(SIGALRM, timer_handler);

  memset(&ioctl64, 0, 64);

  ioctl64.signature = PMC_IOCTL_SIGNATURE;
  ioctl64.majorFunction = 0x5760;
  ioctl64.minorFunction = 0x000D;
  ioctl64.length = 64;
  ioctl64.status = 0;
  ioctl64.functionSpecificArea[0] = getpid();
  myPid = getpid();
  printf("pid %d 0x%08x\n", myPid, myPid);

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf("sleeptest: %2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  status = agSendIOCTL( 0x01,
                        &ioctl64,
                        ioctl64.length);

  printf("sleeptest: return from agSendIOCTL\n");
  if ( status )
  {
    printf( "sleeptest: status = %x\n", status);
  }

  for(ptr = (bit32 *)&ioctl64, x = 0; x < ioctl64.length; x++)
  {
    printf( "sleeptest: %2d - %p - 0x%08x\n", x, &ptr[x], ptr[x]);
  }

  setitimer(ITIMER_REAL, &my_timer, 0);
  sem_wait(&mutex);

  printf("sleeptest: time to sleep z Z z Z z Z\n");
  printf("sleeptest: exit %d\n", status);

  sem_destroy(&mutex);

  return status;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility to
  send an IOCTL call to the driver
Parameters:
  cardHandle   -  Handle to the HBA( card handle is the handle obtained
          by a previous call to osOpenHBA)
  IoctlBuffer  -  Pointer to the Buffer which contains
          IOCTL Information
  Size         -  Size of the buffer
Return:
  IOCTL_CALL_SUCCESS - successful in making an IOCTL call
            and received some response
  IOCTL_CALL_FAIL    - failed
*****************************************************************************/
int agSendIOCTL(bit32 ioctlCode, void *pIoctl, bit32 size)
{
  int ret = 0;
  int fd = -1;
  ioctl64_t *pIoctl64 = pIoctl;
//  char dev[64] = "/dev/psx_ntb";
  char dev[64] = "/dev/ioctl_dev";

  printf("agSendIOCTL: ioctlcode %d\n", ioctlCode);
  printf("agSendIOCTL: pIoctl %p\n", pIoctl);
  printf("agSendIOCTL: size %d\n", size);

  fd = open(dev, O_RDWR, 0);
  if(fd < 0)
  {
    printf("agSendIOCTL: open ERROR %d\n", fd);
    printf("agSendIOCTL: open errno %d - %s\n", errno, strerror(errno));
    return -1;
  }
  else
  {
    printf("agSendIOCTL: sending ioctl\n");
  }

  //ret = ioctl( fd, pIoctl64->majorFunction, pIoctl);
  ret = ioctl( fd, AGTIAPI_IOCTL, pIoctl);
  if ( ret <  0 )
  {
    printf("agSendIOCTL: errno %s\n", strerror(errno));
    return -1;
  }
  else
  {
    printf("agSendIOCTL: success %d\n", ret);
  }
  printf("agSendIOCTL: closing fd\n"); 
  close(fd);

  printf("agSendIOCTL: exit\n"); 
  return 0;
}

int convertStringLower(char *buffer)
{
  char *s;
  //convert to lower case
  for (s=buffer; *s; s++)
    *s=(char)tolower(*s);
  return 0;
}

void signal_handler(int signum)
{
	printf("%s: enter\n", __FUNCTION__);
	if(signum == SIGIO)
		printf("%s: SIGIO\n", __FUNCTION__);

	sem_post(&mutex);

	printf("%s: exit\n", __FUNCTION__);
}

void timer_handler(int signum)
{
	printf("%s: enter %x \n", __FUNCTION__, signum);
	sem_post(&mutex);
	printf("%s: exit\n", __FUNCTION__);
}

void PrintUsage(void)
{
  printf( "\n****************************************************************"
          "*************\n" );
  printf("SPCAPP Help Menu:\n");
  printf("usage:\tspcapp [help] \n");
  printf("\tspcapp [getInfo]\n");
  printf("\tspcapp [twiAccess]\n");
  printf("\tspcapp [vgpioAccess]\n");
  printf("\tspcapp [fanAccess]\n");
  printf("\tspcapp [dieTemp]\n");
  printf("\tspcapp [fwflash <file name>]\n");
  printf("\tspcapp [getFWLog <logX|logY|logZ>]\n");
  printf("\tspcapp [pmon]\n");
  printf("\tspcapp [portLane]\n");
  printf("\tspcapp [portArbitration]\n");
  printf("\tspcapp [mcOverlay]\n");
  printf("\tspcapp [stackbifurcation]\n");
  printf("\tspcapp [portPtP2PBind]\n");
  printf("\tspcapp [diagTLPInject]\n");
  printf("\tspcapp [diagTLPGenChk]\n");
  printf("\tspcapp [diagPortEyeCapt]\n");
  printf("\tspcapp [diagPortVHist]\n");
  printf("\tspcapp [diagPortTSSMLog]\n");
  printf("\tspcapp [diagPortTLPAnlzr]\n");
  printf("\tspcapp [portLnAdptObj]\n");
  printf("\tspcapp [readRxDiag]\n");
  printf("\tspcapp [writeTxDiag]\n");
  printf("\tspcapp [readVpd]\n");
  printf("\tspcapp [userDefn]\n");
  return;
}

int userDefn(int numArg, char **pArg)
{
  bit32 status = 0;
  ioctl64_t ioctl64;  

  // Timer
  struct timeval my_val =      {3, 0};
  struct timeval my_interval = {3, 0};
  struct itimerval my_timer =  {my_interval, my_val};

  sem_init(&mutex, 0, 0);
  signal(SIGALRM, timer_handler);

  // Init ioctl
  memset(&ioctl64, 0, sizeof(ioctl64_t));
  ioctl64.signature = PMC_IOCTL_SIGNATURE;
  ioctl64.majorFunction = 0x0000;
  ioctl64.minorFunction = 0x0000;
  ioctl64.length = 64;
  ioctl64.status = 0;
  ioctl64.functionSpecificArea[0] = 0;

  // Send ioctl
  status = agSendIOCTL(0x01, &ioctl64, ioctl64.length);

  // Sleep
  sem_wait(&mutex);
  sem_destroy(&mutex);

  return status;
}

int help(void)
{

  DPRINTF("%s: Enter\n", __FUNCTION__);

  DPRINTF("%s: Exit\n", __FUNCTION__);
  return 0;
}

int getInfo(void)
{

  DPRINTF("%s: Enter\n", __FUNCTION__);

  DPRINTF("%s: Exit\n", __FUNCTION__);
  return 0;
}

