/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbCmdEntries.c
 *
 *  @brief    This file contains command entries for OMX func/api and OMTB
 *            utility commands.
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
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "mxDbg.h"
#include "omtbCommon.h"
#include "omtbCmd.h"

/******************************************************************************\
 *      Local Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Extern Object Definitions 
\******************************************************************************/

extern MX_DBG_CMD_TABLE_T funcCmdTbl;             // Command table for FUNC mode
extern MX_DBG_CMD_TABLE_T cmdTblOmxApi;           // Command table for API mode
#ifdef OMTB_FF_VD_H264
extern MX_DBG_CMD_ENTRY_T cmdEntryH264VdecConfig; // H264Vdec cfg params
#endif
#ifdef OMTB_FF_AD_G711
extern MX_DBG_CMD_ENTRY_T cmdEntryG711AdecConfig; // G711Adec cfg params
#endif
#ifdef OMTB_FF_VE_H264
extern MX_DBG_CMD_ENTRY_T cmdEntryH264VencConfig; // H264Venc cfg params
#endif
#ifdef OMTB_FF_VPS_FCC
extern MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfccConfig; // VPSSVfcc cfg params
#endif
#ifdef OMTB_FF_VPS_FDC
extern MX_DBG_CMD_ENTRY_T cmdEntryVPSSVfdcConfig; // VPSSVfdc cfg params
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
extern MX_DBG_CMD_ENTRY_T cmdEntryDisplayConfig;  // DSS cfg params
#endif



/******************************************************************************\
 *      Globals 
\******************************************************************************/

MX_DBG_CMD_TABLE_T setpCmdTbl;                    /* command table for setp */


/******************************************************************************\
 *      Public Function Prototype
\******************************************************************************/


/******************************************************************************\
 *      Static Globals
\******************************************************************************/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbSetp;/* Command entry for setp */
PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbFunc;/* command entry for func command*/
MX_DBG_CMD_TABLE_T cmdTblOmx;       /* command table for omx command */
MX_DBG_CMD_ENTRY_T cmdEntryOmx;     /* command entry for omx command */


/******************************************************************************\
 *      Private Function Declarations 
\******************************************************************************/


/*******************************************************************************
 *  Second Level Command Entries/Tables - omx
 ******************************************************************************/
 
/* setp command list entry */

PRIVATE MX_DBG_CMD_ENTRY_T* setpCmdList[] =
{
#ifdef OMTB_FF_VD_H264
  &cmdEntryH264VdecConfig,
#endif
#ifdef OMTB_FF_AD_G711
  &cmdEntryG711AdecConfig,
#endif
#ifdef OMTB_FF_VE_H264
  &cmdEntryH264VencConfig,
#endif
#ifdef OMTB_FF_VPS_FCC
  &cmdEntryVPSSVfccConfig,
#endif
#ifdef OMTB_FF_VPS_FDC
  &cmdEntryVPSSVfdcConfig,
#endif
#ifdef OMTB_FF_DRIVER_DISPLAY
  &cmdEntryDisplayConfig,
  NULL
#endif
};


/* setp command table */

MX_DBG_CMD_TABLE_T setpCmdTbl=
{
  MX_DBG_ACCESS_ALL,
  sizeof(setpCmdList) / sizeof(setpCmdList[0]),
  setpCmdList
};


/*******************************************************************************
 *  OMTB OMX API/Func Mode Commands
 ******************************************************************************/

/* Dummy entry to isolate OMTB OMX commands */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbOmxCmd = 
{
  "",
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
  NULL,
  "",
  "OMTB Commands:",
  "",
  ""
};

 
/* func command entry */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbFunc = 
{
  "func",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  &funcCmdTbl,
  OMTB_CmdApi,
  "",
  "",
  "",
  ""
};


/* comp command entry */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbApi = 
{
  "api",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR |
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  &cmdTblOmxApi,
  OMTB_CmdApi,
  "",
  "",
  "",
  ""
};


/*******************************************************************************
 *  Utility Commands
 ******************************************************************************/

/* Dummy entry to isolate utility commands */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbUtilityCmd = 
{
  "",
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
  NULL,
  "",
  "Utility Commands:",
  "",
  ""
};


/* Run OMTB using DVS script */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbRunScript =
{
  "-s",
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
  OMTB_CmdRunScript,
  "",
  "<script_file_name>",
  "",
  ""
};


/* setp command entry */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbSetp = 
{
  "setp",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR, 
  0,
  0,
  0,
  0,
  0,
  0,
  &setpCmdTbl,
  OMTB_CmdSetTemplate,
  "",
  "<template #>",
  "",
  ""
};


/* getp command entry */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbGetp = 
{
  "getp",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR,
  0,
  0,
  0,
  0,
  0,
  0,
  &setpCmdTbl,
  OMTB_CmdShowTemplate,
  "",
  "<template #>",
  "",
  ""
};


/* Get OMTB version information*/

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbReleaseInfo =
{
  "omtb_rel_info",
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
  OMTB_CmdReleaseInfo,
  "",
  "",
  "",
  ""
};


/* Remove OMTB configuration template from memory */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbAddCfg =
{
  "add",
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
  OMTB_CmdAddCfg,
  "",
  "<template #>",
  "",
  ""
};


/* Remove OMTB configuration template from memory */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbRemoveCfg =
{
  "remove",
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
  OMTB_CmdRemoveCfg,
  "",
  "<template #>",
  "",
  ""
};


/* Load OMTB configuration */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbLoadCfg =
{
  "load",
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
  OMTB_CmdLoadCfg,
  "",
  "<template #> <cfg_file_name>",
  "",
  ""
};


/* Store OMTB configuration */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbStoreCfg =
{
  "store",
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
  OMTB_CmdStoreCfg,
  "",
  "<template #> <cfg_file_name>",
  "",
  ""
};


/* reset */
PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryOmtbResetProfile = 
{
  "reset",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  &setpCmdTbl,
  OMTB_CmdResetTemplate,
  "",
  "<template #>",
  "",
  ""
};


/* cpu load command entry */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryToggleArmLoadDisp =
{
  "tog_arm_load",
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
  OMTB_CmdShowArmLoad,
  "",
  "",
  "",
  ""
};


/* OMX Component Debug log enable/disable */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntryToggleCompDbgLog =
{
  "comp_dbg_log",
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
  OMTB_CmdToggleCompDbgLog,
  "",
  "",
  "",
  ""
};


/* OMTB Debug level setting */

PRIVATE MX_DBG_CMD_ENTRY_T cmdEntrySetOmtbDbgLevel =
{
  "omtb_dbg_lvl",
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
  OMTB_CmdSetDbgLogLevel,
  "",
  "<debug level>\n\
where <debug level> can be, 0x00 - Disable Logs;\n\
                            0x02 - Simple Logs;\n\
                            0x04 - Default Logs;\n\
                            0x08 - Function Names;\n\
                            0x10 - Debug Logs;\n\
                            0x80 - Compulsary Logs/Error Logs",
  "",
  ""
};


/*******************************************************************************
 *  First Level Command Entry/Table - omx
 ******************************************************************************/

/* omx command list */
PRIVATE MX_DBG_CMD_ENTRY_T* cmdListOmx[] =
{
  &cmdEntryOmtbOmxCmd,
  &cmdEntryOmtbFunc,
  &cmdEntryOmtbApi,
  &cmdEntryOmtbUtilityCmd,
  &cmdEntryOmtbRunScript,
  &cmdEntryOmtbSetp,
  &cmdEntryOmtbGetp,
  &cmdEntryOmtbReleaseInfo,
  &cmdEntryOmtbAddCfg,
  &cmdEntryOmtbRemoveCfg,
  &cmdEntryOmtbLoadCfg,
  &cmdEntryOmtbStoreCfg,
  &cmdEntryOmtbResetProfile,
  &cmdEntryToggleArmLoadDisp,
  &cmdEntryToggleCompDbgLog,
  &cmdEntrySetOmtbDbgLevel,
  NULL
};


/* omx command table */
MX_DBG_CMD_TABLE_T cmdTblOmx =
{
  MX_DBG_ACCESS_ALL,
  sizeof(cmdListOmx) / sizeof(cmdListOmx[0]),
  cmdListOmx 
};


/* omx command entry */
MX_DBG_CMD_ENTRY_T cmdEntryOmx = 
{
  "omx",
  MX_DBG_CMD_TYPE_CMD_TABLE,
  MX_DBG_CMD_OPT_INST_CUR     | 
  MX_DBG_CMD_OPT_VALID_PTR,
  0,
  0,
  0,
  0,
  0,
  0,
  &cmdTblOmx,
  NULL,
  "",
  "",
  "",
  ""
};


/*----------------------------- end of file ---------------------------------*/
