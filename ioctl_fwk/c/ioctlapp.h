



const char *gHeader  = "=============================================";
const char *gCpright = "hello allen";
#define HEADER "============================================="
#define COPYRIGHT "allen Copyright (c)  All rights reserved."

#define IOCTL_SIGNATURE 0x1234
#define VERSION "0.1"
#define FALSE               0
#define TRUE                1
#define AGTIAPI_IOCTL    _IOWR( AGTIAPI_IOCTL_BASE, 0, int )
#define AGTIAPI_IOCTL_BASE                  'x'

/* Application opcodes that user will input. The enumeration will start
   at 8 becuase the ntb driver uses 1 - 7
 */
typedef enum OPCODE_
{
  PSXAPP_OPCODE_UNKNOWN, 
  PSXAPP_OPCODE_TEST01,
  PSXAPP_OPCODE_TEST02,
  WAKE_UP,
  SLEEPTEST,
  PSXAPP_OPCODE_HELP = 8,
  PSXAPP_OPCODE_GET_INFO,
  PSXAPP_OPCODE_USERDEFN
} OPCODE;

typedef struct Command_s
{
  char commandString[32];
  OPCODE commandOpcode;

}Command_t;

typedef struct ioctl64_s
{
    bit32 signature;
    bit16 majorFunction;
    bit16 minorFunction;
    bit32 length;
    bit32 status;
    bit32 functionSpecificArea[12];
}ioctl64_t;

// Function declarations
int convertStringLower(char *buffer);
void signal_handler(int signum);
void timer_handler(int signum);
void PrintUsage(void);
int userDefn(int numArg, char **pArg);

// Test commands
void PrintUsage(void);
int test01(int numArg, bit32 cmd);
int test02(int numArg);

int agSendIOCTL(bit32 ioctlCode, void *pIoctl, bit32 Size);
int help(void);
int getInfo(void);
int userDefn(int numArg, char **pArg);

