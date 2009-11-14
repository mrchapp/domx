/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbCmd.h
 *
 *  @brief    This file contains omtb prototypes/declarations for cfg templates
 *            and OMTB utility commands.
 *
 * <B> History: </B>
 *
 *    # September-21-2009   AVME System Test    Initial Version
 */

#ifndef __OMTB_CMD_H__
#define __OMTB_CMD_H__


/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#include <ctype.h>

/* OMX standard header files */
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "mxDbg.h"
#include "omtbCommon.h"


/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Macros
\******************************************************************************/


/******************************************************************************\
 *      Globals
\******************************************************************************/


/******************************************************************************\
 *      External Object Definitions
\******************************************************************************/


/******************************************************************************\
 *      Public Function Prototypes
\******************************************************************************/

/******************************************************************************\
 *      OMTB_CmdParseOmx Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function compares the first command if first command matches
 *            with the string mentioned parses rest of commands else returns 
 *            appropriate error code
 *
 *  @param    argc: Number of arguments received from command line
 *            argv: Array of pointers to commands 
 *                  entered through the command line
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

void OMTB_CmdParseOmx(
        UINT32  argc,
        CHAR    *argv[]
);


/******************************************************************************\
 *      OMTB_OmxReturnStatus Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function prints the proper error code for OMX API return
 *            values
 *
 *  @param    Retruns status from OMX API's
 *
 *  @return   None
 *
 */

void OMTB_OmxReturnStatus(OMX_ERRORTYPE  status);


/******************************************************************************\
 *      OMTB_DimtSkipWhiteSpace Function Prototype
\******************************************************************************/
/**
 *
 *  @brief   This Function Skip White Space from command line  
 *
 *  @param   Pointer to MX_DBG_CMD_CONTEXT_T structure
 *           Pointer to MX_DBG_CMD_Entry_T structure
 *           Character Double Pointer containing the command
 *
 *  @return  ERR_NOERR           successful,
 *           ERR_END_OF_CMD      help was requested,
 *           ERR_INVNAME         otherwise.
 */

MX_Result OMTB_DimtSkipWhiteSpace(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdApi Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes OMTB_DimtSkipWhiteSpace function after
 *            omx command is invoked and then parses the command.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdApi(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdReleaseInfo Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function displays OMTB application version number.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdReleaseInfo(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdStoreCfg Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function stores OMTB configuration to specified file.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdStoreCfg(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdLoadCfg Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function loads OMTB configuration from specified file.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdLoadCfg(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdAddCfg Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function allocates specified OMTB configuration template 
 *            memory.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdAddCfg(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdRemoveCfg Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function releases specified OMTB configuration template 
 *            memory.
 *
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdRemoveCfg(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdRunScript Function Prototype
\******************************************************************************/
/**
 *
 *  @brief   
 *            This Function runs OMTB using DVS script.
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *
 */

MX_Result
OMTB_CmdRunScript(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdShowArmLoad Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is used to ENABLE/DISABLE arm load measuring thread
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdShowArmLoad(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdToggleCompDbgLog Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is used to ENABLE/DISABLE component debug logs.
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdToggleCompDbgLog(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdSetDbgLogLevel Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is used to set OMTB debug log level for display.
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdSetDbgLogLevel(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdSetTemplate Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is a wrapper on top of OMTB_CmdTemplate and is 
 *            called if user wants to set parameter in the template
 *
 *  @param    pContext   : Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            pCmdEntry :Pointer to MX_DBG_CMD_Entry_T structure
 *            ppCmdLine :Character Double Pointer containing the command
 *
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdSetTemplate(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdShowTemplate Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is a wrapper on top of OMTB_CmdTemplate and is 
 *            called if user wants to display the parameter present in the
 *            template
 *
 *  @param    pContext   : Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            pCmdEntry :Pointer to MX_DBG_CMD_Entry_T structure
 *            ppCmdLine :Character Double Pointer containing the command
 *
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdShowTemplate(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


/******************************************************************************\
 *      OMTB_CmdResetTemplate Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function is a wrapper on top of OMTB_CmdTemplate and is 
 *            called if user wants to reset the parameter present in the
 *            template
 *
 *  @param    pContext   : Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            pCmdEntry :Pointer to MX_DBG_CMD_Entry_T structure
 *            ppCmdLine :Character Double Pointer containing the command
 *
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

MX_Result
OMTB_CmdResetTemplate(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*----------------------------- end of file ---------------------------------*/
