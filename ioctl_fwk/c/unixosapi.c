#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<stdbool.h>
#include "ihbaapi.h"
#include "osapi.h"
#include "psxapp.h"

#define allenf printf

#ifdef FREEBSD
// Payload Wraper for ioctl commands defined in ostypes.h
datatosend  tosend;
datatosend *loadtosend = &tosend;
#endif

unsigned int device_id[AG_MAX_CARDS];
agNameMap_t cardMapping[AG_MAX_ADAPTERS];

/***************************************************************************** 
Purpose:        
  Get the agNameMap_t given a file handle
Parameters:    
  cardHandle    - file handle
Return:  
  NULL      - failed
  !NULL     - agNameMap_t
*****************************************************************************/
agNameMap_t * agGetCardMapping(agHandle_t cardHandle) {
  return(agNameMap_t  *)cardHandle;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility
  to initialize and generate Name Mapping
Parameters:
  none
Return:
  HBA_STATUS_OK - Successfully initialized and and generated Name Map
*****************************************************************************/
agStatus_t agLoadLibrary()
{
  agStatus_t status;
  bit32      index;

  // Initialize the Global Name array
  for (index =0; index < AG_MAX_ADAPTERS; index++)
  {
    memset(&(cardMapping[index]),0,sizeof(agNameMap_t));
    cardMapping[index].adapterIndex = -1;
    cardMapping[index].handle = -1;
  }

  // We will query all the adapters on the system and pick only ours
  status = agGenerateNameMap();
  return HBA_STATUS_OK;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility to
  generate Name Mapping
Parameters:
  none
Return:
  HBA_STATUS_OK - Successfully generated Name Map
*****************************************************************************/
agStatus_t  agGenerateNameMap()
{
  agHandle_t handle;
  char       devAddress[32];
  agStatus_t status;
  bit32      index;
  bit32      hIndex = 0;
  bit32      endNameList = 0;
  int        filehandle;

  status = HBA_STATUS_OK;

  // Generate the NameMap
  for ( index =0; ( index < AG_MAX_ADAPTERS )  && ( !endNameList ); index++ )
  {

#ifdef FREEBSD
    sprintf( devAddress, "/dev/spcv%d", index ); // char dev gen'd by make_dev 
#else // #ifdef LINUX
    sprintf( devAddress, "/dev/spc%d", index );
#endif

    filehandle = open( devAddress, O_RDWR, 0 );
    if ( filehandle < 0 )
    {
      endNameList = 1;
      break;
    }
    else
    {
      handle = &cardMapping[hIndex];
      handle->handle = filehandle;
      cardMapping[hIndex].adapterIndex = index;
      sprintf( cardMapping[hIndex].adapterName, "PMC-%02x", hIndex );
      strcpy( cardMapping[hIndex].pathName, devAddress );
      hIndex++;
      close( filehandle );
    }
  }
  return status;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility to
  free the Name Mapping
Parameters:
  none
Return:
  HBA_STATUS_OK - Successfully freed Name Mapping
*****************************************************************************/
agStatus_t agFreeLibrary()
{
  memset(cardMapping, 0, (sizeof(agNameMap_t) * AG_MAX_ADAPTERS) );
  return HBA_STATUS_OK;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility to
  get the number of Adapters
Parameters:
  none
Return:
  count - number of Adapters Found
*****************************************************************************/
bit32  agGetNumberOfAdapters()
{
  bit32 count;
  printf("agGetNumberOfAdapters: start\n");

  for ( count = 0; count < AG_MAX_ADAPTERS; count++ )
  {
    printf("agGetNumberOfAdapters: count = %d\n", count);
    if ( cardMapping[count].adapterIndex == -1 )
      break;
  }

  printf("agGetNumberOfAdapters: end\n");
  return count;
}

/*****************************************************************************
Purpose:
  This function is called by the Management utility to
  to open the device handle for a HBA port related the the card number
Parameters:
  cardNum   -  cardNum is the HBA card number
Return:
  agHandle_t - device handle to the HBA port
*****************************************************************************/
agHandle_t  agOpenHBA(bit32 cardNum)
{
  bit32   ret = 0;
  bit32   i;
  bit32   handle;
  handle = open ( cardMapping[cardNum].pathName, O_RDWR, 0 );
  if ( handle < 0 )
  {
    printf( "Can't open %s\n",cardMapping[cardNum].pathName );
  }
  cardMapping[cardNum].handle = handle;
  return &cardMapping[cardNum];
}

/**********************************************************************
This function is called by the Management utility to
 close the SCSI port handle.
Input Parameters:
 cardHandle   - Handle to the HBA( card handle is the handle obtained
                by a previous call to osOpenHBA)
 Returns:
 HBA_STATUS_OK - successful in closing the card handle
***********************************************************************/
agStatus_t  agCloseHBA( agHandle_t cardHandle )
{
  close( cardHandle->handle );
  cardHandle->handle = AG_INVALID_HANDLE;
  return HBA_STATUS_OK;
}

/*****************************************************************************
Purpose:
Parameters:
Return:
*****************************************************************************/
bit32 agGetMemoryRequired( agHandle_t cardHandle ) {
  return 0;
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
agStatus_t agSendIOCTL( agHandle_t    cardHandle,
                        bit32         ioctlCode,
                        void         *pIoctl,
                        bit32         Size )
{
  int ret = 0;
#ifdef FREEBSD
  loadtosend->data     = pIoctl;
  loadtosend->datasize = Size;
  ret = ioctl( cardHandle->handle, AGTIAPI_IOCTL, loadtosend );
  if ( ret != 0 )
#else // #ifdef LINUX
  ret = ioctl( cardHandle->handle, AGTIAPI_IOCTL, pIoctl );
  if ( ret <  0 )
#endif
  {
    allenf("agSendIOCTL: errno %s\n", strerror(errno));
    close( cardHandle->handle );
    return IOCTL_CALL_FAIL;
  }
  else
  {
    allenf("agSendIOCTL: IOCTL_CALL_SUCCESS %d\n", IOCTL_CALL_SUCCESS);
    return IOCTL_CALL_SUCCESS;
  }
}

/*****************************************************************************
Purpose:
Parameters:
Return:
*****************************************************************************/
long getFileLength( const char *filename )
{
  long length = -1;
  struct stat MyStat;
  if ( 0 == stat(filename, &MyStat) )
  {
    length = MyStat.st_size;
  }
  return length;
}


/*****************************************************************************
Purpose:
Parameters:
Return:
*****************************************************************************/
int GetSPCCardInfo( bit32 numberofadapters )
{
  tiIOCTLPayload_t *payload;
  bit32             length;
  bit8             *ioctlbuffer = NULL;
  bit32             adapterIndex;
  agHandle_t        adapterhandle;
  agStatus_t        status = 0;    

  for (adapterIndex = 0; adapterIndex <numberofadapters; adapterIndex++)
  {
    adapterhandle = agOpenHBA( adapterIndex );
    if ( adapterhandle < 0 )
    {
      printf( "Unable to open the adapter %d\n", adapterIndex );
      return 1;
    }
    printf( "SPCv Card %d:\n", adapterIndex );
    length =
      sizeof(CardInfo_t) + sizeof(tiIOCTLPayload_t)-1 + agGetMemoryRequired(0);
    ioctlbuffer = calloc( length, 1 );

    if ( !ioctlbuffer )
    {
      printf( "** Error: Unable to allocate %d bytes ioctl buffer\n", length );
      agCloseHBA( adapterhandle );
      return 1;
    }
    memset( ioctlbuffer, 0, length);
    payload =
      (tiIOCTLPayload_t *)( (bit8 *)ioctlbuffer + agGetMemoryRequired(0) );
    payload->Signature     = ( bit32 )PMC_IOCTL_SIGNATURE;
    payload->MajorFunction = IOCTL_MJ_GET_FW_REV;
    payload->Length        = sizeof(CardInfo_t) + sizeof(tiIOCTLPayload_t) - 1;
    payload->Status        = 0;

    status = agSendIOCTL( adapterhandle,
                          IOCTL_CODE_UNDEFINED,
                          ioctlbuffer,
                          length );
    if (status)
    {
      printf ( "\n** Error: Error in IOCTL_MJ_GET_FW_REV status = %x retcode "
               "%x\n",
               status, 
               payload->Status );
      free ( ioctlbuffer );
      agCloseHBA( adapterhandle );
      return 1;
    }
    if ( payload->Status == IOCTL_CALL_SUCCESS )
    {
      printf ( "--------------------\n" );
      printf ( "Current FW Revision Number  : %4x\n",
               *(bit32*)&payload->FunctionSpecificArea );  

      payload->MajorFunction = IOCTL_MN_GET_CARD_INFO;  
      status = agSendIOCTL( adapterhandle,
                            IOCTL_CODE_UNDEFINED,
                            ioctlbuffer,
                            length );
      if ( status )
      {
        printf ( "\n** Error: Error in IOCTL_MN_GET_CARD_INFO status = %x "
                 "retcode %x\n",
                 status, 
                 payload->Status );
        free ( ioctlbuffer );
        agCloseHBA( adapterhandle );
        return 1;
      }
      else
        PrintSPCCardInfo( payload->FunctionSpecificArea, adapterIndex );
    }
    else
    {
      printf( "Error reading card information!!!\n" );
      free( ioctlbuffer );
      agCloseHBA( adapterhandle );
      return 1;
    }
    agCloseHBA( adapterhandle );
  }
  return 0;
}

/*****************************************************************************
Purpose:
Parameters:
Return:
*****************************************************************************/
void PrintSPCCardInfo( char *buffer, bit32 adapterIndex )
{
  CardInfo_t  *CardInfo;
  int          index;

  CardInfo  = (CardInfo_t *)buffer;
  device_id[adapterIndex] = CardInfo->deviceId;
  if ( !fw_update )
  {
    printf ( "BusNumber                   : %2x\n", CardInfo->busNum      );
    printf ( "SlotNumber                  : %02x\n", CardInfo->deviceNum   );
    printf ( "Device ID                   : %4x\n", CardInfo->deviceId    );
    printf ( "Vendor ID                   : %4x\n", CardInfo->vendorId    );
    printf ( "pciIOAddrLow                : %08x\n", CardInfo->pciIOAddrLow);
    printf ( "pciIOAddrUp                 : %08x\n", CardInfo->pciIOAddrUp );
    printf ( "pciMemBase                  : %8x\n", (unsigned int) CardInfo->pciMemBase  );
    for ( index = 0; index < PCI_NUMBER_BARS; index++ )
    {
     if ( CardInfo->pciMemBaseSpc[index] == 0 )
       break;
     printf ( "BAR %d Lower Addess          : %8x\n",
              index,
              (unsigned int) CardInfo->pciMemBaseSpc[index] );
    }
  }
}

int ExtractForensicData( agHandle_t adapterhandle, char* aFlName )
{
  printf ( " \'nonfatal\' not available in this version of Linux spcapp\n" ); 
  printf ( " see use of dpmc_lean instead \n" );
  printf ( " adapterhandle %lx  : %s\n", (long int)adapterhandle, aFlName );
  return 0;
}
// get the SCSI address, including pathId,targetId,Lun.
BOOL
GetSCSIAddress(agHandle_t adapterhandle,
         char *pszDeviceName,
         PSCSI_ADDRESS pScsiAddress
         )
{
//  HANDLE hDevice = NULL;
  bit32 dwByteSize = 0;
  bit32 dwByteReturned = 0;
  int hDevice = 0;
  int length = 0;
  bit8 *ioctlbuffer = NULL;
  tiIOCTLPayload_t  *payload = NULL;
  int status = 0;
  int i = 0;
  int *ptr = NULL;
  //agNameMap_t adapterhandle;
  //agHandle_t adapterhandle;

  allenf("GetSCSIAddress:  start\n");

  // *** HARDCODED card #!!! FIX
  //adapterhandle = GetCardHandle( atoi(token) );
  if (adapterhandle == NULL)
  {
    puts("\nnGetDeviceInfo: Invalid Card Number 3!!\n");
    return 1;
  }

  hDevice = open(pszDeviceName, O_RDWR, 0);

  if (hDevice < 0)
  {
    printf("GetSCSIAddress:  ERROR open(%s, ...)! %s\n", pszDeviceName, strerror(errno));
    return FALSE;
  }


  // this gets the adapter handle.
  // ******************************
  // HARDCODED adapter Index!! Fix later
  // ******************************
  // adapterhandle = agOpenHBA( 0 );
  //allenf("GetSCSIAddress:  opening adapterhandle = 0\n");

/*  adapterhandle = agOpenHBA( 0 );
      if ( adapterhandle < 0 )
      {
        printf( "GetSCSIAddress: Unable to open the adapter %d\n", 0 );
        return 1;
      }
*/

/*  hDevice = CreateFile(pszDeviceName, GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                OPEN_EXISTING, 0, NULL);

  if (INVALID_HANDLE_VALUE == hDevice)
  {
    //printf("Windows CreateFile last error =%ld, when open %s\n", GetLastError(), pszDeviceName);
    return FALSE;
  }
*/
  dwByteSize = sizeof(SCSI_ADDRESS) + sizeof(tiIOCTLPayload_t) - 1;
  pScsiAddress->Length = dwByteSize;
  ioctlbuffer = calloc( dwByteSize, 1 );
  allenf("GetSCSIAddress: dwByteSize %d\n", dwByteSize);

  if ( !ioctlbuffer )
  {
    printf( "** Error: Unable to allocate %d bytes ioctl buffer\n", length );
    close( hDevice );
    return 1;
  }
/*
  length = sizeof(SCSI_ADDRESS) + sizeof(tiIOCTLPayload_t) - 1;
  ioctlbuffer = calloc( length, 1 );
  allenf("GetSCSIAddress:  length = %d\n", length);
*/

/*  if ( !ioctlbuffer )
  {
    printf( "** Error: Unable to allocate %d bytes ioctl buffer\n", length );
    //close( hDevice );
    agCloseHBA( adapterhandle );
    return 1;
  }*/

  memset( ioctlbuffer, 0, dwByteSize);
  payload = (tiIOCTLPayload_t *) ioctlbuffer;
  payload->Signature     = ( bit32 )PMC_IOCTL_SIGNATURE;
  // payload->MajorFunction = IOCTL_MJ_GET_DEVICE_INFO;
  payload->MajorFunction = 0x1B;
  payload->MinorFunction = 0;
  payload->Length        = 128;
  //payload->Length        = sizeof(CardInfo_t) + sizeof(tiIOCTLPayload_t) - 1;
  payload->Status        = 5;
  payload->Reserved = 0xFFFFFFFF;

  allenf("GetSCSIAddress: payload->Length %d\n", payload->Length);

  ptr = (int*)&payload->Signature;

  for(i = 0; i < payload->Length; i++, ptr++)
  {
    if(*ptr == 0) continue;
    allenf("GetSCSIAddress: %4d - 0x%08x\n", i, *ptr);
  }

  allenf("GetSCSIAddress: sending ioctl...\n");
  // Fix this inconvenient datatype #define: agNameMap_t
//  status = ioctl(adapterhandle->handle, AGTIAPI_IOCTL, ioctlbuffer );
  status = agSendIOCTL(adapterhandle,
             IOCTL_CODE_UNDEFINED,
             ioctlbuffer,
             payload->Length
             );
  allenf("GetSCSIAddress: exited from ioctl\n");
  allenf("GetSCSIAddress: status %d\n", status);
  allenf("GetSCSIAddress: DUMP:\n");

  ptr = (int*)&payload->Signature;

  for(i = 0; i < payload->Length; i++)
  {
    if(*ptr == 0) continue;
    allenf("GetSCSIAddress: %4d - 0x%08x\n", i, *ptr++);
  }
  allenf("GetSCSIAddress:\n");

  if ( status != 0)
  {
    printf( "\n** Error: Error in IOCTL_MJ_GET_DEVICE_LIST status = %x "
            "retcode %x\n",
            status,
            payload->Status );
    free( ioctlbuffer );
    agCloseHBA( adapterhandle );
    //close device if testing that particular code

    return FALSE;
  }
/*fResult = DeviceIoControl(hDevice,
                IOCTL_SCSI_GET_ADDRESS,
                pScsiAddress,
                dwByteSize,
                pScsiAddress,
                dwByteSize,
                &dwByteReturned,
                NULL
                );
  if (!fResult)
  {
    printf("GetSCSIAddress %s: Failed to call DeviceIoControl, last error =%d \n", pszDeviceName, GetLastError());
  }

  CloseHandle(hDevice);
*/  return TRUE;
}

BOOL DumpTargetDeviceInfo(agHandle_t adapterhandle, tdDeviceInfoPayload_t *pDeviceInfo, int index)
{
  tiIOCTLPayload_t  *payload;
  bit32       length;
  //bit8                *ioctlbuffer = NULL;
  bit8 ioctlbuffer[2048] = {0};
  tdDeviceInfoPayload_t *pDevInfo = NULL;
  char                szDeviceType[32] = {0};
  char                szLinkRate[32] = {0};
  agStatus_t      status=0;

  allenf("DumpTargetDeviceInfo: start\n");

  length = sizeof(tdDeviceInfoPayload_t) + sizeof(tiIOCTLPayload_t) - 1 + agGetMemoryRequired(0);
  //ioctlbuffer = malloc(length);

  if (!ioctlbuffer)
  {
    printf("** Error: Unable to allocate %d bytes ioctl buffer\n", length);
    return FALSE;
  }

  memset(ioctlbuffer, 0, length);
  payload = (tiIOCTLPayload_t *)((bit8 *)ioctlbuffer + agGetMemoryRequired(0));
  payload->Signature = ( bit32 )PMC_IOCTL_SIGNATURE;
  payload->MajorFunction = IOCTL_MJ_GET_DEVICE_INFO;
  payload->Length = sizeof(tdDeviceInfoPayload_t) + sizeof(tiIOCTLPayload_t) - 1;
  payload->Status = 0;
  memcpy(payload->FunctionSpecificArea, pDeviceInfo, sizeof(tdDeviceInfoPayload_t));

  status = agSendIOCTL(adapterhandle,
             IOCTL_CODE_UNDEFINED,
             ioctlbuffer,
             payload->Length
             );
  if (status)
  {
    printf("\n** Error: Error in IOCTL_MJ_GET_DEVICE_INFO status = %x retcode %x\n",
      status,
      payload->Status );
    //free(ioctlbuffer);
    return FALSE;
  }

  pDevInfo = (tdDeviceInfoPayload_t*)payload->FunctionSpecificArea;

  /* Parse Device Type */
  switch(pDevInfo->devInfo.deviceType)
  {
  case 0:
    sprintf(szDeviceType, "STP Device");
    break;
  case 1:
    sprintf(szDeviceType, "SSP Device");
    break;
  case 2:
    sprintf(szDeviceType, "Direct SATA");
    break;
  default:
    sprintf(szDeviceType, "Unknown");
    break;
  }

  /* parse link rate */
  switch(pDevInfo->devInfo.linkRate)
  {
  case 8:
    sprintf(szLinkRate, " 1.5 Gbit/s");
    break;
  case 9:
    sprintf(szLinkRate, " 3.0 Gbit/s");
    break;
  case 10:
    sprintf(szLinkRate, " 6.0 Gbit/s");
    break;
  case 11:
    sprintf(szLinkRate, "12.0 Gbit/s");
    break;
  default:
    sprintf(szLinkRate, "Unknown    ");
    break;
  }

  /* print the SAS address and device id of the specified target device with SCSI addess */
  //printf("\nDisk  SCSIAddr    DevType     LinkRate    PhyID      SASAddr      Encrypt  DIF\n");
  //if (pDeviceInfo->devInfo.deviceType == 2)
  //{
  if (index != 0)
  {
    printf("%-4d  %d:%-3d:%d    %-11s %s   %-2d   0x%x 0x%x %5s   %3s\n",
      index,
      pDevInfo->PathId,
      pDevInfo->TargetId,
      pDevInfo->Lun,
      szDeviceType,
      szLinkRate,
      pDevInfo->devInfo.phyId,
      pDevInfo->devInfo.sasAddressHi,
      pDevInfo->devInfo.sasAddressLo,
	  pDevInfo->devInfo.isEncryption?"Y":"N",
	  pDevInfo->devInfo.isDIF?"Y":"N");
  }
  else
  {
    printf("%d:%-3d:%d    %-11s   %s   %-2d   0x%x 0x%x %5s   %3s\n",
      pDevInfo->PathId,
      pDevInfo->TargetId,
      pDevInfo->Lun,
      szDeviceType,
      szLinkRate,
      pDevInfo->devInfo.phyId,
      pDevInfo->devInfo.sasAddressHi,
      pDevInfo->devInfo.sasAddressLo,
	  pDevInfo->devInfo.isEncryption?"Y":"N",
	  pDevInfo->devInfo.isDIF?"Y":"N");
  }

  //allenf("DumpTargetDeviceInfo: before free(ioctlbuffer)\n");
  //free(ioctlbuffer);

  allenf("DumpTargetDeviceInfo: end\n");

  return TRUE;
}

int GetDeviceInfo( int numArg, char** pArg )
{
  tdDeviceInfoPayload_t DeviceInfo;
  agHandle_t      adapterhandle;
  PCHAR               token = NULL;
  BOOL                fIsDumpAll = FALSE;
  char                pszDeviceName[256]={0};
  int                 index = 1;
  SCSI_ADDRESS        scsiAddress;
  BOOL                fIsHaveTarget = FALSE;

  allenf("GetDeviceInfo: start\n");
  allenf("GetDeviceInfo: numArg %d\n", numArg);
  if(numArg != 2 && numArg != 5 )
  {
    printf("\nGetDeviceInfo: Invalid Parameters 1!!\n");
    PrintUsage();
    return 1;
  }

  //  Get next token which should specify which spc to get core dump from
  token = *pArg;
  if (token == NULL)
  {
    puts("\nnGetDeviceInfo: Invalid Parameters 2!!\n");
    //PrintUsage();
    return 1;
  }
  adapterhandle = GetCardHandle( atoi(token) );

  if (adapterhandle == NULL)
  {
    puts("\nnGetDeviceInfo: Invalid Card Number 3!!\n");
    return 1;
  }
  //  get the dumpall option
  token = *(++pArg);
  if (token == NULL)
  {
    puts("\nInvalid Parameters 4!!\n");
    PrintUsage();
    agCloseHBA(adapterhandle);
    return 1;
  }

  if ( strcmp(token, "YES") == 0 )
  {
    fIsDumpAll = TRUE;
  }
  else if ( strcmp(token, "NO") == 0)
  {
    fIsDumpAll = FALSE;
  }
  else
  {
    puts("\nInvalid Parameters 5!!\n");
    PrintUsage();
    agCloseHBA(adapterhandle);
    return 1;
  }

  memset(&DeviceInfo, 0, sizeof(tdDeviceInfoPayload_t));
  memset(&scsiAddress, 0, sizeof(SCSI_ADDRESS));

  allenf("GetDeviceInfo: fIsDumpAll %d\n", fIsDumpAll);

  if (fIsDumpAll)
  {
    printf("\nDisk  SCSIAddr    DevType     LinkRate    PhyID      SASAddr      Encrypt  DIF\n");

    //while ( index < SCSI_MAXIMUM_TARGETS_PER_BUS * SCSI_MAXIMUM_BUSES)
    index = 0;
    while ( index < 8)
    {
      sprintf(pszDeviceName, "/dev/sg%d", index);
      allenf("GetDeviceInfo: pszDeviceName = %s\n", pszDeviceName);

      allenf("GetDeviceInfo: index = %d\n", index);
      //if (GetSCSIAddress(adapterhandle, pszDeviceName, &scsiAddress) )
      int j = 0;
      while(j < 128)
      {
        allenf("GetDeviceInfo: pathID = %d, tgtID = %d, lun = %d\n", scsiAddress.PathId, scsiAddress.TargetId, scsiAddress.Lun);

        DeviceInfo.PathId = scsiAddress.PathId = index ;
        DeviceInfo.TargetId = scsiAddress.TargetId = j++;
        DeviceInfo.Lun = scsiAddress.Lun;

        if (DumpTargetDeviceInfo(adapterhandle, &DeviceInfo, index))
        {
          fIsHaveTarget = TRUE;
        }
      }
/*      else
        allenf("GetDeviceInfo: FAIL GetSCSIAddress() @ index = %d\n", index);*/
      scsiAddress.PathId++;
      scsiAddress.TargetId++;
      scsiAddress.Lun++;
      index++;
      sprintf(pszDeviceName, "\\\\.\\PHYSICALDRIVE%i", index);
    }

    if (!fIsHaveTarget)
    {
        puts("\n No target disk drive!!\n");
    }
  }
  else
  {
    //  Get next token which should specify which path id
    token = *(++pArg);
    if (token == NULL)
    {
      puts("\nInvalid Parameters!!\n");
      PrintUsage();
      agCloseHBA(adapterhandle);
      return 1;
    }

    DeviceInfo.PathId = atoi(token);

    //  Get the next token which should specify which target device id
    token = *(++pArg);
    if (token == NULL)
    {
      puts("\nInvalid Parameters!!\n");
      PrintUsage();
      agCloseHBA(adapterhandle);
      return 1;
    }

    DeviceInfo.TargetId = atoi(token);

    //  Get the next token which should specify which lun
    token = *(++pArg);
    if (token == NULL)
    {
      puts("\nInvalid Parameters!!\n");
      PrintUsage();
      agCloseHBA(adapterhandle);
      return 1;
    }

    DeviceInfo.Lun = atoi(token);

    printf("\nDisk  SCSIAddr    DevType     LinkRate    PhyID      SASAddr      Encrypt  DIF\n");

    DumpTargetDeviceInfo(adapterhandle, &DeviceInfo, 0);
  }

  agCloseHBA(adapterhandle);

  allenf("GetDeviceInfo: end\n");

  return 0;
}

int GetOrSetRegister( int numArg, char** pArg )
{
  printf ( " \'register\' not available in this version of Linux spcapp\n" ); 
  return 1;
}

int Rescan( int numArg, char**pArg )
{
  if ( numArg > 1 )
    printf ( "Rescan not available for Linux  %x  : %s\n",
             numArg, *pArg );
  return 1;
}

int GetLevel( int numArg, char**pArg )
{
  if ( numArg > 1 )
    printf ( "GetLevel not available for Linux  %x  : %s\n",
             numArg, *pArg );
  return 1;
}

int SetLevel( int numArg, char**pArg )
{
  if ( numArg > 1 )
    printf ( "SetLevel not available for Linux  %x  : %s\n",
             numArg, *pArg );
  return 1;
}


