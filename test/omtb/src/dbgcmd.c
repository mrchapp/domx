/******************************************************************************\
* FILE PURPOSE: MXP Enhanced Debug Command Interpreter
*               
********************************************************************************
* FILE NAME:    dbgCmd.c
*
* DESCRIPTION:  
*               
*
* (C) Copyright 2003 Telogy Networks, Inc.
*******************************************************************************/

/******************************************************************************\
 *     Standard Includes
\******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>


/******************************************************************************\
 *     Customized Includes
\******************************************************************************/

#include "swDef.h"
#include "rtxErr.h"
/* #include "swpform.h" */
#include "mxDbg.h"
#ifdef WTSD
#include "timm_osal_interfaces.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
#endif

#ifdef USE_OSAL
#include "osal_mem.h"
#include "osal_trc.h"
#endif


#define COMPONENTID             OSAL_CMD_INTRPTR_MAPID
#define COMP_NAME               "CMD_INTRPTR_DBG"

#define IS_LINE_END(c)      (((c) == '\0') || ((c) == '\r') || ((c) == '\n'))
#define IS_WORD_END(c)      (isspace(c) || IS_LINE_END(c))

MX_DBG_CMD_MODE_T   mx_dbg_cmd_mode = MX_DBG_CMD_MODE_SET;
MX_DBG_CMD_TYPE_T   mx_dbg_last_cmd_type = MX_DBG_CMD_TYPE_CMD_TABLE;

/****************** Utility Functions *****************************************/

/******************************************************************************\
 * Function:    XdbgCmdHelpTest
 *******************************************************************************
 * Description: Check whether help has been requested for this command.  
 *              If so, print the short/long help description.
 *
 * Returns:     ERR_END_OF_CMD      help description was requested, 
 *              ERR_NOERR           no help request.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdHelpTest
(
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  char*                   p_cmd_name
)
{
  BOOL    help_req = FALSE;
  
  if (!strcmp(p_cmd_name, "?"))
  {
    XdbgPrintf("%s\n", p_cmd_entry->short_desc);
    help_req = (BOOL)TRUE;
  }
  else if (!strcmp(p_cmd_name, "??"))
  {
    XdbgPrintf("%s\n", p_cmd_entry->long_desc);
    help_req = (BOOL)TRUE;
  }
  
  if (help_req)
  {
    if (p_cmd_entry->options & MX_DBG_CMD_OPT_CUSTOM_HELP)
    {
      /* Use cmd_func to print the custom help description */
      mx_dbg_cmd_mode = MX_DBG_CMD_MODE_HELP;
      p_cmd_entry->cmd_func(NULL, p_cmd_entry, NULL);
      mx_dbg_cmd_mode = MX_DBG_CMD_MODE_SET;
    }
    XdbgPutS("OK\n");
    return ((MX_Result)ERR_END_OF_CMD);
  }
  else
  {
    return ((MX_Result)ERR_NOERR);
  }
}

/******************************************************************************\
 * Function:     XdbgCmdNextArg
 *******************************************************************************
 * Description: Get the next argument from the command line
 *
 * Returns:     ERR_NOERR           successful 
 *              ERR_END_OF_CMD      reached the end of the line
 ******************************************************************************/
MX_Result
XdbgCmdNextArg
(
  char**                      pp_cmd_line,
  char*                       p_cmd
)
{
  char*   p_argv = NULL;
  char*   p_char = NULL;
  UINT8   length = 0;
  
  p_char = (char*) *pp_cmd_line;
  /* Skip leading white space. Stop if comment or end of line. */
  while (isspace((int)*p_char))
  {
      p_char++;
  }
  
  if (IS_LINE_END(*p_char))
  {
      /* Reached the end of the line. */
      return ((MX_Result)ERR_END_OF_CMD);
  }
  
  p_argv = p_char;      /* Mark the start of the argument */
  
  /* While character is not NULL and not a Space */
  while (!IS_WORD_END((int)*p_char))
  {
#if 0
    /* Convert to lower case */
    if (isupper((int)*p_char))
    {
      *p_char = tolower((int)*p_char);
    }
#endif
    
    /* Move through the command */
    p_char++;
    length++;
  }
  
  memcpy(p_cmd, p_argv, length);
  p_cmd[length] = '\0';
  *pp_cmd_line = p_char;
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdNextVar
 *******************************************************************************
 * Description: Get the next argument from the command line
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_END_OF_CMD      otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdNextVar
(
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  char*                       p_cmd_name
)
{
  if (XdbgCmdNextArg(pp_cmd_line, p_cmd_name) != ERR_NOERR)
  {
      /* Missing argument */
      return ((MX_Result)ERR_INVNAME);
  }
  
  if (XdbgCmdHelpTest(p_cmd_entry, p_cmd_name) != ERR_NOERR)
  {
      return ((MX_Result)ERR_END_OF_CMD);
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdNextNum
 *******************************************************************************
 * Description: .
 *
 * Returns:     None
 ******************************************************************************/
MX_Result
XdbgCmdNextNum
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT32*                     p_val
)
{
  char                        p_cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  MX_Result                   result;
  
  /* Read TCID */
  if ((result = XdbgCmdNextVar(p_cmd_entry, pp_cmd_line, p_cmd_name))
                  != ERR_NOERR)
  {
    
    if (result == ERR_INVNAME)
    {
      XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    }
    return result;
  }
  
  *p_val = (UINT32)strtoul(p_cmd_name, NULL, 0);
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdNextCmd
 *******************************************************************************
 * Description: .
 *
 * Returns:     None
 ******************************************************************************/
MX_Result 
XdbgCmdGetPtr
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  void**                      pp_var
)
{
  if (XdbgContextRead(p_context, pp_var) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
                p_context->syntax_str);
    return ((MX_Result)ERR_INVNAME);
  }
  
  *pp_var = (((UINT8*)*pp_var) + (p_cmd_entry->location));
  
  return ((MX_Result)ERR_NOERR);
}


/****************** Valid Params Functions ************************************/

/******************************************************************************\
 * Function:    XdbgCmdValidParamsSet
 *******************************************************************************
 * Description: .
 *
 * Returns:     None
 ******************************************************************************/
void
XdbgCmdValidParamsSet
(
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  void*                       p_var,
  BOOL                        set_valid_params
)
{
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_VALID_BITMAP)
  {
    if (p_cmd_entry->options & MX_DBG_CMD_OPT_32BIT_BITMAP)
    {
      UINT32*     p_valid_params;
      p_valid_params = (UINT32*)((UINT8*)p_var + p_cmd_entry->gen_param1);
      if (set_valid_params)
      {
        *p_valid_params |= (p_cmd_entry->gen_param2);
      }
      else
      {
        *p_valid_params &= (~p_cmd_entry->gen_param2);
      }
    }
    else
    {
      UINT16*     p_valid_params;
      p_valid_params = (UINT16*)((UINT8*)p_var + p_cmd_entry->gen_param1);
      if (set_valid_params)
      {
        *p_valid_params |= (UINT16)p_cmd_entry->gen_param2;
      }
      else
      {
        *p_valid_params &= ~(UINT16)p_cmd_entry->gen_param2;
      }
    }
  }
}

/******************************************************************************\
 * Function:    XdbgCmdValidParamsCheck
 *******************************************************************************
 * Description: .
 *
 * Returns:     None
 ******************************************************************************/
BOOL
XdbgCmdValidParamsCheck
(
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  void*                       p_var
)
{
  UINT32      valid_params;
  
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_VALID_BITMAP)
  {
    if (p_cmd_entry->options & MX_DBG_CMD_OPT_32BIT_BITMAP)
    {
      valid_params = *((UINT32*)((UINT8*)p_var + p_cmd_entry->gen_param1));
    }
    else
    {
      valid_params = *((UINT16*)((UINT8*)p_var + p_cmd_entry->gen_param1));
    }
    if ((valid_params & p_cmd_entry->gen_param2) == 0)
    {
      return FALSE;
    }
  }
  return ((BOOL)TRUE);
}

/******************************************************************************\
 * Function:    XdbgCmdValidParamsCopy
 *******************************************************************************
 * Description: .
 *
 * Returns:     None
 ******************************************************************************/
void
XdbgCmdValidParamsCopy
(
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  void*                       p_var_src,
  void*                       p_var_dst,
  BOOL                        copy            /* TRUE copy, FALSE update */
)
{
  UINT32                  valid_params_src = 0;
  
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_VALID_BITMAP)
  {
    if (p_cmd_entry->options & MX_DBG_CMD_OPT_32BIT_BITMAP)
    {
      UINT32*     p_valid_params_dst;
      
      valid_params_src = *((UINT32*)((UINT8*)p_var_src + 
                          p_cmd_entry->gen_param1));
      p_valid_params_dst = ((UINT32*)((UINT8*)p_var_dst + 
                          p_cmd_entry->gen_param1));
      if (valid_params_src & p_cmd_entry->gen_param2)
      {
        *p_valid_params_dst |= (p_cmd_entry->gen_param2);
      }
      else if (copy)
      {
        *p_valid_params_dst &= (~p_cmd_entry->gen_param2);
      }
    }
    else
    {
      UINT16*     p_valid_params_dst;
      
      valid_params_src = *((UINT16*)((UINT8*)p_var_src + 
                          p_cmd_entry->gen_param1));
      p_valid_params_dst = ((UINT16*)((UINT8*)p_var_dst + 
                          p_cmd_entry->gen_param1));
      if (valid_params_src & p_cmd_entry->gen_param2)
      {
        *p_valid_params_dst |= (p_cmd_entry->gen_param2);
      }
      else if (copy)
      {
        *p_valid_params_dst &= ((UINT16)(~p_cmd_entry->gen_param2));
      }
    }
  }
}


/****************** Stack Functions *******************************************/

/******************************************************************************\
 * Function:    XdbgContextPush
 *******************************************************************************
 * Description: Push to stack
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_QFULL           stack is full.
 ******************************************************************************/
MX_Result
XdbgContextPush
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  void*                       p_var
)
{
  if (p_context->cur_inst == MX_DBG_CMD_CONTEXT_MAX_DEPTH)
  {
    return ((MX_Result)ERR_QFULL);
  }
  p_context->inst[++p_context->cur_inst] = p_var;
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgContextPop
 *******************************************************************************
 * Description: Pop from stack
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_QEMPTY          stack is empty.
 ******************************************************************************/
MX_Result
XdbgContextPop
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  void**                      p_var
)
{
  if (p_context->cur_inst == MX_DBG_CMD_CONTEXT_INIT_INST)
  {
    return ((MX_Result)ERR_QEMPTY);
  }
  *p_var = p_context->inst[p_context->cur_inst--];
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgContextWrite
 *******************************************************************************
 * Description: Write to top of stack without updating cur_inst
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_QEMPTY          stack is empty.
 ******************************************************************************/
MX_Result
XdbgContextWrite
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  void*                       p_var
)
{
  if (p_context->cur_inst == MX_DBG_CMD_CONTEXT_INIT_INST)
  {
    return ((MX_Result)ERR_QEMPTY);
  }
  p_context->inst[p_context->cur_inst] = p_var;
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgContextRead
 *******************************************************************************
 * Description: Read from top of stack without updating cur_inst
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_QEMPTY          stack is empty.
 ******************************************************************************/
MX_Result
XdbgContextRead
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  void**                      p_var
)
{
  if (p_context->cur_inst == MX_DBG_CMD_CONTEXT_INIT_INST)
  {
    return ((MX_Result)ERR_QEMPTY);
  }
  *p_var = p_context->inst[p_context->cur_inst];
  return ((MX_Result)ERR_NOERR);
}



/****************** Lookup Functions ******************************************/

/******************************************************************************\
 * Function:    XdbgLookupCmd
 *******************************************************************************
 * Description: Search for a command entry in the given command
 *              table whose name matches the input command name
 * Returns:     pointer to command entry        successful, 
 *              NULL                            otherwise.
 ******************************************************************************/
MX_DBG_CMD_ENTRY_T*
XdbgLookupCmd
(
  MX_DBG_CMD_TABLE_T*     p_cmd_tbl,
  char*                   p_cmd_name
)
{
  UINT8                   i = 0;
  
  while (p_cmd_tbl->p_entries[i] != NULL)
  {
    if (!strcmp((char*)(p_cmd_tbl->p_entries[i]->name), p_cmd_name))
    {
      return (p_cmd_tbl->p_entries[i]);
    }
    i++;
  }
  
  return NULL;
}

/******************************************************************************\
 * Function:    XdbgLookupVal
 *******************************************************************************
 * Description: Search for an entry whose value is identical to 
 *              the given string.
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
PRIVATE MX_Result
XdbgLookupVal
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  UINT32                  val
)
{
  while (strcmp(p_enum_tbl->name, ""))
  {
    if (p_enum_tbl->value == val)
    {
      return ((MX_Result)ERR_NOERR);
    }
    p_enum_tbl++;
  }
  
  return ((MX_Result)ERR_INVNAME);
}

/******************************************************************************\
 * Function:    XdbgLookupEnum
 *******************************************************************************
 * Description: Search for an entry whose name is identical to 
 *              the given string and set val equal to the value 
 *              of the entry
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
MX_Result
XdbgLookupEnum
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  char*                   p_cmd_name, 
  UINT32*                 p_val
)
{
  while (strcmp(p_enum_tbl->name, ""))
  {
    if (!strcmp(p_enum_tbl->name, p_cmd_name))
    {
      *p_val = p_enum_tbl->value;
      return ((MX_Result)ERR_NOERR);
    }
    p_enum_tbl++;
  }
  
  return ((MX_Result)ERR_INVNAME);
}

/******************************************************************************\
 * Function:    XdbgLookupEnumNum
 *******************************************************************************
 * Description: Search for an entry whose name or value is identical to 
 *              the given argument.
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
PRIVATE MX_Result
XdbgLookupEnumNum
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  char*                   p_cmd_name, 
  UINT32*                 p_val
)
{
  if (XdbgLookupEnum(p_enum_tbl, p_cmd_name, p_val) != ERR_NOERR)
  {
    /* The variable name is not found in the Enum table. 
     * Read the numerical value and compared it to the
     * permitted values in the table.
     */
    *p_val = (UINT32)strtoul(p_cmd_name, NULL, 0);
  }
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgLookupEnumBitmap
 *******************************************************************************
 * Description: Search for an entry whose name is identical to the given 
 *              string. Continue with next arguments until the search 
 *              fails.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
MX_Result
XdbgLookupEnumBitmap
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  char*                   p_cmd_name, 
  UINT32*                 p_val,
  char**                  pp_cmd_line
)
{
  
  UINT32                  arg_value;
  char*                   p_temp;
  char                    p_var_name[MX_DBG_CMD_NAME_MAX_LEN];
  
  if (XdbgLookupEnum(p_enum_tbl, p_cmd_name, p_val) != ERR_NOERR)
  {
    return ((MX_Result)ERR_INVNAME);
  }
  
  /* Remember the pointer prior to parsing the next argument.
   * This will be used to backtrack, if the token is not a 
   * valid argument for this variable.
   */
  p_temp = *pp_cmd_line;
  
  while (XdbgCmdNextArg(pp_cmd_line, p_var_name) == ERR_NOERR)
  {
    if (XdbgLookupEnum(p_enum_tbl, p_var_name, &arg_value) != ERR_NOERR)
    {
      /* This must be the next command, parse the token */
      *pp_cmd_line = p_temp;
      return ((MX_Result)ERR_NOERR);
    }
    
    *p_val |= arg_value;
    p_temp = *pp_cmd_line;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgLookupEnumNumBitmap
 *******************************************************************************
 * Description:  Search for an entry whose name is identical to 'p_cmd_name'.
 *
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
PRIVATE MX_Result
XdbgLookupEnumNumBitmap
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  char*                   p_cmd_name, 
  UINT32*                 p_val,
  char**                  pp_cmd_line
)
{
  UINT32                  arg_value;
  UINT32                  mask = 0;
  char*                   p_temp;
  char                    p_var_name[MX_DBG_CMD_NAME_MAX_LEN];
  
  /* Build the mask for allowed bits */
  while (strcmp(p_enum_tbl->name, ""))
  {
    mask |= p_enum_tbl->value;
    p_enum_tbl++;
  }
  
  if (XdbgLookupEnum(p_enum_tbl, p_cmd_name, p_val) != ERR_NOERR)
  {
    *p_val = (UINT32)strtoul(p_cmd_name, NULL, 0);
  }
  
  /* Remember the pointer prior to parsing the next argument.
   * This will be used to backtrack, if the token is not a 
   * valid argument for this variable.
   */
  p_temp = *pp_cmd_line;
  
  while (XdbgCmdNextArg(pp_cmd_line, p_var_name) == ERR_NOERR)
  {
    if (XdbgLookupEnum(p_enum_tbl, p_var_name, &arg_value) != ERR_NOERR)
    {
      arg_value = (UINT32)strtoul(p_var_name, NULL, 0);
    }
    
    *p_val |= arg_value;
    p_temp = *pp_cmd_line;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgLookupEnumName
 *******************************************************************************
 * Description: Search for an entry whose value is identical to 
 *              'val' and copy the entry's name to 'p_str'
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
MX_Result
XdbgLookupEnumName
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  UINT32                  val,
  char*                   p_str
)
{
  while (strcmp(p_enum_tbl->name, ""))
  {
    if (p_enum_tbl->value == val)
    {
      strcpy(p_str, p_enum_tbl->name);
      return ((MX_Result)ERR_NOERR);
    }
    p_enum_tbl++;
  }
  
  return ((MX_Result)ERR_INVNAME);
}

/******************************************************************************\
 * Function:    XdbgLookupEnumBitmapName
 *******************************************************************************
 * Description: Search for entriese whose values are included in 
 *              'val' and copy the entry's name to 'p_str'
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         match not found.
 ******************************************************************************/
MX_Result
XdbgLookupEnumBitmapName
(
  MX_DBG_ENUM_ENTRY_T*    p_enum_tbl,
  UINT32                  val,
  char*                   p_str
)
{
  BOOL        first_arg = TRUE;
  
  if (XdbgLookupEnumName(p_enum_tbl, val, p_str) == ERR_NOERR)
  {
    return ((MX_Result)ERR_NOERR);
  }
  
  while (strcmp(p_enum_tbl->name, ""))
  {
    if ((p_enum_tbl->value & val) == p_enum_tbl->value)
    {
      if (first_arg)
      {
        Xsprintf(p_str, "%s", p_enum_tbl->name);
        first_arg = FALSE;
      }
      else
      {
        Xsprintf(p_str, "%s %s", p_str, p_enum_tbl->name);
      }
      if ((val &= (~p_enum_tbl->value)) == 0)
      {
        return ((MX_Result)ERR_NOERR);
      }
    }
    p_enum_tbl++;
  }
  
  return ((MX_Result)ERR_INVNAME);
}



/****************** Write Functions *******************************************/

/******************************************************************************\
 * Function:    XdbgCmdWriteInteger
 *******************************************************************************
 * Description: Write an integer value to the variable.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdWriteInteger
(
  MX_DBG_CMD_TYPE_T   cmd_type,
  void*               p_var,
  UINT32              val
)
{
  switch (cmd_type)
  {
    case MX_DBG_CMD_TYPE_VAR_UINT32:
        {
          UINT32*     p_uint32 = (UINT32*)p_var;
          
          *p_uint32 = (UINT32)val;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT32:
        {
          SINT32*     p_sint32 = (SINT32*)p_var;
          
          *p_sint32 = (SINT32)val;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT16:
        {
          UINT16*     p_uint16 = (UINT16*)p_var;
          
          *p_uint16 = (UINT16)val;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT16:
        {
          SINT16*     p_sint16 = (SINT16*)p_var;
          
          *p_sint16 = (SINT16)val;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT8:
        {
          UINT8*      p_uint8 = (UINT8*)p_var;
          
          *p_uint8 = (UINT8)val;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT8:
        {
          SINT8*      p_sint8 = (SINT8*)p_var;
          
          *p_sint8 = (SINT8)val;
        }
        break;
    
    default:
        return ((MX_Result)ERR_INVNAME);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdWrite
 *******************************************************************************
 * Description: Write a value to the variable.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdWrite
(
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  char*                   p_cmd_name, 
  void*                   p_var,
  char**                  pp_cmd_line
)
{
  if (!strcmp(p_cmd_name, "default"))
  {
    /* Set the variable to its default value */
    if (p_cmd_entry->type < MX_DBG_CMD_TYPE_VAR_STR_P)
    {
      if (XdbgCmdWriteInteger(p_cmd_entry->type, 
                               p_var, 
                               p_cmd_entry->def_value) != ERR_NOERR)
      {
        return ((MX_Result)ERR_INVNAME);
      }
    }
    else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_P)
    {
      char**      pp_str = (char**)p_var;
      
      memcpy(*pp_str, (char*)p_cmd_entry->def_value, 
                      strlen((char*)p_cmd_entry->def_value));
    }
    else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_BUF)
    {
      char*       p_str = (char*)p_var;
      
      memcpy(p_str, (char*)p_cmd_entry->def_value, 
                      strlen((char*)p_cmd_entry->def_value));
    }
    else
    {
      return ((MX_Result)ERR_INVNAME);
    }
    
    return ((MX_Result)ERR_NOERR);
  }
  
  if (p_cmd_entry->type < MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    UINT32      value;
    
    /* Read the integer value */
    switch (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
    {
      case MX_DBG_CMD_OPT_ENUM:
          if (XdbgLookupEnum( p_cmd_entry->format, 
                              p_cmd_name, 
                              &value) != ERR_NOERR)
          {
            return ((MX_Result)ERR_INVNAME);
          }
          break;
      
      case MX_DBG_CMD_OPT_ENUM_OR_NUM:
          if (XdbgLookupEnumNum(  p_cmd_entry->format, 
                                  p_cmd_name, 
                                  &value) != ERR_NOERR)
          {
            return ((MX_Result)ERR_INVNAME);
          }
          break;
      
      case MX_DBG_CMD_OPT_ENUM_BITMAP:
          if (XdbgLookupEnumBitmap(p_cmd_entry->format, 
                                   p_cmd_name, 
                                   &value, 
                                   pp_cmd_line) != ERR_NOERR)
          {
            return ((MX_Result)ERR_INVNAME);
          }
          break;
      
      case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
          if (XdbgLookupEnumNumBitmap(p_cmd_entry->format, 
                                      p_cmd_name, 
                                      &value, 
                                      pp_cmd_line) != ERR_NOERR)
          {
            return ((MX_Result)ERR_INVNAME);
          }
          break;
      
      default:
          /* Not an ENUM. Read the numerical value */
          value = (UINT32)strtoul(p_cmd_name, NULL, 0);
          break;
    }
    
    /* Check the range */
    switch (p_cmd_entry->type)
    {
      case MX_DBG_CMD_TYPE_VAR_UINT32:
      case MX_DBG_CMD_TYPE_VAR_UINT16:
      case MX_DBG_CMD_TYPE_VAR_UINT8:
          {
            if (
                (value < p_cmd_entry->min_value) || 
                (value > p_cmd_entry->max_value)
               )
            {
              return ((MX_Result)ERR_INVNAME);
            }
          }
          break;
      
      case MX_DBG_CMD_TYPE_VAR_SINT32:
      case MX_DBG_CMD_TYPE_VAR_SINT16:
      case MX_DBG_CMD_TYPE_VAR_SINT8:
          {
            if (
                ((SINT32)value < (SINT32)p_cmd_entry->min_value) || 
                ((SINT32)value > (SINT32)p_cmd_entry->max_value)
               )
            {
              return ((MX_Result)ERR_INVNAME);
            }
          }
          break;
      
      default:
          return ((MX_Result)ERR_INVNAME);
          break;
    }
    
    if (XdbgCmdWriteInteger( p_cmd_entry->type, 
                             p_var, 
                             value) != ERR_NOERR)
    {
      return ((MX_Result)ERR_INVNAME);
    }
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    char**      pp_str = (char**)p_var;
    
    if (
        (strlen(p_cmd_name) >= p_cmd_entry->min_value) &&
        (strlen(p_cmd_name) <= p_cmd_entry->max_value)
       )
    {
      memcpy(*pp_str, p_cmd_name, strlen(p_cmd_name));
    }
    else
    {
      return ((MX_Result)ERR_INVNAME);
    }
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_BUF)
  {
    char*       p_str = (char*)p_var;
    
    if (
        (strlen(p_cmd_name) >= p_cmd_entry->min_value) &&
        (strlen(p_cmd_name) <= p_cmd_entry->max_value)
       )
    {
      strcpy(p_str, p_cmd_name);
    }
    else
    {
      return ((MX_Result)ERR_INVNAME);
    }
  }
  return ((MX_Result)ERR_NOERR);
}


/****************** Read Functions ********************************************/

/******************************************************************************\
 * Function:    XdbgCmdReadInteger
 *******************************************************************************
 * Description: Read an integer value
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdReadInteger
(
  MX_DBG_CMD_TYPE_T   cmd_type,
  void*               p_var,
  UINT32*             p_val
)
{
  switch (cmd_type)
  {
    case MX_DBG_CMD_TYPE_VAR_UINT32:
        {
          UINT32*     p_uint32 = (UINT32*)p_var;
          
          *p_val = (UINT32)*p_uint32;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT32:
        {
          SINT32*     p_sint32 = (SINT32*)p_var;
          
          *p_val = (UINT32)*p_sint32;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT16:
        {
          UINT16*     p_uint16 = (UINT16*)p_var;
          
          *p_val = (UINT32)*p_uint16;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT16:
        {
          SINT16*     p_sint16 = (SINT16*)p_var;
          
          *p_val = (UINT32)*p_sint16;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT8:
        {
          UINT8*      p_uint8 = (UINT8*)p_var;
          
          *p_val = (UINT32)*p_uint8;
        }
        break;
    
    case MX_DBG_CMD_TYPE_VAR_SINT8:
        {
          SINT8*      p_sint8 = (SINT8*)p_var;
          
          *p_val = (UINT32)*p_sint8;
        }
        break;
    
    default:
        return ((MX_Result)ERR_END_OF_CMD);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdConvertInteger
 *******************************************************************************
 * Description: Read an integer value
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdConvertInteger
(
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  UINT32                  val,
  char*                   p_str
)
{
  char*       p_format;
  
  switch (p_cmd_entry->type)
  {
    case MX_DBG_CMD_TYPE_VAR_UINT32:
    case MX_DBG_CMD_TYPE_VAR_SINT32:
        if (p_cmd_entry->options & MX_DBG_CMD_OPT_OCT)
        {
          p_format = "O%011o";
        }
        else if (
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_HEX) ||
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_BIN)
                )
        {
          p_format = "0x%08x";
        }
        else
        {
          if (p_cmd_entry->type & MX_DBG_CMD_TYPE_VAR_UINT32)
          {
            p_format = "%lu";
          }
          else
          {
            Xsprintf(p_str, "%l", (SINT32)val);
            break;
          }
        }
        Xsprintf(p_str, p_format, (UINT32)val);
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT16:
    case MX_DBG_CMD_TYPE_VAR_SINT16:
        if (p_cmd_entry->options & MX_DBG_CMD_OPT_OCT)
        {
          p_format = "O%06o";
        }
        else if (
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_HEX) ||
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_BIN)
                )
        {
          p_format = "0x%04x";
        }
        else
        {
          if (p_cmd_entry->type & MX_DBG_CMD_TYPE_VAR_UINT16)
          {
            p_format = "%u";
          }
          else
          {
            Xsprintf(p_str, "%d", (SINT16)val);
            break;
          }
        }
        Xsprintf(p_str, p_format, (UINT16)val);
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT8:
    case MX_DBG_CMD_TYPE_VAR_SINT8:
        if (p_cmd_entry->options & MX_DBG_CMD_OPT_OCT)
        {
          p_format = "O%03o";
        }
        else if (
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_HEX) ||
                  (p_cmd_entry->options & MX_DBG_CMD_OPT_BIN)
                )
        {
          p_format = "0x%02x";
        }
        else
        {
          if (p_cmd_entry->type & MX_DBG_CMD_TYPE_VAR_UINT8)
          {
            p_format = "%u";
          }
          else
          {
            Xsprintf(p_str, "%d", (SINT8)val);
            break;
          }
        }
        Xsprintf(p_str, p_format, (UINT8)val);
        break;
    
    default:
        Xsprintf(p_str, "Invalid value");
        return ((MX_Result)ERR_END_OF_CMD);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdRead
 *******************************************************************************
 * Description: Read the value of a variable.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdRead
(
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  void*                   p_var,
  char*                   p_str
)
{
  MX_DBG_CMD_OPT_T        cmd_options;
  UINT32                  val;
  MX_Result               result;
  
  cmd_options = p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK;
  if (p_cmd_entry->type < MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    if ((result = XdbgCmdReadInteger(p_cmd_entry->type, p_var, &val))
                != ERR_NOERR)
    {
      return result;
    }
    
    /* Read the integer value */
    switch (cmd_options)
    {
      case MX_DBG_CMD_OPT_ENUM:
      case MX_DBG_CMD_OPT_ENUM_OR_NUM:
          if ((result = XdbgLookupEnumName(p_cmd_entry->format, 
                                           val, 
                                           p_str)) == ERR_NOERR)
          {
            return ((MX_Result)ERR_NOERR);
          }
          
          if (cmd_options == MX_DBG_CMD_OPT_ENUM_OR_NUM)
          {
            result = ERR_NOERR;         /* Show the numerical value */
          }
          break;
      
      case MX_DBG_CMD_OPT_ENUM_BITMAP:
      case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
          if ((result = XdbgLookupEnumBitmapName( p_cmd_entry->format, 
                                                  val, 
                                                  p_str)) == ERR_NOERR)
          {
            return ((MX_Result)ERR_NOERR);
          }
          
          if (cmd_options == MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP)
          {
            result = ERR_NOERR;         /* Show the numerical value */
          }
          break;
      
      default:
          result = ERR_NOERR;
          break;
    }
    
    if (result == ERR_NOERR)
    {
      XdbgCmdConvertInteger(p_cmd_entry, val, p_str);
    }
    else
    {
      Xsprintf(p_str, "Invalid value");
    }
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    char**      pp_str = (char**)p_var;
    
    memcpy(p_str, *pp_str, strlen(*pp_str));
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_BUF)
  {
    /* char*       p_str = (char*)p_var; */
    
    memcpy(p_str, p_var, strlen(p_var));
    p_str[strlen(p_var)] = '\0';
  }
  
  return ((MX_Result)ERR_NOERR);
}



/****************** Generic cmd_func() Functions ******************************/

/******************************************************************************\
 * Function:    XdbgCmdTblFuncSet
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_TABLE
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdTblFuncSet
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  void*       p_var;      /* pointer to the data structure */
  MX_Result   result;
  
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  if (XdbgContextPush(p_context, p_var) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- PUSH failed: context stack is full.\n", 
                p_context->syntax_str);
    return ((MX_Result)ERR_INVNAME);
  }
  
  /* Continue parsing the rest of the line */
  if ((result = XdbgCmdParse(p_context, p_cmd_entry, pp_cmd_line)) != ERR_NOERR) 
  {
    return result;
  }
  
  XdbgCmdValidParamsSet(p_cmd_entry, p_var, (BOOL)TRUE);
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdTblFuncShow
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_TABLE
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdTblFuncShow
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_line
)
{
  void*                       p_var = NULL;
  char*                       p_str = NULL;
  MX_Result                   result;
  
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  if (!XdbgCmdValidParamsCheck(p_cmd_entry, p_var))
  {
    return ((MX_Result)ERR_NOERR);
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  if (XdbgContextPush(p_context, p_var) != ERR_NOERR) 
  {
    XdbgPrintf("%s: PUSH failed: context stack is full.\n", p_cmd_entry->name);
    return ((MX_Result)ERR_END_OF_CMD);
  }
  
  /* Remember the line char string prior to adding the new command */
  p_str = (*pp_line + strlen(*pp_line));
  
  if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
  {
    XdbgPrintf("%s%s\n", *pp_line, p_cmd_entry->show_desc);
    strcat(*pp_line, "   ");
  }
  else
  {
    Xsprintf(*pp_line, "%s %s", *pp_line, p_cmd_entry->name);
  }
  
  p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
  
  result = XdbgCmdExec(p_context, pp_line);
  
  /* Resume the line char string */
  memset(p_str, 0, strlen(*pp_line));
  
  return (result);
}

/******************************************************************************\
 * Function:    XdbgCmdTblFuncReset
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_TABLE
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdTblFuncReset
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  void*                       p_var = NULL;
  char*                       p_char = NULL;
  MX_Result                   result;
  
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  if (XdbgContextPush(p_context, p_var) != ERR_NOERR) 
  {
    XdbgPrintf("%s: PUSH failed: context stack is full.\n", p_cmd_entry->name);
    return ((MX_Result)ERR_END_OF_CMD);
  }
  
  p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
  
  p_char = (char*) *pp_cmd_line;
  
  /* Skip leading white space. Stop if comment or end of line. */
  while (isspace((int)*p_char))
  {
    p_char++;
  }
  *pp_cmd_line = p_char;
  
  if ((**pp_cmd_line) != 0)
  {
    /* Continue parsing the rest of the line */
    if ((result = XdbgCmdParse(p_context,p_cmd_entry, pp_cmd_line))!=ERR_NOERR) 
    {
      return result;
    }
  }
  else
  {
    if ((result = XdbgCmdExec(p_context, pp_cmd_line)) != ERR_NOERR) 
    {
      return result;
    }
    
    XdbgCmdValidParamsSet(p_cmd_entry, p_var, FALSE);
  }
  
  return (result);
}

/******************************************************************************\
 * Function:    XdbgCmdTblFuncHelp
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_TABLE
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdTblFuncHelp
(
  MX_DBG_CMD_CONTEXT_T*   p_context,
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  char**                  pp_cmd_line
)
{
  MX_Result               result = ERR_INVNAME;
  char                    temp_str[MX_DBG_CMD_CONTEXT_STR_LENGTH];
  
  /* Remember the current syntax line */
  strcpy(temp_str, p_context->syntax_str);
  
  if (strstr(p_cmd_entry->name, *pp_cmd_line) != NULL)
  {
    Xsprintf(p_context->syntax_str, "%s %s", 
        p_context->syntax_str, p_cmd_entry->name);
    if (strcmp(p_cmd_entry->syntax_str, "") != 0)
    {
      Xsprintf(p_context->syntax_str, "%s %s", 
          p_context->syntax_str, p_cmd_entry->syntax_str);
    }
    XdbgPrintf("%s <params>\n", p_context->syntax_str);
    
    /* Restore the syntax line */
    strcpy(p_context->syntax_str, temp_str);
    return ((MX_Result)ERR_NOERR);
  }
  
  p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
  
  Xsprintf(p_context->syntax_str, "%s %s", 
      p_context->syntax_str, 
      p_cmd_entry->name);
  if (strcmp(p_cmd_entry->syntax_str, "") != 0)
  {
    Xsprintf(p_context->syntax_str, "%s %s", 
        p_context->syntax_str, p_cmd_entry->syntax_str);
  }
  
  result = XdbgCmdHelp(p_context, *pp_cmd_line);
  
  /* Restore the syntax line */
  strcpy(p_context->syntax_str, temp_str);
  
  return result;
}

/******************************************************************************\
 * Function:    XdbgCmdTblFunc
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_TABLE
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdTblFunc
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  MX_DBG_CMD_PARSE_FUNC_T*    func_ptr = NULL;
  
  /* Check the command type */
  if (p_cmd_entry->type != MX_DBG_CMD_TYPE_CMD_TABLE)
  {
    XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    return ((MX_Result)ERR_INVNAME);
  }
  
  switch (mx_dbg_cmd_mode)
  {
    case MX_DBG_CMD_MODE_SET:
        func_ptr = XdbgCmdTblFuncSet;
        break;
    
    case MX_DBG_CMD_MODE_SHOW:
    case MX_DBG_CMD_MODE_LIST:
        func_ptr = XdbgCmdTblFuncShow;
        break;
    
    case MX_DBG_CMD_MODE_HELP:
        func_ptr = XdbgCmdTblFuncHelp;
        break;
    
    case MX_DBG_CMD_MODE_RESET:
        func_ptr = XdbgCmdTblFuncReset;
        break;
    
    default:
        XdbgPrintf("%s: Invalid command mode (%d)\n", p_cmd_entry->name, 
                    mx_dbg_cmd_mode);
        break;
  }
  return (func_ptr(p_context, p_cmd_entry, pp_cmd_line));
}

/******************************************************************************\
 * Function:    XdbgCmdVarFuncSet
 *******************************************************************************
 * Description: cmd_func for TYPE_VAR_XXX
 *              The value(s) for the variable is expected next.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdVarFuncSet
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  void*       p_var;
  char        p_cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  MX_Result   result;
  
  if ((result = XdbgCmdNextVar(p_cmd_entry, pp_cmd_line, p_cmd_name))
                  != ERR_NOERR)
  {
    if (result == ERR_INVNAME)
    {
      XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    }
    return result;
  }
  
  /* Get the pointer to the upper level structure */
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  /* Assign the variable */
  if (XdbgCmdWrite(p_cmd_entry, p_cmd_name, p_var, pp_cmd_line) != ERR_NOERR)
  {
    XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    return ((MX_Result)ERR_INVNAME);
  }
  
  XdbgCmdValidParamsSet(p_cmd_entry, p_var, (BOOL)TRUE);
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdVarFuncShow
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_VAR
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdVarFuncShow
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_line
)
{
  void*                       p_var = NULL;
  char                        p_str[256];
  MX_Result                   result;
  
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  if (!XdbgCmdValidParamsCheck(p_cmd_entry, p_var))
  {
    return ((MX_Result)ERR_NOERR);
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  if (
       (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_P)   ||
       (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_BUF)
     )
  {
    XdbgCmdRead(p_cmd_entry, p_var, p_str);
  }
  else
  {
    XdbgCmdRead(p_cmd_entry, p_var, p_str);
  }
  
  if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
  {
    XdbgPrintf("%s%s%s\n", *pp_line, p_cmd_entry->show_desc, p_str);
  }
  else
  {
    XdbgPrintf("%s %s %s\n", *pp_line, p_cmd_entry->name, p_str);
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdVarFuncReset
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_VAR
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdVarFuncReset
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  void*       p_var;
  MX_Result   result;
  
  /* Get the pointer to the upper level structure */
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var;
    p_var = *p_ptr;
  }
  
  /* Reset the variable */
  if (p_cmd_entry->type < MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    if (XdbgCmdWriteInteger( p_cmd_entry->type, 
                             p_var, 
                       (UINT32)0) != ERR_NOERR)
    {
      return ((MX_Result)ERR_INVNAME);
    }
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_P)
  {
    char**      pp_str = (char**)p_var;
    
    memset(*pp_str, 0, 1);
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_VAR_STR_BUF)
  {
    char*       p_str = (char*)p_var;
    
    memset(p_str, 0, 1);
  }
  
  XdbgCmdValidParamsSet(p_cmd_entry, p_var, FALSE);
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:    XdbgCmdVarFuncHelp
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_VAR
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
PRIVATE MX_Result
XdbgCmdVarFuncHelp
(
  MX_DBG_CMD_CONTEXT_T*   p_context,
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  char**                  pp_cmd_line
)
{
  char                    buf[512];
  
  if (strstr(p_cmd_entry->name, *pp_cmd_line) != NULL)
  {
    memset(buf, 0, sizeof(buf));
    XdbgCmdSyntaxGet(p_cmd_entry, buf);
    XdbgPrintf("%s %s ", p_context->syntax_str, p_cmd_entry->name);
    XdbgPutS(buf);
    XdbgPutS("\n");
    return ((MX_Result)ERR_NOERR);
  }
  return ((MX_Result)ERR_INVNAME);
}


/******************************************************************************\
 * Function:    XdbgCmdVarFunc
 *******************************************************************************
 * Description: cmd_func for TYPE_CMD_VAR
 *              One or more commands are expected after this one.
 *
 * Returns:     ERR_NOERR           successful, 
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdVarFunc
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line
)
{
  MX_DBG_CMD_PARSE_FUNC_T*    func_ptr = NULL;
  
  /* Check the command type */
  if (
      (p_cmd_entry->type < MX_DBG_CMD_TYPE_VAR_UINT32) ||
      (p_cmd_entry->type > MX_DBG_CMD_TYPE_VAR_STR_BUF)
     )
  {
    XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    return ((MX_Result)ERR_INVNAME);
  }
  
  switch (mx_dbg_cmd_mode)
  {
    case MX_DBG_CMD_MODE_SET:
        func_ptr = XdbgCmdVarFuncSet;
        break;
    
    case MX_DBG_CMD_MODE_SHOW:
    case MX_DBG_CMD_MODE_LIST:
        func_ptr = XdbgCmdVarFuncShow;
        break;
    
    case MX_DBG_CMD_MODE_HELP:
        func_ptr = XdbgCmdVarFuncHelp;
        break;
    
    case MX_DBG_CMD_MODE_RESET:
        func_ptr = XdbgCmdVarFuncReset;
        break;
    
    default:
        XdbgPrintf("%s: Invalid command mode (%d)\n", p_cmd_entry->name, 
                    mx_dbg_cmd_mode);
        break;
  }
  return (func_ptr(p_context, p_cmd_entry, pp_cmd_line));
}

/******************************************************************************\
 * Function:     XdbgCmdVarBufferSet
 *******************************************************************************
 * Description:  Assign the contents of a buffer
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdVarBufferSet
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT16                      length,
  void*                       p_buffer,
  UINT16                      num_bits
)
{
  char                        p_cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  UINT8*                      p_uint8  = NULL;
  UINT16*                     p_uint16 = NULL;
  UINT32*                     p_uint32 = NULL;
  MX_Result                   result;
  
  switch (num_bits)
  {
    case 8:
        p_uint8  = (UINT8*)p_buffer;
        break;
    
    case 16:
        p_uint16 = (UINT16*)p_buffer;
        break;
    
    case 32:
        p_uint32 = (UINT32*)p_buffer;
        break;
    
    default:
        XdbgCmdSyntaxShow(p_context, p_cmd_entry);
        return ((MX_Result)ERR_INVNAME);
        break;
  }
  
  while (length > 0)
  {
    if ((result = XdbgCmdNextVar(p_cmd_entry, pp_cmd_line, p_cmd_name))
                    != ERR_NOERR)
    {
      if (result == ERR_INVNAME)
      {
        strcpy(p_cmd_name, "0");
      }
    }
    
    /* Assign the variable */
    switch (num_bits)
    {
      case 8:
          *p_uint8++  = (UINT8)strtoul(p_cmd_name, NULL, 0);
          break;
      
      case 16:
          *p_uint16++ = (UINT16)strtoul(p_cmd_name, NULL, 0);
          break;
      
      case 32:
          *p_uint32++ = (UINT32)strtoul(p_cmd_name, NULL, 0);
          break;
      
      default:
          break;
    }
    length--;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgCmdVarBufferShow
 *******************************************************************************
 * Description:  Show the contents of a buffer
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdVarBufferShow
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT16                      length,
  void*                       p_buffer,
  UINT16                      num_bits
)
{
  void*                       p_var = NULL;
  char                        p_str[256];
  char                        p_element[9];
  UINT8*                      p_uint8  = NULL;
  UINT16*                     p_uint16 = NULL;
  UINT32*                     p_uint32 = NULL;
  UINT32                      val;
  UINT16                      buf_len = length;
  UINT16                      num_elements = 0;
  UINT16                      num_elements_per_line = 0;
  MX_Result                   result;
  
  if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
          != ERR_NOERR)
  {
    return result;
  }
  
  if (!XdbgCmdValidParamsCheck(p_cmd_entry, p_var))
  {
    return ((MX_Result)ERR_NOERR);
  }
  
  switch (num_bits)
  {
    case 8:
        p_uint8  = (UINT8*)p_buffer;
        num_elements_per_line = (UINT16)16;
        break;
    
    case 16:
        p_uint16 = (UINT16*)p_buffer;
        num_elements_per_line = (UINT16)12;
        break;
    
    case 32:
        p_uint32 = (UINT32*)p_buffer;
        num_elements_per_line = (UINT16)8;
        break;
    
    default:
        XdbgCmdSyntaxShow(p_context, p_cmd_entry);
        return ((MX_Result)ERR_INVNAME);
        break;
  }
  
  if (length > 4)
  {
    if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
    {
      XdbgPrintf("%s%scontinued on the next line\n", *pp_cmd_line, 
                  p_cmd_entry->show_desc);
    }
    else
    {
      XdbgPrintf("%s %s ", *pp_cmd_line, p_cmd_entry->name);
    }
  }
  
  while (buf_len > 0)
  {
    switch (num_bits)
    {
      case 8:
          if ((result = XdbgCmdReadInteger((MX_DBG_CMD_TYPE_T)
                                           MX_DBG_CMD_TYPE_VAR_UINT8, 
                                           p_uint8, 
                                           &val)) != ERR_NOERR)
          {
            return result;
          }
          Xsprintf(p_element, "0x%02x", (UINT8)val);
          p_uint8++;
          break;
      
      case 16:
          if ((result = XdbgCmdReadInteger((MX_DBG_CMD_TYPE_T)
                                           MX_DBG_CMD_TYPE_VAR_UINT16, 
                                           p_uint16, 
                                           &val)) != ERR_NOERR)
          {
            return result;
          }
          Xsprintf(p_element, "0x%04x", (UINT16)val);
          p_uint16++;
          break;
      
      case 32:
          if ((result = XdbgCmdReadInteger((MX_DBG_CMD_TYPE_T)
                                           MX_DBG_CMD_TYPE_VAR_UINT32, 
                                           p_uint32, 
                                           &val)) != ERR_NOERR)
          {
            return result;
          }
          Xsprintf(p_element, "0x%08x", (UINT32)val);
          p_uint32++;
          break;
      
      default:
          break;
    }
    
    if (buf_len == length)
    {
      if (length > num_elements_per_line)
      {
        Xsprintf(p_str, " %s", p_element);
      }
      else
      {
        Xsprintf(p_str, "%s", p_element);
      }
    }
    else
    {
      Xsprintf(p_str, "%s %s", p_str, p_element);
    }
    buf_len--;
    if ((length > 4) && (++num_elements == num_elements_per_line))
    {
      if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
      {
        XdbgPrintf("\t\t%s\n", p_str);
      }
      else
      {
        XdbgPrintf("%s", p_str);
      }
      memset(p_str, 0, strlen(p_str));
      num_elements = 0;
    }
  }
  
  if (length <= 4)
  {
    if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
    {
      XdbgPrintf("%s%s%s\n", *pp_cmd_line, p_cmd_entry->show_desc, p_str);
    }
    else
    {
      XdbgPrintf("%s %s %s\n", *pp_cmd_line, p_cmd_entry->name, p_str);
    }
  }
  else
  {
    if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
    {
      XdbgPrintf("\t\t%s\n", p_str);
    }
    else
    {
      XdbgPrintf("%s\n", p_str);
    }
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgCmdVarBufferReset
 *******************************************************************************
 * Description:  Reset a buffer
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdVarBufferReset
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT16                      length,
  void*                       p_buffer,
  UINT16                      num_bits
)
{
  UINT8*                      p_uint8  = NULL;
  UINT16*                     p_uint16 = NULL;
  UINT32*                     p_uint32 = NULL;
  
  switch (num_bits)
  {
    case 8:
        p_uint8  = (UINT8*)p_buffer;
        break;
    
    case 16:
        p_uint16  = (UINT16*)p_buffer;
        break;
    
    case 32:
        p_uint32  = (UINT32*)p_buffer;
        break;
    
    default:
        XdbgCmdSyntaxShow(p_context, p_cmd_entry);
        return ((MX_Result)ERR_INVNAME);
        break;
  }
  
  while (length-- > 0)
  {
    switch (num_bits)
    {
      case 8:
          *p_uint8++  = 0;
          break;
      
      case 16:
          *p_uint16++  = 0;
          break;
      
      case 32:
          *p_uint32++  = 0;
          break;
      
      default:
          break;
    }
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgCmdParse
 *******************************************************************************
 * Description:  Parse and execute the command line string
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdParse
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_prev_cmd_entry,
  char**                      pp_cmd_line
)
{
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry;
  MX_DBG_CMD_ENTRY_T*         p_last_cmd_entry;
  MX_DBG_CMD_TABLE_T*         p_cmd_tbl;
  char                        p_cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  char*                       cur_arg = NULL;
  char*                       p_syntax_str = NULL;
  MX_Result                   result;
  
  /* Remember the current command table, because it should be 
   * looked up for subsequent parsing at this level.
   */
  p_cmd_tbl   = p_context->p_cur_cmd_tbl;
  cur_arg     = *pp_cmd_line;
  p_cmd_entry = p_prev_cmd_entry;
  p_last_cmd_entry = p_cmd_entry;
  
  /* Get the next argument */
  while (XdbgCmdNextArg(pp_cmd_line, p_cmd_name) == ERR_NOERR)
  {
    /* Find the command entry */
    if ((p_cmd_entry = XdbgLookupCmd(p_cmd_tbl, p_cmd_name)) == NULL)
    {
      /* Match not found. Is this a help request? */
      if (XdbgCmdHelpTest(p_cmd_entry, p_cmd_name) != ERR_NOERR)
      {
        return ((MX_Result)ERR_END_OF_CMD);
      }
      
      if (p_context->p_valid_cmd_entry == NULL)
      {
        p_context->p_valid_cmd_entry = p_last_cmd_entry;
      }
      
      if (
          (p_context->cur_inst == 0) ||
          (p_context->cur_inst == MX_DBG_CMD_CONTEXT_INIT_INST)
         )
      {
        /* Search failed at the top-most level */
        XdbgCmdSyntaxShow(p_context, p_context->p_valid_cmd_entry);
        return ((MX_Result)ERR_INVNAME);
      }
      
      /* Backtrack */
      *pp_cmd_line = cur_arg;
      p_context->cur_inst--;
      return ((MX_Result)ERR_NOERR);        
    }
    
    /* A valid command entry was found */
    mx_dbg_last_cmd_type = p_cmd_entry->type;
    p_context->p_valid_cmd_entry = NULL;
    
    if (p_cmd_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
    {
      p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
    }
    
    /* Remember the valid syntax string so far */
    if (strcmp(p_cmd_entry->name, "help") == 0)
    {
      return p_cmd_entry->cmd_func(p_context, p_cmd_entry, pp_cmd_line);
    }
    p_syntax_str = &p_context->syntax_str[0] + strlen(p_context->syntax_str);
    Xsprintf(p_context->syntax_str, "%s %s", 
        p_context->syntax_str, p_cmd_entry->name);
    
    if (p_cmd_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
    {
      p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
      p_last_cmd_entry = p_cmd_entry;
      
      if (strcmp(p_cmd_entry->syntax_str, "") != 0)
      {
        Xsprintf(p_context->syntax_str, "%s %s", 
            p_context->syntax_str, p_cmd_entry->syntax_str);
      }
    }
    strcpy(p_context->cmd_line_str, p_context->syntax_str);
    
    /* Execute the command */
    if ((result = p_cmd_entry->cmd_func(p_context, p_cmd_entry, pp_cmd_line)) 
                    != ERR_NOERR)
    {
      return result;
    }
    
    cur_arg = *pp_cmd_line;
    memset(p_syntax_str, 0, strlen(p_syntax_str));
    if (p_context->p_valid_cmd_entry == NULL)
    {
      strcpy(p_context->cmd_line_str, p_context->syntax_str);
    }
  }
  
  if ((**pp_cmd_line) != 0)
  {
    return ((MX_Result)ERR_INVNAME);         /* Parsing incomplete */
  }
  
  if (
       (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SET)       &&
       (mx_dbg_last_cmd_type == MX_DBG_CMD_TYPE_CMD_TABLE)
     )
  {
    XdbgCmdSyntaxShow(p_context, p_cmd_entry);
    return ((MX_Result)ERR_INVNAME);
  }
  
  p_context->cur_inst--;
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgCmdExec
 *******************************************************************************
 * Description:  Parse and execute the command line string
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdExec
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  char**                      pp_line
)
{
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry;
  MX_DBG_CMD_TABLE_T*         p_cmd_tbl;
  MX_Result                   result;
  UINT8                       i = 0;
  
  p_cmd_tbl = p_context->p_cur_cmd_tbl;
  while ((p_cmd_entry = p_cmd_tbl->p_entries[i]) != NULL)
  {
    if ((result = p_cmd_entry->cmd_func(p_context, p_cmd_entry, pp_line))
                    != ERR_NOERR)
    {
      return ((MX_Result)ERR_END_OF_CMD);
    }
    i++;
  }
  
  p_context->cur_inst--;
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
 * Function:     XdbgCmdHelp
 *******************************************************************************
 * Description:  Show the help description for the given command
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdHelp
(
  MX_DBG_CMD_CONTEXT_T*   p_context,
  char*                   p_cmd_name
)
{
  UINT8                   i = 0;
  MX_DBG_CMD_TABLE_T*     p_cmd_tbl;
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry;
  MX_Result               result = ERR_INVNAME;
  
  p_cmd_tbl = p_context->p_cur_cmd_tbl;
  while ((p_cmd_entry = p_cmd_tbl->p_entries[i]) != NULL)
  {
    if (p_cmd_entry->cmd_func(p_context, p_cmd_entry, &p_cmd_name)
                == ERR_NOERR)
    {
      /* Successful match */
      result = ERR_NOERR;
    }
    i++;
  }
  return result;
}

/******************************************************************************\
 * Function:     XdbgCmdVarCopy
 *******************************************************************************
 * Description:  Copy the contents of command entry
 *
 * Returns:
 ******************************************************************************/
void
XdbgCmdVarCopy
(
  MX_DBG_CMD_CONTEXT_T*   p_context_src,
  MX_DBG_CMD_CONTEXT_T*   p_context_dst,
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  BOOL                    copy            /* TRUE copy, FALSE update */
)
{
  void*                   p_var_src = NULL;
  void*                   p_var_dst = NULL;
  UINT32                  valid_params_src = 0;
  UINT16                  size;
  
  if (XdbgContextRead(p_context_src, &p_var_src) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
        p_context_src->syntax_str);
    return;
  }
  
  if (XdbgContextRead(p_context_dst, &p_var_dst) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
        p_context_src->syntax_str);
    return;
  }
  
  p_var_src = (UINT8*)p_var_src + p_cmd_entry->location;
  p_var_dst = (UINT8*)p_var_dst + p_cmd_entry->location;
  
  XdbgCmdValidParamsCopy(p_cmd_entry, p_var_src, p_var_dst, copy);
  
  /* Indirect addressing */
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
  {
    void**      p_ptr;
    
    p_ptr = (void**)p_var_src;
    p_var_src = *p_ptr;
    p_ptr = (void**)p_var_dst;
    p_var_dst = *p_ptr;
  }
  
  switch (p_cmd_entry->type)
  {
    case MX_DBG_CMD_TYPE_VAR_UINT32:
    case MX_DBG_CMD_TYPE_VAR_SINT32:
        size = sizeof(UINT32);
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT16:
    case MX_DBG_CMD_TYPE_VAR_SINT16:
        size = sizeof(UINT16);
        break;
    
    case MX_DBG_CMD_TYPE_VAR_UINT8:
    case MX_DBG_CMD_TYPE_VAR_SINT8:
        size = sizeof(UINT8);
        break;
    
    case MX_DBG_CMD_TYPE_VAR_STR_P:
    case MX_DBG_CMD_TYPE_VAR_STR_BUF:
        size = p_cmd_entry->max_value;
        break;
    
    default:
        size = 0;
        break;
  }
  
  if (copy)
  {
    memcpy(p_var_dst, p_var_src, size);
  }
  else if (valid_params_src & p_cmd_entry->gen_param2)
  {
    memcpy(p_var_dst, p_var_src, size);
  }
}

/******************************************************************************\
 * Function:     XdbgCmdTblCopy
 *******************************************************************************
 * Description:  Copy the contents of command entry
 *
 * Returns:
 ******************************************************************************/
void
XdbgCmdTblCopy
(
  MX_DBG_CMD_CONTEXT_T*   p_context_src,
  MX_DBG_CMD_CONTEXT_T*   p_context_dst,
  MX_DBG_CMD_ENTRY_T*     p_cmd_entry,
  BOOL                    copy            /* TRUE copy, FALSE update */
)
{
  UINT8                   i = 0;
  MX_DBG_CMD_TABLE_T*     p_cmd_tbl;
  MX_DBG_CMD_ENTRY_T*     p_entry;
  void*                   p_var_src = NULL;
  void*                   p_var_dst = NULL;
  
  p_cmd_tbl = p_context_src->p_cur_cmd_tbl;
  while ((p_entry = p_cmd_tbl->p_entries[i]) != NULL)
  {
    if (p_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
    {
      if (XdbgContextRead(p_context_src, &p_var_src) != ERR_NOERR) 
      {
        XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
            p_context_src->syntax_str);
        return;
      }
      
      if (XdbgContextRead(p_context_dst, &p_var_dst) != ERR_NOERR) 
      {
        XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
            p_context_src->syntax_str);
        return;
      }
      
      p_var_src = (UINT8*)p_var_src + p_entry->location;
      p_var_dst = (UINT8*)p_var_dst + p_entry->location;
      
      /* Push the source pointer to on the stack */
      if (XdbgContextPush(p_context_src, p_var_src) != ERR_NOERR) 
      {
        XdbgPrintf("Push failed: context stack is full.\n");
        return;
      }
      
      /* Push the destination pointer to on the stack */
      if (XdbgContextPush(p_context_dst, p_var_dst) != ERR_NOERR) 
      {
        XdbgPrintf("Push failed: context stack is full.\n");
        return;
      }
      
      p_context_src->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_entry->format;
      p_context_dst->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_entry->format;
      XdbgCmdTblCopy(p_context_src, p_context_dst, p_entry, copy);
    }
    else
    {
      XdbgCmdVarCopy(p_context_src, p_context_dst, p_entry, copy);
    }
    i++;
  }
  
  if (XdbgContextRead(p_context_src, &p_var_src) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
        p_context_src->syntax_str);
    return;
  }
  
  if (XdbgContextRead(p_context_dst, &p_var_dst) != ERR_NOERR) 
  {
    XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
        p_context_src->syntax_str);
    return;
  }
  
  /* Special Case for prof in DIM_PROFILE_CONFIG2 */
  if (strcmp(p_cmd_entry->name, "prof") == 0)
  {
    memcpy(p_var_dst, p_var_src, (17u * 8u));
  }
  
  XdbgCmdValidParamsCopy(p_cmd_entry, p_var_src, p_var_dst, copy);
  
  p_context_src->cur_inst--;
  p_context_dst->cur_inst--;
}

/******************************************************************************\
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
void
XdbgCmdSyntaxGet
(
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char*                       p_str        
)
{
  MX_DBG_ENUM_ENTRY_T*        p_enum_tbl = NULL;
  char*                       p_char = NULL;
  UINT16                      i;
  
  
  if (p_cmd_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
  {
    strcpy(p_str, p_cmd_entry->syntax_str);
    return;
  }
  
  if (strcmp(p_cmd_entry->syntax_str, "") != 0)
  {
    Xsprintf(p_str, "%s ", p_cmd_entry->syntax_str);
  }
  
  if (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
  {
    Xsprintf(p_str, "%s<", p_str);
    
    i = 0;
    p_enum_tbl = (MX_DBG_ENUM_ENTRY_T*)p_cmd_entry->format;
    while (strcmp(p_enum_tbl[i].name, ""))
    {
      Xsprintf(p_str, "%s%s|", p_str, p_enum_tbl[i].name);
      i++;
    }
    
    p_char = &p_str[0] + (strlen(p_str) - 1u);
    *p_char = '>';
    
    if (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_BITMAP_MASK)
    {
      Xsprintf(p_str, "%s (multiple values allowed)", p_str);
    }
  }
}

/******************************************************************************\
 * Function:     XdbgCmdSyntaxShow
 *******************************************************************************
 * Description:  Show the syntac of command
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result 
XdbgCmdSyntaxShow
(    
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry
)
{
  MX_DBG_CMD_TABLE_T*         p_cmd_tbl = NULL;
  MX_DBG_CMD_ENTRY_T*         p_temp_cmd_entry = NULL;
  char                        buf[512];
  char*                       p_syntax_str = NULL;
  UINT8                       i = 0;
  
  memset(buf, 0, sizeof(buf));
  XdbgCmdSyntaxGet(p_cmd_entry, buf);
  p_syntax_str = &p_context->cmd_line_str[0] + strlen(p_context->cmd_line_str);
  
  if (p_cmd_entry->type > MX_DBG_CMD_TYPE_CMD_TABLE)
  {
    XdbgPutS(p_context->cmd_line_str);
    XdbgPutS(" ");
    XdbgPutS(buf);
    XdbgPutS("\n");
  }
  else if (p_cmd_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
  {
    p_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
    
    while ((p_temp_cmd_entry = p_cmd_tbl->p_entries[i]) != NULL)
    {
      memset(buf, 0, sizeof(buf));
      XdbgCmdSyntaxGet(p_temp_cmd_entry, buf);
      
      XdbgPrintf("%s %s", p_context->cmd_line_str, 
          p_temp_cmd_entry->name);
      if (strcmp(buf, "") != 0)
      {
        XdbgPutS(" ");
        XdbgPutS(buf);
      }
      if (p_temp_cmd_entry->type == MX_DBG_CMD_TYPE_CMD_TABLE)
      {
        XdbgPutS(" <params>");
      }
      XdbgPutS("\n");
      *p_syntax_str = '\0';
      i++;
    }
  }
  
  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 * Function:     XdbgCmdParseShow
 *******************************************************************************
 * Description:  Parse and execute the command line string
 *
 * Returns:     ERR_NOERR           successful,
 *              ERR_INVNAME         otherwise.
 ******************************************************************************/
MX_Result
XdbgCmdParseShow
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_prev_cmd_entry, 
  char**                      pp_cmd_line,
  char**                      pp_line
)
{
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry;
  MX_DBG_CMD_ENTRY_T*         p_last_cmd_entry;
  MX_DBG_CMD_TABLE_T*         p_cmd_tbl;
  char                        p_cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  char*                       cur_arg = NULL;
  char*                       p_syntax_str = NULL;
  void*                       p_var = NULL;
  char*                       p_str = NULL;
  BOOL                        empty_line = TRUE;
  MX_Result                   result;
  
  /* Remember the current command table, because it should be 
   * looked up for subsequent parsing at this level.
   */
  p_cmd_tbl   = p_context->p_cur_cmd_tbl;
  cur_arg     = *pp_cmd_line;
  p_cmd_entry = p_prev_cmd_entry;
  
  p_last_cmd_entry = p_cmd_entry;
  
  /* Get the next argument */
  while (XdbgCmdNextArg(pp_cmd_line, p_cmd_name) == ERR_NOERR)
  {
    empty_line = FALSE;
    
    /* Find the command entry */
    if ((p_cmd_entry = XdbgLookupCmd(p_cmd_tbl, p_cmd_name)) == NULL)
    {
      XdbgCmdSyntaxShow(p_context, p_last_cmd_entry);
      return ((MX_Result)ERR_INVNAME);
    }
    
    /* A valid ommand entry was found */
    mx_dbg_last_cmd_type = p_cmd_entry->type;
    
    /* Remember the valid syntax string so far */
    p_syntax_str = &p_context->syntax_str[0] + strlen(p_context->syntax_str);
    Xsprintf(p_context->syntax_str, "%s %s", 
        p_context->syntax_str, p_cmd_entry->name);
    
    if (p_cmd_entry->type != MX_DBG_CMD_TYPE_CMD_TABLE)
    {
      if ((result = p_cmd_entry->cmd_func(p_context, p_cmd_entry, pp_line))
              != ERR_NOERR)
      {
        return result;
      }
      cur_arg = *pp_cmd_line;
      memset(p_syntax_str, 0, strlen(p_syntax_str));
      continue;
    }
    else
    {
      p_last_cmd_entry = p_cmd_entry;
      p_context->p_cur_cmd_tbl = (MX_DBG_CMD_TABLE_T*)p_cmd_entry->format;
      
      if (strcmp(p_cmd_entry->syntax_str, "") != 0)
      {
        Xsprintf(p_context->syntax_str, "%s %s", 
            p_context->syntax_str, p_cmd_entry->syntax_str);
      }
      
      /* Get the pointer to the upper level structure */
      if ((result = XdbgCmdGetPtr(p_context, p_cmd_entry, pp_cmd_line, &p_var))
              != ERR_NOERR)
      {
        return result;
      }
      
      /* Indirect addressing */
      if (p_cmd_entry->options & MX_DBG_CMD_OPT_INDIRECT)
      {
        void**      p_ptr;
        
        p_ptr = (void**)p_var;
        p_var = *p_ptr;
      }
      
      if (XdbgContextPush(p_context, p_var) != ERR_NOERR) 
      {
        XdbgPrintf("%s <-- PUSH failed: context stack is full.\n", 
                    p_context->syntax_str);
        return ((MX_Result)ERR_INVNAME);
      }
      
      /* Remember the line char string prior to adding the new command */
      p_str = ((*pp_line) + strlen(*pp_line));
      
      if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
      {
        XdbgPrintf("%s%s\n", *pp_line, p_cmd_entry->show_desc);
        strcat(*pp_line, "   ");
      }
      else
      {
        Xsprintf(*pp_line, "%s %s", *pp_line, p_cmd_entry->name);
      }
      
      if((result = XdbgCmdParseShow(p_context,p_cmd_entry,pp_cmd_line,pp_line))
                  != ERR_NOERR)
      {
        return result;
      }
      
      /* Resume the line char string */
      memset(p_str, 0, strlen(*pp_line));
      memset(p_syntax_str, 0, strlen(p_syntax_str));
      cur_arg = *pp_cmd_line;
    }
  }
  
  if (empty_line)
  {
    XdbgCmdExec(p_context, pp_line);
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
MX_Result
XdbgCmdVarFuncBitfield
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT32                      mask
)
{
  char                        cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  UINT16*                     p_var;
  UINT32                      bit_field;
  char                        enum_str[64];
  char*                       p_str = NULL;
  MX_Result                   result;
  
  switch (mx_dbg_cmd_mode)
  {
    case MX_DBG_CMD_MODE_SET:
        /* Read the next argument in command line */
        if ((result = XdbgCmdNextVar(p_cmd_entry, pp_cmd_line, cmd_name))
                        != ERR_NOERR)
        {
          if (result == ERR_INVNAME)
          {
              XdbgCmdSyntaxShow(p_context, p_cmd_entry);
          }
          return result;
        }
        
        switch (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
        {
          case MX_DBG_CMD_OPT_ENUM:
              if (XdbgLookupEnum( p_cmd_entry->format, 
                                  cmd_name, 
                                  &bit_field) != ERR_NOERR)
              {
                  XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                  return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_OR_NUM:
              if (XdbgLookupEnumNum(  p_cmd_entry->format, 
                                      cmd_name, 
                                      &bit_field) != ERR_NOERR)
              {
                  XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                  return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_BITMAP:
              if (XdbgLookupEnumBitmap(p_cmd_entry->format, 
                                       cmd_name, 
                                       &bit_field, 
                                       pp_cmd_line) != ERR_NOERR)
              {
                  XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                  return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
              if (XdbgLookupEnumNumBitmap(p_cmd_entry->format, 
                                          cmd_name, 
                                          &bit_field, 
                                          pp_cmd_line) != ERR_NOERR)
              {
                  XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                  return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          default:
              /* Not an ENUM. Read the numerical value */
              bit_field = (UINT32)strtoul(cmd_name, NULL, 0);
              break;
        }
        
        /* Get the pointer to variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        
        *p_var = ((*p_var & (~mask)) | bit_field);
        break;
    
    case MX_DBG_CMD_MODE_SHOW:
    case MX_DBG_CMD_MODE_LIST:
        /* Get the pointer to the variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        
        bit_field = ((*p_var) & (mask));
        
        /* Read the integer value */
        switch (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
        {
          case MX_DBG_CMD_OPT_ENUM:
          case MX_DBG_CMD_OPT_ENUM_OR_NUM:
              result = XdbgLookupEnumName(p_cmd_entry->format, 
                                          bit_field, 
                                          enum_str);
              
              break;
          
          case MX_DBG_CMD_OPT_ENUM_BITMAP:
          case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
              result = XdbgLookupEnumBitmapName(p_cmd_entry->format, 
                                                bit_field, 
                                                enum_str);
              break;
          
          default:
              result = ERR_NOERR;
              break;
        }
        
        if (result != ERR_NOERR)
        {
          if (
               (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_OR_NUM)   ||
               (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP) 
              )
          {
            XdbgCmdConvertInteger(p_cmd_entry, bit_field, enum_str);
          }
          else
          {
            Xsprintf(enum_str, "Invalid value");
          }
        }
        
        /* Remember the line char string prior to adding the new command */
        p_str = ((*pp_cmd_line) + strlen(*pp_cmd_line));
        
        if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
        {
          XdbgPrintf("%s%s%s\n", *pp_cmd_line, p_cmd_entry->show_desc, 
              enum_str);
          strcat(*pp_cmd_line, "   ");
        }
        else
        {
          XdbgPrintf("%s %s %s\n", *pp_cmd_line, p_cmd_entry->name, enum_str);
        }
        
        /* Resume the line char string */
        memset(p_str, 0, strlen(*pp_cmd_line));
        break;
    
    case MX_DBG_CMD_MODE_HELP:
        return (XdbgCmdVarFunc(p_context, p_cmd_entry, pp_cmd_line));
        break;
    
    case MX_DBG_CMD_MODE_RESET:
        /* Get the pointer to the variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        *p_var = ((*p_var) & (~mask));
        if ((*p_var) == 0)
        {
          XdbgCmdValidParamsSet(p_cmd_entry, p_var, FALSE);
        }
        break;
    
    default:
        XdbgPrintf("%s: Invalid command mode (%d)\n", p_cmd_entry->name, 
            mx_dbg_cmd_mode);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
MX_Result
XdbgCmdVarFuncBitfieldShift
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT32                      mask,
  UINT16                      shift
)
{
  char                        cmd_name[MX_DBG_CMD_NAME_MAX_LEN];
  UINT16*                     p_var;
  UINT32                      bit_field;
  char                        enum_str[64];
  char*                       p_str = NULL;
  MX_Result                   result;
  
  switch (mx_dbg_cmd_mode)
  {
    case MX_DBG_CMD_MODE_SET:
        /* Read the next argument in command line */
        if ((result = XdbgCmdNextVar(p_cmd_entry, pp_cmd_line, cmd_name))
                        != ERR_NOERR)
        {
          if (result == ERR_INVNAME)
          {
            XdbgCmdSyntaxShow(p_context, p_cmd_entry);
          }
          return result;
        }
        
        /* Read the integer value */
        switch (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
        {
          case MX_DBG_CMD_OPT_ENUM:
              if (XdbgLookupEnum( p_cmd_entry->format, 
                                  cmd_name, 
                                  &bit_field) != ERR_NOERR)
              {
                XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_OR_NUM:
              if (XdbgLookupEnumNum(  p_cmd_entry->format, 
                                      cmd_name, 
                                      &bit_field) != ERR_NOERR)
              {
                XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_BITMAP:
              if (XdbgLookupEnumBitmap(p_cmd_entry->format, 
                                       cmd_name, 
                                       &bit_field, 
                                       pp_cmd_line) != ERR_NOERR)
              {
                XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
              if (XdbgLookupEnumNumBitmap(p_cmd_entry->format, 
                                          cmd_name, 
                                          &bit_field, 
                                          pp_cmd_line) != ERR_NOERR)
              {
                XdbgCmdSyntaxShow(p_context, p_cmd_entry);
                return ((MX_Result)ERR_INVNAME);
              }
              break;
          
          default:
              /* Not an ENUM. Read the numerical value */
              bit_field = (UINT32)strtoul(cmd_name, NULL, 0);
              break;
        }
        
        /* Get the pointer to variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        
        *p_var = (((*p_var) & (~mask)) | (bit_field << shift));
        break;
    
    case MX_DBG_CMD_MODE_SHOW:
    case MX_DBG_CMD_MODE_LIST:
        /* Get the pointer to the variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        
        bit_field = (((*p_var) & (mask)) >> (shift));
        
        /* Read the integer value */
        switch (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_MASK)
        {
          case MX_DBG_CMD_OPT_ENUM:
          case MX_DBG_CMD_OPT_ENUM_OR_NUM:
              result = XdbgLookupEnumName(p_cmd_entry->format, 
                                          bit_field, 
                                          enum_str);
              
              break;
          
          case MX_DBG_CMD_OPT_ENUM_BITMAP:
          case MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP:
              result = XdbgLookupEnumBitmapName(p_cmd_entry->format, 
                                                bit_field, 
                                                enum_str);
              break;
          
          default:
              result = ERR_NOERR;
              break;
        }
        
        if (result != ERR_NOERR)
        {
          if (
               (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_OR_NUM)   ||
               (p_cmd_entry->options & MX_DBG_CMD_OPT_ENUM_OR_NUM_BITMAP) 
              )
          {
            XdbgCmdConvertInteger(p_cmd_entry, bit_field, enum_str);
          }
          else
          {
            Xsprintf(enum_str, "Invalid value");
          }
        }
        
        /* Remember the line char string prior to adding the new command */
        p_str = ((*pp_cmd_line) + strlen(*pp_cmd_line));
        
        if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_SHOW)
        {
          XdbgPrintf("%s%s%s\n", *pp_cmd_line, p_cmd_entry->show_desc, 
              enum_str);
          strcat(*pp_cmd_line, "   ");
        }
        else
        {
          XdbgPrintf("%s %s %s\n", *pp_cmd_line, p_cmd_entry->name, enum_str);
        }
        
        /* Resume the line char string */
        memset(p_str, 0, strlen(*pp_cmd_line));
        break;
    
    case MX_DBG_CMD_MODE_HELP:
        return (XdbgCmdVarFunc(p_context, p_cmd_entry, pp_cmd_line));
        break;
    
    case MX_DBG_CMD_MODE_RESET:
        /* Get the pointer to the variable */
        if (XdbgContextRead(p_context, (void*)&p_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- Read failed: context stack is empty.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        *p_var = ((*p_var) & (~(mask << shift)));
        if ((*p_var) == 0)
        {
          XdbgCmdValidParamsSet(p_cmd_entry, p_var, FALSE);
        }
        break;
    
    default:
        XdbgPrintf("%s: Invalid command mode (%d)\n", p_cmd_entry->name, 
            mx_dbg_cmd_mode);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/******************************************************************************\
* FUNCTION:                                                
********************************************************************************
* DESCRIPTION:      
********************************************************************************
* RETURNS:                                                                      
*******************************************************************************/
MX_Result
XdbgCmdPtrFunc
(
  MX_DBG_CMD_CONTEXT_T*       p_context,
  MX_DBG_CMD_ENTRY_T*         p_cmd_entry,
  char**                      pp_cmd_line,
  UINT16                      size
)
{
  void*                       ptr   = NULL;
  void**                      pp_var = NULL;
  MX_Result                   result;
  
#ifdef USE_OSAL
  OSAL_STATUS_T osal_status = OSAL_SUCCESS;
#endif /*USE_OSAL*/
  
  if (mx_dbg_cmd_mode == MX_DBG_CMD_MODE_HELP)
  {
    return XdbgCmdTblFunc(p_context, p_cmd_entry, pp_cmd_line);
  }
  
  ptr   = (void*)p_context->inst[p_context->cur_inst];
  pp_var = (void**)((UINT8*)ptr + p_cmd_entry->location);
  
  switch (mx_dbg_cmd_mode)
  {
    case MX_DBG_CMD_MODE_SET:
        if ((*pp_var) == NULL)
        {
          #ifdef USE_OSAL
            osal_status = osal_mem_alloc(pp_var, (Uint32)size);
            if (osal_status != OSAL_SUCCESS) {
              osal_trc_print((Uint32)COMPONENTID, (Uint8)OSAL_TRC_ERR,COMP_NAME,
                             "osal_mem_alloc failed: %s\n", 
                             osal_get_error_msg (osal_status));
            }
          #else
//            *pp_var = (void*)malloc(size);
            *pp_var = (void*)TIMM_OSAL_Malloc( 
                                      size,
                                      TIMM_OSAL_TRUE, 
                                      0, 
                                      TIMMOSAL_MEM_SEGMENT_EXT);
          #endif /* USE_OSAL */
          if ((*pp_var) == NULL)
          {
            XdbgPrintf("%s Cannot allocate memory.\n", 
                p_context->syntax_str);
            return ((MX_Result)ERR_INVNAME);
          }
        }
        
        if (XdbgContextPush(p_context, *pp_var) != ERR_NOERR) 
        {
          XdbgPrintf("%s <-- PUSH failed: context stack is full.\n", 
              p_context->syntax_str);
          return ((MX_Result)ERR_INVNAME);
        }
        
        /* Continue parsing the rest of the line */
        if ((result = XdbgCmdParse(p_context, p_cmd_entry, pp_cmd_line)) 
                != ERR_NOERR) 
        {
          return result;
        }
        XdbgCmdValidParamsSet(p_cmd_entry, pp_var, (BOOL)TRUE);
        break;
    
    case MX_DBG_CMD_MODE_SHOW:
    case MX_DBG_CMD_MODE_LIST:
        if ((*pp_var) == NULL)
        {
          return ((MX_Result)ERR_NOERR);
        }
        
        return XdbgCmdTblFunc(p_context, p_cmd_entry, pp_cmd_line);
    
    case MX_DBG_CMD_MODE_RESET:
        if ((*pp_var) == NULL)
        {
          return ((MX_Result)ERR_NOERR);
        }
        
        result = XdbgCmdTblFunc(p_context, p_cmd_entry, pp_cmd_line);
        #ifdef USE_OSAL
          osal_status=osal_mem_free(*pp_var);
          if (osal_status != OSAL_SUCCESS) {
            osal_trc_print((Uint32)COMPONENTID, (Uint8)OSAL_TRC_ERR, COMP_NAME,
                           "osal_mem_free failed: %s\n", 
                           osal_get_error_msg (osal_status));
          }
        #else
//          free(*pp_var);
          TIMM_OSAL_Free(*pp_var);
        #endif /* USE_OSAL */
        *pp_var = NULL;
        XdbgCmdValidParamsSet(p_cmd_entry, pp_var, FALSE);
        return result;
    
    default:
        XdbgPrintf("%s: Invalid command mode (%d)\n", 
            p_cmd_entry->name, mx_dbg_cmd_mode);
        break;
  }
  
  return ((MX_Result)ERR_NOERR);
}

/*----------------------------- end of file ----------------------------------*/
