/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbH264VdecCfgCmdEntries.c
 *
 *  @brief    This file contains command entries of H264 Vdec configuration
 *            parameters.
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
 *  OMTB component configuration parameter command entries - h264vdec
 ******************************************************************************/

/*******************************************************************************
 *      H264 decoder config params
 ******************************************************************************/

/* Entry for Set OMX Component Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecCompName =
{
  "compname",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,compName),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,compName),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecInFile =
{
  "infile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecOutFile =
{
  "outfile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,outputFile),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,outputFile),
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


/* Entry for Set Output File Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecFrameData =
{
  "frame_size_file",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameSizeFile),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameSizeFile),
  0,
  0,
  OMTB_MAX_LEN,
  (int)NULL,
  NULL,
  XdbgCmdVarFunc,
  "Frame Size File name..........................",
  "<file_name>",
  "",
  ""
};


/*Entry for Set Input Frame Width*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecWidth =
{
  "frame_width",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameWidth),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameWidth),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecHeight =
{
  "frame_height",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameHeight),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,frameHeight),
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

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VdecOutputChromaFormat[] =
{
  {"420P",OMX_COLOR_FormatYUV420Planar, ""},
  {"420PP",OMX_COLOR_FormatYUV420SemiPlanar, ""},
  {"422P",OMX_COLOR_FormatYUV422Planar, ""},
  {"422YUYV", OMX_COLOR_FormatYCbYCr, ""},
  {"", 0, ""}
};


/* Entry for Set O/P Color Format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecOutputChromaFormat =
{
  "op_chromat_format",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,colorFormat),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,colorFormat),
  0,
  OMX_COLOR_FormatYUV420Planar,
  OMX_COLOR_FormatYCbYCr,
  OMX_COLOR_FormatYUV422Planar,
  &enumTblH264VdecOutputChromaFormat,
  XdbgCmdVarFunc,
  "O/P Chroma Format.............................",
  "",
  "",
  ""
};

#ifdef OMTB_FF_DRIVER_DISPLAY
/* Enum Entry for Supported O/P Color Format */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblDisplayOutputType[] =
{
  {"lcd",OMTB_DISP_LCD, ""},
  {"svideo",OMTB_DISP_SVIDEO, ""},
  {"dvi",OMTB_DISP_DVI, ""},
  {"", 0, ""}
};


/* Entry for Set O/P Color Format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVidDispType =
{
  "disp_type",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OmtbDispInfo,dispOutput),
  FIELDOFFSET(OmtbDispInfo,device) -
  FIELDOFFSET(OmtbDispInfo,dispOutput),
  0 ,
  OMTB_DISP_LCD,
  OMTB_DISP_DVI,
  OMTB_DISP_DVI,
  &enumTblDisplayOutputType,
  XdbgCmdVarFunc,
  "O/P Display Type..............................",
  "",
  "",
  ""
};

#endif
/* Enum Entry for Selecting Data O/P Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblDataOutputMode[] =
{
  {"file",  OMTB_FILE,    ""},
  {"driver",OMTB_DRIVER,  ""},
  {"none",  OMTB_NONE,    ""},
  {"",      0,            ""}
};


/* Entry for Setting Data Output Mode */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecOutputMode =
{
  "data_opmode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,dataOutPutMode),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,dataOutPutMode),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecFlagUseAllocInBuffer =
{
  "inbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,flagUseInBuffer),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,flagUseInBuffer),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecFlagUseAllocOutBuffer =
{
  "outbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,flagUseOutBuffer),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,flagUseOutBuffer),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecNumInBuf =
{
  "numinbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,numInBuf),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,numInBuf),
  0,
  0,
  OMTB_H264_VDEC_MAX_NUM_IN_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of i/p buffer..........................",
  "<# of input buffers>",
  "",
  ""
};


/*Entry for Set number of o/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VdecNumOutBuf =
{
  "numoutbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,numOutBuf),
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VDEC_CONFIG_PARAMS,numOutBuf),
  0,
  0,
  OMTB_H264_VDEC_MAX_NUM_OUT_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of o/p buffer..........................",
  "<# of output buffers>",
  "",
  ""
};

#if 0

/* Enum Entry for Supported Display Format */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblDisplayOutputFormat[] =
{
  {"qcif_pal",  OMTB_QCIF_PAL,  ""},
  {"qcif_ntsc", OMTB_QCIF_NTSC, ""},
  {"cif_pal",   OMTB_CIF_PAL,   ""},
  {"cif_ntsc",  OMTB_CIF_NTSC,  ""},
  {"pal",       OMTB_PAL,       ""},
  {"ntsc",      OMTB_NTSC,      ""},
  {"",          0,              ""}
};



/* Entry for Set Display Format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryVidDisFormat =
{
  "disp_format",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OmtbDispInfo,std),
  FIELDOFFSET(OmtbDispInfo,device) -
  FIELDOFFSET(OmtbDispInfo,std),
  0,
  OMTB_QCIF_PAL,
  OMTB_CIF_PAL,
  OMTB_NTSC,
  &enumTblDisplayOutputFormat,
  XdbgCmdVarFunc,
  "Display Format.............................",
  "",
  "",
  ""
};
#endif

#ifdef OMTB_FF_DRIVER_DISPLAY
/* h264videc config command table Entry */

MX_DBG_CMD_ENTRY_T* cmdListDispConfig[] =
{
  &cmdEntryVidDispType,
//  &cmdEntryVidDisFormat,
  NULL
};
#endif

/* h264vdec config command table Entry */

MX_DBG_CMD_ENTRY_T* cmdListH264VdecConfig[] =
{
  &cmdEntryH264VdecCompName,
  &cmdEntryH264VdecInFile,
  &cmdEntryH264VdecOutFile,
  &cmdEntryH264VdecFrameData,
  &cmdEntryH264VdecWidth,
  &cmdEntryH264VdecHeight,
  &cmdEntryH264VdecOutputChromaFormat,
  &cmdEntryH264VdecOutputMode,
  &cmdEntryH264VdecFlagUseAllocInBuffer,
  &cmdEntryH264VdecFlagUseAllocOutBuffer,
  &cmdEntryH264VdecNumInBuf,
  &cmdEntryH264VdecNumOutBuf,
  NULL
};

#ifdef OMTB_FF_DRIVER_DISPLAY
/* Display config command table */

MX_DBG_CMD_TABLE_T cmdTblDispConfig =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListDispConfig)/sizeof(cmdListDispConfig[0]),
  cmdListDispConfig
};
#endif

/* h264vdec config command table */

MX_DBG_CMD_TABLE_T cmdTblH264VdecConfig =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListH264VdecConfig)/sizeof(cmdListH264VdecConfig[0]),
  cmdListH264VdecConfig
};

#ifdef OMTB_FF_DRIVER_DISPLAY
/* Video Display config command entry */

MX_DBG_CMD_ENTRY_T cmdEntryDisplayConfig =
{
  "v4l2disp",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_TEMPLATE_INFO,v4l2DispTemplate),
  FIELDOFFSET(OMTB_TEMPLATE_INFO,h264VdecTemplate) -
  FIELDOFFSET(OMTB_TEMPLATE_INFO,v4l2DispTemplate),
  0,
  0,
  0,
  0,
  &cmdTblDispConfig,
  XdbgCmdTblFunc,
  "\nV4L2 Dispaly Config Parameters:\n",
  "",
  "",
  ""
};
#endif

/* H264vdec config command entry */

MX_DBG_CMD_ENTRY_T cmdEntryH264VdecConfig =
{
  OMTB_H264_VDEC_COMP_REF_STR,
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_TEMPLATE_INFO,h264VdecTemplate),
  0,
  0,
  0,
  0,
  0,
  &cmdTblH264VdecConfig,
  XdbgCmdTblFunc,
  "\nH264 Video Decoder Config Parameters:\n",
  "",
  "",
  ""
};


/*----------------------------- end of file ---------------------------------*/
