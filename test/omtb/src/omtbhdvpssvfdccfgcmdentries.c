/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbHDVPSSVfdcCfgCmdEntries.c
 *
 *  @brief    This file contains command entries of VPSS VFDC configuration
 *            parameters.
 *
 * <B> History: </B>
 *
 *    # October-12-2009   AVME System Test    Initial Version
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
 *  OMTB component configuration parameter command entries - vfcc
 ******************************************************************************/

/*******************************************************************************
 *      H264 decoder config params
 ******************************************************************************/

/* Entry for Set OMX Component Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcCompName =
{
  "compname",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,compName),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,compName),
  0,
  0,
  OMX_MAX_STRINGNAME_SIZE,
  (int)NULL,
  NULL,
  XdbgCmdVarFunc,
  "OMX Component name............................",
  "<omx_comp_name>",
  "",
  ""
};


/* Entry for Set Input File Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcInFile =
{
  "infile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile),
  0,
  0,
  OMTB_MAX_LEN,
  (int)NULL,
  NULL,
  XdbgCmdVarFunc,
  "Input File name...............................",
  "<file_name>",
  "",
  ""
};


/* Entry for Set Output File Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcOutFile =
{
  "outfile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,outputFile),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,outputFile),
  0,
  0,
  OMTB_MAX_LEN,
  (int)NULL,
  NULL,
  XdbgCmdVarFunc,
  "Output File name..............................",
  "<file_name>",
  "",
  ""
};


/*Entry for Set Input Frame Width*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcWidth =
{
  "frame_width",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,frameWidth),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,frameWidth),
  0,
  0,
  OMTB_MAX_VID_WIDTH,
  OMTB_MAX_VID_WIDTH,
  NULL,
  XdbgCmdVarFunc,
  "Input Frame Width.............................",
  "<width>",
  "",
  ""
};


/*Entry for Set Input Frame Height*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcHeight =
{
  "frame_height",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,frameHeight),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,frameHeight),
  0,
  0,
  OMTB_MAX_VID_HEIGHT,
  OMTB_MAX_VID_HEIGHT,
  NULL,
  XdbgCmdVarFunc,
  "Input Frame Height............................",
  "<Height>",
  "",
  ""
};


/* Enum Entry for Supported O/P Color Format */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblVPSSVfdcOutputChromaFormat[] =
{
  {"420P",OMX_COLOR_FormatYUV420Planar, ""},
  {"420PP",OMX_COLOR_FormatYUV420SemiPlanar, ""},
  {"422P",OMX_COLOR_FormatYUV422Planar, ""},
  {"422YUYV", OMX_COLOR_FormatYCbYCr, ""},
  {"", 0, ""}
};


/* Entry for Set O/P Color Format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcOutputChromaFormat =
{
  "op_chromat_format",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,colorFormat),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,colorFormat),
  0,
  OMX_COLOR_FormatYUV420Planar,
  OMX_COLOR_FormatYCbYCr,
  OMX_COLOR_FormatYUV422Planar,
  &enumTblVPSSVfdcOutputChromaFormat,
  XdbgCmdVarFunc,
  "O/P Chroma Format.............................",
  "",
  "",
  ""
};


/* Enum Entry for Selecting Data O/P Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblDataOutputMode[] =
{
  {"file",  OMTB_FILE,    ""},
  {"driver",OMTB_DRIVER,  ""},
  {"none",  OMTB_NONE,    ""},
  {"",      0,            ""}
};


/* Entry for Setting Data Output Mode */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcOutputMode =
{
  "data_opmode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,dataOutPutMode),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,dataOutPutMode),
  0,
  OMTB_FILE,
  OMTB_NONE,
  OMTB_DRIVER,
  &enumTblDataOutputMode,
  XdbgCmdVarFunc,
  "Data O/P Mode.................................",
  "",
  "",
  ""
};

/* Enum Entry for Selecting OMX_AllocateBuffer or OMX_UseBuffer Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblBufAllocationMode[] =
{
  {"use",      OMX_TRUE,  ""},
  {"allocate", OMX_FALSE, ""},
  {"",         0,         ""}
};


/* Entry for Setting Use/Allocate input buffer */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcFlagUseAllocInBuffer =
{
  "inbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,flagUseInBuffer),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,flagUseInBuffer),
  0,
  OMX_FALSE,
  OMX_TRUE,
  OMX_TRUE,
  &enumTblBufAllocationMode,
  XdbgCmdVarFunc,
  "I/P Buffer allocation Mode....................",
  "",
  "",
  ""
};


/* Entry for Setting Use/Allocate output buffer */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcFlagUseAllocOutBuffer =
{
  "outbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,flagUseOutBuffer),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,flagUseOutBuffer),
  0,
  OMX_FALSE,
  OMX_TRUE,
  OMX_TRUE,
  &enumTblBufAllocationMode,
  XdbgCmdVarFunc,
  "O/P Buffer allocation Mode....................",
  "",
  "",
  ""
};


/*Entry for Set number of i/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcNumInBuf =
{
  "numinbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,numInBuf),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,numInBuf),
  0,
  0,
  OMTB_VPSS_VFDC_MAX_NUM_IN_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of i/p buffer..........................",
  "<# of input buffers>",
  "",
  ""
};


/*Entry for Set number of o/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcNumOutBuf =
{
  "numoutbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,numOutBuf),
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_VPSS_VFDC_CONFIG_PARAMS,numOutBuf),
  0,
  0,
  OMTB_VPSS_VFDC_MAX_NUM_OUT_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of o/p buffer..........................",
  "<# of output buffers>",
  "",
  ""
};


/* vfcc config command table Entry */

MX_DBG_CMD_ENTRY_T* cmdListVPSSVfdcConfig[] =
{
  &cmdEntryVPSSVfdcCompName,
  &cmdEntryVPSSVfdcInFile,
  &cmdEntryVPSSVfdcOutFile,
  &cmdEntryVPSSVfdcWidth,
  &cmdEntryVPSSVfdcHeight,
  &cmdEntryVPSSVfdcOutputChromaFormat,
  &cmdEntryVPSSVfdcOutputMode,
  &cmdEntryVPSSVfdcFlagUseAllocInBuffer,
  &cmdEntryVPSSVfdcFlagUseAllocOutBuffer,
  &cmdEntryVPSSVfdcNumInBuf,
  &cmdEntryVPSSVfdcNumOutBuf,
  NULL
};


/* vfcc config command table */

MX_DBG_CMD_TABLE_T cmdTblVPSSVfdcConfig =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListVPSSVfdcConfig)/sizeof(cmdListVPSSVfdcConfig[0]),
  cmdListVPSSVfdcConfig
};


/* VPSS VFDC config command entry */

MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcConfig =
{
  OMTB_VPSS_VFDC_COMP_REF_STR,
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_TEMPLATE_INFO,vpssVfccTemplate),
  0,
  0,
  0,
  0,
  0,
  &cmdTblVPSSVfdcConfig,
  XdbgCmdTblFunc,
  "\nVPSS VFDC Config Parameters:\n",
  "",
  "",
  ""
};


/*----------------------------- end of file ---------------------------------*/
