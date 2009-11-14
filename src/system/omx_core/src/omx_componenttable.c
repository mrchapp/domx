/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  omx_componenttable.c
*   This file contains prototypes for a  list of all the components.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_core\src
*
*  @rev 1.0
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 *!24-Dec-2008 Navneet Basutkar navneet@ti.com: Updated for H264 Encoder
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
//#include <stdlib.h>
/*-------program files ----------------------------------------*/
#include "stdio.h"
#include "../inc/omx_componenttable.h"

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
#ifdef DUCATI_APP_M3
#define BUILD_OMX_H264E
#define BUILD_OMX_H264D
#define OMX_BASE_TEST
#define BUILD_OMX_JPEGD
#define BUILD_OMX_CAM
#endif

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

//#ifdef DUCATI_SYS_M3
extern OMX_ERRORTYPE OMX_Sample_Proxy_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_H264VE_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_H264VD_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_Camera_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_JPEGD_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);

#ifdef DUCATI_SYS_M3
extern OMX_ERRORTYPE OMX_JPEGD_Proxy_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_H264VE_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_Camera_Proxy_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_H264VD_Proxy_ComponentInit (OMX_HANDLETYPE hComponent);
#endif

#ifdef OMX_BASE_TEST
extern OMX_ERRORTYPE OMX_TI_SAMPLE_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_TTC_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_NONTI_TTC_ComponentInit(OMX_HANDLETYPE hComponent);
#endif


#ifdef DUCATI_APP_M3
extern OMX_ERRORTYPE OMX_TI_Camera_ComponentInit(OMX_HANDLETYPE hComponent);
#endif
/*
extern OMX_ERRORTYPE OMX_H264VD_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_JPEG_DEC_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_H264VE_ComponentInit(OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_H264VD_ComponentInit(OMX_HANDLETYPE hComponent);
*/

/* ==========================================================================*/
/*
 * OMX_COMPONENTLIST Structure maintain the table for component names and
 * function pointer for the respective component init functions.
 * Component name in the table must be passed by the application when it calls
 * the OMX_GetHandle function.
 *
 * The component name must be null terminated and follow the follwing format:
 *              OMX.<VENDOR_ID>.<COMPONENT_TYPE>.<COMPONENT_NAME>
 *
 * where:
 *       <VENDOR_ID> - 2 or 3 letter vendor id. Example: TI, PSW etc..
 *       <COMPONENT_TYPE> - Type of the component. Example: AUDIO, VIDEO etc.
 *       <COMPONENT_NAME> - Name of the componet. Example DSP.MIXER etc..
 *
 * example: "OMX.TI.AUDIO.DSP.MIXER\0"
 *
 * Note: Last component entry of the structure need to fill with (NULL,NULL)
 */
/* ==========================================================================*/
OMX_COMPONENTLIST OMXComponentList[] = {

{"OMX.TI.DUCATI1.MISC.SAMPLE", &OMX_Sample_Proxy_ComponentInit, 0, NULL},
{"OMX.TI.DUCATI1.VIDEO.H264E", &OMX_H264VE_Proxy_ComponentInit, 0, NULL},
{"OMX.TI.DUCATI1.VIDEO.H264D", &OMX_H264VD_Proxy_ComponentInit, 0, NULL},
{"OMX.TI.DUCATI1.VIDEO.CAMERA", &OMX_Camera_Proxy_ComponentInit, 0, NULL},
{"OMX.TI.DUCATI1.IMAGE.JPEGD", &OMX_JPEGD_Proxy_ComponentInit, 0, NULL},

#ifdef OMX_BASE_TEST
    {"OMX.TI.MISC.SAMPLE", &OMX_TI_SAMPLE_ComponentInit, 0, NULL},
    {"OMX.TI.MISC.TTC", &OMX_TI_TTC_ComponentInit, 0, NULL},
    {"OMX.NONTI.MISC.TTC", &OMX_NONTI_TTC_ComponentInit, 0, NULL},
#endif
#ifdef BUILD_OMX_JPEGD
    {"OMX.TI.DUCATI1.IMAGE.JPEGD", &OMX_TI_JPEG_DEC_ComponentInit, 0, NULL},
#endif
#ifdef BUILD_OMX_H264D
    {"OMX.TI.VID.H264D", &OMX_H264VD_ComponentInit, 0, NULL},
#endif
#ifdef BUILD_OMX_H264E
    {"OMX.TI.VIDEO.H264E", &OMX_TI_H264VE_ComponentInit, 0, NULL},
#endif
#ifdef BUILD_OMX_H264D
    {"OMX.TI.VIDEO.H264D", &OMX_H264VD_ComponentInit, 0, NULL},
#endif
#ifdef BUILD_OMX_CAM
    {"OMX.TI.DUCATI1.VIDEO.CAMERA", &OMX_TI_Camera_ComponentInit, 0, NULL},
#endif
    {NULL, NULL, 0, 0}
};

/*--------function prototypes ---------------------------------*/

