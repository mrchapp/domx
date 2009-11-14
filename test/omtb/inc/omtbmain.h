/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbMain.h
 *
 *  @brief    This file includes data types, defines etc for OMTB core files.
 *
 * <B> History: </B>
 *
 *   -# May-11-2009   AVME System Test    Inital Version
 *
 */

#ifndef __OMTB_MAIN_H__
#define __OMTB_MAIN_H__


/******************************************************************************\
*     Includes
\******************************************************************************/

#include "mxDbg.h"


/******************************************************************************/
 
#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
*     Defines
\******************************************************************************/

#define OMTB_BS_CHAR                    (0x8)
#define OMTB_SPACE_CHAR                 (" ")
#define OMTB_MAX_LEN                    (512)
#define OMTB_MAX_FRM_SZ_BYTES           (10)  /* Required for video decoders */
#define OMTB_LOOP                       (OMX_TRUE)
#define OMTB_PROMPT                     ("OMTB> ")
#define OMTB_APP_EXIT_CMD_STR           ("exit")
#define OMTB_KHRONOS_TS_CMD_STR         ("omx conf")
#define OMTB_MAX_INS                    (10)
#define OMTB_MAX_TEMPLATE               (10)
#define OMTB_MAX_FRAME                  (5000)
#define OMTB_INVALID_PARAM              (-1)

/* Max data types limits */

#define OMTB_MIN_VAL_S8                 (0xFF)
#define OMTB_MAX_VAL_S8                 (0x7F)
#define OMTB_MIN_VAL_U8                 (0)
#define OMTB_MAX_VAL_U8                 (0xFF)

#define OMTB_MIN_VAL_S16                (0xFFFF)
#define OMTB_MAX_VAL_S16                (0x7FFF)
#define OMTB_MIN_VAL_U16                (0)
#define OMTB_MAX_VAL_U16                (0xFFFF)

#define OMTB_MIN_VAL_S32                (0xFFFFFFFF)
#define OMTB_MAX_VAL_S32                (0x7FFFFFFF)
#define OMTB_MIN_VAL_U32                (0)
#define OMTB_MAX_VAL_U32                (0xFFFFFFFF)


/* OMTB script handling related defines */
#define OMTB_SCRIPT_CMD_LINE_ARG_CNT    (3)
#define OMTB_SCRIPT_OPTION              (-s)
#define OMTB_SCRIPT_OPTION_IDX          (1)
#define OMTB_SCRIPT_NAME_IDX            (2)

/* OMTB Application Version Number */
#define OMTB_REL_INFO_STRING            ("OMTB 0.00.05")

/* Specification version */
#define OMTB_VERSION_MAJOR              (0x01)
#define OMTB_VERSION_MINOR              (0x01)
#define OMTB_VERSION_REVISION           (0x00)
#define OMTB_VERSION_STEP               (0x00)

/* Default width and height used for video decoders */
#define OMTB_VIDDEC_DEF_WIDTH           (176)
#define OMTB_VIDDEC_DEF_HEIGHT          (144)

#define OMTB_VIDENC_DEF_WIDTH           (176)
#define OMTB_VIDENC_DEF_HEIGHT          (144)

#define OMTB_ARM_LOAD_GROUP_PRIORITY    (2)         /* Task group priority */
#define OMTB_ARM_LOAD_TSK_STACK_SIZE    (30 * 1024) /* Task stack size */

/******************************************************************************\
*     Data type definitions
\******************************************************************************/

 /* Enum for OMTB Return Values */
typedef enum OmtbRet
{
  OMTB_NOT_SUPPORTED          = -3, /* Not supported */
  OMTB_INSUFFICIENT_RESOURCES = -2, /* Insufficient resources */
  OMTB_FAIL                   = -1, /* Error */
  OMTB_SUCCESS                =  0, /* Success */
  OMTB_NOTSUPPORTED
}OmtbRet;


/* Enum for selecting O/P mode */
typedef enum
{
  OMTB_FILE,
  OMTB_DRIVER,
  OMTB_NONE
} OmtbOutPutMode;

 
/******************************************************************************\
*     Macros
\******************************************************************************/

#define ASSERT(condition)     (assert(condition));

/******************************************************************************\
*     Globals
\******************************************************************************/


/******************************************************************************\
*     External Object Definitions 
\******************************************************************************/


/******************************************************************************\
*     Public Function Prototypes
\******************************************************************************/

/******************************************************************************\
 *      OMTB_RunScript Function Prototype
\******************************************************************************/
/*
 *
 *  @brief   
 *            This function runs the DVS script containing set of commands.
 *
 *  @param in:
 *            scriptFileName: Pointer to script file name
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

MX_Result OMTB_RunScript( char *scriptFileName);


/******************************************************************************\
 *      OMTB_AddCfg Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    
 *            This function allocates the memory for the specified template.
 *
 *  @param in:
 *            template: Template no which is to be allocated.
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/Memory allocation 
 *                                error.
 */

MX_Result OMTB_AddCfg(unsigned int template);


/******************************************************************************\
 *      OMTB_RemoveCfg Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    
 *            This function releases the memory of the specified template.
 *
 *  @param in:
 *            template: Template no which is to be removed from memory
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/file name.
 */

MX_Result OMTB_RemoveCfg(unsigned int template);


/******************************************************************************\
 *      OMTB_CmdParse Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    This function copies the content from command line into a local
 *            buffer and send it for further parsing 
 *
 *  @param in:
 *            argc: No of arguments  
 *            argv: Array of pointers containing the strings enetered 
 *                  through command line
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            None
 */

void OMTB_CmdParse (int argc, char ** argv);


/******************************************************************************\
 *      procCmdLine Function Prototype
\******************************************************************************/
/*
 *
 *  @brief    
 *            This function processes OMTB commands and parses the command 
 *            into tokens.
 *
 *  @param in:
 *            cmdStrPtr: Pointer to input command string
 *
 *  @param Out:
 *            None
 *
 *  @return   
 *            ERR_NOERR           successful,
 *
 *            ERR_INVNAME         Incorrect template number/file name.
 */

/* This function is defined in cmdLine.c file but declaration is added here to 
   avoid separate header file for the single function of the command parser */
void procCmdLine(char *cmdStrPtr);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/
