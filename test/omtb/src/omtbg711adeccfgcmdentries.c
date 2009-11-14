/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbG711AdecCfgCmdEntries.c
 *
 *  @brief    This file contains command entries of G711 Adec configuration
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
 *  OMTB component configuration parameter command entries - g711adec
 ******************************************************************************/
 
/*******************************************************************************
 *      G711 decoder config params
 ******************************************************************************/

/* Entry for Set OMX Component Name */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecCompName =
{
  "compname",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,compName),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,compName),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecInFile =
{
  "infile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecOutFile =
{
  "outfile",
  MX_DBG_CMD_TYPE_VAR_STR_BUF,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,outputFile),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,outputFile),
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


/* Enum Entry for Selecting Data O/P Mode */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblDataOutputMode[] =
{
  {"file",  OMTB_FILE,    ""},
  {"driver",OMTB_DRIVER,  ""},
  {"none",  OMTB_NONE,    ""},
  {"",      0,            ""}
};


/* Entry for Setting Data Output Mode */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecOutputMode =
{
  "data_opmode",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,dataOutPutMode),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,dataOutPutMode),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecFlagUseAllocInBuffer =
{
  "inbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,flagUseInBuffer),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,flagUseInBuffer),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecFlagUseAllocOutBuffer =
{
  "outbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_ENUM          |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,flagUseOutBuffer),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,flagUseOutBuffer),
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

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecNumInBuf =
{
  "numinbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,numInBuf),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,numInBuf),
  0,
  0,
  OMTB_G711_ADEC_MAX_NUM_IN_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of i/p buffer..........................",
  "<# of i/p buffers>",
  "",
  ""
};


/*Entry for Set number of o/p buffers */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryG711AdecNumOutBuf =
{
  "numoutbuf",
  MX_DBG_CMD_TYPE_VAR_UINT32,
  MX_DBG_CMD_OPT_INST_CUR     |
  MX_DBG_CMD_OPT_DEC      |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,numOutBuf),
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,inputFile) -
  FIELDOFFSET(OMTB_G711_ADEC_CONFIG_PARAMS,numOutBuf),
  0,
  0,
  OMTB_G711_ADEC_MAX_NUM_OUT_BUFS,
  0,
  NULL,
  XdbgCmdVarFunc,
  "Number of o/p buffer..........................",
  "<# of output buffers>",
  "",
  ""
};


/* G711adec config command table Entry */

MX_DBG_CMD_ENTRY_T* cmdListG711AdecConfig[] =
{
  &cmdEntryG711AdecCompName,
  &cmdEntryG711AdecInFile,
  &cmdEntryG711AdecOutFile,
  &cmdEntryG711AdecOutputMode,
  &cmdEntryG711AdecFlagUseAllocInBuffer,
  &cmdEntryG711AdecFlagUseAllocOutBuffer,
  &cmdEntryG711AdecNumInBuf,
  &cmdEntryG711AdecNumOutBuf,
  NULL
};


/* G711adec config command table*/

MX_DBG_CMD_TABLE_T cmdTblG711AdecConfig =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListG711AdecConfig)/sizeof(cmdListG711AdecConfig[0]),
  cmdListG711AdecConfig
};


/* G711adec config command entry */

MX_DBG_CMD_ENTRY_T cmdEntryG711AdecConfig =
{
  OMTB_G711_ADEC_COMP_REF_STR,
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  FIELDOFFSET(OMTB_TEMPLATE_INFO,g711AdecTemplate),
  0,
  0,
  0,
  0,
  0,
  &cmdTblG711AdecConfig,
  XdbgCmdTblFunc,
  "\nG711 Audio Decoder Config Parameters:\n",
  "",
  "",
  ""
};


/*----------------------------- end of file ---------------------------------*/
