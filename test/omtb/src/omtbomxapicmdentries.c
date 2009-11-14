/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbOmxApiCmdEntries.c
 *
 *  @brief    This file contains command entries of OMX apis and omx func mode
 *            handlers.
 *
 * <B> History: </B>
 *
 *    # September-21-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>

/* OMX standard header files */
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "mxDbg.h"
#include "omtbCommon.h"
#include "omtbApi.h"


/******************************************************************************\
 *      Local Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Extern Object Definitions
\******************************************************************************/

/******************************************************************************\
 *      Globals
\******************************************************************************/

MX_DBG_CMD_TABLE_T   funcCmdTbl;              // Command table for func command


/******************************************************************************\
 *      Public Function Prototype
\******************************************************************************/


/******************************************************************************\
 *      Static Globals
\******************************************************************************/


/******************************************************************************\
 *      Private Function Declarations
\******************************************************************************/

/*******************************************************************************
 *  OMX API mode OR Func Mode Command Entries/Tables - api/func
 ******************************************************************************/
 
/*******************************************************************************
 *      OMX API command entry
 ******************************************************************************/

/* Entry for OMX Core Init */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxInit =
{
  "init",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxInit,
  "",
  "",
  "",
  ""
};


/* Entry for OMX Core DeInit */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxDeInit =
{
  "uninit",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxDeinit,
  "",
  "",
  "",
  ""
};


/* Entry for OMX Component Get Handle */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetHandle =
{
  "gethandle",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetHandle,
  "",
  "  <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> <template #>",
  "",
  ""
};


/* Entry for OMX Component Get Version */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetCompVersion =
{
  "compver",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetCompVersion,
  "",
  "    <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #>",
  "",
  ""
};


/* Entry for OMX Component Set Param */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSetParam =
{
  "setparam",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSetParam,
  "",
  "   <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<template #> <port #> \
<OMX_IndexConfigVideoFramerate | OMX_IndexConfigVideoBitrate |\
 OMX_TI_IndexConfigVideoMESearchRange | OMX_IndexConfigVideoIntraVOPRefresh\
 OMX_TI_IndexConfigVideoQPSettings | OMX_IndexConfigVideoAVCIntraPeriod\
 OMX_TI_IndexConfigSliceSettings | OMX_TI_IndexConfigVideoPixelInfo>",
  "",
  ""
};


/* Entry for OMX Component Set Config */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSetConfig =
{
  "setconfig",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSetConfig,
  "",
  "  <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<template #> <port #> \
<OMX_IndexConfigVideoFramerate | OMX_IndexConfigVideoBitrate |\
 OMX_TI_IndexConfigVideoMESearchRange | OMX_IndexConfigVideoIntraVOPRefresh\
 OMX_TI_IndexConfigVideoQPSettings | OMX_IndexConfigVideoAVCIntraPeriod\
 OMX_TI_IndexConfigSliceSettings | OMX_TI_IndexConfigVideoPixelInfo>",
  "",
  ""
};


/* Entry for OMX Component Get Param */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetParam =
{
  "getparam",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetParam,
  "",
  "   <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<template #> <port #> \
<OMX_IndexConfigVideoFramerate | OMX_IndexConfigVideoBitrate |\
 OMX_TI_IndexConfigVideoMESearchRange | OMX_IndexConfigVideoIntraVOPRefresh\
 OMX_TI_IndexConfigVideoQPSettings | OMX_IndexConfigVideoAVCIntraPeriod\
 OMX_TI_IndexConfigSliceSettings | OMX_TI_IndexConfigVideoPixelInfo>",
  "",
  ""
};


/* Entry for OMX Component Get Config */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetConfig =
{
  "getconfig",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetConfig,
  "",
  "  <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<template #> <port #> \
<OMX_IndexConfigVideoFramerate | OMX_IndexConfigVideoBitrate |\
 OMX_TI_IndexConfigVideoMESearchRange | OMX_IndexConfigVideoIntraVOPRefresh\
 OMX_TI_IndexConfigVideoQPSettings | OMX_IndexConfigVideoAVCIntraPeriod\
 OMX_TI_IndexConfigSliceSettings | OMX_TI_IndexConfigVideoPixelInfo>",
  "",
  ""
};


/* Entry for OMX Component Get Extension Index */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetExtensionIndex =
{
  "getextindex",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetExtensionIndex,
  "",
  "<h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> <Index String>",
  "",
  ""
};


/* Entry for OMX Component Get State */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxGetState =
{
  "getstate",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxGetState,
  "",
  "   <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #>",
  "",
  ""
};


/* Entry for OMX Component Get State */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSetupTunnel =
{
  "setuptunnel",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSetupTunnel,
  "",
  "<h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> <port #> \
<h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> <port #>",
  "",
  ""
};


/* Entry for OMX Component Free Handle */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxFreeHandle =
{
  "freehandle",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxFreeHandle,
  "",
  " <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #>",
  "",
  ""
};


/* Entry for OMX Send Command - state*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSendCommandState =
{
  "state",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSendCommandState,
  "",
  "<h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<loaded | idle | exec | pause | waitforresources>",
  "",
  ""
};

/* Entry for OMX Send Command - port*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSendCommandPort =
{
  "port",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSendCommandPort,
  "",
  " <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<enable|disable> <port #>",
  "",
  ""
};


/* Entry for OMX Send Command - flush*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSendCommandFlush =
{
  "flush",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSendCommandFlush,
  "",
  "<h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> <port #>",
  "",
  ""
};


/* Entry for OMX Send Command - mark*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSendCommandMark =
{
  "mark",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OmxSendCommandMark,
  "",
  " <h264vdec | g711adec | h264venc | vfcc | vfdc > <instance #> \
<h264vdec | g711adec | h264venc | vfcc | vfdc \
(mark component name)> <instance # (of mark component)>",
  "",
  ""
};


/* sencommand entry command list */

PRIVATE MX_DBG_CMD_ENTRY_T* cmdListOmxApiSendCommand[] =
{
  &cmdEntryOmxSendCommandState,
  &cmdEntryOmxSendCommandPort,
  &cmdEntryOmxSendCommandFlush,
  &cmdEntryOmxSendCommandMark,
  NULL
};


/* Entry for OMX Component Get Extension Index */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOMXComponentNameEnum =
{
  "getcompname",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OMXComponentNameEnum,
  "",
  "",
  "",
  ""
};


/* Entry for OMX Component Get Extension Index */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOMXRoleEnum =
{
  "rolesbycomp",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OMXRoleEnum,
  "",
  "<Component name>",
  "",
  ""
};


/* Entry for OMX Component Get Extension Index */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOMXComponentEnumByRole =
{
  "compsbyrole",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_OMXComponentEnumByRole,
  "",
  "<Role name>",
  "",
  ""
};


/* comp command table */

MX_DBG_CMD_TABLE_T cmdTblOmxApiSendCommand =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListOmxApiSendCommand)/sizeof(cmdListOmxApiSendCommand[0]),
  cmdListOmxApiSendCommand
};


/* Entry for OMX Send Command*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmxSendCommand =
{
  "sendcommand",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  &cmdTblOmxApiSendCommand,
  OMTB_OmxCmdSendCommand,
  "",
  "",
  "",
  ""
};


/* comp entry command list */

PRIVATE MX_DBG_CMD_ENTRY_T* cmdListOmxApi[] =
{
  &cmdEntryOmxInit,
  &cmdEntryOmxGetHandle,
  &cmdEntryOmxGetCompVersion,
//  &cmdEntryOmxSetParam,
//  &cmdEntryOmxSetConfig,
  &cmdEntryOmxSendCommand,  
  &cmdEntryOmxSetupTunnel,
//  &cmdEntryOmxGetParam,
//  &cmdEntryOmxGetConfig,
  &cmdEntryOmxGetState,
  &cmdEntryOmxGetExtensionIndex,
  &cmdEntryOMXComponentNameEnum,
  &cmdEntryOMXRoleEnum,
  &cmdEntryOMXComponentEnumByRole,
  &cmdEntryOmxFreeHandle,
  &cmdEntryOmxDeInit,
  NULL
};

/* comp command table */

MX_DBG_CMD_TABLE_T cmdTblOmxApi =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListOmxApi)/sizeof(cmdListOmxApi[0]),
  cmdListOmxApi
};


/*******************************************************************************
 *      Func command entries
 ******************************************************************************/
#ifdef OMTB_FF_VD_H264
/* Command entry for video decoders */
PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryFuncVidDec = 
{
  "viddec",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_FuncViddec,
  "",
  "<template #> <instance #> <h264vdec>",
  "",
  ""
};

#endif
#ifdef OMTB_FF_AD_G711
/* Command entry for audio decoders */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryFuncAudDec = 
{
  "auddec",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_FuncAuddec,
  "",
  "<template #> <instance #> <g711adec>",
  "",
  ""
};

#endif
#ifdef OMTB_FF_VE_H264
/* Command entry for video encoders */
PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryFuncVidEnc = 
{
  "videnc",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_FuncVidenc,
  "",
  "<template #> <instance #> <h264venc>",
  "",
  ""
};
#endif

#ifdef OMTB_FF_VPS_FCC
/* Command entry for HD VPSS */
PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryFuncHdVpss = 
{
  "hdvpss",
  MX_DBG_CMD_TYPE_VAR_UINT16,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  NULL,
  OMTB_FuncVpss,
  "",
  "<template #> <instance #> <vfcc | vfdc>",
  "",
  ""
};

#endif

/* func command list entry */

PRIVATE MX_DBG_CMD_ENTRY_T* funcCmdList[] =
{
#ifdef OMTB_FF_VD_H264
  &cmdEntryFuncVidDec,
#endif
#ifdef OMTB_FF_AD_G711
  &cmdEntryFuncAudDec,
#endif
#ifdef OMTB_FF_VE_H264
  &cmdEntryFuncVidEnc,
#endif
#ifdef OMTB_FF_VPS_FCC
  &cmdEntryFuncHdVpss,
#endif
  NULL
};


/* func command table */

MX_DBG_CMD_TABLE_T funcCmdTbl=
{
  MX_DBG_ACCESS_ALL,
  sizeof(funcCmdList) / sizeof(funcCmdList[0]),
  funcCmdList 
};


/*----------------------------- end of file ---------------------------------*/
