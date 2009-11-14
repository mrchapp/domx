/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/
/**
 *  @file  omx_componenttable.h
 *  This file contains the component table definition
 *
 *  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_core\inc
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 20-Dec-2008 x0052661@ti.com, initial version
 *================================================================*/

#ifndef _OMX_COMPONENT_TABLE_H_
#define _OMX_COMPONENT_TABLE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include "omx_core.h"

/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */

#define OMX_MAX_COMP_ROLES 10

/* ==========================================================================*/
/*
 * OMX_COMPONENTLIST Structure maintain the table for component names and
 * function pointer for the respective component init functions.
 * Component name in the table must be passed by the application when it calls
 * the OMX_GetHandle function.
 *
 */
/* ==========================================================================*/
typedef struct OMX_COMPONENTLIST {

    /* pointer to store the component name. The component name must be null
     * terminated and follow the follwing format:
     *              OMX.<VENDOR_ID>.<COMPONENT_TYPE>.<COMPONENT_NAME>
     * where:
     *     <VENDOR_ID> - 2 or 3 letter vendor id. Example: TI, PSW
     *     <COMPONENT_TYPE> - Type of the component. Example: AUDIO, VIDEO
     *     <COMPONENT_NAME> - Name of the componet. Example: DSP.MIXER
     *
     * example: "OMX.TI.AUDIO.DSP.MIXER\0"
     */
    OMX_STRING  cComponentName;

    /* The exported ComponentInit function of the component */
    OMX_ERRORTYPE  (*pComponentInit)(OMX_HANDLETYPE hComponent);

    /* Number of Roles Supported by a Component  */
    OMX_U32 nNumRoles;

    /* List of Roles that each Component Supports*/
    OMX_U8 roleArray[OMX_MAX_COMP_ROLES][OMX_MAX_STRINGNAME_SIZE];

} OMX_COMPONENTLIST;

/* external definition for the ComponentTable */
extern OMX_COMPONENTLIST OMXComponentList[];


/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_COMPONENT_TABLE_H_ */







