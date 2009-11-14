/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbH264VencCfgCmdEntries.c
 *
 *  @brief    This file contains command entries of H264 Venc configuration
 *            parameters.
 *
 * <B> History: </B>
 *
 *    # October-2-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>

/* OMX standard header files */
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#ifdef WTSD
#include "omx_ti_video.h"
#include "omx_ti_index.h"
#else
#include "wtsd17_omx_ti_video.h"
#include "wtsd17_omx_ti_index.h"
#endif

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
 *  OMTB component configuration parameter command entries - h264venc
 ******************************************************************************/

/*******************************************************************************
 *      H264 encoder config params
 ******************************************************************************/

/*******************************************************************************
 *                    Basic configuration Params
 ******************************************************************************/

/* Entry for Set OMX Component Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencCompName =
{
  "compname",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,compName),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,compName),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencInFile =
{
  "infile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencOutFile =
{
  "outfile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,outputFile),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,outputFile),
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


/* Entry for Set Input Frame Width */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencWidth =
{
  "frame_width",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameWidth),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameWidth),
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


/* Entry for Set Input Frame Height */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencHeight =
{
  "frame_height",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameHeight),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameHeight),
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


/* Enum Entry for Supported I/P Color Format */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencInputChromaFormat[] =
{
  {"420P",    OMX_COLOR_FormatYUV420Planar, ""},
  {"420PP",   OMX_COLOR_FormatYUV420SemiPlanar, ""},
  {"422P",    OMX_COLOR_FormatYUV422Planar, ""},
  {"422YUYV", OMX_COLOR_FormatYCbYCr, ""},
  {"", 0, ""}
};


/* Entry for Set I/P Color Format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencInputChromaFormat =
{
  "ip_chromat_format",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,colorFormat),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,colorFormat),
  0,
  OMX_COLOR_FormatYUV420Planar,
  OMX_COLOR_FormatYCbYCr,
  OMX_COLOR_FormatYUV422Planar,
  &enumTblH264VencInputChromaFormat,
  XdbgCmdVarFunc,
  "I/P Chroma Format.............................",
  "",
  "",
  ""
};


/* Enum Entry for Supported bitstream format */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencBitStrmFmt[] =
{
  {"byte",   OMX_Video_BitStreamFormatByte, ""},
  {"nal",    OMX_Video_BitStreamFormatNAL, ""},
  {"", 0, ""}
};


/* Entry for Set bitstream format */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencBitStrmFmt =
{
  "bitstrm_fmt",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,bitStreamFormat),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,bitStreamFormat),
  0,
  OMX_Video_BitStreamFormatByte,
  OMX_Video_BitStreamFormatByte,
  OMX_Video_BitStreamFormatNAL,
  &enumTblH264VencBitStrmFmt,
  XdbgCmdVarFunc,
  "Bitstream Format..............................",
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencOutputMode =
{
  "data_opmode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,dataOutPutMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,dataOutPutMode),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFlagUseAllocInBuffer =
{
  "inbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,flagUseInBuffer),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,flagUseInBuffer),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFlagUseAllocOutBuffer =
{
  "outbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,flagUseOutBuffer),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,flagUseOutBuffer),
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


/* Entry for Set number of i/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNumInBuf =
{
  "numinbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,numInBuf),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,numInBuf),
  0,
  0,
  OMTB_H264_VENC_MAX_NUM_IN_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of i/p buffer..........................",
  "<# of input buffers>",
  "",
  ""
};


/* Entry for Set number of o/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNumOutBuf =
{
  "numoutbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,numOutBuf),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,numOutBuf),
  0,
  0,
  OMTB_H264_VENC_MAX_NUM_OUT_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of o/p buffer..........................",
  "<# of output buffers>",
  "",
  ""
};


/* Enum Entry for Supported AVC profiles */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencProfile[] =
{
  {"baseline",  OMX_VIDEO_AVCProfileBaseline, ""},
  {"main",      OMX_VIDEO_AVCProfileMain, ""},
  {"extended",  OMX_VIDEO_AVCProfileExtended, ""},
  {"high",      OMX_VIDEO_AVCProfileHigh, ""},
  {"high10",    OMX_VIDEO_AVCProfileHigh10, ""},
  {"high422",   OMX_VIDEO_AVCProfileHigh422, ""},
  {"high444",   OMX_VIDEO_AVCProfileHigh444, ""},
  {"", 0, ""}
};


/* Entry for Set AVC profile */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencProfile =
{
  "profile",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,profileLevel.eProfile),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,profileLevel.eProfile),
  0,
  OMX_VIDEO_AVCProfileBaseline,
  OMX_VIDEO_AVCProfileBaseline,
  OMX_VIDEO_AVCProfileMax,
  &enumTblH264VencProfile,
  XdbgCmdVarFunc,
  "AVC Profile...................................",
  "",
  "",
  ""
};


/* Enum Entry for Supported AVC levels */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencLevel[] =
{
  {"1",     OMX_VIDEO_AVCLevel1, ""},
  {"1.0",   OMX_VIDEO_AVCLevel1, ""},
  {"1b",    OMX_VIDEO_AVCLevel1b, ""},
  {"1.1",   OMX_VIDEO_AVCLevel11, ""},
  {"1.2",   OMX_VIDEO_AVCLevel12, ""},
  {"1.3",   OMX_VIDEO_AVCLevel13, ""},
  {"2",     OMX_VIDEO_AVCLevel2, ""},
  {"2.0",   OMX_VIDEO_AVCLevel2, ""},
  {"2.1",   OMX_VIDEO_AVCLevel21, ""},
  {"2.2",   OMX_VIDEO_AVCLevel22, ""},
  {"3",     OMX_VIDEO_AVCLevel3, ""},
  {"3.0",   OMX_VIDEO_AVCLevel3, ""},
  {"3.1",   OMX_VIDEO_AVCLevel31, ""},
  {"3.2",   OMX_VIDEO_AVCLevel32, ""},
  {"4",     OMX_VIDEO_AVCLevel4, ""},
  {"4.0",   OMX_VIDEO_AVCLevel4, ""},
  {"4.1",   OMX_VIDEO_AVCLevel41, ""},
  {"4.2",   OMX_VIDEO_AVCLevel42, ""},
  {"5",     OMX_VIDEO_AVCLevel5, ""},
  {"5.0",   OMX_VIDEO_AVCLevel5, ""},
  {"5.1",   OMX_VIDEO_AVCLevel51, ""},
  {"", 0, ""}
};


/* Entry for Set AVC level */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencLevel =
{
  "level",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,profileLevel.eLevel),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,profileLevel.eLevel),
  0,
  OMX_VIDEO_AVCLevel1,
  OMX_VIDEO_AVCLevel1,
  OMX_VIDEO_AVCLevelMax,
  &enumTblH264VencLevel,
  XdbgCmdVarFunc,
  "AVC Level.....................................",
  "",
  "",
  ""
};


/* Enum Entry for Supported I/P frame content type */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencInFrmContentType[] =
{
  {"progressive", OMX_Video_Progressive,            ""},
  {"interlaced",  OMX_Video_Interlaced,             ""},
  {"topfld",      OMX_Video_Interlaced_Topfield,    ""},
  {"botfld",      OMX_Video_Interlaced_Bottomfield, ""},
  {"",            0,                                ""}
};


/* Entry for Set I/P frame content type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencInFrmContentType =
{
  "frm_type",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputImgFmt.eContentType),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputImgFmt.eContentType),
  0,
  OMX_Video_Progressive,
  OMX_Video_Progressive,
  OMX_Video_Interlaced_Bottomfield,
  &enumTblH264VencInFrmContentType,
  XdbgCmdVarFunc,
  "Input Content Type............................",
  "",
  "",
  ""
};


/* Enum Entry for Supported interlace coding type */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencInterlacedCodingType[] =
{
  {"picaff",      OMX_Video_Interlace_PICAFF,         ""},
  {"mbaff",       OMX_Video_Interlace_MBAFF,          ""},
  {"fldonly",     OMX_Video_Interlace_Fieldonly,      ""},
  {"fldonly_mrf", OMX_Video_Interlace_Fieldonly_MRF,  ""},
  {"fldonly_arf", OMX_Video_Interlace_Fieldonly_ARF,  ""},
  {"fldonly_spf", OMX_Video_Interlace_Fieldonly_SPF,  ""},
  {"", 0, ""}
};


/* Entry for Set interlace coding type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencInterlacedCodingType =
{
  "interlaced_type",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputImgFmt.eInterlaceCodingType),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputImgFmt.eInterlaceCodingType),
  0,
  OMX_Video_Interlace_PICAFF,
  OMX_Video_Interlace_PICAFF,
  OMX_Video_Interlace_Fieldonly_SPF,
  &enumTblH264VencInterlacedCodingType,
  XdbgCmdVarFunc,
  "Interlace Coding Type.........................",
  "",
  "",
  ""
};


/* Enum Entry for enabling or disabling flag */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencLoopFilter[] =
{
  {"enable",                  OMX_VIDEO_AVCLoopFilterEnable,                ""},
  {"disable",                 OMX_VIDEO_AVCLoopFilterDisable,               ""},
  {"disable_slice_boundary",  OMX_VIDEO_AVCLoopFilterDisableSliceBoundary,  ""},
  {"",                        0,                                            ""}
};


/* Entry for Set loop filter */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamLoopFilter =
{
  "loop_filter",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.eLoopFilterMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.eLoopFilterMode),
  0,
  OMX_VIDEO_AVCLoopFilterEnable,
  OMX_VIDEO_AVCLoopFilterDisable,
  OMX_VIDEO_AVCLoopFilterMax,
  &enumTblH264VencLoopFilter,
  XdbgCmdVarFunc,
  "Loop Filter Mode..............................",
  "",
  "",
  ""
};


/* Enum Entry for enabling or disabling flag */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencOnOff[] =
{
  {"enable",  OMX_TRUE,   ""},
  {"disable", OMX_FALSE,  ""},
  {"",        0,          ""}
};


/* Entry for Set CABAC mode */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamCABAC =
{
  "cabac",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEntropyCodingCABAC),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEntropyCodingCABAC),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "CABAC.........................................",
  "",
  "",
  ""
};


/* Entry for Set FMO */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamFMO =
{
  "fmo",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableFMO),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableFMO),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Flexible Macroblock Ordering..................",
  "",
  "",
  ""
};


/* Entry for Set Constrained Intra Prediction */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamConstIntraPred =
{
  "const_intra_pred",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bconstIpred),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bconstIpred),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Constrained Intra Prediction..................",
  "",
  "",
  ""
};


/* Entry for Set hadamard transform */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamUseHadamard =
{
  "hadamard",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bUseHadamard),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bUseHadamard),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Hadamard Transform............................",
  "",
  "",
  ""
};


/* Entry for Set ASO flag */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamEnableASO =
{
  "aso",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableASO),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableASO),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Arbitrary Slice Ordering......................",
  "",
  "",
  ""
};


/* Entry for Set RS flag */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamEnableRS =
{
  "rs",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableRS),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableRS),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Redundant Slices..............................",
  "",
  "",
  ""
};


/* Entry for Set UEP flag */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamEnableUEP =
{
  "uep",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableUEP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bEnableUEP),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Unequal Error Protection......................",
  "",
  "",
  ""
};


/* Entry for Set Frame MBs only flag */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamFrameMBsOnly =
{
  "frm_mbs_only",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bFrameMBsOnly),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bFrameMBsOnly),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Frame MBs Only................................",
  "",
  "",
  ""
};


/* Entry for Set Frame feild MB switching flag */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamMBAFF =
{
  "mbaff",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bMBAFF),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bMBAFF),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Frame Field MB Switching......................",
  "",
  "",
  ""
};


/* Entry for Set Weighted Prediction flag for P frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamWeightedPPrediction =
{
  "weight_pred",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bWeightedPPrediction),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bWeightedPPrediction),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Weighted Prediction for P frames..............",
  "",
  "",
  ""
};


/* Entry for Set Method used for Luma motion vector for B_Skip, B_Direct_16x16 
   and B_Direct_8x8 */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamDirect8x8Inference =
{
  "dir_8x8_ref",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bDirect8x8Inference),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bDirect8x8Inference),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Luma Motion Vector Method for B Frames........",
  "",
  "",
  ""
};


/* Entry for Set Weighted Prediction flag for P frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamDirectSpatialTemporal =
{
  "dir_spatial_temporal",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bDirectSpatialTemporal),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.bDirectSpatialTemporal),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Spatial/Temporal mode for B Frame Coding......",
  "",
  "",
  ""
};


/* Entry for Set num MBs between slice headers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamSliceHeaderSpacing =
{
  "num_mb_bet_slice_header",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nSliceHeaderSpacing),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nSliceHeaderSpacing),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_SLICE_HDR_SPACING,
  OMTB_H264_VENC_MAX_NUM_MB,
  NULL,
  XdbgCmdVarFunc,
  "Num MBs Between Slice Header..................",
  "<# of MBs Between Slice Header>",
  "",
  ""
};


/* Entry for Set max P frame between each I frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamPFrames =
{
  "max_p_frms",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nPFrames),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nPFrames),
  0,
  OMTB_H264_VENC_MIN_P_FRM_INTERVAL,
  OMTB_H264_VENC_DEF_P_FRM_INTERVAL,
  OMTB_H264_VENC_MAX_P_FRM_INTERVAL,
  NULL,
  XdbgCmdVarFunc,
  "Max # of P Frames between each I Frames.......",
  "<Max # of P Frames between each I Frames>",
  "",
  ""
};


/* Entry for Set max B frame between each I frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamBFrames =
{
  "max_b_frms",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nBFrames),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nBFrames),
  0,
  OMTB_H264_VENC_MIN_B_FRM_INTERVAL,
  OMTB_H264_VENC_DEF_B_FRM_INTERVAL,
  OMTB_H264_VENC_MAX_B_FRM_INTERVAL,
  NULL,
  XdbgCmdVarFunc,
  "Max # of B Frames between each I Frames.......",
  "<Max # of B Frames between each I Frames>",
  "",
  ""
};


/* Entry for Set num refrence frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamRefFrames =
{
  "num_ref_frms",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefFrames),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefFrames),
  0,
  OMTB_H264_VENC_MIN_NUM_REF_FRM,
  OMTB_H264_VENC_DEF_NUM_REF_FRM,
  OMTB_H264_VENC_MAX_NUM_REF_FRM,
  NULL,
  XdbgCmdVarFunc,
  "# of Refrence Frames..........................",
  "<# of Refrence Frames>",
  "",
  ""
};


/* Entry for Set index in ref frm buffer of trailing frames list */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamRefIdx10ActiveMinus1 =
{
  "ref_idx_10_bwd_list",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefIdx10ActiveMinus1),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefIdx10ActiveMinus1),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_REF_IDX_10,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "Refrence Index in Bwd Frames list.............",
  "<Refrence Index in Bwd Frames list>",
  "",
  ""
};


/* Entry for Set index in ref frm buffer of forward frames list */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamRefIdx11ActiveMinus1 =
{
  "ref_idx_11_fwd_list",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefIdx11ActiveMinus1),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nRefIdx11ActiveMinus1),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_REF_IDX_11,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "Refrence Index in Fwd Frames list.............",
  "<Refrence Index in Fwd Frames list>",
  "",
  ""
};


/* Entry for Set picture types allowed in bitstream */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamAllowedPictureTypes =
{
  "allowed_pic_type",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nAllowedPictureTypes),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nAllowedPictureTypes),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_ALLOWED_PIC_TYPE,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "Picture Types Allowed In Bitstream............",
  "<Picture Types Allowed In Bitstream>",
  "",
  ""
};


/* Entry for Set weighted Biprediction modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamWeightedBipredicitonMode =
{
  "weighted_bipred_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nWeightedBipredicitonMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nWeightedBipredicitonMode),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_WEIGHTED_BIPRED_MODE,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "Weighted Biprediciton Mode....................",
  "<Weighted Biprediciton Mode>",
  "",
  ""
};


/* Entry for Set weighted Biprediction modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAVCParamCabacInitIdc =
{
  "cabac_init_idx",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nCabacInitIdc),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcParams.nCabacInitIdc),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_CABAC_INIT_IDX,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "CABAC Context Init Index......................",
  "<CABAC Context Init Index>",
  "",
  ""
};


/* Entry for Set num slice groups */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFMONumSliceGrp =
{
  "num_slice_grp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.nNumSliceGroups),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.nNumSliceGroups),
  0,
  OMTB_MIN_VAL_U8,
  OMTB_H264_VENC_DEF_NUM_SLICE_GRP,
  OMTB_MAX_VAL_U8,
  NULL,
  XdbgCmdVarFunc,
  "Num Slice Group...............................",
  "<Slice Group #>",
  "",
  ""
};


/* Entry for Set slice group map type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFMOSliceGrpMapType =
{
  "slice_grp_map",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.nSliceGroupMapType),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.nSliceGroupMapType),
  0,
  OMTB_MIN_VAL_U8,
  OMTB_H264_VENC_DEF_SLICE_GRP_MAP_TYPE,
  OMTB_MAX_VAL_U8,
  NULL,
  XdbgCmdVarFunc,
  "Slice Group map type..........................",
  "<Slice Group Map Type>",
  "",
  ""
};


/* Enum Entry for Supported slice mode type */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencSliceModeType[] =
{
  {"default",      OMX_VIDEO_SLICEMODE_AVCDefault,    ""},
  {"mbslice",      OMX_VIDEO_SLICEMODE_AVCMBSlice,    ""},
  {"byteslice",    OMX_VIDEO_SLICEMODE_AVCByteSlice,  ""},
  {"",             0,                                 ""}
};


/* Entry for Set slice mode type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFMOSliceModeType =
{
  "slice_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.eSliceMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcFMO.eSliceMode),
  0,
  OMX_VIDEO_SLICEMODE_AVCDefault,
  OMX_VIDEO_SLICEMODE_AVCDefault,
  OMX_VIDEO_SLICEMODE_AVCLevelMax,
  &enumTblH264VencSliceModeType,
  XdbgCmdVarFunc,
  "Slice Mode Type...............................",
  "",
  "",
  ""
};


/* Enum Entry for Supported rate control algorithm */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencRateCtrlAlg[] =
{
  {"disable",         OMX_Video_ControlRateDisable,             ""},
  {"variable",        OMX_Video_ControlRateVariable,            ""},
  {"constant",        OMX_Video_ControlRateConstant,            ""},
  {"var_frm_skip",    OMX_Video_ControlRateVariableSkipFrames,  ""},
  {"const_frm_skip",  OMX_Video_ControlRateConstantSkipFrames,  ""},
  {"",                0,                                        ""}
};


/* Entry for Set rate control algorithm */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencRateCtrlAlg =
{
  "rate_ctrl_alg",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,rateControlAlg),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,rateControlAlg),
  0,
  OMX_Video_ControlRateDisable,
  OMX_Video_ControlRateDisable,
  OMX_Video_ControlRateMax,
  &enumTblH264VencRateCtrlAlg,
  XdbgCmdVarFunc,
  "Rate Control Algorithm........................",
  "",
  "",
  ""
};


/* Enum Entry for Supported transform blk size */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencTransformBlkSize[] =
{
  {"4x4",       OMX_Video_Transform_Block_Size_4x4,       ""},
  {"8x8",       OMX_Video_Transform_Block_Size_8x8,       ""},
  {"adaptive",  OMX_Video_Transform_Block_Size_Adaptive,  ""},
  {"",          0,                                        ""}
};


/* Entry for Set transform blk size */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencTransformBlkSize =
{
  "dct_blk_size",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,transformBlockSize),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,transformBlockSize),
  0,
  OMX_Video_Transform_Block_Size_4x4,
  OMX_Video_Transform_Block_Size_4x4,
  OMX_Video_Transform_Block_Size_Adaptive,
  &enumTblH264VencTransformBlkSize,
  XdbgCmdVarFunc,
  "Transform Block Size..........................",
  "",
  "",
  ""
};


/* Enum Entry for Supported encoding preset settings */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencPreset[] =
{
  {"default",   OMX_Video_Enc_Default,                ""},
  {"hq",        OMX_Video_Enc_High_Quality,           ""},
  {"userdef",   OMX_Video_Enc_User_Defined,           ""},
  {"hsmq",      OMX_Video_Enc_High_Speed_Med_Quality, ""},
  {"msmq",      OMX_Video_Enc_Med_Speed_Med_Quality,  ""},
  {"mshq",      OMX_Video_Enc_Med_Speed_High_Quality, ""},
  {"hs",        OMX_Video_Enc_High_Speed,             ""},
  {"",          0,                                    ""}
};


/* Entry for Set encoding preset setting */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencPreset =
{
  "enc_preset",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,encoderPreset.eEncodingModePreset),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,encoderPreset.eEncodingModePreset),
  0,
  OMX_Video_Enc_Default,
  OMX_Video_Enc_Default,
  OMX_Video_Enc_High_Speed,
  &enumTblH264VencPreset,
  XdbgCmdVarFunc,
  "Encoder Preset................................",
  "",
  "",
  ""
};


/* Enum Entry for Supported rate control settings */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencRateCtrlPreset[] =
{
  {"default",   OMX_Video_RC_Low_Delay,     ""},
  {"hq",        OMX_Video_RC_Storage,       ""},
  {"userdef",   OMX_Video_RC_Twopass,       ""},
  {"hsmq",      OMX_Video_RC_None,          ""},
  {"msmq",      OMX_Video_RC_User_Defined,  ""},
  {"",          0,                          ""}
};


/* Entry for Set rate control setting */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencRateCtrlPreset =
{
  "rate_ctrl_preset",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,encoderPreset.eRateControlPreset),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,encoderPreset.eRateControlPreset),
  0,
  OMX_Video_RC_Low_Delay,
  OMX_Video_RC_None,
  OMX_Video_RC_User_Defined,
  &enumTblH264VencRateCtrlPreset,
  XdbgCmdVarFunc,
  "Rate Control Preset...........................",
  "",
  "",
  ""
};


/*******************************************************************************
 *                    Dynamic configuration Params
 ******************************************************************************/
/* Entry for Set Framerate */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencFramerate =
{
  "framerate",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameRate.xEncodeFramerate),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,frameRate.xEncodeFramerate),
  0,
  0,
  OMTB_H264_VENC_DEF_FRM_RATE,
  OMTB_H264_VENC_MAX_FRM_RATE,
  NULL,
  XdbgCmdVarFunc,
  "Frame rate....................................",
  "<framerate>",
  "",
  ""
};


/* Entry for Set Bitrate */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencBitrate =
{
  "bitrate",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,bitRate.nEncodeBitrate),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,bitRate.nEncodeBitrate),
  0,
  0,
  OMTB_H264_VENC_DEF_BIT_RATE,
  OMTB_H264_VENC_MAX_BIT_RATE,
  NULL,
  XdbgCmdVarFunc,
  "Bitrate.......................................",
  "<bitrate>",
  "",
  ""
};


/* Enum Entry for Supported motion vector accuracy */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencMVAccuracy[] =
{
  {"pel",         OMX_Video_MotionVectorPixel,      ""},
  {"halfpel",     OMX_Video_MotionVectorHalfPel,    ""},
  {"quarterpel",  OMX_Video_MotionVectorQuarterPel, ""},
  {"eighthpel",   OMX_Video_MotionVectorEighthPel,  ""},
  {"",            0,                                ""}
};


/* Entry for Set motion vector accuracy */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMVAccuracy =
{
  "mvaccuracy",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.eMVAccuracy),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.eMVAccuracy),
  0,
  OMX_Video_MotionVectorQuarterPel,
  OMX_Video_MotionVectorQuarterPel,
  OMX_Video_MotionVectorMax,
  &enumTblH264VencMVAccuracy,
  XdbgCmdVarFunc,
  "Motion vector accuracy........................",
  "",
  "",
  ""
};


/* Entry for Set ME horizontal Search Range - P frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEHorSearchRangeP =
{
  "mesr_horp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nHorSearchRangeP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nHorSearchRangeP),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_MESR_HORP,
  OMTB_MAX_VID_WIDTH,
  NULL,
  XdbgCmdVarFunc,
  "ME Horizontal Search Range for P Frames.......",
  "<ME Horizontal Search range for P frames>",
  "",
  ""
};


/* Entry for Set ME vertical Search Range - P frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEVerSearchRangeP =
{
  "mesr_verp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nVerSearchRangeP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nVerSearchRangeP),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_MESR_VERP,
  OMTB_MAX_VID_HEIGHT,
  NULL,
  XdbgCmdVarFunc,
  "ME Vertical Search Range for P Frames.........",
  "<ME Vertical Search range for P frames>",
  "",
  ""
};


/* Entry for Set ME horizontal Search Range - B frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEHorSearchRangeB =
{
  "mesr_horb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nHorSearchRangeB),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nHorSearchRangeB),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_MESR_HORB,
  OMTB_MAX_VID_WIDTH,
  NULL,
  XdbgCmdVarFunc,
  "ME Horizontal Search Range for B Frames.......",
  "<ME Horizontal Search range for B frames>",
  "",
  ""
};


/* Entry for Set ME vertical Search Range - B frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEVerSearchRangeB =
{
  "mesr_verb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nVerSearchRangeB),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meSearchrange.nVerSearchRangeB),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_MESR_VERB,
  OMTB_MAX_VID_HEIGHT,
  NULL,
  XdbgCmdVarFunc,
  "ME Vertical Search Range for B Frames.........",
  "<ME Vertical Search Range for B Frames>",
  "",
  ""
};


/* Entry for Set Intra Refresh VOP */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraRefreshVOP =
{
  "intra_refresh_vop",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,forceFrame.IntraRefreshVOP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,forceFrame.IntraRefreshVOP),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "Intra Refresh VOP.............................",
  "",
  "",
  ""
};


/* Entry for Set QP for I frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencQpI =
{
  "qpi",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpI),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpI),
  0,
  OMTB_H264_VENC_MIN_QP_IPB,
  OMTB_H264_VENC_DEF_QP_I,
  OMTB_H264_VENC_MAX_QP_IPB,
  NULL,
  XdbgCmdVarFunc,
  "QP for I frames...............................",
  "<QP # - For I frames>",
  "",
  ""
};


/* Entry for Set QP for P frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencQpP =
{
  "qpp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpP),
  0,
  OMTB_H264_VENC_MIN_QP_IPB,
  OMTB_H264_VENC_DEF_QP_P,
  OMTB_H264_VENC_MAX_QP_IPB,
  NULL,
  XdbgCmdVarFunc,
  "QP for P frames...............................",
  "<QP # - For P frames>",
  "",
  ""
};


/* Entry for Set QP offset for B frames */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencQpOffsetB =
{
  "qpb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpOffsetB),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,qpSettings.nQpOffsetB),
  0,
  OMTB_H264_VENC_MIN_QP_IPB,
  OMTB_H264_VENC_DEF_QP_B,
  OMTB_H264_VENC_MAX_QP_IPB,
  NULL,
  XdbgCmdVarFunc,
  "QP offset for B frames........................",
  "<QP Offset # - For B frames>",
  "",
  ""
};


/* Entry for Set IDR - Intra period */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIDRPeriod =
{
  "intra_period",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcIntraPeriod.nIDRPeriod),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,avcIntraPeriod.nIDRPeriod),
  0,
  OMTB_H264_VENC_MIN_I_FRM_INTERVAL,
  OMTB_H264_VENC_DEF_I_FRM_INTERVAL,
  OMTB_H264_VENC_MAX_I_FRM_INTERVAL,
  NULL,
  XdbgCmdVarFunc,
  "IDR period....................................",
  "<IDR Period>",
  "",
  ""
};


/* Entry for Set slice mode type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencSliceSettings =
{
  "slice_set_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,sliceSettings.eSliceMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,sliceSettings.eSliceMode),
  0,
  OMX_VIDEO_SLICEMODE_AVCDefault,
  OMX_VIDEO_SLICEMODE_AVCDefault,
  OMX_VIDEO_SLICEMODE_AVCLevelMax,
  &enumTblH264VencSliceModeType,
  XdbgCmdVarFunc,
  "Slice Mode Type...............................",
  "",
  "",
  ""
};


/* Entry for Set slice size */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencSliceSize =
{
  "slice_size",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,sliceSettings.nSlicesize),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,sliceSettings.nSlicesize),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_SLICE_SIZE,
  OMTB_H264_VENC_MAX_SLICE_SIZE,
  NULL,
  XdbgCmdVarFunc,
  "Slice Size....................................",
  "<Slice Size>",
  "",
  ""
};


/* Entry for Set Input Pixel Width */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencPixelInfoWidth =
{
  "pixel_width",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,pixelInfo.nWidth),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,pixelInfo.nWidth),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VID_WIDTH,
  OMTB_MAX_VID_WIDTH,
  NULL,
  XdbgCmdVarFunc,
  "Input Pixel Width.............................",
  "<Pixel Width>",
  "",
  ""
};


/* Entry for Set Input Pixel Height */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencPixelInfoHeight =
{
  "pixel_height",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,pixelInfo.nHeight),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,pixelInfo.nHeight),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VID_HEIGHT,
  OMTB_MAX_VID_HEIGHT,
  NULL,
  XdbgCmdVarFunc,
  "Input Pixel Height............................",
  "<Pixel Height>",
  "",
  ""
};


/*******************************************************************************
 *                    Advanced configuration Params
 ******************************************************************************/

/* Entry for Set NALU SOS */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNaluStartOfSeq =
{
  "nalu_sos",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nStartofSequence),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nStartofSequence),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "NALU Start of Sequence........................",
  "<NALU Start Of Sequence>",
  "",
  ""
};


/* Entry for Set NALU EOS */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNaluEndOfSeq =
{
  "nalu_eos",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nEndofSequence),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nEndofSequence),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "NALU End of Sequence..........................",
  "<NALU End Of Sequence>",
  "",
  ""
};


/* Entry for Set NALU IDR frame number */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNaluIDR =
{
  "nalu_idr",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nIDR),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nIDR),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "NALU IDR Frame #..............................",
  "<NALU IDR Frame #>",
  "",
  ""
};


/* Entry for Set NALU Intra picture number */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNaluIntraPicture =
{
  "nalu_intra",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nIntraPicture),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nIntraPicture),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "NALU Intra Picture #..........................",
  "<NALU Intra Picture #>",
  "",
  ""
};


/* Entry for Set NALU Non Intra picture number */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencNaluNonIntraPicture =
{
  "nalu_nonintra",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nNonIntraPicture),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,naluSettings.nNonIntraPicture),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "NALU Non Intra Picture #......................",
  "<NALU Non Intra Picture #>",
  "",
  ""
};


/* Enum Entry for Selecting Minimum Block Size */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblMEMinBlockSize[] =
{
  {"16x16", OMX_Video_Block_Size_16x16, ""},
  {"8x8",   OMX_Video_Block_Size_8x8,   ""},
  {"8x4",   OMX_Video_Block_Size_8x4,   ""},
  {"4x8",   OMX_Video_Block_Size_4x8,   ""},
  {"4x4",   OMX_Video_Block_Size_4x4,   ""},
  {"",      0,            ""}
};


/* Entry for Set ME block size - P frame */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEMinBlockSizeP =
{
  "me_min_blk_sizep",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meBlockSize.eMinBlockSizeP),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meBlockSize.eMinBlockSizeP),
  0,
  OMX_Video_Block_Size_16x16,
  OMX_Video_Block_Size_16x16,
  OMX_Video_Block_Size_4x4,
  &enumTblMEMinBlockSize,
  XdbgCmdVarFunc,
  "ME Min Block Size - P Frame...................",
  "",
  "",
  ""
};


/* Entry for Set ME block size - B frame */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencMEMinBlockSizeB =
{
  "me_min_blk_sizeb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meBlockSize.eMinBlockSizeB),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,meBlockSize.eMinBlockSizeB),
  0,
  OMX_Video_Block_Size_16x16,
  OMX_Video_Block_Size_16x16,
  OMX_Video_Block_Size_4x4,
  &enumTblMEMinBlockSize,
  XdbgCmdVarFunc,
  "ME Min Block Size - B Frame...................",
  "<ME Min Block Size - B Frame>",
  "",
  ""
};


/* Enum Entry for Selecting Intra Refresh Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblIntraRefreshMode[] =
{
  {"cyclic",    OMX_VIDEO_IntraRefreshCyclic,   ""},
  {"adaptive",  OMX_VIDEO_IntraRefreshAdaptive, ""},
  {"both",      OMX_VIDEO_IntraRefreshBoth,     ""},
  {"",          0,                              ""}
};


/* Entry for Set Intra refresh period */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraRefreshMode =
{
  "intra_refresh_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.eRefreshMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.eRefreshMode),
  0,
  OMX_VIDEO_IntraRefreshCyclic,
  OMX_VIDEO_IntraRefreshBoth,
  OMX_VIDEO_IntraRefreshMax,
  &enumTblIntraRefreshMode,
  XdbgCmdVarFunc,
  "Intra Refresh Mode............................",
  "",
  "",
  ""
};


/* Entry for Set Number of intra macroblocks to refresh in a frame when AIR 
   is enabled */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraRefreshAirMBs =
{
  "intra_refresh_airmb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nAirMBs),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nAirMBs),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_MAX_NUM_MB,
  NULL,
  XdbgCmdVarFunc,
  "# of Intra MBs to be Refreshed................",
  "<# of Intra MBs to be Refreshed>",
  "",
  ""
};


/* Entry for Set Number of times a motion marked macroblock has to be intra 
   coded */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraRefreshAirRef =
{
  "intra_refresh_airref",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nAirRef),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nAirRef),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "# of times Motion marked MB be Intra Coded....",
  "<# of times Motion marked MB to be Intra Coded>",
  "",
  ""
};


/* Entry for Set Number of consecutive macroblocks to be coded as intra when
   CIR is enabled */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraRefreshCirMBs =
{
  "intra_refresh_cirmb",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nCirMBs),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraRefresh.nCirMBs),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_MAX_NUM_MB,
  NULL,
  XdbgCmdVarFunc,
  "# of Consecutive MBs to be Intra Coded........",
  "<# of Consecutive MBs to be Intra Coded>",
  "",
  ""
};


/* Entry for Set Aspect Ratio Present Falg */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencVUIAspectRatioPresent =
{
  "vui_ar_flag",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.bAspectRatioPresent),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.bAspectRatioPresent),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "VUI Aspect Ratio Present Flag.................",
  "",
  "",
  ""
};


/* Enum Entry for Selecting Pixel Aspect Ratio */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblPixelAspectRatio[] =
{
  {"none",    OMX_Video_AR_Unspecified, ""},
  {"1x1",     OMX_Video_AR_Square,      ""},
  {"12x11",   OMX_Video_AR_12_11,       ""},
  {"10x11",   OMX_Video_AR_10_11,       ""},
  {"16x11",   OMX_Video_AR_16_11,       ""},
  {"40x33",   OMX_Video_AR_40_33,       ""},
  {"24x11",   OMX_Video_AR_24_11,       ""},
  {"20x11",   OMX_Video_AR_20_11,       ""},
  {"32x11",   OMX_Video_AR_32_11,       ""},
  {"80x33",   OMX_Video_AR_80_33,       ""},
  {"18x11",   OMX_Video_AR_18_11,       ""},
  {"15x15",   OMX_Video_AR_15_15,       ""},
  {"64x33",   OMX_Video_AR_64_33,       ""},
  {"160x99",  OMX_Video_AR_160_99,      ""},
  {"4x3",     OMX_Video_AR_4_3,         ""},
  {"3x2",     OMX_Video_AR_3_2,         ""},
  {"2x1",     OMX_Video_AR_2_1,         ""},
  {"",        0,                        ""}
};


/* Entry for Set Pixel Aspect Ratio */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencVUIPixelAspectRatio =
{
  "vui_pixel_ar",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.ePixelAspectRatio),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.ePixelAspectRatio),
  0,
  OMX_Video_AR_Unspecified,
  OMX_Video_AR_4_3,
  OMX_Video_AR_Extended,
  &enumTblPixelAspectRatio,
  XdbgCmdVarFunc,
  "VUI Pixel Aspect Ratio........................",
  "",
  "",
  ""
};


/* Entry for Set Pixel Value Range is Full Range or not */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencVUIFullRange =
{
  "vui_pixval_full_range_flag",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.bFullRange),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vuiSettings.bFullRange),
  0,
  OMX_FALSE,
  OMX_FALSE,
  OMX_TRUE,
  &enumTblH264VencOnOff,
  XdbgCmdVarFunc,
  "VUI Pixel Value Full Range Flag...............",
  "",
  "",
  ""
};


/* Entry for Set 4x4 Luma Intra Modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraPredLumaIntra4x4Enable =
{
  "luma_intra_4x4_pred_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra4x4Enable),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra4x4Enable),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_LUMA_INTRA_4x4_PRED,
  OMTB_H264_VENC_MAX_LUMA_INTRA_4x4_PRED,
  NULL,
  XdbgCmdVarFunc,
  "Luma Intra 4x4 Pred Modes Enabled.............",
  "<0x000 to 0x1FF - (Bit positions (MSB to LSB): \
HOR_UP|VER_LEFT|HOR_DOWN|VER_RIGHT|DIAG_DOWN_RIGHT|DIAG_DOWN_LEFT|DC|HOR|VER)>",
  "",
  ""
};


/* Entry for Set 8x8 Luma Intra Modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraPredLumaIntra8x8Enable =
{
  "luma_intra_8x8_pred_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra8x8Enable),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra8x8Enable),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_LUMA_INTRA_8x8_PRED,
  OMTB_H264_VENC_MAX_LUMA_INTRA_8x8_PRED,
  NULL,
  XdbgCmdVarFunc,
  "Luma Intra 8x8 Pred Modes Enabled.............",
  "<0x000 to 0x1FF - (Bit positions (MSB to LSB): \
HOR_UP|VER_LEFT|HOR_DOWN|VER_RIGHT|DIAG_DOWN_RIGHT|DIAG_DOWN_LEFT|DC|HOR|VER)>",
  "",
  ""
};


/* Entry for Set 16x16 Luma Intra Modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraPredLumaIntra16x16Enable =
{
  "luma_intra_16x16_pred_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra16x16Enable),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nLumaIntra16x16Enable),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_LUMA_INTRA_16x16_PRED,
  OMTB_H264_VENC_MAX_LUMA_INTRA_16x16_PRED,
  NULL,
  XdbgCmdVarFunc,
  "Luma Intra 16x16 Pred Modes Enabled...........",
  "<0x00 to 0x0F - (Bit positions (MSB to LSB): \
PLANE|DC|HOR|VER>",
  "",
  ""
};


/* Entry for Set 8x8 Chroma Intra Modes */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraPredChromaIntra8x8Enable =
{
  "chroma_intra_8x8_pred_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nChromaIntra8x8Enable),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.nChromaIntra8x8Enable),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_H264_VENC_DEF_CHROMA_INTRA_8x8_PRED,
  OMTB_H264_VENC_MAX_CHROMA_INTRA_8x8_PRED,
  NULL,
  XdbgCmdVarFunc,
  "Chroma Intra 8x8 Pred Modes Enabled...........",
  "<0x00 to 0x0F - (Bit positions (MSB to LSB): \
PLANE|DC|HOR|VER>",
  "",
  ""
};


/* Enum Entry for Selecting chroma component */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblIntraPredChromaComponentEnable[] =
{
  {"cr_only",     OMX_Video_Chroma_Component_Cr_Only,     ""},
  {"cb_cr_both",  OMX_Video_Chroma_Component_Cb_Cr_Both,  ""},
  {"",            0,                                      ""}
};


/* Entry for Set Pixel Aspect Ratio */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencIntraPredChromaComponentEnable =
{
  "chroma_intra_pred_comp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.eChromaComponentEnable),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,intraPred.eChromaComponentEnable),
  0,
  OMX_Video_Chroma_Component_Cr_Only,
  OMX_Video_Chroma_Component_Cr_Only,
  OMX_Video_Chroma_Component_Cb_Cr_Both,
  &enumTblIntraPredChromaComponentEnable,
  XdbgCmdVarFunc,
  "Chroma Intra Component........................",
  "",
  "",
  ""
};


/* Entry for Set FMO Num Slice Groups */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMONumSliceGroups =
{
  "fmo_num_slice_grp",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nNumSliceGroups),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nNumSliceGroups),
  0,
  OMTB_MIN_VAL_U8,
  OMTB_MIN_VAL_U8,
  OMTB_MAX_VAL_U8,
  NULL,
  XdbgCmdVarFunc,
  "FMO Num Slice Groups..........................",
  "<FMO Num Slice Groups>",
  "",
  ""
};


/* Entry for Set FMO Slice Group Map Type */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGroupMapType =
{
  "fmo_slice_grp_map_type",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupMapType),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupMapType),
  0,
  OMTB_MIN_VAL_U8,
  OMTB_MIN_VAL_U8,
  OMTB_MAX_VAL_U8,
  NULL,
  XdbgCmdVarFunc,
  "FMO Slice Groups Map Type.....................",
  "<FMO Slice Groups Map Type>",
  "",
  ""
};


/* Enum Entry for Selecting Slice Group Change Direction */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblSliceGrpChangeDir[] =
{
  {"raster",        OMX_Video_Raster_Scan,          ""},
  {"clkwise",       OMX_Video_Clockwise,            ""},
  {"right",         OMX_Video_Right,                ""},
  {"rev_raster",    OMX_Video_Reverse_Raster_Scan,  ""},
  {"anti_clkwise",  OMX_Video_Counter_Clockwise,    ""},
  {"left",          OMX_Video_Left,                 ""},
  {"",              0,                              ""}
};


/* Entry for Set FMO Slice Group Change Direction */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGrpChangeDir =
{
  "fmo_slice_grp_chng_dir",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.eSliceGrpChangeDir),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.eSliceGrpChangeDir),
  0,
  OMX_Video_Raster_Scan,
  OMX_Video_Raster_Scan,
  OMX_Video_Left,
  &enumTblSliceGrpChangeDir,
  XdbgCmdVarFunc,
  "FMO Slice Groups Change Direction.............",
  "",
  "",
  ""
};


/* Entry for Set FMO Slice Group Change Rate */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGroupChangeRate =
{
  "fmo_slice_grp_chng_rate",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupChangeRate),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupChangeRate),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "FMO Slice Groups Change Rate..................",
  "<FMO Slice Groups Change Rate>",
  "",
  ""
};


/* Entry for Set FMO Slice Group Change Cycle */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGroupChangeCycle =
{
  "fmo_slice_grp_chng_cycle",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupChangeCycle),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupChangeCycle),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "FMO Slice Groups Change Cycle.................",
  "<FMO Slice Groups Change Cycle>",
  "",
  ""
};


/* Entry for Set FMO Slice Group Param 0 */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGroupParams0 =
{
  "fmo_slice_grp_param0",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupParams[0]),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupParams[0]),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "FMO Slice Groups Param 0......................",
  "<FMO Slice Groups Param 0>",
  "",
  ""
};


/* Entry for Set FMO Slice Group Param 1 */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencAdvancedFMOSliceGroupParams1 =
{
  "fmo_slice_grp_param1",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupParams[1]),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,advancedFMO.nSliceGroupParams[1]),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "FMO Slice Groups Param 1......................",
  "<FMO Slice Groups Param 1>",
  "",
  ""
};


/* Enum Entry for Selecting Video Data Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblVidDataMode[] =
{
  {"fixed_length",  OMX_Video_FixedLength,  ""},
  {"slice",         OMX_Video_SliceMode,    ""},
  {"num_mb_rows",   OMX_Video_NumMBRows,    ""},
  {"frame",         OMX_Video_EntireFrame,  ""},
  {"",              0,                      ""}
};


/* Entry for Set Vid Num Data Units */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencVidDataMode =
{
  "vid_data_mode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM         |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vidDataMode.eDataMode),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vidDataMode.eDataMode),
  0,
  OMX_Video_FixedLength,
  OMX_Video_EntireFrame,
  OMX_Video_EntireFrame,
  &enumTblVidDataMode,
  XdbgCmdVarFunc,
  "Video Data Mode...............................",
  "",
  "",
  ""
};


/* Entry for Set Vid Num Data Units */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryH264VencVidDataModeNumDataUnits =
{
  "vid_num_data_unit",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vidDataMode.nNumDataUnits),
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_H264_VENC_CONFIG_PARAMS,vidDataMode.nNumDataUnits),
  0,
  OMTB_MIN_VAL_U32,
  OMTB_MIN_VAL_U32,
  OMTB_MAX_VAL_U32,
  NULL,
  XdbgCmdVarFunc,
  "Video Num Data Units..........................",
  "<Video Num Data Units>",
  "",
  ""
};


/* h264venc config command table Entry */

MX_DBG_CMD_ENTRY_T* cmdListH264VencConfig[] =
{
  &cmdEntryH264VencCompName,
  &cmdEntryH264VencInFile,
  &cmdEntryH264VencOutFile,
  &cmdEntryH264VencWidth,
  &cmdEntryH264VencHeight,
  &cmdEntryH264VencInputChromaFormat,
  &cmdEntryH264VencProfile,
  &cmdEntryH264VencLevel,
  &cmdEntryH264VencInFrmContentType,
  &cmdEntryH264VencInterlacedCodingType,
  &cmdEntryH264VencAVCParamLoopFilter,
  &cmdEntryH264VencAVCParamCABAC,
  &cmdEntryH264VencAVCParamFMO,
  &cmdEntryH264VencAVCParamConstIntraPred,
  &cmdEntryH264VencAVCParamUseHadamard,
  &cmdEntryH264VencAVCParamEnableASO,
  &cmdEntryH264VencAVCParamEnableRS,
  &cmdEntryH264VencAVCParamEnableUEP,
  &cmdEntryH264VencAVCParamFrameMBsOnly,
  &cmdEntryH264VencAVCParamMBAFF,
  &cmdEntryH264VencAVCParamWeightedPPrediction,
  &cmdEntryH264VencAVCParamDirect8x8Inference,
  &cmdEntryH264VencAVCParamDirectSpatialTemporal,
  &cmdEntryH264VencAVCParamSliceHeaderSpacing,
  &cmdEntryH264VencAVCParamPFrames,
  &cmdEntryH264VencAVCParamBFrames,
  &cmdEntryH264VencAVCParamRefFrames,
  &cmdEntryH264VencAVCParamRefIdx10ActiveMinus1,
  &cmdEntryH264VencAVCParamRefIdx11ActiveMinus1,
  &cmdEntryH264VencAVCParamAllowedPictureTypes,
  &cmdEntryH264VencAVCParamWeightedBipredicitonMode,
  &cmdEntryH264VencAVCParamCabacInitIdc,
  &cmdEntryH264VencFMONumSliceGrp,
  &cmdEntryH264VencFMOSliceGrpMapType,
  &cmdEntryH264VencFMOSliceModeType,
  &cmdEntryH264VencRateCtrlAlg,
  &cmdEntryH264VencTransformBlkSize,
  &cmdEntryH264VencPreset,
  &cmdEntryH264VencRateCtrlPreset,
  &cmdEntryH264VencBitStrmFmt,
  &cmdEntryH264VencFramerate,
  &cmdEntryH264VencBitrate,
  &cmdEntryH264VencMVAccuracy,
  &cmdEntryH264VencMEHorSearchRangeP,
  &cmdEntryH264VencMEVerSearchRangeP,
  &cmdEntryH264VencMEHorSearchRangeB,
  &cmdEntryH264VencMEVerSearchRangeB,
  &cmdEntryH264VencIntraRefreshVOP,
  &cmdEntryH264VencQpI,
  &cmdEntryH264VencQpP,
  &cmdEntryH264VencQpOffsetB,
  &cmdEntryH264VencIDRPeriod,
  &cmdEntryH264VencSliceSettings,
  &cmdEntryH264VencSliceSize,
  &cmdEntryH264VencPixelInfoWidth,
  &cmdEntryH264VencPixelInfoHeight,
  &cmdEntryH264VencNaluStartOfSeq,
  &cmdEntryH264VencNaluEndOfSeq,
  &cmdEntryH264VencNaluIDR,
  &cmdEntryH264VencNaluIntraPicture,
  &cmdEntryH264VencNaluNonIntraPicture,
  &cmdEntryH264VencMEMinBlockSizeP,
  &cmdEntryH264VencMEMinBlockSizeB,
  &cmdEntryH264VencIntraRefreshMode,
  &cmdEntryH264VencIntraRefreshAirMBs,
  &cmdEntryH264VencIntraRefreshAirRef,
  &cmdEntryH264VencIntraRefreshCirMBs,
  &cmdEntryH264VencVUIAspectRatioPresent,
  &cmdEntryH264VencVUIPixelAspectRatio,
  &cmdEntryH264VencVUIFullRange,
  &cmdEntryH264VencIntraPredLumaIntra4x4Enable,
  &cmdEntryH264VencIntraPredLumaIntra8x8Enable,
  &cmdEntryH264VencIntraPredLumaIntra16x16Enable,
  &cmdEntryH264VencIntraPredChromaIntra8x8Enable,
  &cmdEntryH264VencIntraPredChromaComponentEnable,
  &cmdEntryH264VencAdvancedFMONumSliceGroups,
  &cmdEntryH264VencAdvancedFMOSliceGroupMapType,
  &cmdEntryH264VencAdvancedFMOSliceGrpChangeDir,
  &cmdEntryH264VencAdvancedFMOSliceGroupChangeRate,
  &cmdEntryH264VencAdvancedFMOSliceGroupChangeCycle,
  &cmdEntryH264VencAdvancedFMOSliceGroupParams0,
  &cmdEntryH264VencAdvancedFMOSliceGroupParams1,
  &cmdEntryH264VencVidDataMode,
  &cmdEntryH264VencVidDataModeNumDataUnits,
  
  &cmdEntryH264VencOutputMode,
  &cmdEntryH264VencFlagUseAllocInBuffer,
  &cmdEntryH264VencFlagUseAllocOutBuffer,
  &cmdEntryH264VencNumInBuf,
  &cmdEntryH264VencNumOutBuf,
  NULL
};


/* h264venc config command table */

MX_DBG_CMD_TABLE_T cmdTblH264VencConfig =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListH264VencConfig)/sizeof(cmdListH264VencConfig[0]),
  cmdListH264VencConfig
};


/* H264venc config command entry */

MX_DBG_CMD_ENTRY_T cmdEntryH264VencConfig =
{
  OMTB_H264_VENC_COMP_REF_STR,
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_TEMPLATE_INFO,h264VencTemplate),
  0,
  0,
  0,
  0,
  0,
  &cmdTblH264VencConfig,
  XdbgCmdTblFunc,
  "\nH264 Video Encoder Config Parameters:\n",
  "",
  "",
  ""
};


/*----------------------------- end of file ---------------------------------*/
