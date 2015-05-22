
#ifndef __OSTYPES_H
#define __OSTYPES_H

#include <stdio.h>
#include <stddef.h>
#ifdef FREEBSD
#include <sys/fcntl.h>
#else // #ifdef LINUX
#include <fcntl.h>
#include <linux/ioctl.h>
#endif
#include <sys/stat.h>
#include <sys/ioctl.h>

typedef unsigned int    bit32;
typedef unsigned short  bit16;
typedef unsigned char   bit8;
typedef unsigned long   bit32_64;

typedef bit32           HBA_HANDLE;
typedef bit32           agStatus_t;
typedef bit32           agBoolean;
#define agBOOLEAN       bit32

#define agTRUE          1
#define agFALSE         0
#define AG_MAX_CARDS	4

#define MAX_COMMAND_STACK                   4
#define DSC_MAXIMUM_ELEMENT_PER_SEGMENT     32
#define DSC_MAXIMUM_ELEMENTS                1024
#define EVT_MAXIMUM_ELEMENT_PER_SEGMENT     32
#define EVT_MAXIMUM_ELEMENTS                1024
#define SCSI_MAXIMUM_TARGETS_PER_BUS        256
#define SCSI_MAXIMUM_BUSES                  16

#ifdef FREEBSD
// Payload Wraper for ioctl commands
typedef struct datatosendt {
  bit32 datasize;
  bit8 *data;
} datatosend;
#endif

#define PMC_SIERRA_IOCTL_SIGNATURE          "PMC-STRG"
#define PMC_IOCTL_SPC                       0x1234
#define AG_INVALID_HANDLE                   -1
#define AGTIAPI_IOCTL_BASE                  'x'

#ifdef FREEBSD
        // sending payload datatosend
#define AGTIAPI_IOCTL    _IOWR( AGTIAPI_IOCTL_BASE, 0, datatosend )
#else   // #ifdef LINUX
#define AGTIAPI_IOCTL    _IOWR( AGTIAPI_IOCTL_BASE, 0, int )
#endif

#define RMDISK_DRIVER_NAME                  "AGRMDISK"

#ifdef _DEBUG
	#define dbgPrintf printf
#else
	#define dbgPrintf
#endif

typedef enum _IOCTL_TARGET_TYPE
{
  IOCTL_TARGET_MINIPORT,
  IOCTL_TARGET_KERNEL_RMDISK
} IOCTL_TARGET_TYPE;

#define   IOCTL_CODE_UNDEFINED  0
#define   IOCTL_BUF_SIZE        4096

// Global structure used to map AdapterName to "SasPortn"
typedef struct agNameMap 
{
  char        adapterName[32];  // AdapterName as application sees it
  char        pathName[32];     // pathName as we see it "/dev/spc"
  int         adapterIndex;     // 'n' as in "\\??\\SasPortn"
  int         handle;           // HBA Handle
} agNameMap_t;

#define   agHandle_t  agNameMap_t*

#define AGRMDISK_IOCTL_TYPE 40000
//
// The IOCTL function codes from 0x800 to 0xFFF are for customer use.
//
#define IOCTL_AGRMDISK_MGMT_COMMAND \
    CTL_CODE( AGRMDISK_IOCTL_TYPE, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS  )

/*
 * The following enum type define the values that will be set in in the
 * comParmStatus_t value field for enumnerated parameters. Some types will
 * be used for multiple parameters as noted. Numeric parameters are stored
 * as directly.
 */
typedef enum
{
  AUTH_NONE  = 0x01,
  AUTH_KRB5  = 0x02,
  AUTH_SPKM1 = 0x04,
  AUTH_SPKM2 = 0x08,
  AUTH_SRP   = 0x10,
  AUTH_CHAP  = 0x20
} comLoginAuth_t; /* AUTH_METHOD */

#define MAX_KEY_VALUE_SIZE         255

#define PCI_NUMBER_BARS              6

typedef struct _CardInfo {
  bit32    pciIOAddrLow;                   // PCI IOBASE lower
  bit32    pciIOAddrUp;                    // PCI IOBASE Upper
  bit32_64 pciMemBase;                     // PCI MEMBASE, physical
  bit32_64 pciMemBaseSpc[PCI_NUMBER_BARS]; // PCI MEMBASE, physical

  bit16     deviceId;                      // PCI device id
  bit16     vendorId;                      // PCI Vendor id
  bit32     busNum;
  bit32     deviceNum;
} CardInfo_t;

void PrintSPCCardInfo( char *, bit32);

#endif
