/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbApi.c
 *
 *  @brief    This file configures h264/G711 specific config parameters and
 *            contains function dispatchers for each OMX IL APIs.
 *
 * <B> History: </B>
 *
 *    # June-1-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#ifdef WTSD
#include <string.h>
#include <assert.h>
#endif

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

#include "omtbDebug.h"
#ifdef WTSD
#include "timm_osal_interfaces.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
#endif

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

/* Pointer to OMTB configuration template */
extern OMTB_TEMPLATE_INFO    *omtbCfgTemplate[OMTB_MAX_INS];

#ifdef OMTB_FF_VD_H264
/* Pointer to H264 vdec config params */
extern OMTB_H264_VDEC_CONFIG_PARAMS *h264VdecConfig;
#endif

#ifdef OMTB_FF_AD_G711
/* Pointer to G711 adec config params */
extern OMTB_G711_ADEC_CONFIG_PARAMS *g711AdecConfig;
#endif

#ifdef OMTB_FF_VE_H264
/* Pointer to H264 venc config params */
extern OMTB_H264_VENC_CONFIG_PARAMS *h264VencConfig;
#endif

#ifdef OMTB_FF_VPS_FCC
/* Pointer to VPSS VFCC config params */
extern OMTB_VPSS_VFCC_CONFIG_PARAMS *vpssVfccConfig;
#endif

#ifdef OMTB_FF_VPS_FDC
/* Pointer to VPSS VFDC config params */
extern OMTB_VPSS_VFDC_CONFIG_PARAMS *vpssVfdcConfig;
#endif

#ifdef OMTB_FF_DRIVER_DISPLAY
/* Pointer to V4L2 disp config params */
extern OmtbDispInfo *dssConfig;
#endif

#ifdef OMTB_FF_DRIVER_AUDIO
/* Pointer to ALSA play config params */
extern OmtbAudInfo *alsaConfig;
#endif

/******************************************************************************\
 *      Globals
\******************************************************************************/

OMTB_COMP_HANDLE     omtbClass[OMTB_MAX_CLASS];    // OMTB class array

OMTB_DATA_THREAD_ARG funcThreadArg[OMTB_MAX_INS];  // Func/API thread arguments

/* Streaming (I/P and O/P) thread arguments */
OMTB_DATA_THREAD_ARG strmThreadArg[OMTB_MAX_INS];


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
 *      OMTB_GetCompHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief   This Function returns the component handle based on the component
 *           name and instance number.
 *
 *  @param   componentRequested - Pointer to the requested component name
 *
 *           instanceNo - Component instance number
 *
 *  @return  OMX_HANDLETYPE      component handle,
 *           NULL                otherwise.
 */

OMX_HANDLETYPE OMTB_GetCompHandle(
  char *componentRequested,
  unsigned int instanceNo
)
{
#ifdef OMTB_FF_VD_H264
  if(!strcmp(componentRequested,OMTB_H264_VDEC_COMP_REF_STR))
  {
    return (omtbClass[OMTB_H264_VDEC_CLASS].handle[instanceNo]);
  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF_STR))
  {
    return (omtbClass[OMTB_G711_ADEC_CLASS].handle[instanceNo]);
  }
#endif
#ifdef OMTB_FF_VE_H264
  if(!strcmp(componentRequested,OMTB_H264_VENC_COMP_REF_STR))
  {
    return (omtbClass[OMTB_H264_VENC_CLASS].handle[instanceNo]);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested,OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    return (omtbClass[OMTB_VPSS_VFCC_CLASS].handle[instanceNo]);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  if(!strcmp(componentRequested,OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    return (omtbClass[OMTB_VPSS_VFDC_CLASS].handle[instanceNo]);
  }
#endif
  else
  {
    PRINT(PRINT_ERR_MSG, "%s component instance %d not created",
                          componentRequested, instanceNo);
    return NULL;
  }
}


/******************************************************************************\
 *      OMTB_StateToStr Function Definition
\******************************************************************************/
/**
 *
 *  @brief   This Function returns the string equivalent for component state.
 *
 *  @param   state - component state
 *
 *  @return  char* - Pointer to state string
 *
 */

char *OMTB_StateToStr(OMX_STATETYPE state)
{
  
  /* Display the current state of the component */
  switch((int)state)
  {
    case OMX_StateInvalid:
      return("OMX_StateInvalid");

    case OMX_StateLoaded:
      return("OMX_StateLoaded");

    case OMX_StateIdle:
      return("OMX_StateIdle");

    case OMX_StateExecuting:
      return("OMX_StateExecuting");

    case OMX_StatePause:
      return("OMX_StatePause");

    case OMX_StateWaitForResources:
      return("OMX_StateWaitForResources");
      
    default:
      return("OMX_StateUndefined");

  }

}


/******************************************************************************\
 *      OMTB_GetCompVersion Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Functionn invokes OMX_GetComponentVersion API
 *
 *  @param    OMX Handle
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 */

OMX_ERRORTYPE OMTB_GetCompVersion(OMX_HANDLETYPE handle)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  char component[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMX_VERSIONTYPE pComponentVersion;
  OMX_VERSIONTYPE pSpecVersion;
  OMX_UUIDTYPE pComponentUUID;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != handle);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering Function:%s\n", __func__);

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Get the component version number */
  err = OMX_GetComponentVersion(handle,
                                component,
                                &pComponentVersion,
                                &pSpecVersion,
                                &pComponentUUID);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetComponentVersion");

  if(OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, "OMX_GetComponentVersion Resulted an Error");
    OMTB_OmxReturnStatus(err);
    return err;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    PRINT(PRINT_MUST,"Component Version is:%d.%d.%d\n",
          pComponentVersion.s.nVersionMajor,
          pComponentVersion.s.nVersionMinor,
          pComponentVersion.s.nRevision);
    PRINT(PRINT_MUST,"Spec Version is:%8x\n",
                      (unsigned int)pSpecVersion.nVersion);
  }

  PRINT(PRINT_FUNCTION_NAME, "Leaving Function:%s\n", __func__);

  return OMX_ErrorNone;
}


/******************************************************************************\
 *      OMTB_OmxInit Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes the OMX_Init API
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
OMTB_OmxInit(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE omxStatus = OMX_ErrorNone;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* OpenMax core initialization */
  omxStatus = OMX_Init ();

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_Init");

  OMTB_OmxReturnStatus(omxStatus);
  return (result);
}


/******************************************************************************\
 *      OMTB_OmxGetHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes H264VDEC_GetHandle/G711ADEC_GetHandle API
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
OMTB_OmxGetHandle(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE omxStatus = OMX_ErrorNone;
  unsigned int template = 0, instanceNo = 0;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }
  
  /* Read instance number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Read Template Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  {
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, 
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }


#ifdef OMTB_FF_VD_H264
  /* Invoke appropriate components's GetHandle function */
  if(!strcmp(componentRequested,OMTB_H264_VDEC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_H264_VDEC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG,
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    h264VdecConfig = (OMTB_H264_VDEC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->h264VdecTemplate);
                          
#ifdef OMTB_FF_DRIVER_DISPLAY
    dssConfig = (OmtbDispInfo *)&(omtbCfgTemplate[template]->v4l2DispTemplate);
#endif
    omxStatus = H264VDEC_GetHandle( &omtbClass[OMTB_H264_VDEC_CLASS], 
                                    instanceNo);
  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_G711_ADEC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG,
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    g711AdecConfig = (OMTB_G711_ADEC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->g711AdecTemplate);
                          
#ifdef OMTB_FF_DRIVER_AUDIO
    alsaConfig = (OmtbAudInfo *)&(omtbCfgTemplate[template]->alsaPlayTemplate);
#endif
    omxStatus = G711ADEC_GetHandle( &omtbClass[OMTB_G711_ADEC_CLASS], 
                                    instanceNo);
  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_H264_VENC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG,
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    h264VencConfig = (OMTB_H264_VENC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->h264VencTemplate);
                          
    omxStatus = H264VENC_GetHandle( &omtbClass[OMTB_H264_VENC_CLASS], 
                                    instanceNo);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_VPSS_VFCC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG,
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    vpssVfccConfig = (OMTB_VPSS_VFCC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->vpssVfccTemplate);
                          
    omxStatus = VPSSVFCC_GetHandle( &omtbClass[OMTB_VPSS_VFCC_CLASS], 
                                    instanceNo);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_VPSS_VFDC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG,
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    vpssVfdcConfig = (OMTB_VPSS_VFDC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->vpssVfdcTemplate);
                          
    omxStatus = VPSSVFDC_GetHandle( &omtbClass[OMTB_VPSS_VFDC_CLASS], 
                                    instanceNo);
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }
  
  if(OMX_ErrorNone != omxStatus)
    return ((MX_Result)ERR_INVNAME);
  else
    return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_OmxFreeHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMX_FreeHandle
 *            API for requested component and particular state
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */

#if 1
MX_Result
OMTB_OmxFreeHandle(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int instanceNo = 0;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMX_HANDLETYPE handle = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Delete the component instance */
  err = OMX_FreeHandle(handle);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeHandle");

  if(OMX_ErrorNone == err)
  {
    OMTB_OmxReturnStatus(err);

    /* Reelase the app pvt data structure memory of the component */
    /* Assign NULL pointer to instance management structure entry */
#ifdef OMTB_FF_VD_H264
    if(!strcmp(componentRequested,OMTB_H264_VDEC_COMP_REF_STR)) 
    {
      handle = NULL;
      H264VDEC_FreeMemory((OMTB_H264_VDEC_PRIVATE*)
                 omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo]);
      omtbClass[OMTB_H264_VDEC_CLASS].handle[instanceNo] = handle;
      PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted %s Instance #: %d\n\n",
          OMTB_H264_VDEC_COMP_REF_STR, instanceNo);

    }
#endif
#ifdef OMTB_FF_AD_G711
    else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF_STR)) 
    {
      handle = NULL;
      G711ADEC_FreeMemory((OMTB_G711_ADEC_PRIVATE* )
                 omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo]);
      omtbClass[OMTB_G711_ADEC_CLASS].handle[instanceNo] = handle;
      PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted %s Instance #: %d\n\n",
          OMTB_G711_ADEC_COMP_REF_STR, instanceNo);

    }
#endif
#ifdef OMTB_FF_VE_H264
    else if(!strcmp(componentRequested,OMTB_H264_VENC_COMP_REF_STR)) 
    {
      handle = NULL;
      H264VENC_FreeMemory((OMTB_H264_VENC_PRIVATE*)
                 omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo]);
      omtbClass[OMTB_H264_VENC_CLASS].handle[instanceNo] = handle;
      PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted %s Instance #: %d\n\n",
          OMTB_H264_VENC_COMP_REF_STR, instanceNo);

    }
#endif
#ifdef OMTB_FF_VPS_FCC
    else if(!strcmp(componentRequested,OMTB_VPSS_VFCC_COMP_REF_STR)) 
    {
      handle = NULL;
      VPSSVFCC_FreeMemory((OMTB_VPSS_VFCC_PRIVATE*)
                 omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo]);
      omtbClass[OMTB_VPSS_VFCC_CLASS].handle[instanceNo] = handle;
      PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted %s Instance #: %d\n\n",
          OMTB_VPSS_VFCC_COMP_REF_STR, instanceNo);

    }
#endif
#ifdef OMTB_FF_VPS_FDC
    else if(!strcmp(componentRequested,OMTB_VPSS_VFDC_COMP_REF_STR)) 
    {
      handle = NULL;
      VPSSVFDC_FreeMemory((OMTB_VPSS_VFDC_PRIVATE*)
                 omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo]);
      omtbClass[OMTB_VPSS_VFDC_CLASS].handle[instanceNo] = handle;
      PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted %s Instance #: %d\n\n",
          OMTB_VPSS_VFDC_COMP_REF_STR, instanceNo);

    }
 #endif 
  }

  else
  {
    OMTB_OmxReturnStatus(err);
  }
  return (result);
}
#endif


/******************************************************************************\
 *      OMTB_OmxGetCompVersion Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMTB_GetCompVersion
 *            API for requested component
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
OMTB_OmxGetCompVersion(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  unsigned int instanceNo = 0;
  OMX_HANDLETYPE handle = NULL;

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
      return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }
  else
  {
    /* Get the component version info */
    result = OMTB_GetCompVersion(handle);

    if(OMX_ErrorNone != result)
      return ERR_INVNAME;
  }

  return ((MX_Result)ERR_NOERR);
}


/******************************************************************************\
 *      OMTB_OmxSetParam Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes SetParameter
 *            API for requested component
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
OMTB_OmxSetParam(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  unsigned int template = 0, portIdx = 0;
  unsigned int instanceNo = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_HANDLETYPE handle = NULL;
  void *pCompCfgStruct = NULL;
  unsigned int nConfigIndex = OMX_IndexMax;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char indexStr[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Read Template Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  {
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG,
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read port index Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&portIdx);
  if (ERR_NOERR != result)
  {
    return result;
  }
  
  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read cfg structure index string */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, indexStr);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  #ifdef OMTB_FF_VD_H264
  /* Invoke appropriate components's GetHandle function */
  if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VDEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = G711ADEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VENC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFCC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFDC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  if(OMTB_SUCCESS != err)
  {
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Send the portInit parameter structure */
  err = OMX_SetParameter(handle, nConfigIndex, pCompCfgStruct);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxSetConfig Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes SetConfig
 *            API for requested component
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
OMTB_OmxSetConfig(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  unsigned int template = 0, portIdx = 0;
  unsigned int instanceNo = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_HANDLETYPE handle = NULL;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  void *pCompCfgStruct = NULL;
  unsigned int nConfigIndex = OMX_IndexMax;
  char indexStr[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Read Template Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  {
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG,
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read port index Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&portIdx);
  if (ERR_NOERR != result)
  {
    return result;
  }
  
  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read cfg structure index string */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, indexStr);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

#ifdef OMTB_FF_VD_H264
  /* Invoke appropriate components's GetHandle function */
  if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VDEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif  
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = G711ADEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VENC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFCC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFDC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  if(OMTB_SUCCESS != err)
  {
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Send the portInit parameter structure */
  err = OMX_SetConfig(handle, nConfigIndex, pCompCfgStruct);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetConfig");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxSetupTunnel Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function will set ups the tunnel between the specified port 
 *            of two specified components.
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
OMTB_OmxSetupTunnel(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int instanceNoCompA = 0, instanceNoCompB = 0;
  unsigned int compAPortOut = 0, compBPortIn = 0;
  OMX_HANDLETYPE handleCompOutA = NULL, handleCompInB = NULL;
  char componentRequestedA[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char componentRequestedB[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read first component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequestedA);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance number of the first component */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, 
                          (UINT32*)&instanceNoCompA);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Read output port number of the first component */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine,
                          (UINT32 *)&compAPortOut);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handleCompOutA = OMTB_GetCompHandle(componentRequestedA, instanceNoCompA);

  if(NULL == handleCompOutA)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", 
    componentRequestedA, instanceNoCompA);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read second component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequestedB);
  if (ERR_NOERR != result)
  {
    if (result == ERR_INVNAME)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance number of the second component */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine,
                          (UINT32*)&instanceNoCompB);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Read input port number of the second component */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine,
                          (UINT32 *)&compBPortIn);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handleCompInB = OMTB_GetCompHandle(componentRequestedB, instanceNoCompB);

  if(NULL == componentRequestedB)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", 
    componentRequestedB, instanceNoCompB);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Setup tunnel between the specified port of two components */
  err = OMX_SetupTunnel(handleCompOutA, compAPortOut, 
                        handleCompInB , compBPortIn);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetupTunnel");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
    
    PRINT(PRINT_DEBUG_MESSAGE, "Tunnel setup successful \
<component , instance # , port #> ==> <%s , %d , %d> <==> <%s , %d , %d>\n", 
    componentRequestedA, instanceNoCompA, compAPortOut,
    componentRequestedB, instanceNoCompB, compBPortIn);
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxSendCommandState Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMX_SendCommand
 *            API for requested component and particular state
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
OMTB_OmxSendCommandState(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_COMMANDTYPE commandType = OMX_CommandStateSet;
  unsigned int instanceNo = 0, threadRes = TIMM_OSAL_ERR_NONE;
  OMX_STATETYPE prevState = OMX_StateInvalid;
  OMX_STATETYPE nParam = 0;
  OMX_PTR pCmdData = NULL;
  OMX_HANDLETYPE handle = NULL;
  char cmdName[OMTB_MAX_LEN] = {'\0'};
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
#ifdef OMTB_FF_VD_H264
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
#endif
#ifdef OMTB_FF_AD_G711
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
#endif
#ifdef OMTB_FF_VE_H264
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
#endif
#ifdef OMTB_FF_VPS_FCC
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
#endif
#ifdef OMTB_FF_VPS_FDC
  OMTB_VPSS_VFDC_PRIVATE* vpssVfdcClient = NULL;
#endif

  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Perform the state transition inside the component */
  commandType = OMX_CommandStateSet;
  pCmdData = NULL;
  
  /* Read command params */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, cmdName);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  if(0 == strcmp(cmdName, "idle"))
  {
    nParam = OMX_StateIdle;
  }
  else if (0 == strcmp(cmdName, "exec"))
  {
    nParam = OMX_StateExecuting;
  }
  else if (0 == strcmp(cmdName, "pause"))
  {
    nParam = OMX_StatePause;
  }
  else if (0 == strcmp(cmdName, "loaded"))
  {
    nParam = OMX_StateLoaded;
  }
  else if (0 == strcmp(cmdName, "waitforresources"))
  {
    nParam = OMX_StateWaitForResources;
  }
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

#ifdef WTSD_TEMP
  PRINT(PRINT_DEBUG_MESSAGE, "Before OMX_GetState \n");
#endif

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Get the current state of the component */
  err = OMX_GetState(handle, &prevState);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetState");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
#ifdef WTSD_TEMP
  PRINT(PRINT_DEBUG_MESSAGE, "Before OMX_SendCommand, nparam = %x \n", nParam);
#endif

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component for specified state transition */
  err = OMX_SendCommand(handle, commandType, nParam, pCmdData);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand - OMX_CommandStateSet");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);

    /* Perform resource handling for state transitions */
    switch(nParam)
    {
      case OMX_StateLoaded:

#ifdef OMTB_FF_VD_H264
        if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
        {
          h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
              omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
          
          if(OMX_TRUE == h264VdecClient->flagThrCreated)
          {
            /* Exit the input data read thread */
            h264VdecClient->inPortParams.flagInputEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  h264VdecClient->inPortParams.inputDataReady);

            /* Exit the output data write thread */
            h264VdecClient->outPortParams.flagEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  h264VdecClient->outPortParams.outputDataReady);

            /* Wait till state tansition is complete */
            /*TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND); */

            /* waiting for input buffer read thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  h264VdecClient->inPortParams.inputDataReadThrExit,
                  TIMM_OSAL_SUSPEND);

            /* waiting for output buffer write thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  h264VdecClient->outPortParams.outputDataWriteThrExit,
                  TIMM_OSAL_SUSPEND);
            
          } // if(OMX_TRUE == h264VdecClient->flagThrCreated)
          else
          {
            /* Currently event is not reported by component on loaded state */
            /*TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);*/
          }

          /* release the resources acquire by the component */
          err = H264VDEC_FreeResources(h264VdecClient);

          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }

          /* Initialize EOS flag */
          h264VdecClient->inPortParams.flagInputEos = OMX_FALSE;
          h264VdecClient->outPortParams.flagEos = OMX_FALSE;

          /* Initialize I/O thread control sempahore */
          TIMM_OSAL_SemaphoreReset(
                h264VdecClient->inPortParams.inputDataReady,
                0);
          TIMM_OSAL_SemaphoreReset(
                h264VdecClient->outPortParams.outputDataReady,
                0);

          /* Initialize I/O thread exit control sempahore */
          TIMM_OSAL_SemaphoreReset(
                h264VdecClient->inPortParams.inputDataReadThrExit, 0);
          TIMM_OSAL_SemaphoreReset(
                h264VdecClient->outPortParams.outputDataWriteThrExit, 0);

        } // if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_AD_G711
        else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
        {
          g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
              omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
          
          if(OMX_TRUE == g711AdecClient->flagThrCreated)
          {
            /* Exit the input data read thread */
            g711AdecClient->inPortParams.flagInputEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  g711AdecClient->inPortParams.inputDataReady);

            /* Exit the output data write thread */
            g711AdecClient->outPortParams.flagEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  g711AdecClient->outPortParams.outputDataReady);

            /* Wait till state tansition is complete */
            /*TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND); */
            
            /* waiting for input buffer read thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  g711AdecClient->inPortParams.inputDataReadThrExit,
                  TIMM_OSAL_SUSPEND);

            /* waiting for output buffer write thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  g711AdecClient->outPortParams.outputDataWriteThrExit,
                  TIMM_OSAL_SUSPEND);

          } // if(OMX_TRUE == g711AdecClient->flagThrCreated)
          else
          {
            /* Currently event is not reported by component on loaded state */
            /*TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND); */
          }
          
          /* release the resources acquire by the component */
          err = G711ADEC_FreeResources(g711AdecClient);
          
          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }

          /* Initialize EOS flag */
          g711AdecClient->inPortParams.flagInputEos = OMX_FALSE;
          g711AdecClient->outPortParams.flagEos = OMX_FALSE;

          /* Initialize I/O thread control sempahore */
          TIMM_OSAL_SemaphoreReset(
                g711AdecClient->inPortParams.inputDataReady,
                0);
          TIMM_OSAL_SemaphoreReset(
                g711AdecClient->outPortParams.outputDataReady,
                0);

          /* Initialize I/O thread exit control sempahore */
          TIMM_OSAL_SemaphoreReset(
                g711AdecClient->inPortParams.inputDataReadThrExit, 0);
          TIMM_OSAL_SemaphoreReset(
                g711AdecClient->outPortParams.outputDataWriteThrExit, 0);
          
        } // else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VE_H264
        else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
        {
          h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
              omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
          
          if(OMX_TRUE == h264VencClient->flagThrCreated)
          {
            /* Exit the input data read thread */
            h264VencClient->inPortParams.flagInputEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  h264VencClient->inPortParams.inputDataReady);

            /* Exit the output data write thread */
            h264VencClient->outPortParams.flagEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  h264VencClient->outPortParams.outputDataReady);

            /* Wait till state tansition is complete */
            /*TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND); */

            /* waiting for input buffer read thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  h264VencClient->inPortParams.inputDataReadThrExit,
                  TIMM_OSAL_SUSPEND);

            /* waiting for output buffer write thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  h264VencClient->outPortParams.outputDataWriteThrExit,
                  TIMM_OSAL_SUSPEND);
            
          } // if(OMX_TRUE == h264VencClient->flagThrCreated)
          else
          {
            /* Currently event is not reported by component on loaded state */
            /*TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);*/
          }

          /* release the resources acquire by the component */
          err = H264VENC_FreeResources(h264VencClient);

          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }

          /* Initialize EOS flag */
          h264VencClient->inPortParams.flagInputEos = OMX_FALSE;
          h264VencClient->outPortParams.flagEos = OMX_FALSE;

          /* Initialize I/O thread control sempahore */
          TIMM_OSAL_SemaphoreReset(
                h264VencClient->inPortParams.inputDataReady,
                0);
          TIMM_OSAL_SemaphoreReset(
                h264VencClient->outPortParams.outputDataReady,
                0);

          /* Initialize I/O thread exit control sempahore */
          TIMM_OSAL_SemaphoreReset(
                h264VencClient->inPortParams.inputDataReadThrExit, 0);
          TIMM_OSAL_SemaphoreReset(
                h264VencClient->outPortParams.outputDataWriteThrExit, 0);

        } // if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FCC
        else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
        {
          vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
              omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
          
          if(OMX_TRUE == vpssVfccClient->flagThrCreated)
          {
            /* Exit the input data read thread */
            vpssVfccClient->inPortParams.flagInputEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  vpssVfccClient->inPortParams.inputDataReady);

            /* Exit the output data write thread */
            vpssVfccClient->outPortParams.flagEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  vpssVfccClient->outPortParams.outputDataReady);

            /* Wait till state tansition is complete */
            /*TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND); */

            /* waiting for input buffer read thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  vpssVfccClient->inPortParams.inputDataReadThrExit,
                  TIMM_OSAL_SUSPEND);

            /* waiting for output buffer write thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  vpssVfccClient->outPortParams.outputDataWriteThrExit,
                  TIMM_OSAL_SUSPEND);
            
          } // if(OMX_TRUE == vpssVfccClient->flagThrCreated)
          else
          {
            /* Currently event is not reported by component on loaded state */
            /*TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);*/
          }

          /* release the resources acquire by the component */
          err = VPSSVFCC_FreeResources(vpssVfccClient);

          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }

          /* Initialize EOS flag */
          vpssVfccClient->inPortParams.flagInputEos = OMX_FALSE;
          vpssVfccClient->outPortParams.flagEos = OMX_FALSE;

          /* Initialize I/O thread control sempahore */
          TIMM_OSAL_SemaphoreReset(
                vpssVfccClient->inPortParams.inputDataReady,
                0);
          TIMM_OSAL_SemaphoreReset(
                vpssVfccClient->outPortParams.outputDataReady,
                0);

          /* Initialize I/O thread exit control sempahore */
          TIMM_OSAL_SemaphoreReset(
                vpssVfccClient->inPortParams.inputDataReadThrExit, 0);
          TIMM_OSAL_SemaphoreReset(
                vpssVfccClient->outPortParams.outputDataWriteThrExit, 0);

        } // if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FDC
        else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
        {
          vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
              omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
          
          if(OMX_TRUE == vpssVfdcClient->flagThrCreated)
          {
            /* Exit the input data read thread */
            vpssVfdcClient->inPortParams.flagInputEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  vpssVfdcClient->inPortParams.inputDataReady);

            /* Exit the output data write thread */
            vpssVfdcClient->outPortParams.flagEos = OMX_TRUE;
            TIMM_OSAL_SemaphoreRelease(
                  vpssVfdcClient->outPortParams.outputDataReady);

            /* Wait till state tansition is complete */
            /*TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND); */

            /* waiting for input buffer read thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  vpssVfdcClient->inPortParams.inputDataReadThrExit,
                  TIMM_OSAL_SUSPEND);

            /* waiting for output buffer write thread exit */
            TIMM_OSAL_SemaphoreObtain(
                  vpssVfdcClient->outPortParams.outputDataWriteThrExit,
                  TIMM_OSAL_SUSPEND);
            
          } // if(OMX_TRUE == vpssVfdcClient->flagThrCreated)
          else
          {
            /* Currently event is not reported by component on loaded state */
            /*TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);*/
          }

          /* release the resources acquire by the component */
          err = VPSSVFDC_FreeResources(vpssVfdcClient);

          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }

          /* Initialize EOS flag */
          vpssVfdcClient->inPortParams.flagInputEos = OMX_FALSE;
          vpssVfdcClient->outPortParams.flagEos = OMX_FALSE;

          /* Initialize I/O thread control sempahore */
          TIMM_OSAL_SemaphoreReset(
                vpssVfdcClient->inPortParams.inputDataReady,
                0);
          TIMM_OSAL_SemaphoreReset(
                vpssVfdcClient->outPortParams.outputDataReady,
                0);

          /* Initialize I/O thread exit control sempahore */
          TIMM_OSAL_SemaphoreReset(
                vpssVfdcClient->inPortParams.inputDataReadThrExit, 0);
          TIMM_OSAL_SemaphoreReset(
                vpssVfdcClient->outPortParams.outputDataWriteThrExit, 0);

        } // if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
#endif        
        break;

      case OMX_StateIdle:

        if(OMX_StateLoaded == prevState)
        {
#ifdef OMTB_FF_VD_H264
          if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
          {
            h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
                omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
            
            /* Set the port params */
            err = H264VDEC_SetPortDefinition(h264VdecClient);

            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
              return ERR_INVNAME;
            }
            
            /* Allocate I/O buffers & wait till transition to idle */
            err = H264VDEC_AllocateResources(h264VdecClient);
            
          } // if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_AD_G711
          else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
          {
            g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
                omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
            
            /* Set the port params */
            err = G711ADEC_SetPortDefinition(g711AdecClient);

            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
              return ERR_INVNAME;
            }
            
            /* Allocate I/O buffers & wait till transition to idle */
            err = G711ADEC_AllocateResources(g711AdecClient);
            
          } // else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VE_H264
          else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
          {
            h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
                omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
            
            /* Set the port params */
            err = H264VENC_SetPortDefinition(h264VencClient);

            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
              return ERR_INVNAME;
            }
            
            /* Allocate I/O buffers & wait till transition to idle */
            err = H264VENC_AllocateResources(h264VencClient);
            
          } // else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FCC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
          {
            vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
            
            /* Set the port params */
            err = VPSSVFCC_SetPortDefinition(vpssVfccClient);

            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
              return ERR_INVNAME;
            }
            
            /* Allocate I/O buffers & wait till transition to idle */
            err = VPSSVFCC_AllocateResources(vpssVfccClient);
            
          } // else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FDC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
          {
            vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
            
            /* Set the port params */
            err = VPSSVFDC_SetPortDefinition(vpssVfdcClient);

            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
              return ERR_INVNAME;
            }
            
            /* Allocate I/O buffers & wait till transition to idle */
            err = VPSSVFDC_AllocateResources(vpssVfdcClient);
            
          } // else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
#endif
          else
            break;

          if(OMX_ErrorNone != err)
          {
            OMTB_OmxReturnStatus(err);
            return ERR_INVNAME;
          }
          
        } // if(OMX_StateLoaded == prevState)
        else if( OMX_StateExecuting == prevState  || 
                 OMX_StatePause     == prevState  )
        {
#ifdef OMTB_FF_VD_H264
          if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
          {
            h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
                omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
          
            TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
          }
#endif
#ifdef OMTB_FF_AD_G711
          else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
          {
            g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
                omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
          
            TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            
          }
#endif
#ifdef OMTB_FF_VE_H264
          else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
          {
            h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
                omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
          
            TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);
          }
#endif
#ifdef OMTB_FF_VPS_FCC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
          {
            vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
          
            TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);
          }
#endif
#ifdef OMTB_FF_VPS_FDC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
          {
            vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
          
            TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);
          }
#endif
        } // if( OMX_StateExecuting == prevState || ...
          
        break;

      case OMX_StateExecuting:

        if(OMX_StatePause != prevState)
        {
#ifdef OMTB_FF_VD_H264
          if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR) &&
              OMX_FALSE == ((OMTB_H264_VDEC_PRIVATE*)
                omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo])\
                ->flagThrCreated)
          {
            h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
                omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];

            if( OMX_FALSE == h264VdecClient->inPortParams.flagInputEos  &&
                OMX_FALSE == h264VdecClient->outPortParams.flagEos)
            {
              funcThreadArg[instanceNo].h264VdecClient = h264VdecClient;
#ifdef OMTB_FF_DRIVER_DISPLAY                                
              funcThreadArg[instanceNo].dssClient =
                  omtbClass[OMTB_H264_VDEC_CLASS].dssClient[instanceNo];
#endif
              /* Create execution thread for API mode */
              threadRes = TIMM_OSAL_CreateTask(
                (void*)&( omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[\
                          instanceNo]),
                          (void *)H264VDEC_ApiTsk,
                          0,
                          &funcThreadArg[instanceNo],
                          OMTB_H264_VDEC_TSK_STACK_SIZE,
                          OMTB_H264_VDEC_GROUP_PRIORITY,
                          (signed char*)"H264_VDEC_OMX_API_MODE_TSK");
              
              if(TIMM_OSAL_ERR_NONE != threadRes)
              {
                PRINT(PRINT_ERR_MSG,
                      "Unable to start H264 video decode thread\n");
                return ERR_INVNAME;
              }
              else
              {
                /* set flag used to determine Func/API thread is created or 
                   not */
                h264VdecClient->flagThrCreated = OMX_TRUE;
              }

            } // if( OMX_FALSE == h264VdecClient->inPortParams.flagInputEos ...
            else
            {
              PRINT(PRINT_MUST,
                      "In <%s>: EOS is reached\n", __func__);

              /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
              TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            }
            
          } // if(!strcmp(componentRequested,OMTB_H264_VDEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_AD_G711          
          else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR) &&
                  OMX_FALSE == ((OMTB_G711_ADEC_PRIVATE*)\
                  (omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo])\
                  )->flagThrCreated)
          {
            g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*) 
                omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];

            if(OMX_FALSE == g711AdecClient->inPortParams.flagInputEos &&
               OMX_FALSE == g711AdecClient->outPortParams.flagEos)
            {
              funcThreadArg[instanceNo].g711AdecClient = g711AdecClient;
                                
              funcThreadArg[instanceNo].audPlayDrvClient =
                  omtbClass[OMTB_G711_ADEC_CLASS].audPlayDrvClient[instanceNo];

              /* Create execution thread for API mode */
              threadRes = TIMM_OSAL_CreateTask(
                (void*)&( omtbClass[OMTB_G711_ADEC_CLASS].execThreadId[\
                          instanceNo]),
                          (void *)G711ADEC_ApiTsk,
                          0,
                          &funcThreadArg[instanceNo],
                          OMTB_G711_ADEC_TSK_STACK_SIZE,
                          OMTB_G711_ADEC_GROUP_PRIORITY,
                          (signed char*)"G711_ADEC_OMX_API_MODE_TSK");
              
              if(TIMM_OSAL_ERR_NONE != threadRes) 
              {
                PRINT(PRINT_ERR_MSG,
                      "Unable to start G711 audio decode thread\n");
                return ERR_INVNAME;
              }
              else
              {
                /* set flag used to determine Func/API thread is created or 
                   not */
                g711AdecClient->flagThrCreated = OMX_TRUE;
              }
              
            } // if(OMX_FALSE == g711AdecClient->inPortParams.flagInputEos ...
            else
            {
              PRINT(PRINT_MUST,
                      "In <%s>: EOS is reached\n", __func__);

              /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
              TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            }
            
          } // else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VE_H264
          else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR) &&
              OMX_FALSE == ((OMTB_H264_VENC_PRIVATE*)
                omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo])\
                ->flagThrCreated)
          {
            h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
                omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];

            if( OMX_FALSE == h264VencClient->inPortParams.flagInputEos  &&
                OMX_FALSE == h264VencClient->outPortParams.flagEos)
            {
              funcThreadArg[instanceNo].h264VencClient = h264VencClient;
                                
              /* Create execution thread for API mode */
              threadRes = TIMM_OSAL_CreateTask(
                (void*)&( omtbClass[OMTB_H264_VENC_CLASS].execThreadId[\
                          instanceNo]),
                          (void *)H264VENC_ApiTsk,
                          0,
                          &funcThreadArg[instanceNo],
                          OMTB_H264_VENC_TSK_STACK_SIZE,
                          OMTB_H264_VENC_GROUP_PRIORITY,
                          (signed char*)"H264_VENC_OMX_API_MODE_TSK");
              
              if(TIMM_OSAL_ERR_NONE != threadRes)
              {
                PRINT(PRINT_ERR_MSG,
                      "Unable to start H264 video encode thread\n");
                return ERR_INVNAME;
              }
              else
              {
                /* set flag used to determine Func/API thread is created or 
                   not */
                h264VencClient->flagThrCreated = OMX_TRUE;
              }

            } // if( OMX_FALSE == h264VencClient->inPortParams.flagInputEos ...
            else
            {
              PRINT(PRINT_MUST,
                      "In <%s>: EOS is reached\n", __func__);

              /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
              TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            }
            
          } // else if(!strcmp(componentRequested,OMTB_H264_VENC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FCC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR) &&
              OMX_FALSE == ((OMTB_VPSS_VFCC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo])\
                ->flagThrCreated)
          {
            vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];

            if( OMX_FALSE == vpssVfccClient->inPortParams.flagInputEos  &&
                OMX_FALSE == vpssVfccClient->outPortParams.flagEos)
            {
              funcThreadArg[instanceNo].vpssVfccClient = vpssVfccClient;
                                
              /* Create execution thread for API mode */
              threadRes = TIMM_OSAL_CreateTask(
                (void*)&( omtbClass[OMTB_VPSS_VFCC_CLASS].execThreadId[\
                          instanceNo]),
                          (void *)VPSSVFCC_ApiTsk,
                          0,
                          &funcThreadArg[instanceNo],
                          OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                          OMTB_VPSS_VFCC_GROUP_PRIORITY,
                          (signed char*)"VPSS_VFCC_OMX_API_MODE_TSK");
              
              if(TIMM_OSAL_ERR_NONE != threadRes)
              {
                PRINT(PRINT_ERR_MSG,
                      "Unable to start vpss vfcc thread\n");
                return ERR_INVNAME;
              }
              else
              {
                /* set flag used to determine Func/API thread is created or 
                   not */
                vpssVfccClient->flagThrCreated = OMX_TRUE;
              }

            } // if( OMX_FALSE == vpssVfccClient->inPortParams.flagInputEos ...
            else
            {
              PRINT(PRINT_MUST,
                      "In <%s>: EOS is reached\n", __func__);

              /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
              TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            }
            
          } // else if(!strcmp(componentRequested,OMTB_VPSS_VFCC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FDC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR) &&
              OMX_FALSE == ((OMTB_VPSS_VFDC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo])\
                ->flagThrCreated)
          {
            vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];

            if( OMX_FALSE == vpssVfdcClient->inPortParams.flagInputEos  &&
                OMX_FALSE == vpssVfdcClient->outPortParams.flagEos)
            {
              funcThreadArg[instanceNo].vpssVfdcClient = vpssVfdcClient;
                                
              /* Create execution thread for API mode */
              threadRes = TIMM_OSAL_CreateTask(
                (void*)&( omtbClass[OMTB_VPSS_VFDC_CLASS].execThreadId[\
                          instanceNo]),
                          (void *)VPSSVFDC_ApiTsk,
                          0,
                          &funcThreadArg[instanceNo],
                          OMTB_VPSS_VFDC_TSK_STACK_SIZE,
                          OMTB_VPSS_VFDC_GROUP_PRIORITY,
                          (signed char*)"VPSS_VFDC_OMX_API_MODE_TSK");
              
              if(TIMM_OSAL_ERR_NONE != threadRes)
              {
                PRINT(PRINT_ERR_MSG,
                      "Unable to start vpss vfdc thread\n");
                return ERR_INVNAME;
              }
              else
              {
                /* set flag used to determine Func/API thread is created or 
                   not */
                vpssVfdcClient->flagThrCreated = OMX_TRUE;
              }

            } // if( OMX_FALSE == vpssVfdcClient->inPortParams.flagInputEos ...
            else
            {
              PRINT(PRINT_MUST,
                      "In <%s>: EOS is reached\n", __func__);

              /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
              TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);
            }
            
          } // else if(!strcmp(componentRequested,OMTB_VPSS_VFDC_COMP_REF_STR))
#endif
          else
          {
            /* Don't create thread for exec -> idle -> exec transition, only 
               perform state tansition */
#ifdef OMTB_FF_VD_H264
            if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
            {
              h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
                  omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
              
              TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);

              /* Use initial I/O buffers & make empty/fill this buffer call */
              err = H264VDEC_UseInitialResources(h264VdecClient);
              
              if(OMX_ErrorNone != err)
              {
                OMTB_OmxReturnStatus(err);
              }
            }
#endif
#ifdef OMTB_FF_AD_G711
            else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
            {
              g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
                  omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
              
              TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
                            
              /* Use initial I/O buffers & make empty/fill this buffer call */
              err = G711ADEC_UseInitialResources(g711AdecClient);
              
              if(OMX_ErrorNone != err)
              {
                OMTB_OmxReturnStatus(err);
              }
              
            } // else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF ...
#endif
#ifdef OMTB_FF_VE_H264
            else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
            {
              h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
                  omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
              
              TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);

              /* Use initial I/O buffers & make empty/fill this buffer call */
              err = H264VENC_UseInitialResources(h264VencClient);
              
              if(OMX_ErrorNone != err)
              {
                OMTB_OmxReturnStatus(err);
              }
              
            } // else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF ...
#endif
#ifdef OMTB_FF_VPS_FCC
            else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
            {
              vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
                  omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
              
              TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);

              /* Use initial I/O buffers & make empty/fill this buffer call */
              err = VPSSVFCC_UseInitialResources(vpssVfccClient);
              
              if(OMX_ErrorNone != err)
              {
                OMTB_OmxReturnStatus(err);
              }
              
            } // else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF ...
#endif
#ifdef OMTB_FF_VPS_FDC
            else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
            {
              vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
                  omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
              
              TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);

              /* Use initial I/O buffers & make empty/fill this buffer call */
              err = VPSSVFDC_UseInitialResources(vpssVfdcClient);
              
              if(OMX_ErrorNone != err)
              {
                OMTB_OmxReturnStatus(err);
              }
              
            } // else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF ...
#endif            
          } // // else if(!strcmp(componentRequested,OMTB_VPSS_VFDC ... else ...
          
        } // if(OMX_StatePause != prevState)
        else
        {
          /* Don't create thread for pause -> exec transition, only perform
             state tansition */
#ifdef OMTB_FF_VD_H264
          if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
          {
            h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
                omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
            
            TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);

            /* Use initial I/O buffers & make empty/fill this buffer call */
            err = H264VDEC_UseInitialResources(h264VdecClient);
            
            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
            }
          }
#endif
#ifdef OMTB_FF_AD_G711
          else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
          {
            g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
                omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
            
            TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);

            /* Use initial I/O buffers & make empty/fill this buffer call */
            err = G711ADEC_UseInitialResources(g711AdecClient);
            
            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
            }
            
          } // else if(!strcmp(componentRequested,OMTB_G711_ADEC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VE_H264
          else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
          {
            h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
                omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
            
            TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);

            /* Use initial I/O buffers & make empty/fill this buffer call */
            err = H264VENC_UseInitialResources(h264VencClient);
            
            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
            }
            
          } // else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FCC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
          {
            vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
            
            TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);

            /* Use initial I/O buffers & make empty/fill this buffer call */
            err = VPSSVFCC_UseInitialResources(vpssVfccClient);
            
            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
            }
            
          } // else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
#endif
#ifdef OMTB_FF_VPS_FDC
          else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
          {
            vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
                omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
            
            TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);

            /* Use initial I/O buffers & make empty/fill this buffer call */
            err = VPSSVFDC_UseInitialResources(vpssVfdcClient);
            
            if(OMX_ErrorNone != err)
            {
              OMTB_OmxReturnStatus(err);
            }
            
          } // else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
#endif          
        } // if(OMX_StatePause != prevState) ... else ...
      
        break;

      case OMX_StatePause:

        /* Wait for state transition to complete */
#ifdef OMTB_FF_VD_H264
        if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
        {
          h264VdecClient =  (OMTB_H264_VDEC_PRIVATE*)
              omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];
          
          TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
        }
#endif
#ifdef OMTB_FF_AD_G711
        else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
        {
          g711AdecClient =  (OMTB_G711_ADEC_PRIVATE*)
              omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
          
          TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem,
                  TIMM_OSAL_SUSPEND);
        }
#endif
#ifdef OMTB_FF_VE_H264
        else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
        {
          h264VencClient =  (OMTB_H264_VENC_PRIVATE*)
              omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
          
          TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem,
                  TIMM_OSAL_SUSPEND);
        }
#endif
#ifdef OMTB_FF_VPS_FCC
        else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
        {
          vpssVfccClient =  (OMTB_VPSS_VFCC_PRIVATE*)
              omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
          
          TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem,
                  TIMM_OSAL_SUSPEND);
        }
#endif
#ifdef OMTB_FF_VPS_FDC
        else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
        {
          vpssVfdcClient =  (OMTB_VPSS_VFDC_PRIVATE*)
              omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
          
          TIMM_OSAL_SemaphoreObtain(vpssVfdcClient->eventSem,
                  TIMM_OSAL_SUSPEND);
        }
#endif        
        break;

      case OMX_StateWaitForResources:

        /* Nothing to do as of now */
        break;

      default:

        /* Nothing to do as of now */
        break;

    } // switch(nParam)
    
    return ERR_NOERR;

  } // if(OMX_ErrorNone != err) ... else ...

}


/******************************************************************************\
 *      OMTB_OmxSendCommandPort Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMX_SendCommand
 *            API for requested component and port.
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
OMTB_OmxSendCommandPort(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_COMMANDTYPE commandType = OMX_CommandPortEnable;
  unsigned int instanceNo = 0;
  OMX_STATETYPE nParam = 0;
  OMX_PTR pCmdData = NULL;
  OMX_HANDLETYPE handle = NULL;
  char cmdName[OMTB_MAX_LEN] = {'\0'};
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Enable or disable the port of a component */
  /* Read command params */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, cmdName);
  if (ERR_NOERR != result)
  {
    if (result == ERR_INVNAME)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }
  
  if(0 == strcmp(cmdName, "enable"))
  {
    commandType = OMX_CommandPortEnable;
  }
  else if(0 == strcmp(cmdName, "disable"))
  {
    commandType = OMX_CommandPortDisable;
  }
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }
  
  pCmdData = NULL;
      
  /* Read port Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&nParam);
  if (ERR_NOERR != result)
  {
    return result;
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to enable/disable the port */
  err = OMX_SendCommand(handle, commandType, nParam, pCmdData);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC(
            "OMX_SendCommand - OMX_CommandPortEnable/OMX_CommandPortDisable");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    return ERR_NOERR;

  } // if(OMX_ErrorNone != err) ... else ...

}


/******************************************************************************\
 *      OMTB_OmxSendCommandFlush Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMX_SendCommand
 *            API for requested component and flushes the buffer of particular
 *            port.
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
OMTB_OmxSendCommandFlush(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_COMMANDTYPE commandType = OMX_CommandFlush;
  unsigned int instanceNo = 0;
  OMX_STATETYPE nParam = 0;
  OMX_PTR pCmdData = NULL;
  OMX_HANDLETYPE handle = NULL;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Flush the buufer on specific port of the component */
  /* Read port Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&nParam);
  if (ERR_NOERR != result)
  {
    return result;
  }
  commandType = OMX_CommandFlush;
  pCmdData = NULL;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to flush the buffer on specified port */
  err = OMX_SendCommand(handle, commandType, nParam, pCmdData);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand - OMX_CommandFlush");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    return ERR_NOERR;
    
  } // if(OMX_ErrorNone != err) ... else ...

}


/******************************************************************************\
 *      OMTB_OmxSendCommandMark Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes OMX_SendCommand
 *            API for requested component and marks particular buffer.
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
OMTB_OmxSendCommandMark(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_COMMANDTYPE commandType = OMX_CommandMarkBuffer;
  unsigned int instanceNo = 0;
  OMX_STATETYPE nParam = 0;
  OMX_PTR pCmdData = NULL;
  OMX_HANDLETYPE handle = NULL;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  OMX_MARKTYPE omxMarkType;
  char markComponentName[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  unsigned int markCompInstanceNo = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Mark a buffer and specify which other component will raise the event */
  /* mark received */

  /* Read component name which will mark the buffer */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, markComponentName);
  if (ERR_NOERR != result)
  {
    if (result == ERR_INVNAME)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number of component which will mark the buffer */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine,
                          (UINT32 *)&markCompInstanceNo);
  if (ERR_NOERR != result)
  {

    return result;
  }

  /* Update the mark component and data based on the component specified */
 #ifdef OMTB_FF_VD_H264
  if(!strcmp(markComponentName, OMTB_H264_VDEC_COMP_REF_STR))
  {
    omxMarkType.hMarkTargetComponent =
        omtbClass[OMTB_H264_VDEC_CLASS].handle[markCompInstanceNo];

    omxMarkType.pMarkData =
        omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[markCompInstanceNo];
  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(markComponentName, OMTB_G711_ADEC_COMP_REF_STR))
  {
    omxMarkType.hMarkTargetComponent =
        omtbClass[OMTB_G711_ADEC_CLASS].handle[markCompInstanceNo];

    omxMarkType.pMarkData =
        omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[markCompInstanceNo];
  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(markComponentName, OMTB_H264_VENC_COMP_REF_STR))
  {
    omxMarkType.hMarkTargetComponent =
        omtbClass[OMTB_H264_VENC_CLASS].handle[markCompInstanceNo];

    omxMarkType.pMarkData =
        omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[markCompInstanceNo];
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(markComponentName, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    omxMarkType.hMarkTargetComponent =
        omtbClass[OMTB_VPSS_VFCC_CLASS].handle[markCompInstanceNo];

    omxMarkType.pMarkData =
        omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[markCompInstanceNo];
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(markComponentName, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    omxMarkType.hMarkTargetComponent =
        omtbClass[OMTB_VPSS_VFDC_CLASS].handle[markCompInstanceNo];

    omxMarkType.pMarkData =
        omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[markCompInstanceNo];
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  pCmdData = &omxMarkType;
  commandType = OMX_CommandMarkBuffer;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to mark the buffer by specified component */
  err = OMX_SendCommand(handle, commandType, nParam, pCmdData);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand - OMX_CommandMarkBuffer");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    return ERR_NOERR;
    
  } // if(OMX_ErrorNone != err) ... else ...

}


/******************************************************************************\
 *      OMTB_OmxGetParam Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes GetParameter
 *            API for requested component
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
OMTB_OmxGetParam(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  unsigned int template = 0, portIdx = 0;
  unsigned int instanceNo = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_HANDLETYPE handle = NULL;
  void *pCompCfgStruct = NULL;
  unsigned int nConfigIndex = OMX_IndexMax;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char indexStr[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Read Template Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  {
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG,
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read port index Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&portIdx);
  if (ERR_NOERR != result)
  {
    return result;
  }
  
  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read cfg structure index string */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, indexStr);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Invoke appropriate components's GetParam function */
#ifdef OMTB_FF_VD_H264
  if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VDEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = G711ADEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VENC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFCC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFDC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  if(OMTB_SUCCESS != err)
  {
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Send the portInit parameter structure */
  err = OMX_GetParameter(handle, nConfigIndex, pCompCfgStruct);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxGetConfig Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function parses the command and invokes GetConfig
 *            API for requested component
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
OMTB_OmxGetConfig(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  unsigned int template = 0, portIdx = 0;
  unsigned int instanceNo = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_HANDLETYPE handle = NULL;
  void *pCompCfgStruct = NULL;
  unsigned int nConfigIndex = OMX_IndexMax;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char indexStr[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read component type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    return result;
  }

  /* Read Template Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  {
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG,
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read port index Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&portIdx);
  if (ERR_NOERR != result)
  {
    return result;
  }
  
  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read cfg structure index string */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, indexStr);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
     return result;
  }

  /* Invoke appropriate components's GetHandle function */
#ifdef OMTB_FF_VD_H264
  if(!strcmp(componentRequested, OMTB_H264_VDEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VDEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_AD_G711
  else if(!strcmp(componentRequested, OMTB_G711_ADEC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = G711ADEC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);

  }
#endif
#ifdef OMTB_FF_VE_H264
  else if(!strcmp(componentRequested, OMTB_H264_VENC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = H264VENC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FCC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFCC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
#ifdef OMTB_FF_VPS_FDC
  else if(!strcmp(componentRequested, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    /* Get the cfg structure index and pointer */
    err = VPSSVFDC_GetStructIdx(indexStr, 
                                template,
                                portIdx,
                                &nConfigIndex,
                                &pCompCfgStruct);
  }
#endif
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  if(OMTB_SUCCESS != err)
  {
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Send the portInit parameter structure */
  err = OMX_GetConfig(handle, nConfigIndex, pCompCfgStruct);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetConfig");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxGetState Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function will give the current state of the component.
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
OMTB_OmxGetState(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int instanceNo = 0;
  OMX_HANDLETYPE handle= NULL;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char stateStr[OMTB_MAX_LEN] = {"\0"};
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Get the current state of the component */
  err = OMX_GetState(handle, &pState);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetState");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;

    /* get equivalent string for the state */
    strcpy(stateStr, OMTB_StateToStr(pState));

    PRINT(PRINT_MUST, "<component, instance , state> ==> <%s, %d , %s>\n", 
    componentRequested, instanceNo, stateStr);
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OmxGetExtensionIndex Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function translates the vendor specific structure string to 
 *            openmax standard structure index.
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
OMTB_OmxGetExtensionIndex(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int instanceNo = 0;
  OMX_HANDLETYPE handle = NULL;
  char componentRequested[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  char vendorSpecificStr[OMX_MAX_STRINGNAME_SIZE]  = {"\0"};
  void *pAppData = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentRequested);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Read instance Number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* Read vendor specific structure string */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, vendorSpecificStr);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  /* Get the compoent handle from instance management structure */
  handle = OMTB_GetCompHandle(componentRequested, instanceNo);

  if(NULL == handle)
  {
    PRINT(PRINT_ERR_MSG, "Invalid parameters - %s component instance \
%d might not be created !!!\n", componentRequested, instanceNo);

    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Fet the extension index for the vendor specific structure string */
  err = OMX_GetExtensionIndex(handle, vendorSpecificStr, pAppData);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetExtensionIndex");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else
  {
    OMTB_OmxReturnStatus(err);
    result = ERR_NOERR;
    
    PRINT(PRINT_MUST, "GetExtensionIndex for <component, instance #, string>:\
 <%s , %d , %s>\n", componentRequested, instanceNo, vendorSpecificStr);
  }

  return result;
}


/******************************************************************************\
 *      OMTB_OMXComponentNameEnum Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function enumerates through all the names of recognized 
 *            components in the system to detect all the components in the 
 *            system run-time.
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
OMTB_OMXComponentNameEnum(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  char name[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  int index = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  MX_Result result = ERR_NOERR;

  PRINT(PRINT_FUNCTION_NAME, "Entering <function>: <%s>\n", __func__);
  
  while(OMTB_LOOP) 
  {
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
    
    /* enumerates through all the names of recognized components in system */
    err = OMX_ComponentNameEnum (name, OMX_MAX_STRINGNAME_SIZE, index);

    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_ComponentNameEnum");
    
    if ((NULL != name) && (OMX_ErrorNone == err))
    {
      PRINT(PRINT_MUST, "Component %i: %s\n", index, name);
    }
    else if (OMX_ErrorNoMore == err)
    {
      OMTB_OmxReturnStatus(err);
      break;
    }
    else
      break;

    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      result = ERR_INVNAME;
      break;
    }
    
    index++;
  } // while(OMTB_LOOP)
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <function>: <%s>\n", __func__);
  return result;
}


/******************************************************************************\
 *      OMTB_OMXRoleEnum Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function queries all the roles fulfilled by a given a 
 *            component.
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
OMTB_OMXRoleEnum(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
) 
{
  OMX_U32 numOfRoles = 0;
  OMX_U8 **strOfRoles = NULL;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  int index = 0;
  char componentName[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  MX_Result result = ERR_NOERR;

  PRINT(PRINT_FUNCTION_NAME, "Entering <function>: <%s>\n", __func__);

  /* Read component name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, componentName);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Get number of roles of component */
  err = OMX_GetRolesOfComponent(componentName, &numOfRoles, NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetRolesOfComponent");
  
  if (OMX_ErrorNone != err) 
  {
    PRINT(PRINT_ERR_MSG, 
        "In %s: Not able to retrieve number of roles of the given component\n",
        __func__);
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
    return result;
  }
  PRINT(PRINT_MUST, "<component , # of roles> : <%s , %i>\n",
                    componentName, (int) numOfRoles);

  if(0 == numOfRoles)
  {
    PRINT(PRINT_ERR_MSG, 
      "The Number or roles is 0\nThe component selected is not correct for \
the purpose of this test\n");    
    OMTB_OmxReturnStatus(err);
  }
  else 
  {
    /* allocate the memory based on the number of roles of the component */
    strOfRoles = (OMX_U8**)
                 TIMM_OSAL_Malloc( numOfRoles * sizeof(OMX_STRING),
                                   TIMM_OSAL_TRUE,
                                   0,
                                   TIMMOSAL_MEM_SEGMENT_EXT);
    for (index = 0; index < numOfRoles; index++)
    {
      *(strOfRoles + index) = (OMX_U8*)
                 TIMM_OSAL_Malloc( numOfRoles * OMX_MAX_STRINGNAME_SIZE,
                                   TIMM_OSAL_TRUE,
                                   0,
                                   TIMMOSAL_MEM_SEGMENT_EXT);
    }

    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
    
    /* Get the role string for the component */
    err = OMX_GetRolesOfComponent(componentName, &numOfRoles, strOfRoles);

    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetRolesOfComponent");
    
    if (OMX_ErrorNone != err) 
    {
      PRINT(PRINT_ERR_MSG, "Not able to retrieve the roles of the component\n");
      OMTB_OmxReturnStatus(err);
      result = ERR_INVNAME;
    }
    else if(NULL != strOfRoles)
    {
      /* Display the component roles */
      for (index = 0; index < numOfRoles; index++)
      {
        PRINT(PRINT_MUST, "<Role # , Component Role> : <%i , %s>\n", 
                                (index + 1), *(strOfRoles + index));
      }
    }
    else
    {
      PRINT(PRINT_ERR_MSG, "Role string is NULL!!!\n");
      err = OMX_ErrorInvalidComponentName;
      OMTB_OmxReturnStatus(err);
      result = ERR_INVNAME;
    }

    /* Release the memory allocated for the number of roles of the component */
    for (index = 0; index < numOfRoles; index++)
    {
      TIMM_OSAL_Free(*(strOfRoles + index));
    }
    TIMM_OSAL_Free(strOfRoles);
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <function>: <%s>\n", __func__);
  return result;
}


/******************************************************************************\
 *      OMTB_OMXComponentEnumByRole Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function queries the names of all installed components that 
 *            support a given role.
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
OMTB_OMXComponentEnumByRole(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  OMX_U32 numCompPerRole = 0;
  OMX_U8 **strOfCompPerRole = NULL;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  int index = 0;
  char roleName[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  MX_Result result = ERR_NOERR;

  PRINT(PRINT_FUNCTION_NAME, "Entering <function>: <%s>\n", __func__);

  /* Read component role name */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, roleName);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Getting number of components per role for given role name */
  err = OMX_GetComponentsOfRole(roleName, &numCompPerRole, NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetComponentsOfRole");
  
  if (OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, 
          "Not able to retrieve number of components of a given role\n");
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
    return result;
  }

  /* allocate the memory to hold name of the component supporting given role */
  strOfCompPerRole = (OMX_U8**)
                     TIMM_OSAL_Malloc( numCompPerRole * sizeof(OMX_STRING),
                                       TIMM_OSAL_TRUE,
                                       0,
                                       TIMMOSAL_MEM_SEGMENT_EXT);

  for (index = 0; index < numCompPerRole; index++)
  {
    strOfCompPerRole[index] = (OMX_U8*)
                      TIMM_OSAL_Malloc( OMX_MAX_STRINGNAME_SIZE,
                                        TIMM_OSAL_TRUE,
                                        0,
                                        TIMMOSAL_MEM_SEGMENT_EXT);
  }
  
  PRINT(PRINT_MUST, "Number of components per role <%s> : %i\n", 
        roleName, (int)numCompPerRole);

  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Get all the components supporting given role */
  err = OMX_GetComponentsOfRole(roleName, &numCompPerRole, strOfCompPerRole);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetComponentsOfRole");
  
  if (OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, 
          "Not able to retrieve the components of a given role\n");
    OMTB_OmxReturnStatus(err);
    result = ERR_INVNAME;
  }
  else if(0 < numCompPerRole)
  {
    /* Display the name of the components supporting given role */
    PRINT(PRINT_MUST, "The components supporting given role <%s> are:\n",
          roleName);
    for (index = 0; index < numCompPerRole; index++)
    {
      PRINT(PRINT_MUST, "%s\n", strOfCompPerRole[index]);
    }
  }
  
  /* Release the memory allocated for name of he component */
  for (index = 0; index < numCompPerRole; index++)
  {
    if(strOfCompPerRole[index])
    {
      TIMM_OSAL_Free(strOfCompPerRole[index]);
      strOfCompPerRole[index] = NULL;
    }
  }

  if(strOfCompPerRole)
  {
    TIMM_OSAL_Free(strOfCompPerRole);
    strOfCompPerRole = NULL;
  }

  PRINT(PRINT_FUNCTION_NAME, "Leaving <function>: <%s>\n", __func__);
  return result;
}


/******************************************************************************\
 *      OMTB_OmxDeinit Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes OMX_Deinit API
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
OMTB_OmxDeinit(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result  result = ERR_NOERR;;
  OMX_ERRORTYPE omxStatus = OMX_ErrorNone;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* OpenMax core uninitialization */
  omxStatus = OMX_Deinit ();

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_Deinit");

  if(OMX_ErrorNone == omxStatus)
  {
    OMTB_OmxReturnStatus(omxStatus);
  }
  else
  {
    OMTB_OmxReturnStatus(omxStatus);
  }

  return (result);
}


#ifdef OMTB_FF_VD_H264
/******************************************************************************\
 *      OMTB_FuncViddec Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes the H264VDEC_FuncTsk Thread
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


MX_Result OMTB_FuncViddec(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0, instanceNo = 0;
  int threadRes = TIMM_OSAL_ERR_NONE;
  char codecType[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  
  /* Read the temmplate number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, 
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read the instance number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  /* read the codec type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, codecType);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {    
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  if(!strcmp(codecType, OMTB_H264_VDEC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_H264_VDEC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG, 
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    /* Update the config param pointers based on the template number */
    h264VdecConfig = (OMTB_H264_VDEC_CONFIG_PARAMS*)
                          &(omtbCfgTemplate[template]->h264VdecTemplate);
#ifdef OMTB_FF_DRIVER_DISPLAY
    dssConfig = (OmtbDispInfo *)&(omtbCfgTemplate[template]->v4l2DispTemplate);
#endif
    /* Create the component instance */
    result = H264VDEC_GetHandle(&omtbClass[OMTB_H264_VDEC_CLASS], instanceNo);
    
    if(OMX_ErrorNone != result)
    {
      PRINT(PRINT_ERR_MSG, "Error in %s instance creation\n", codecType);
      return ((MX_Result)ERR_INVNAME);
    }
    
    funcThreadArg[instanceNo].h264VdecClient = (OMTB_H264_VDEC_PRIVATE*)
        omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[instanceNo];

#ifdef OMTB_FF_DRIVER_DISPLAY                    
    funcThreadArg[instanceNo].dssClient = 
        omtbClass[OMTB_H264_VDEC_CLASS].dssClient[instanceNo];
#endif
    /* create the execution thread for func mode */
    threadRes = TIMM_OSAL_CreateTask(
              (void*)&omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[instanceNo],
                        (void *)H264VDEC_FuncTsk,
                        0,
                        &funcThreadArg[instanceNo],
                        OMTB_H264_VDEC_TSK_STACK_SIZE,
                        OMTB_H264_VDEC_GROUP_PRIORITY,
                        (signed char*)"H264_VDEC_OMX_FUNC_MODE_TSK");
              
    if(TIMM_OSAL_ERR_NONE != threadRes)
    {
      PRINT(PRINT_ERR_MSG, "Unable to start %s thread\n", codecType);
    }
    else
    {
      /* set flag used to determine Func/API thread is created or not */
      ((OMTB_H264_VDEC_PRIVATE*)\
      (omtbClass[OMTB_H264_VDEC_CLASS].h264VdecClient[\
            instanceNo]))->flagThrCreated = OMX_TRUE;
    }

  }
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    PRINT(PRINT_ERR_MSG, "Wrong video decoder type selected\n");
    return ERR_INVNAME;
  }

  return ERR_NOERR;
}
#endif

#ifdef OMTB_FF_AD_G711
/******************************************************************************\
 *      OMTB_FuncAuddec Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes the G711ADEC_FuncTsk Thread
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


MX_Result OMTB_FuncAuddec(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0, instanceNo = 0;
  int threadRes = TIMM_OSAL_ERR_NONE;
  char codecType[OMX_MAX_STRINGNAME_SIZE] = {"\0"};

  /* Read the temmplate number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, 
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read the instance number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  /* read the codec type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, codecType);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {    
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  if(!strcmp(codecType, OMTB_G711_ADEC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_G711_ADEC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG, 
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }

    /* Update the config param pointers based on the template number */    
    g711AdecConfig = (OMTB_G711_ADEC_CONFIG_PARAMS*)
                        &(omtbCfgTemplate[template]->g711AdecTemplate);
                        
#ifdef OMTB_FF_DRIVER_AUDIO
    alsaConfig = (OmtbAudInfo *)&(omtbCfgTemplate[template]->alsaPlayTemplate);
#endif
    /* Create the component instance */
    result = G711ADEC_GetHandle(&omtbClass[OMTB_G711_ADEC_CLASS], instanceNo);
    
    if(OMX_ErrorNone != result)
    {
      PRINT(PRINT_ERR_MSG, "Error in %s instance creation\n", codecType);
      return ((MX_Result)ERR_INVNAME);
    }
    
    funcThreadArg[instanceNo].g711AdecClient = (OMTB_G711_ADEC_PRIVATE*)
        omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[instanceNo];
                    
    funcThreadArg[instanceNo].audPlayDrvClient = 
        omtbClass[OMTB_G711_ADEC_CLASS].audPlayDrvClient[instanceNo];

    /* create the execution thread for func mode */
    threadRes = TIMM_OSAL_CreateTask(
            (void*)&( omtbClass[OMTB_G711_ADEC_CLASS].execThreadId[instanceNo]),
                      (void *)G711ADEC_FuncTsk,
                      0,
                      &funcThreadArg[instanceNo],
                      OMTB_G711_ADEC_TSK_STACK_SIZE,
                      OMTB_G711_ADEC_GROUP_PRIORITY,
                      (signed char*)"G711_ADEC_OMX_FUNC_MODE_TSK");
              
    if(TIMM_OSAL_ERR_NONE != threadRes)
    {
      PRINT(PRINT_ERR_MSG, "Unable to start %s thread\n", codecType);
    }
    else
    {
      /* set flag used to determine Func/API thread is created or not */
      ((OMTB_G711_ADEC_PRIVATE*)\
      (omtbClass[OMTB_G711_ADEC_CLASS].g711AdecClient[\
          instanceNo]))->flagThrCreated = OMX_TRUE;
    }
    
  }
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    PRINT(PRINT_ERR_MSG, "Wrong audio decoder type selected\n");
    return ERR_INVNAME;
  }

  return ERR_NOERR;
}
#endif

#ifdef OMTB_FF_VE_H264
/******************************************************************************\
 *      OMTB_FuncVidenc Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes the H264VENC_FuncTsk Thread
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


MX_Result OMTB_FuncVidenc(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0, instanceNo = 0;
  int threadRes = TIMM_OSAL_ERR_NONE;
  char codecType[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  
  /* Read the temmplate number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, 
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read the instance number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  /* read the codec type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, codecType);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {    
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  if(!strcmp(codecType, OMTB_H264_VENC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_H264_VENC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG, 
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    /* Update the config param pointers based on the template number */
    h264VencConfig = (OMTB_H264_VENC_CONFIG_PARAMS*)
                          &(omtbCfgTemplate[template]->h264VencTemplate);

    /* Create the component instance */
    result = H264VENC_GetHandle(&omtbClass[OMTB_H264_VENC_CLASS], instanceNo);
    
    if(OMX_ErrorNone != result)
    {
      PRINT(PRINT_ERR_MSG, "Error in %s instance creation\n", codecType);
      return ((MX_Result)ERR_INVNAME);
    }
    
    funcThreadArg[instanceNo].h264VencClient = (OMTB_H264_VENC_PRIVATE*)
        omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[instanceNo];
                    
    /* create the execution thread for func mode */
    threadRes = TIMM_OSAL_CreateTask(
              (void*)&omtbClass[OMTB_H264_VENC_CLASS].execThreadId[instanceNo],
                        (void *)H264VENC_FuncTsk,
                        0,
                        &funcThreadArg[instanceNo],
                        OMTB_H264_VENC_TSK_STACK_SIZE,
                        OMTB_H264_VENC_GROUP_PRIORITY,
                        (signed char*)"H264_VENC_OMX_FUNC_MODE_TSK");
              
    if(TIMM_OSAL_ERR_NONE != threadRes)
    {
      PRINT(PRINT_ERR_MSG, "Unable to start %s thread\n", codecType);
    }
    else
    {
      /* set flag used to determine Func/API thread is created or not */
      ((OMTB_H264_VENC_PRIVATE*)\
      (omtbClass[OMTB_H264_VENC_CLASS].h264VencClient[\
            instanceNo]))->flagThrCreated = OMX_TRUE;
    }

  }
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    PRINT(PRINT_ERR_MSG, "Wrong video encoder type selected\n");
    return ERR_INVNAME;
  }

  return ERR_NOERR;
}

#endif
#ifdef OMTB_FF_VPS_FCC
/******************************************************************************\
 *      OMTB_FuncVpss Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes the VPSSVFCC_FuncTsk & VPSSVFDC_FuncTsk 
 *            Thread
 *
 *  @param    Pointer to MX_DBG_CMD_CONTEXT_T structure
 *            Pointer to MX_DBG_CMD_Entry_T structure
 *            Character Double Pointer containing the command
 *
 *  @return   ERR_NOERR           successful,
 *            ERR_END_OF_CMD      help was requested,
 *            ERR_INVNAME         otherwise.
 */


MX_Result OMTB_FuncVpss(
  MX_DBG_CMD_CONTEXT_T*   pContext,
  MX_DBG_CMD_ENTRY_T*     pCmdEntry,
  char**                  ppCmdLine
)
{
  MX_Result result = ERR_NOERR;
  unsigned int template = 0, instanceNo = 0;
  int threadRes = TIMM_OSAL_ERR_NONE;
  char codecType[OMX_MAX_STRINGNAME_SIZE] = {"\0"};
  
  /* Read the temmplate number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32 *)&template);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  if(OMTB_MAX_TEMPLATE <= template)
  {
    /* Invalid template */
    PRINT(PRINT_ERR_MSG, 
          "Invalid template number: %d - Valid range: <0 to %d>\n",
          template, OMTB_MAX_TEMPLATE - 1);
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return ERR_INVNAME;
  }

  /* Read the instance number */
  result = XdbgCmdNextNum(pContext, pCmdEntry, ppCmdLine, (UINT32*)&instanceNo);
  if (ERR_NOERR != result)
  { 
    return result;
  }

  /* read the codec type */
  result = XdbgCmdNextVar(pCmdEntry, ppCmdLine, codecType);
  if (ERR_NOERR != result)
  {
    if (ERR_INVNAME == result)
    {    
      XdbgCmdSyntaxShow(pContext, pCmdEntry);
    }
    return result;
  }

  if(!strcmp(codecType, OMTB_VPSS_VFCC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_VPSS_VFCC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG, 
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    /* Update the config param pointers based on the template number */
    vpssVfccConfig = (OMTB_VPSS_VFCC_CONFIG_PARAMS*)
                          &(omtbCfgTemplate[template]->vpssVfccTemplate);

    /* Create the component instance */
    result = VPSSVFCC_GetHandle(&omtbClass[OMTB_VPSS_VFCC_CLASS], instanceNo);
    
    if(OMX_ErrorNone != result)
    {
      PRINT(PRINT_ERR_MSG, "Error in %s instance creation\n", codecType);
      return ((MX_Result)ERR_INVNAME);
    }
    
    funcThreadArg[instanceNo].vpssVfccClient = (OMTB_VPSS_VFCC_PRIVATE*)
        omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[instanceNo];
                    
    /* create the execution thread for func mode */
    threadRes = TIMM_OSAL_CreateTask(
              (void*)&omtbClass[OMTB_VPSS_VFCC_CLASS].execThreadId[instanceNo],
                        (void *)VPSSVFCC_FuncTsk,
                        0,
                        &funcThreadArg[instanceNo],
                        OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                        OMTB_VPSS_VFCC_GROUP_PRIORITY,
                        (signed char*)"VPSS_VFCC_OMX_FUNC_MODE_TSK");
              
    if(TIMM_OSAL_ERR_NONE != threadRes)
    {
      PRINT(PRINT_ERR_MSG, "Unable to start %s thread\n", codecType);
    }
    else
    {
      /* set flag used to determine Func/API thread is created or not */
      ((OMTB_VPSS_VFCC_PRIVATE*)\
      (omtbClass[OMTB_VPSS_VFCC_CLASS].vpssVfccClient[\
            instanceNo]))->flagThrCreated = OMX_TRUE;
    }

  } // if(!strcmp(codecType, OMTB_VPSS_VFCC_COMP_REF_STR))
  else if(!strcmp(codecType, OMTB_VPSS_VFDC_COMP_REF_STR))
  {
    if(NULL != omtbClass[OMTB_VPSS_VFDC_CLASS].handle[instanceNo])
    {
      /* Specified instance already created */
      /* Invalid instance number */
      PRINT(PRINT_ERR_MSG, 
            "Instance %d already exist - Valid range: <0 to %d>\n",
            instanceNo, OMTB_MAX_INS - 1);
      return ERR_INVNAME;
    }
    
    /* Update the config param pointers based on the template number */
    vpssVfdcConfig = (OMTB_VPSS_VFDC_CONFIG_PARAMS*)
                          &(omtbCfgTemplate[template]->vpssVfdcTemplate);

    /* Create the component instance */
    result = VPSSVFDC_GetHandle(&omtbClass[OMTB_VPSS_VFDC_CLASS], instanceNo);
    
    if(OMX_ErrorNone != result)
    {
      PRINT(PRINT_ERR_MSG, "Error in %s instance creation\n", codecType);
      return ((MX_Result)ERR_INVNAME);
    }
    
    funcThreadArg[instanceNo].vpssVfdcClient = (OMTB_VPSS_VFDC_PRIVATE*)
        omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[instanceNo];
                    
    /* create the execution thread for func mode */
    threadRes = TIMM_OSAL_CreateTask(
              (void*)&omtbClass[OMTB_VPSS_VFDC_CLASS].execThreadId[instanceNo],
                        (void *)VPSSVFDC_FuncTsk,
                        0,
                        &funcThreadArg[instanceNo],
                        OMTB_VPSS_VFDC_TSK_STACK_SIZE,
                        OMTB_VPSS_VFDC_GROUP_PRIORITY,
                        (signed char*)"VPSS_VFDC_OMX_FUNC_MODE_TSK");
              
    if(TIMM_OSAL_ERR_NONE != threadRes)
    {
      PRINT(PRINT_ERR_MSG, "Unable to start %s thread\n", codecType);
    }
    else
    {
      /* set flag used to determine Func/API thread is created or not */
      ((OMTB_VPSS_VFDC_PRIVATE*)\
      (omtbClass[OMTB_VPSS_VFDC_CLASS].vpssVfdcClient[\
            instanceNo]))->flagThrCreated = OMX_TRUE;
    }

  } // else if(!strcmp(codecType, OMTB_VPSS_VFDC_COMP_REF_STR))
  else
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    PRINT(PRINT_ERR_MSG, "Wrong vpss component type selected\n");
    return ERR_INVNAME;
  }

  return ERR_NOERR;
}
#endif

/******************************************************************************\
 *      OMTB_OmxCmdSendCommand Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This Function invokes OMTB_DimtSkipWhiteSpace function after
 *            omx api sendcommand command is invoked and parses rest of the
 *            command.
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
OMTB_OmxCmdSendCommand(
  MX_DBG_CMD_CONTEXT_T*       pContext,
  MX_DBG_CMD_ENTRY_T*         pCmdEntry,
  char**                      ppCmdLine
)
{
  MX_Result result = ERR_NOERR;

  /* Remove the unnecessary white spaces */
  result = OMTB_DimtSkipWhiteSpace(pContext, pCmdEntry, ppCmdLine);
  if (ERR_NOERR != result)
  {
    XdbgCmdSyntaxShow(pContext, pCmdEntry);
    return result;
  }
  
  return XdbgCmdParse(pContext, pCmdEntry, ppCmdLine);
}


/*----------------------------- end of file ---------------------------------*/
