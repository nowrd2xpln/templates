
/*****************************************************************************

* Copyright (c) PMC-Sierra, Inc. 2004-2014. All rights reserved.
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


 * spcioctl.h
 *
 * Common header file for spc IOCTL definitions
 */


#ifndef SPC_IOCTL_H
#define SPC_IOCTL_H


/*
 * Copied from drivers\windows\stor\osioctl.h
 */
/*----- Constants  ---------------------------------------------------*/
#define PMC_SIERRA_IOCTL_ISCSI             0x1234
#define PMC_SIERRA_IOCTL_FC                0x3412
#define PMC_IOCTL_SCSI                     0x1234
#define PMC_SIERRA_INI_IOCTL_FC            0x4567
#define PMC_SIERRA_IOCTL_SPC               0x7890


/*
 * Copied from tisa\api\titypes.h
 */
typedef struct tiIOCTLPayload
{
  ULONG       Signature;
  USHORT      MajorFunction;
  USHORT      MinorFunction;
  USHORT      Length;
  USHORT      Status;
  ULONG       Reserved; /* required for 64 bit alignment */
  UCHAR       FunctionSpecificArea[1];
}tiIOCTLPayload_t;


/*
 * Copied from tisa\sassata\common\tddefs.h
 */
#define PMC_IOCTL_SIGNATURE   0x1234

/*
 * Copied from tisa\api\tidefs.h
 */
/*
 * IOCTL Status Codes
 */
#define IOCTL_ERR_STATUS_OK                  0x00
#define IOCTL_ERR_STATUS_MORE_DATA           0x01
#define IOCTL_ERR_STATUS_NO_MORE_DATA        0x02
#define IOCTL_ERR_STATUS_INVALID_CODE        0x03
#define IOCTL_ERR_STATUS_INVALID_DEVICE      0x04
#define IOCTL_ERR_STATUS_NOT_RESPONDING      0x05
#define IOCTL_ERR_STATUS_INTERNAL_ERROR      0x06
#define IOCTL_ERR_STATUS_NOT_SUPPORTED       0x07
#define IOCTL_ERR_FW_EVENTLOG_DISABLED     0x08


/*
 * Copied from tisa\sassata\common\tdioctl.h
 */
/*
 * PMC-Sierra IOCTL signature
 */
#define PMC_SIERRA_SIGNATURE                0x1234
#define PMC_SIERRA_IOCTL_SIGNATURE          "PMC-STRG"

/*
 * Major function code of IOCTL functions, common to target and initiator.
 */
#define IOCTL_MJ_CARD_PARAMETER             0x01
#define IOCTL_MJ_FW_CONTROL                 0x02
#define IOCTL_MJ_NVMD_GET                   0x03
#define IOCTL_MJ_NVMD_SET                   0x04
#define IOCTL_MJ_GET_EVENT_LOG1             0x05
#define IOCTL_MJ_GET_EVENT_LOG2             0x06
#define IOCTL_MJ_GET_CORE_DUMP        0x07
#define IOCTL_MJ_GET_FW_REV         0x1A
/*
 * Major function code of IOCTL functions, specific to initiator.
 */
#define IOCTL_MJ_INI_ISCSI_DISCOVERY        0x21
#define IOCTL_MJ_INI_SESSION_CONTROL        0x22
#define IOCTL_MJ_INI_SNIA_IMA               0x23
#define IOCTL_MJ_INI_SCSI                   0x24
#define IOCTL_MJ_INI_WMI                    0x25
#define IOCTL_MJ_INI_DRIVER_EVENT_LOG       0x26
#define IOCTL_MJ_INI_PERSISTENT_BINDING     0x27
#define IOCTL_MJ_INI_DRIVER_IDENTIFY        0x28


/*
 * Minor functions for Card parameter IOCTL functions.
 */
#define IOCTL_MN_CARD_GET_VPD_INFO        0x01
#define IOCTL_MN_CARD_GET_PORTSTART_INFO  0x02

/*
 * Minor functions for FW control IOCTL functions.
 */

/* Send FW data requests.
 */
#define IOCTL_MN_FW_DOWNLOAD_DATA         0x01

/* Send the request for burning the new firmware.
 */
#define IOCTL_MN_FW_DOWNLOAD_BURN         0x02

/* Poll for the flash burn phases. Sequences of poll function calls are
 * needed following the IOCTL_MN_FW_DOWNLOAD_BURN, IOCTL_MN_FW_BURN_OSPD
 * and IOCTL_MN_FW_ROLL_BACK_FW functions.
 */
#define IOCTL_MN_FW_BURN_POLL             0x03

/* Instruct the FW to roll back FW to prior revision.
 */
#define IOCTL_MN_FW_ROLL_BACK_FW          0x04

/* Instruct the FW to return the current firmware revision number.
 */
#define IOCTL_MN_FW_VERSION               0x05

/* Retrieve the maximum size of the OS Persistent Data stored on the card.
 */
#define IOCTL_MN_FW_GET_OSPD_SIZE   0x06

/*  Retrieve the OS Persistent Data from the card.
 */
#define IOCTL_MN_FW_GET_OSPD              0x07

/* Send a new OS Persistent Data to the card and burn in flash.
 */
#define IOCTL_MN_FW_BURN_OSPD           0x08

/* Retrieve the trace buffer from the card FW. Only available on the debug
 * version of the FW.
 */
#define IOCTL_MN_FW_GET_TRACE_BUFFER    0x0f

#define IOCTL_MN_NVMD_GET_CONFIG    0x0A
#define IOCTL_MN_NVMD_SET_CONFIG    0x0B

#define IOCTL_MN_FW_GET_CORE_DUMP_AAP1          0x0C
#define IOCTL_MN_FW_GET_CORE_DUMP_IOP         0X0D
#define IOCTL_MN_FW_GET_CORE_DUMP_FLASH_AAP1    0X12
#define IOCTL_MN_FW_GET_CORE_DUMP_FLASH_IOP     0X13

#define IOCTL_MN_FW_GET_EVENT_FLASH_LOG1    0x5A
#define IOCTL_MN_FW_GET_EVENT_FLASH_LOG2    0x6A
#define IOCTL_MN_GET_EVENT_LOG1         0x5B
#define IOCTL_MN_GET_EVENT_LOG2         0x6B

#endif SPC_IOCTL_H