/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbCmd.c
 *
 *  @brief    This file parses omx api, omx setp, omx getp commands and takes
 *            appropriate action based on it. Mainly resposible for template
 *            handling commands.
 *
 * <B> History: </B>
 *
 *    # June-1-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#include <ctype.h>

#ifdef WTSD
#include <string.h>
#endif

/* OMX standard header files */
#include "OMX_Component.h"
#include "OMX_Types.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbDebug.h"
#include "mxDbg.h"
#include "omtbCommon.h"
#include "omtbCmd.h"


/******************************************************************************\
 *      Local Defines
\******************************************************************************/

#define IS_LINE_END(c) (((c) == '\0') || ((c) == '\r') || ((c) == '\n'))


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/


/******************************************************************************\
 *      Extern Object Definitions 
\******************************************************************************/

extern MX_DBG_CMD_MODE_T  mx_dbg_cmd_mode;  // parser debug mode

extern OMTB_TEMPLATE_INFO *omtbCfgTemplate[OMTB_MAX_INS];  // Template params

extern unsigned int armCpuLoadDispFlag; // Arm CPU load display On/Off flag

extern int globDebugDisable;            // Flag to toggle component log display

extern OMTB_DATA_THREAD_ARG funcThreadArg[OMTB_MAX_INS]; // Func/API thread args

extern MX_DBG_CMD_ENTRY_T cmdEntryOmx;     // command entry for omx command

extern MX_DBG_CMD_TABLE_T cmdTblOmx;       // command table for omx command

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


/******************************************************************************\
 *      OMTB_OmxReturnStatus Function Definition
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

void OMTB_OmxReturnStatus(
  OMX_ERRORTYPE  status
)
{
  switch(status)
  {
    case OMX_ErrorNone:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorNone\n");
        break;
    case OMX_ErrorInsufficientResources:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorInsufficientResources\n");
        break;
    case OMX_ErrorUndefined:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorUndefined\n");
        break;
    case OMX_ErrorInvalidComponentName:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorInvalidComponentName\n");
        break;
    case OMX_ErrorComponentNotFound:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorComponentNotFound\n");
        break;
    case OMX_ErrorInvalidComponent:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorInvalidComponent\n");
        break;
    case OMX_ErrorBadParameter:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorBadParameter\n");
        break;
    case OMX_ErrorNotImplemented:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorNotImplemented\n");
        break;
    case OMX_ErrorUnderflow:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorUnderflow\n");
        break;
    case OMX_ErrorOverflow:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorOverflow\n");
        break;
    case OMX_ErrorHardware:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorHardware\n");
        break;
    case OMX_ErrorInvalidState:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorInvalidState\n");
        break;
    case OMX_ErrorStreamCorrupt:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorStreamCorrupt\n");
        break;
    case OMX_ErrorPortsNotCompatible:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorPortsNotCompatible\n");
        break;
    case OMX_ErrorResourcesLost:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorResourcesLost\n");
        break;
    case OMX_ErrorNoMore:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorNoMore\n");
        break;
    case OMX_ErrorVersionMismatch:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorVersionMismatch\n");
        break;
    case OMX_ErrorNotReady:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorNotReady\n");
        break;
    case OMX_ErrorTimeout:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorTimeout\n");
        break;
    case OMX_ErrorSameState:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorSameState\n");
        break;
    case OMX_ErrorResourcesPreempted:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorResourcesPreempted\n");
        break;
    case OMX_ErrorPortUnresponsiveDuringAllocation:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorPortUnresponsiveDuringAllocation\n");
        break;
    case OMX_ErrorPortUnresponsiveDuringDeallocation:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorPortUnresponsiveDuringDeallocation\n");
        break;   /* No more indicies can be enumerated */
    case OMX_ErrorPortUnresponsiveDuringStop:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorPortUnresponsiveDuringStop\n");
        break;
    case OMX_ErrorIncorrectStateTransition:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorIncorrectStateTransition\n");
        break;
    case OMX_ErrorIncorrectStateOperation:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorIncorrectStateOperation\n");
        break;
    case OMX_ErrorUnsupportedSetting:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorUnsupportedSetting\n");
        break;
    case OMX_ErrorUnsupportedIndex:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorUnsupportedIndex\n");
        break;
    case OMX_ErrorBadPortIndex:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorBadPortIndex\n");
        break;
    case OMX_ErrorPortUnpopulated:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorPortUnpopulated\n");
        break;
    case OMX_ErrorComponentSuspended:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorComponentSuspended\n");
        break;
    case OMX_ErrorDynamicResourcesUnavailable:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorDynamicResourcesUnavailable\n");
        break;
    case OMX_ErrorMbErrorsInFrame:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorMbErrorsInFrame\n");
        break;
    case OMX_ErrorFormatNotDetected:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorFormatNotDetected\n");
        break;
    case OMX_ErrorContentPipeOpenFailed:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorContentPipeOpenFailed\n");
        break;
    case OMX_ErrorContentPipeCreationFailed:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorContentPipeCreationFailed\n");
        break;
    case OMX_ErrorSeperateTablesUsed:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorSeperateTablesUsed\n");
        break;
    case OMX_ErrorTunnelingUnsupported:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorTunnelingUnsupported\n");
        break;
    case OMX_ErrorMax:
        PRINT( PRINT_ERR_MSG, "OMX_ErrorMax\n");
        break;
    default:
        PRINT( PRINT_ERR_MSG, "Unknown Error\n");
        break;
  } // switch(status)
}


/******************************************************************************\
 *  OMTB_DimtSkipWhiteSpace Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function Skip White Space from command line  
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
OMTB_DimtSkipWhiteSpace(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  char*   p_char = NULL;

  p_char = (char*) *ppCmdLine;

  /* Skip leading white space. Stop if comment or end of line. */
  while (isspace((INT16)*p_char))
  {
    p_char++;
  }

  if (IS_LINE_END(*p_char))
  {
    return ((MX_Result)ERR_INVNAME);
  }
  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_ShowCmdHelp Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function show the command help it is called from 
 *            OMTB_CmdParseOmx if the first command entered doesn't 
 *            match with the string expected
 *
 *  @param    pContext: Pointer to MX_DBG_CMD_CONTEXT_T
 *
 *  @return   None
 *
 */

PRIVATE void 
OMTB_ShowCmdHelp(
  MX_DBG_CMD_CONTEXT_T*   pContext
)
{
  MX_DBG_CMD_TABLE_T*     pCmdTbl = NULL;
  MX_DBG_CMD_ENTRY_T*     pTempCmdEntry = NULL;
  UINT8                   i = 0;
  UINT8                   j = 0;
  UINT16                  numColumns = 0;
  char                    line[OMTB_MAX_LEN] = {"\0"};
  
  printf("\nomx");
  
  pCmdTbl = (MX_DBG_CMD_TABLE_T*)pContext->p_cur_cmd_tbl;

  numColumns = 3u;
  memset(line, 0, sizeof(line));

  while (NULL != (pTempCmdEntry = pCmdTbl->p_entries[i++]))
  {
    /* Display '-' below each category defined by dummy entry having no 
       command */
    if (strcmp(pTempCmdEntry->name, "") == 0)
    {
      int len = 0;
      printf("%s\n\n", line);
      memset(line, 0, sizeof(line));
      len = printf("\n%s\n", pTempCmdEntry->syntax_str);
      
      while(0 < --len)
        printf("-");
        
      printf("\n");
      j = 0;
      continue;
    }

    /* display the secondary level command in n rows of 3 column each */
    /* no of rows depends on the command entry in the command table of omx */
    switch (j % numColumns)
    {
      case 0:
        
        sprintf(line, "   %-22s", pTempCmdEntry->name); 
        break;

      case 1:
        
        sprintf(line, "%s%-22s", line, pTempCmdEntry->name); 
        break;

      case 2:
        
        printf("%s%-22s\n", line, pTempCmdEntry->name);
        memset(line, 0, sizeof(line));
        break;
      
      default:

        break;
    }
    
    j++;
  }
  printf("%s\n", line);
}


/******************************************************************************\
 *      OMTB_CmdParseOmx Function Definition
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
)
{
  MX_DBG_CMD_CONTEXT_T    cmdCntxt;
  MX_Result               result = ERR_NOERR;

  /* Initialize the command context */
  memset(&cmdCntxt, 0, sizeof(MX_DBG_CMD_CONTEXT_T));
  cmdCntxt.p_cur_cmd_tbl = &cmdTblOmx;
  cmdCntxt.cur_inst      = MX_DBG_CMD_CONTEXT_INIT_INST;
  strcpy(cmdCntxt.syntax_str, "omx");
  strcpy(cmdCntxt.cmd_line_str, cmdCntxt.syntax_str);

  if(2u > argc)
  {
    OMTB_ShowCmdHelp(&cmdCntxt);
    return;
  }

  result = XdbgCmdParse(&cmdCntxt, &cmdEntryOmx, &argv[1]);

  if (ERR_NOERR == result)
  {
    printf("OK\n");
  }
}


/******************************************************************************\
 *      OMTB_CmdApi Function Definition
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
)
{
  MX_Result result = ERR_NOERR;

  /* remove the unnecessary white spaces*/
  result = OMTB_DimtSkipWhiteSpace(pContext, pCmdEntry, ppCmdLine);
  
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }
  
  return XdbgCmdParse(pContext, pCmdEntry, ppCmdLine);
}


/******************************************************************************\
 *      OMTB_CmdReleaseInfo Function Definition
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
)
{
  /* Display the OMTB version number */
  PRINT(PRINT_MUST, "OMTB Version: %s\n", OMTB_REL_INFO_STRING);

  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdStoreCfg Function Definition
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
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0;
  char cfgFileName[OMTB_MAX_LEN] = {"\0"};

  /* Read template number */
  result = XdbgCmdNextNum(pContext,pCmdEntry, ppCmdLine, (UINT32*)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }

  /* Read the config file name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, cfgFileName);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Invoke the store configuration to dump the current cfg to file */
  result = OMTB_StoreCfg(omtbCfgTemplate[template], cfgFileName, template);
  
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error storing config params\n", __func__);
    return result;
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdLoadCfg Function Definition
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
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0;
  char cfgFileName[OMTB_MAX_LEN] = {"\0"};

  /* Read template number */
  result = XdbgCmdNextNum(pContext,pCmdEntry, ppCmdLine, (UINT32*)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }

  /* Read the config file name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, cfgFileName);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* load the template from file having configuration params in specific 
     command format */
  result = OMTB_LoadCfg(omtbCfgTemplate[template], cfgFileName, template);
  
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error loading config params\n", __func__);
    return result;
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdAddCfg Function Definition
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
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0;

  /* Read template number */
  result = XdbgCmdNextNum(pContext,pCmdEntry, ppCmdLine, (UINT32*)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }

  /* add the new configuration template into memory */
  result = OMTB_AddCfg(template);
  
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error allocating cfg params memory\n", __func__);
    return result;
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdRemoveCfg Function Definition
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
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0;

  /* Read template number */
  result = XdbgCmdNextNum(pContext,pCmdEntry, ppCmdLine, (UINT32*)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, "Invalid template number %d -Valid range: <0 to %d>\n",
                          template, OMTB_MAX_TEMPLATE - 1);
    return ERR_INVNAME;
  }

  /* remove the specified configuration template from the memory */
  result = OMTB_RemoveCfg(template);
  
  if(ERR_NOERR != result)
  {
    PRINT(PRINT_ERR_MSG, "%s: Error releasing config params memory\n", 
                          __func__);
    return result;
  }
  
  return ERR_NOERR;
}


/******************************************************************************\
 *      OMTB_CmdRunScript Function Definition
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
)
{
  MX_Result result = ERR_NOERR;
  char scriptFileName[OMTB_MAX_LEN] = {"\0"};

  /* Read the script file name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, scriptFileName);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* run the script */
  result = OMTB_RunScript(scriptFileName);
  
  return result;
}


/******************************************************************************\
 *      OMTB_CmdShowArmLoad Function Definition
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
)
{
  /* Toggle the ARM CPU load display */
  armCpuLoadDispFlag = !(armCpuLoadDispFlag);
  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_CmdToggleCompDbgLog Function Definition
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
)
{
  /* Toggle the component debug log display */
  globDebugDisable = !(globDebugDisable);
  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_CmdSetDbgLogLevel Function Definition
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
)
{
  unsigned int dbgLogLevel = 0;
  MX_Result result = ERR_NOERR;

  /* Read debug log level */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine,
                          (UINT32 *)&dbgLogLevel);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Disable all prints except errors and compulsary prints */
  if (PRINT_NO_OUTPUT == dbgLogLevel)
  {
    omtbDbgLogLevel = PRINT_NO_OUTPUT;
    return ((MX_Result)ERR_NOERR);
  }

  /* Print Simple Message */
  if (PRINT_SIMPLE_MESSAGE & dbgLogLevel)
  {
    omtbDbgLogLevel |= PRINT_SIMPLE_MESSAGE;
  }

  /* Print Default Message */
  if (DEFAULT_MESSAGES & dbgLogLevel)
  {
    omtbDbgLogLevel |= DEFAULT_MESSAGES;
  }

  /* Print Function Name*/
  if (PRINT_FUNCTION_NAME & dbgLogLevel)
  {
    omtbDbgLogLevel |= PRINT_FUNCTION_NAME;
  }

  /* Print Debug Mesaage */
  if (PRINT_DEBUG_MESSAGE & dbgLogLevel)
  {
    omtbDbgLogLevel |= PRINT_DEBUG_MESSAGE;
  }

  /* Print Compulsary Mesaage */
  if (PRINT_MUST & dbgLogLevel)
  {
    omtbDbgLogLevel |= PRINT_MUST;
  }

  /* Print Error Mesaage */
  if (PRINT_ERR_MSG & dbgLogLevel)
  {
    omtbDbgLogLevel |= PRINT_ERR_MSG;
  }
    
  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_CmdTemplateProcess Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function is used to set/show the comamnd in/from the 
 *            selected template
 *
 *  @param    pContext   : Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            pCmdEntry : Pointer to MX_DBG_CMD_Entry_T structure
 *            ppCmdLine : Character Double Pointer containing the command
 *            pTemplate      : Pointer to template
 *            template    : Template No
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


PRIVATE MX_Result
OMTB_CmdTemplateProcess(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine,
  void*                   pTemplate,
  UINT32                  template
)
{
  char                    pStr[OMTB_MAX_LEN] = {"\0"};
  char*                   pLine = NULL;
  MX_Result               result = ERR_NOERR;

  /* Push the pointer to the configuration template on the stack */
  if (ERR_NOERR != XdbgContextPush(pContext, pTemplate)) 
  {
    XdbgPrintf("%s: PUSH failed: context stack is full.\n", pCmdEntry->name);
    return ((MX_Result)ERR_END_OF_CMD);
  }

  if (
      (MX_DBG_CMD_MODE_SET    == mx_dbg_cmd_mode) ||
      (MX_DBG_CMD_MODE_RESET  == mx_dbg_cmd_mode)
     )

  {
    if (MX_DBG_CMD_MODE_RESET == mx_dbg_cmd_mode)
    {
      result = OMTB_DimtSkipWhiteSpace(pContext, pCmdEntry, ppCmdLine);
      if (ERR_NOERR != result)
      {
        return XdbgCmdExec(pContext, ppCmdLine);
      }
    }

    /* Continue parsing the rest of the line */
    result = XdbgCmdParse(pContext, pCmdEntry, ppCmdLine);
    if (ERR_NOERR != result)
    {
      return result;
    }

    return ((MX_Result)ERR_NOERR);
  }

  if (MX_DBG_CMD_MODE_SHOW == mx_dbg_cmd_mode)
  {
    Xsprintf(pStr, "");
  }
  else
  {
    Xsprintf(pStr, "dimt set %s %d", pCmdEntry->name, template);
  }

  pLine = pStr;
  result = XdbgCmdParseShow(pContext, pCmdEntry, ppCmdLine, &pLine);
  if (ERR_NOERR != result)
  {
    return result;
  }

  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_CmdTemplate Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function is used to select the template to be used
 *
 *  @param    pContext   : Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            pCmdEntry : Pointer to MX_DBG_CMD_Entry_T structure
 *            ppCmdLine : Character Double Pointer containing the command
 *
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


PRIVATE MX_Result
OMTB_CmdTemplate(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  UINT16                  template = 0;
  UINT32                  arg = 0;
  OMTB_TEMPLATE_INFO*     pTemplate = NULL;
  MX_Result               result = ERR_NOERR;

  /* Read template number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, &arg);
  if (ERR_NOERR != result)
  {
    return result;
  }
  template = (UINT16)arg;
  
  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    XdbgPrintf("Invalid Template number %d - Valid range: <0 to %d>\n",
                template, OMTB_MAX_TEMPLATE - 1);
    return ((MX_Result)ERR_INVNAME);
  }

  /* Add the specified template if doesn't exist */
  if(NULL == omtbCfgTemplate[template])
  {
    result = OMTB_AddCfg(template);
  
    if(ERR_NOERR != result)
    {
      PRINT(PRINT_ERR_MSG, "%s: Error allocating config params memory\n", 
                            __func__);
      return result;
    }
  }

  /* Set template to be used for OMTB configuration parameters, The last 
   * template id in which parameters were changed will be used regardless 
   * any error occured.
   */
  if(MX_DBG_CMD_MODE_SET == mx_dbg_cmd_mode)
  {
    omtbCfgTemplate[template]->currTemplate = template; 
  }

  /* Update the template pointer to be passed for further processing */
  pTemplate  = omtbCfgTemplate[template];

  if (
      (MX_DBG_CMD_MODE_SHOW == mx_dbg_cmd_mode)  ||
      (MX_DBG_CMD_MODE_LIST == mx_dbg_cmd_mode)
     )
  {
    XdbgPrintf("\nTemplate %d:", template);
    XdbgPutS("\n");
  }

  return OMTB_CmdTemplateProcess(pContext, pCmdEntry, ppCmdLine, 
                                 (void*)pTemplate, (UINT32)template); 
}


/******************************************************************************\
 *      OMTB_CmdSetTemplate Function Definition
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
)
{
  /* Set the appropriate ommand and invok the template processing function */
  mx_dbg_cmd_mode = (MX_DBG_CMD_MODE_T)MX_DBG_CMD_MODE_SET;

  return OMTB_CmdTemplate(pContext, pCmdEntry, ppCmdLine);
}


/******************************************************************************\
 *      OMTB_CmdShowTemplate Function Definition
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
)
{
  /* Set the appropriate ommand and invok the template processing function */
  mx_dbg_cmd_mode = (MX_DBG_CMD_MODE_T)MX_DBG_CMD_MODE_SHOW;

  return OMTB_CmdTemplate(pContext, pCmdEntry, ppCmdLine);
}


/******************************************************************************\
 *      OMTB_CmdResetTemplate Function Definition
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
)
{
  /* Set the appropriate ommand and invok the template processing function */
  mx_dbg_cmd_mode = (MX_DBG_CMD_MODE_T)MX_DBG_CMD_MODE_RESET;

  return OMTB_CmdTemplate(pContext, pCmdEntry, ppCmdLine);
}


/*----------------------------- end of file ---------------------------------*/
