/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file      omtbHDVPSSVfcc.c
 *
 *  @brief     HD VPSS VFCC IL Client Implementation.
 *
 * <B> History: </B>
 *
 *      # October-12-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>

/* OMX standard header files */
#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#include "omx_ti_common.h"
#include "wtsd17_omx_ti_index.h"


/******************************************************************************\
 *      Customized Includes
\******************************************************************************/

#include "omtbDebug.h"
#ifdef WTSD
#include "timm_osal_interfaces.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
#endif

#include "omtbCommon.h"
#include "omtbCmd.h"


/******************************************************************************\
 *      Local Defines
\******************************************************************************/


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Callback structure Definition */

static OMX_CALLBACKTYPE VPSSVFCC_Callback = {
  .EventHandler = VPSSVFCC_EventHandler,      /* Event handler callback */    
  .EmptyBufferDone = VPSSVFCC_EmptyBufferDone,/* Empty buffer done callback */
  .FillBufferDone = VPSSVFCC_FillBufferDone   /* Fill buffer done callback */ 
};


/******************************************************************************\
 *      Extern Object Definitions
\******************************************************************************/

// Component class structure which holds component instance handle, 
// application private data etc
extern OMTB_COMP_HANDLE omtbClass[OMTB_MAX_CLASS];

/* Streaming (I/P and O/P) thread arguments */
extern OMTB_DATA_THREAD_ARG strmThreadArg[OMTB_MAX_INS];

/* Pointer to OMTB configuration template */
extern OMTB_TEMPLATE_INFO *omtbCfgTemplate[OMTB_MAX_INS];


/******************************************************************************\
 *      Globals
\******************************************************************************/

/* Pointer to VFCC config params */
OMTB_VPSS_VFCC_CONFIG_PARAMS *vpssVfccConfig = NULL;


/******************************************************************************\
 *      Static Globals
\******************************************************************************/

static OMX_U64 VPSSVFCC_funcCreateCount = 0;  /* Func create count */
static OMX_U64 VPSSVFCC_apiCreateCount = 0;   /* API create count */ 
                                                                 
static OMX_U64 VPSSVFCC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U64 VPSSVFCC_apiDeleteCount = 0;   /* API delete count */ 

/* Enum Entry for Supported cfg param indexs */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblVpssVfccCfgParamIdx[] =
{
  /* ADD ENTRIES HERE for SetConfig/SetParam/GetConfig/GetParam */
  {"OMX_TI_IndexConfigChannelName", OMX_TI_IndexConfigChannelName,  ""},
  {"", 0, ""}
};


/******************************************************************************\
 *      Public Function Prototypes
\******************************************************************************/

/******************************************************************************\
 *      Private Function Prototypes
\******************************************************************************/


/******************************************************************************\
 *      Function Definitions                                                    
\******************************************************************************/

/******************************************************************************\
 *      VPSSVFCC_AllocateMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate memory for private data structure for HDVPSS VFCC
 *            component
 *
 *  @param in:
 *            vpssVfccClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */

OmtbRet VPSSVFCC_AllocateMemory(OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient)
{
  OmtbRet err = OMTB_SUCCESS;
  unsigned int retVal = TIMM_OSAL_ERR_NONE;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  /* Allocating memory for callback structure */
  vpssVfccClient->cb=(OMX_CALLBACKTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_CALLBACKTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->cb)
  {
    PRINT(PRINT_ERR_MSG, "Callback structure memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for input port definition structure */
  vpssVfccClient->inPortParams.pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->inPortParams.pInPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Input port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for output port definition structure */
  vpssVfccClient->outPortParams.pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                            TIMM_OSAL_TRUE,
                                            0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->outPortParams.pOutPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Output port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Create input pipe for input buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(vpssVfccClient->inPortParams.ipBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      vpssVfccClient->inPortParams.numInBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Create output pipe for output buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(vpssVfccClient->outPortParams.opBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      vpssVfccClient->outPortParams.numOutBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
#if 0
  /* Allocating memory for component event handling semaphore */
  vpssVfccClient->eventSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!vpssVfccClient->eventSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for eof handling semaphore */
  vpssVfccClient->eofSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!vpssVfccClient->eofSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for input data read thread exit semaphore */
  vpssVfccClient->inPortParams.inputDataReadThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->inPortParams.inputDataReadThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  
  /* Allocating memory for output data write thread exit semaphore */
  vpssVfccClient->outPortParams.outputDataWriteThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->outPortParams.outputDataWriteThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  /* Allocating memory for input data read thread control semaphore */
  vpssVfccClient->inPortParams.inputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!vpssVfccClient->inPortParams.inputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for output data write thread semaphore */
  vpssVfccClient->outPortParams.outputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!vpssVfccClient->outPortParams.outputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
#endif
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_AllocateResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for VPSS VFCC.
 *
 *  @param in:
 *            vpssVfccClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_AllocateResources(OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  unsigned char *pBuffer = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  /* Input port buffer allocation */
  for(i = 0; i < vpssVfccClient->inPortParams.pInPortDef->nBufferCountActual; 
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == vpssVfccClient->inPortParams.flagUseInBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                        vpssVfccClient->handle,
                        &vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i],
                        vpssVfccClient->inPortParams.pInPortDef->nPortIndex, 
                        vpssVfccClient,
                        vpssVfccClient->inPortParams.pInPortDef->nBufferSize);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
      /* Allocating memory for input port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                        vpssVfccClient->inPortParams.pInPortDef->nBufferSize,
                        OMTB_CMEM_ALLOCATION,
                        0,
                        TIMMOSAL_MEM_SEGMENT_EXT);
      
      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for i/p port\n");

        /* Function to release the resources allocated by component */
        err = VPSSVFCC_FreeResources(vpssVfccClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer(vpssVfccClient->handle,
                      &vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i],
                      vpssVfccClient->inPortParams.pInPortDef->nPortIndex, 
                      vpssVfccClient,
                      vpssVfccClient->inPortParams.pInPortDef->nBufferSize,
                      pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = VPSSVFCC_FreeResources(vpssVfccClient);

      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }
      
      return tempErr;
    }
    else
    {
      PRINT(PRINT_DEBUG_MESSAGE, \
            "Buffers allocated successfully for i/p port\n");
    }
  }

  /* Output port buffer allocation */
  for(i = 0; i < vpssVfccClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == vpssVfccClient->outPortParams.flagUseOutBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                    vpssVfccClient->handle,
                    &vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff[i],
                    vpssVfccClient->outPortParams.pOutPortDef->nPortIndex,
                    vpssVfccClient,
                    vpssVfccClient->outPortParams.pOutPortDef->nBufferSize);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
      /* Allocating memory for output port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                    vpssVfccClient->outPortParams.pOutPortDef->nBufferSize,
                    OMTB_CMEM_ALLOCATION,
                    0,
                    TIMMOSAL_MEM_SEGMENT_EXT);

      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for o/p port\n");

        /* Function to release the resources allocated by component */
        err = VPSSVFCC_FreeResources(vpssVfccClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer( vpssVfccClient->handle,
                     &vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff[i],
                     vpssVfccClient->outPortParams.pOutPortDef->nPortIndex,
                     vpssVfccClient,
                     vpssVfccClient->outPortParams.pOutPortDef->nBufferSize,
                     pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = VPSSVFCC_FreeResources(vpssVfccClient);

      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }
      
      return tempErr;
    }
    else
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Buffers allocated successfully \
            for o/p port\n");
    }
  }

  PRINT(PRINT_DEBUG_MESSAGE, "Before locking on idle semaphore\n");

  /* waiting for component to go to idle state */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_UseInitialResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Tells VPSS VFCC component to empty/fill the initial buffers.
 *
 *  @param in:
 *            vpssVfccClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_UseInitialResources(
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int dataRead = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  /* Give input buffers to component which is limited by no of input buffers 
     available. Rest of the data will be read on the callback from input data 
     read thread */
  for(i = 0; i < vpssVfccClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
  
    dataRead = fread(
                vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->pBuffer,
                sizeof(char),
                vpssVfccClient->inPortParams.bufferInSize,
                vpssVfccClient->inPortParams.fin);
                      
    vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->nFilledLen =dataRead;
    vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->nOffset = 0;
    vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->nAllocLen = dataRead;
    vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->nInputPortIndex = 0;
    
    PRINT(PRINT_DEBUG_MESSAGE, "--> Read %d bytes from file\n",dataRead);
    PRINT(PRINT_DEBUG_MESSAGE, "Empty first  buffer %x\n",
          (int)vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->pBuffer);
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer(
                        vpssVfccClient->handle,
                        vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]);
    
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_EmptyThisBuffer");
    
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
  }
  
  /* Give output buffers to component which is limited by no of output buffers 
     available. Rest of the data will be written on the callback from output  
     data write thread */
  for(i = 0; i < vpssVfccClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer( 
                      vpssVfccClient->handle,
                      vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff[i]);
    
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FillThisBuffer");
    
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
    else
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Fill O/P Buffer\n");
    }
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_SetPortDefinition Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for VPSS VFCC.
 *
 *  @param in:
 *            vpssVfccClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_SetPortDefinition(OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_PORT_PARAM_TYPE portInit;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
 
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
#if 0   
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader(&portInit,sizeof(OMX_PORT_PARAM_TYPE));
  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;

  /* output buffer size calculation based on input dimension speculation */
  vpssVfccClient->inPortParams.bufferInSize   = OMTB_VPSS_VFCC_MAX_INBUFF_SIZE;
  vpssVfccClient->outPortParams.bufferOutSize = 
                      (vpssVfccClient->outPortParams.comp.vpssVfcc.nWidth *\
                      vpssVfccClient->outPortParams.comp.vpssVfcc.nHeight * 2);
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Initialize the Port*/
  err = OMX_SetParameter( vpssVfccClient->handle,
                          OMX_IndexParamVideoInit,
                          &portInit);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  else
  {
    PRINT(DEFAULT_MESSAGES, "Found The component for decoding\n");
  }
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader( vpssVfccClient->inPortParams.pInPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  vpssVfccClient->inPortParams.pInPortDef->nPortIndex          = 0x0;
  vpssVfccClient->inPortParams.pInPortDef->eDir                = OMX_DirInput;
  vpssVfccClient->inPortParams.pInPortDef->nBufferCountActual  = 
                                  vpssVfccClient->inPortParams.numInBuf;
  vpssVfccClient->inPortParams.pInPortDef->nBufferCountMin     = 1;
  vpssVfccClient->inPortParams.pInPortDef->nBufferSize         = 
                                  vpssVfccClient->inPortParams.bufferInSize;
  vpssVfccClient->inPortParams.pInPortDef->bEnabled            = OMX_TRUE;
  vpssVfccClient->inPortParams.pInPortDef->bPopulated          = OMX_FALSE;
  vpssVfccClient->inPortParams.pInPortDef->eDomain             = 
                                  OMX_PortDomainVideo;
  vpssVfccClient->inPortParams.pInPortDef->bBuffersContiguous  = OMX_FALSE;
  vpssVfccClient->inPortParams.pInPortDef->nBufferAlignment    = 0x0;
  
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the inPort Definition*/
  err = OMX_SetParameter( vpssVfccClient->handle,
                          OMX_IndexParamPortDefinition,
                          vpssVfccClient->inPortParams.pInPortDef);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  
  OMTB_SetHeader( vpssVfccClient->outPortParams.pOutPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  
  vpssVfccClient->outPortParams.pOutPortDef->nPortIndex         = 0x1;
  vpssVfccClient->outPortParams.pOutPortDef->eDir               = OMX_DirOutput;
  vpssVfccClient->outPortParams.pOutPortDef->nBufferCountActual = 
                                  vpssVfccClient->outPortParams.numOutBuf;
  vpssVfccClient->outPortParams.pOutPortDef->nBufferCountMin    = 1;
  vpssVfccClient->outPortParams.pOutPortDef->nBufferSize        = 
                                  vpssVfccClient->outPortParams.bufferOutSize;
  vpssVfccClient->outPortParams.pOutPortDef->bEnabled           = OMX_TRUE;
  vpssVfccClient->outPortParams.pOutPortDef->bPopulated         = OMX_FALSE;
  vpssVfccClient->outPortParams.pOutPortDef->eDomain            = 
                                  OMX_PortDomainVideo;
  vpssVfccClient->outPortParams.pOutPortDef->bBuffersContiguous = OMX_FALSE;
  vpssVfccClient->outPortParams.pOutPortDef->nBufferAlignment   = 0x0;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the OutPort Definition*/
  err = OMX_SetParameter( vpssVfccClient->handle,
                          OMX_IndexParamPortDefinition,
                          vpssVfccClient->outPortParams.pOutPortDef);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
#endif  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_ApiTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    VPSS VFCC API mode Execute Thread
 *
 *  @param in:
 *            void *threadsArg - Pointer to thread arg structure
 *
 *  @param Out:
 *            None
 *
 *  @return   None
 *
 */

void VPSSVFCC_ApiTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->vpssVfccClient);
  
  vpssVfccClient = funcDataThreadArg->vpssVfccClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);

  /* Update the api mode create count */
  VPSSVFCC_apiCreateCount++;
  PRINT(PRINT_MUST, "%s: VPSS VFCC API Mode Create Count: %u\n", __func__,
                     (unsigned int) VPSSVFCC_apiCreateCount);
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem, TIMM_OSAL_SUSPEND);
  
  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = VPSSVFCC_UseInitialResources(vpssVfccClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* Data streaming thread arguments */
  strmThreadArg[vpssVfccClient->instanceNo].vpssVfccClient = vpssVfccClient;
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&vpssVfccClient->inPortParams.inDataStrmThrdId,
                    (void *)VPSSVFCC_InputDataRead,
                    0,
                    &strmThreadArg[vpssVfccClient->instanceNo],
                    OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                    OMTB_VPSS_VFCC_GROUP_PRIORITY,
                    (signed char*)"VPSS_VFCC_DATA_IN_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start VPSS VFCC input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&vpssVfccClient->outPortParams.outDataStrmThrdId,
                    (void *)VPSSVFCC_OutputDataWrite,
                    0,
                    &strmThreadArg[vpssVfccClient->instanceNo],
                    OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                    OMTB_VPSS_VFCC_GROUP_PRIORITY,
                    (signed char*)"VPSS_VFCC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start VPSS VFCC output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eofSem, TIMM_OSAL_SUSPEND);

  /* Update the api mode delete count */
  VPSSVFCC_apiDeleteCount++;
  PRINT(PRINT_MUST, "%s: VPSS VFCC API Mode Delete Count: %u\n", __func__,
                    (unsigned int) VPSSVFCC_apiDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  vpssVfccClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_VPSS_VFCC_CLASS].execThreadId[vpssVfccClient->instanceNo],
      NULL);
}


/******************************************************************************\
 *      VPSSVFCC_GetHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get the handle for VPSS VFCC component
 *
 *  @param in:
 *            classPtr: Pointer to OMTB_COMP_HANDLE structure
 *
 *            instance: Instance number of the component
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_GetHandle(
  OMTB_COMP_HANDLE * classPtr,
  unsigned int instance)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  char inputFile[OMTB_MAX_LEN]  = {"\0"};
  char outputFile[OMTB_MAX_LEN] = {"\0"};
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != classPtr);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instance);

  /* Allocate memory for application private data */
  vpssVfccClient = (OMTB_VPSS_VFCC_PRIVATE *)
                    TIMM_OSAL_Malloc( sizeof(OMTB_VPSS_VFCC_PRIVATE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);

  if (!vpssVfccClient)
  {
    PRINT(PRINT_ERR_MSG, "Error allocating vpssVfccClient structure \n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    return err;
  }
  else
  {
    /* Update the application instance management structure with app pvt data
       structure pointer */
    classPtr->vpssVfccClient[instance] = 
                          (OMTB_VPSS_VFCC_PRIVATE *)vpssVfccClient;
    memset(vpssVfccClient, 0, sizeof(OMTB_VPSS_VFCC_PRIVATE));
    vpssVfccClient->instanceNo = instance;
  }

  /* Make local copy of input, output and framesize file names */
  strcpy(outputFile, vpssVfccConfig->outputFile);
  strcpy(inputFile,  vpssVfccConfig->inputFile);
  strcpy(vpssVfccClient->compName,  vpssVfccConfig->compName);

  PRINT(PRINT_DEBUG_MESSAGE, "Component Name is:%s\n", 
                              vpssVfccClient->compName);
  PRINT(PRINT_DEBUG_MESSAGE, "Input File is:%s\n", inputFile);
  PRINT(PRINT_DEBUG_MESSAGE, "Output File is:%s\n", outputFile);
  
  /* Data output mode selection - File/Driver */
  vpssVfccClient->outPortParams.dataOutPutMode = 
                                            vpssVfccConfig->dataOutPutMode;
  
  /* Buffer allocation mode - OMX_UseBuffer or OMX_AllocateBuffer */
  vpssVfccClient->inPortParams.flagUseInBuffer   = 
                                            vpssVfccConfig->flagUseInBuffer;
  vpssVfccClient->outPortParams.flagUseOutBuffer = 
                                            vpssVfccConfig->flagUseOutBuffer;

  /* no of i/p and o/p buffers */
  vpssVfccClient->inPortParams.numInBuf   = vpssVfccConfig->numInBuf;
  vpssVfccClient->outPortParams.numOutBuf = vpssVfccConfig->numOutBuf;
  
  /* Open the input file */
  vpssVfccClient->inPortParams.fin = fopen(inputFile, "rb");
  if(NULL == vpssVfccClient->inPortParams.fin)
  {
    PRINT(PRINT_ERR_MSG, "Error in opening input file %s\n",inputFile);
    err = OMTB_INSUFFICIENT_RESOURCES;
    VPSSVFCC_FreeMemory(vpssVfccClient);
    return err;
  }

  /* Initialize the counter to get frame size from input data read thread */
  vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount = 0;
  
  if(OMTB_FILE == vpssVfccClient->outPortParams.dataOutPutMode )
  {
    /* Open the output file */
    vpssVfccClient->outPortParams.fout = fopen(outputFile, "wb");
    if(NULL == vpssVfccClient->outPortParams.fout)
    {
      PRINT(PRINT_ERR_MSG, "Error opening output file %s\n",outputFile);
      err = OMTB_INSUFFICIENT_RESOURCES;
      VPSSVFCC_FreeMemory(vpssVfccClient);
      return err;
    }
  }
  else if(OMTB_DRIVER == vpssVfccClient->outPortParams.dataOutPutMode)
  {
    
  }
  
  /* Initialize app pvt structure params from the template */
  vpssVfccClient->outPortParams.comp.vpssVfcc.nWidth = 
                                              vpssVfccConfig->frameWidth;
  vpssVfccClient->outPortParams.comp.vpssVfcc.nHeight = 
                                              vpssVfccConfig->frameHeight;
  vpssVfccClient->compressionFormat = OMX_VIDEO_CodingAVC;
  vpssVfccClient->outPortParams.comp.vpssVfcc.colorFormat = \
                                                OMX_COLOR_FormatYUV420Planar;
  
  /* output buffer size calculation based on input dimension speculation */
  vpssVfccClient->inPortParams.bufferInSize   = OMTB_VPSS_VFCC_MAX_INBUFF_SIZE;
  vpssVfccClient->outPortParams.bufferOutSize = 
                      (vpssVfccClient->outPortParams.comp.vpssVfcc.nWidth *\
                      vpssVfccClient->outPortParams.comp.vpssVfcc.nHeight * 2);
  
  /*Function to allocate memory for app pvt struct members*/
  err = VPSSVFCC_AllocateMemory(vpssVfccClient);

  if(OMTB_SUCCESS != err)
  {
    PRINT(PRINT_ERR_MSG, "Unable to allocate memory for app pvt structure\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    VPSSVFCC_FreeMemory(vpssVfccClient);
    return err;
  }
  
  /* Initialize the semaphores */
  TIMM_OSAL_SemaphoreCreate(&vpssVfccClient->eventSem, 0);
  TIMM_OSAL_SemaphoreCreate(&vpssVfccClient->eofSem, 0);
  TIMM_OSAL_SemaphoreCreate(&vpssVfccClient->inPortParams.inputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&vpssVfccClient->outPortParams.outputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&vpssVfccClient->inPortParams.inputDataReadThrExit, 
                            0);
  TIMM_OSAL_SemaphoreCreate(
                &vpssVfccClient->outPortParams.outputDataWriteThrExit,
                0);
  
  /*Assign the callbacks*/
  *vpssVfccClient->cb = VPSSVFCC_Callback;

  /* Initialize the EOS flag */
  vpssVfccClient->outPortParams.flagEos = OMX_FALSE;
  vpssVfccClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Initialize the flag used to determine whether Func/API thread is created */
  vpssVfccClient->flagThrCreated = OMX_FALSE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* get component handle */
  err = OMX_GetHandle(&vpssVfccClient->handle,
                      (OMX_STRING)vpssVfccClient->compName,
                      vpssVfccClient,
                      vpssVfccClient->cb);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetHandle");
  
  if(OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, "No component named \"%s\" found\n",
                          vpssVfccClient->compName);
    OMTB_OmxReturnStatus(err);
    VPSSVFCC_FreeMemory(vpssVfccClient);
    return err;
  }
  else
  {
    /* Update the application instance management strucutre with handle */
    classPtr->handle[instance] = vpssVfccClient->handle;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-VPSS VFCC Instance #: %d\n\n", instance);
    PRINT(PRINT_SIMPLE_MESSAGE, "Component Handle Available\n");
  }
  
  PRINT(PRINT_DEBUG_MESSAGE, "\nbufferOutSize: %d\n",\
        (int)vpssVfccClient->outPortParams.bufferOutSize);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_GetStructIdx Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get config index and structure pointer based on the index string
 *
 *  @param in:
 *            char *: Pointer to config structure index string
 *
 *            unsigned int: Template number to be used for configuration
 *
 *            unsigned int: Port index
 *
 *  @param Out:
 *            unsigned int*: Pointer to configuration structure index
 *
 *            void *: Pointer to config structure
 *
 *  @return   OmtbRet - OMTB return code
 *
 */

OmtbRet VPSSVFCC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct)
{
  unsigned int enumCount = 0;
  
  /* Use the specified template structure */
  vpssVfccConfig = (OMTB_VPSS_VFCC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->vpssVfccTemplate);

  /* Get the corresponding string for enum values from enum table 
     Set structure index
     Set structure pointer
     Set port index for which parameter to be updated 
     Set structure header info */
  while(strcmp(enumTblVpssVfccCfgParamIdx[enumCount].name , ""))
  {
    if(0 == strcmp(enumTblVpssVfccCfgParamIdx[enumCount].name,
                   indexStr))
    {
      switch(enumTblVpssVfccCfgParamIdx[enumCount].value)
      {

        case OMX_TI_IndexConfigChannelName:
          {
            *nConfigIndex   = enumTblVpssVfccCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &vpssVfccConfig->sChannelName;
            vpssVfccConfig->sChannelName.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_CONFIG_CHANNELNAME));
          }
          return OMTB_SUCCESS;
          
        default:
          {
            return OMTB_FAIL;
          }

      } // switch(enumTblVpssVfccCfgParamIdx[enumCount].value)
     
    } // if(0 == strcmp(enumTblVpssVfccCfgParamIdx[enumCount].name, ...
      
    enumCount++;
  }

  return OMTB_SUCCESS;  
}


/******************************************************************************\
 *      VPSSVFCC_SetParameter Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set aparameter for the appropriate index passsed
 *
 *  @param in:
 *            OMX_HANDLETYPE: Component handle
 *
 *            unsigned int: Port index
 *
 *            char *: Pointer to config structure index string
 *
 *            unsigned int: Template number to be used for configuration
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_SetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int nConfigIndex = OMX_IndexMax;
  void *pCompCfgStruct = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != handle);

  /* Get the cfg structure index and pointer */
  err = VPSSVFCC_GetStructIdx(indexStr, 
                              template,
                              portIdx,
                              &nConfigIndex,
                              &pCompCfgStruct);

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
    PRINT(PRINT_ERR_MSG, "In %s: Error in OMX_SetParameter\n", __func__);
    OMTB_OmxReturnStatus(err);
  }
  else
  {
    OMTB_OmxReturnStatus(err);
  }

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_SetConfig Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set config for the appropriate index passsed
 *
 *  @param in:
 *            OMX_HANDLETYPE: Component handle
 *
 *            unsigned int: Port index
 *
 *            char *: Pointer to config structure index string
 *
 *            unsigned int: Template number to be used for configuration
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_SetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int nConfigIndex = OMX_IndexMax;
  void *pCompCfgStruct = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != handle);

  /* Get the cfg structure index and pointer */
  err = VPSSVFCC_GetStructIdx(indexStr, 
                              template,
                              portIdx,
                              &nConfigIndex,
                              &pCompCfgStruct);

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
    PRINT(PRINT_ERR_MSG, "In %s: Error in OMX_SetConfig\n", __func__);
    OMTB_OmxReturnStatus(err);
  }
  else
  {
    OMTB_OmxReturnStatus(err);
  }
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_GetParameter Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get parameter for the appropriate index passsed
 *
 *  @param in:
 *            OMX_HANDLETYPE: Component handle
 *
 *            unsigned int: Port index
 *
 *            char *: Pointer to config structure index string
 *
 *            unsigned int: Template number to be used for configuration
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_GetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int nConfigIndex = OMX_IndexMax;
  void *pCompCfgStruct = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != handle);

  /* Get the cfg structure index and pointer */
  err = VPSSVFCC_GetStructIdx(indexStr, 
                              template,
                              portIdx,
                              &nConfigIndex,
                              &pCompCfgStruct);

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
    PRINT(PRINT_ERR_MSG, "In %s: Error in OMX_GetParameter\n", __func__);
    OMTB_OmxReturnStatus(err);
  }
  else
  {
    OMTB_OmxReturnStatus(err);
  }

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_GetConfig Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get config for the appropriate index passsed
 *
 *  @param in:
 *            OMX_HANDLETYPE: Component handle
 *
 *            unsigned int: Port index
 *
 *            char *: Pointer to config structure index string
 *
 *            unsigned int: Template number to be used for configuration
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX standard error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_GetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int nConfigIndex = OMX_IndexMax;
  void *pCompCfgStruct = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != handle);

  /* Get the cfg structure index and pointer */
  err = VPSSVFCC_GetStructIdx(indexStr, 
                              template,
                              portIdx,
                              &nConfigIndex,
                              &pCompCfgStruct);

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
    PRINT(PRINT_ERR_MSG, "In %s: Error in OMX_GetConfig\n", __func__);
    OMTB_OmxReturnStatus(err);
  }
  else
  {
    OMTB_OmxReturnStatus(err);
  }
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_FillData Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *
 *            count - Encoded frame number
 *
 *  @param Out:
 *            OMX_BUFFERHEADERTYPE * - Pointer of I/P filled Buffer
 *  @return   No of bytes read
 *
 */

unsigned int VPSSVFCC_FillData(
  OMTB_VPSS_VFCC_PRIVATE* pAppData,
  OMX_BUFFERHEADERTYPE *pBuf,
  unsigned int count)
{
  unsigned int dataRead = 0;

  /* validate the function arguments */
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuf);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)pAppData->instanceNo);
  
  if(!feof(pAppData->inPortParams.fin))
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Entering Function:%s\n",__func__);
    
    PRINT(PRINT_DEBUG_MESSAGE, "nAllocLen is %d\n",
                                (int) pBuf->nAllocLen);
    PRINT(PRINT_DEBUG_MESSAGE, "nFilledLen is: %d\n",
                                (int) pBuf->nFilledLen);
     
     /* Read from I/P file and Fill data in i/p Buffer */
    dataRead = fread( pBuf->pBuffer, 
                      sizeof(char), 
                      pAppData->inPortParams.bufferInSize,
                      pAppData->inPortParams.fin);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Bytes read are:%d\n", dataRead);

    /* Update the buffer header with buffer filled length and alloc length */
    pBuf->nFilledLen = dataRead;
    pBuf->nAllocLen = pAppData->inPortParams.bufferInSize;

    /* Fetch the character from file to check whether EOS is reached */
    fgetc(pAppData->inPortParams.fin);
    if(dataRead < pBuf->nAllocLen || feof(pAppData->inPortParams.fin))
    {
      /* Udate the EOS flag in buffer header */
      pBuf->nFlags = OMX_BUFFERFLAG_EOS;
      PRINT(PRINT_DEBUG_MESSAGE, "%s: End of File reached\n", __func__);
    }
    else
    {
      /* Seek back if EOS is not reached */
      fseek(pAppData->inPortParams.fin, -1, SEEK_CUR);
    }
    
  }
  else
  {
    /* Set the EOS flag and buffer size to 0 if EOS is reached - This should 
       not occur !!! */
    dataRead = 0;
    pBuf->pBuffer = NULL;
    pBuf->nFlags = OMX_BUFFERFLAG_EOS;
    PRINT(PRINT_DEBUG_MESSAGE, "End of File reached with pBuffer: %x\n", 
                                (unsigned int)pBuf->pBuffer);
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)pAppData->instanceNo);
  
  return dataRead;
}


/******************************************************************************\
 *      VPSSVFCC_FuncTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a VPSS VFCC thread which invokes during func command
 *
 *  @param in:
 *            void *threadsArg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void VPSSVFCC_FuncTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->vpssVfccClient);
  
  vpssVfccClient = funcDataThreadArg->vpssVfccClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);
  
  PRINT(PRINT_SIMPLE_MESSAGE, "\nbufferOutSize:%d\n",\
        (int)vpssVfccClient->outPortParams.bufferOutSize);
  
  /* Set the port params */
  err = VPSSVFCC_SetPortDefinition(vpssVfccClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to idle state */
  err = OMX_SendCommand(vpssVfccClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateIdle,
                        NULL);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocate I/O port buffers and wait till state transition to idle */
  err = VPSSVFCC_AllocateResources(vpssVfccClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to executing state */
  err = OMX_SendCommand(vpssVfccClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL);
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem, TIMM_OSAL_SUSPEND);

  /* Update the func create count */
  VPSSVFCC_funcCreateCount++;
  PRINT(PRINT_MUST, "%s: VPSS VFCC FUNC Create Count: %u\n", __func__,
                    (unsigned int) VPSSVFCC_funcCreateCount);

  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = VPSSVFCC_UseInitialResources(vpssVfccClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Update the data streaming thread arguments */
  strmThreadArg[vpssVfccClient->instanceNo].vpssVfccClient = vpssVfccClient;
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&vpssVfccClient->inPortParams.inDataStrmThrdId,
                    (void *)VPSSVFCC_InputDataRead,
                    0,
                    &strmThreadArg[vpssVfccClient->instanceNo],
                    OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                    OMTB_VPSS_VFCC_GROUP_PRIORITY,
                    (signed char*)"VPSS_VFCC_DATA_IN_TSK");
    
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start VPSS VFCC input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&vpssVfccClient->outPortParams.outDataStrmThrdId,
                    (void *)VPSSVFCC_OutputDataWrite,
                    0,
                    &strmThreadArg[vpssVfccClient->instanceNo],
                    OMTB_VPSS_VFCC_TSK_STACK_SIZE,
                    OMTB_VPSS_VFCC_GROUP_PRIORITY,
                    (signed char*)"VPSS_VFCC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start VPSS VFCC output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eofSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "The execution of the video decoding\
                           process is terminated\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* state change of component from executing to idle */
  err = OMX_SendCommand(vpssVfccClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateIdle,
                        NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* waiting for component to go to idle state */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "State transitioned to Idle\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to go to loaded state */
  err = OMX_SendCommand(vpssVfccClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateLoaded,
                        NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Reset the EOS indication flag */
  vpssVfccClient->outPortParams.flagEos = OMX_FALSE;
  vpssVfccClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Function to reelase the resources allocated by component */
  err = VPSSVFCC_FreeResources(vpssVfccClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "State transitioned to loaded\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Delete the component instance */
  err = OMX_FreeHandle(vpssVfccClient->handle);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeHandle");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  else
  {
    /* Update the component instance management strucutre for deleted instance 
       by assigning NULL pointer */
    vpssVfccClient->handle = NULL;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted VPSS VFCC Instance #: %d\n\n", 
          (int) vpssVfccClient->instanceNo);
    omtbClass[OMTB_VPSS_VFCC_CLASS].handle[vpssVfccClient->instanceNo] = NULL;
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "video dec freed\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* OMX core un-initialization */
  err = OMX_Deinit();

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_Deinit");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Reelase the memory allocated for app pvt data */
  VPSSVFCC_FreeMemory(vpssVfccClient);

  /* Update the func delete count */
  VPSSVFCC_funcDeleteCount++;
  PRINT(PRINT_MUST, "%s: VPSS VFCC FUNC Delete Count: %u\n", __func__,
                    (unsigned int) VPSSVFCC_funcDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  vpssVfccClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_VPSS_VFCC_CLASS].execThreadId[vpssVfccClient->instanceNo],
      NULL);
}


/******************************************************************************\
 *      VPSSVFCC_FreeResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            vpssVfccClient: Pointer to OMTB_VPSS_VFCC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_FreeResources(OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  /* Ask component to Free input port buffers */
  
  for(i = 0; i < vpssVfccClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video dec I/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",\
          (unsigned int) vpssVfccClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n", \
          (unsigned int) vpssVfccClient->inPortParams.pInPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",\
          (unsigned int) vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == vpssVfccClient->inPortParams.flagUseInBuffer)
    {
      TIMM_OSAL_CMEM_Free (
              vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]->pBuffer, 
              vpssVfccClient->inPortParams.pInPortDef->nBufferSize);
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
  
    /** freeing buffers of component input ports */
    err = OMX_FreeBuffer( vpssVfccClient->handle,
                      vpssVfccClient->inPortParams.pInPortDef->nPortIndex,
                      vpssVfccClient->inPortParams.comp.vpssVfcc.pInBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
    
  } // for(i = 0; i < vpssVfccClient->inPortParams.pInPortDef
  
  /* Ask component to Free output port buffers */  
  for(i = 0; i < vpssVfccClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video dec O/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",
          (unsigned int) vpssVfccClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n",
          (unsigned int) vpssVfccClient->outPortParams.pOutPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",
          (unsigned int) vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == vpssVfccClient->outPortParams.flagUseOutBuffer)
    {
      TIMM_OSAL_CMEM_Free (
              vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff[i]->pBuffer, 
              vpssVfccClient->outPortParams.pOutPortDef->nBufferSize);
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
  
    err = OMX_FreeBuffer( vpssVfccClient->handle,
                      vpssVfccClient->outPortParams.pOutPortDef->nPortIndex,
                      vpssVfccClient->outPortParams.comp.vpssVfcc.pOutBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
  
  } // for(i = 0; i < vpssVfccClient->outPortParams.pOutPortDef ...
  
  /*wait for event handler to report completion of loaded command */
  TIMM_OSAL_SemaphoreObtain(vpssVfccClient->eventSem, TIMM_OSAL_SUSPEND);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      VPSSVFCC_FreeMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            vpssVfccClient: Pointer to OMTB_VPSS_VFCC_PRIVATE struct
 *
 *  @return   None
 *
 */

void VPSSVFCC_FreeMemory(OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient)
{
  int instanceNo = vpssVfccClient->instanceNo;

  /* validate the function arguments */
  ASSERT(NULL != vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  /* Release callback structure memory */
  if(NULL != vpssVfccClient->cb)
    TIMM_OSAL_Free(vpssVfccClient->cb);
  
  /* Release input port memory */
  if(NULL != vpssVfccClient->inPortParams.pInPortDef)
    TIMM_OSAL_Free(vpssVfccClient->inPortParams.pInPortDef);
  
  /* Release output port memory */
  if(NULL != vpssVfccClient->outPortParams.pOutPortDef)
    TIMM_OSAL_Free(vpssVfccClient->outPortParams.pOutPortDef);
  
  /* Release component event handling semaphore memory */
  if(NULL != vpssVfccClient->eventSem)
    TIMM_OSAL_SemaphoreDelete(vpssVfccClient->eventSem);
  
  /* Release EOF handling semaphore memory */
  if(NULL != vpssVfccClient->eofSem)
    TIMM_OSAL_SemaphoreDelete(vpssVfccClient->eofSem);
  
  /* Release input data read thread control semaphore memory */
  if(NULL != vpssVfccClient->inPortParams.inputDataReady)
    TIMM_OSAL_SemaphoreDelete(vpssVfccClient->inPortParams.inputDataReady);
  
  /* Release output data write thread control semaphore memory */
  if(NULL != vpssVfccClient->outPortParams.outputDataReady)
    TIMM_OSAL_SemaphoreDelete(vpssVfccClient->outPortParams.outputDataReady);

  /* Release input data read thread exit semaphore memory */
  if(NULL != vpssVfccClient->inPortParams.inputDataReadThrExit)
    TIMM_OSAL_SemaphoreDelete(
              vpssVfccClient->inPortParams.inputDataReadThrExit);
  
  /* Release output data write thread exit semaphore memory */
  if(NULL != vpssVfccClient->outPortParams.outputDataWriteThrExit)
    TIMM_OSAL_SemaphoreDelete(
              vpssVfccClient->outPortParams.outputDataWriteThrExit);
  
  if(OMTB_FILE == vpssVfccClient->outPortParams.dataOutPutMode)
  {
    /* closing the output file */
    if(NULL != vpssVfccClient->outPortParams.fout)
      fclose(vpssVfccClient->outPortParams.fout);
  }
  else if(OMTB_DRIVER == vpssVfccClient->outPortParams.dataOutPutMode)
  {

  }
  
  /* closing the input file */
  if(NULL != vpssVfccClient->inPortParams.fin)
    fclose(vpssVfccClient->inPortParams.fin);
  
  /* Release app pvt data private structure */
  if(NULL != vpssVfccClient)
    TIMM_OSAL_Free(vpssVfccClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instanceNo);
}


/******************************************************************************\
 *      VPSSVFCC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the HDVPSS VFCC component
 *
 *  @param in:
 *            hComponent: Component Handle
 *
 *            pAppData  : Pointer to Private Data Structure
 *
 *            eEvent    : Variable to OMX_EVENTTYPE enum
 *
 *            Data1     : data associated with event
 *
 *            Data2     : data associated with event
 *
 *            pEventData: Extra Data associated with the event
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE VPSSVFCC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;

  /* thread argument validation */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  
  vpssVfccClient = pAppData;

  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);

  /* Check the eEvent type */
  switch(eEvent)
  {
    case OMX_EventCmdComplete:  /* Command complete event */
      {
        /* state transition command related event */
        if (OMX_CommandStateSet == Data1)
        {
          PRINT(PRINT_DEBUG_MESSAGE, 
                "In %s: OMX_EventCmdComplete -  OMX_CommandStateSet\n",
                __func__);
          
          switch ((int)Data2)
          {
            case OMX_StateInvalid:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StateInvalid\n");
                  break;
            case OMX_StateLoaded:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StateLoaded\n");
                  break;
            case OMX_StateIdle:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StateIdle\n");
                  break;
            case OMX_StateExecuting:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StateExecuting\n");
                  break;
            case OMX_StatePause:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StatePause\n");
                  break;
            case OMX_StateWaitForResources:
                  PRINT(PRINT_SIMPLE_MESSAGE, "OMX_StateWaitForResources\n");
                  break;
          }

          /* Post the component event sem */
          TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
        }
        /* port enable or disable command related event */
        else if ( OMX_CommandPortEnable   == Data1  || 
                  OMX_CommandPortDisable  == Data1  )
        {
          PRINT(PRINT_SIMPLE_MESSAGE,
                "In %s: OMX_CommandPortEnable/OMX_CommandPortDisable\n",
                __func__);

          /* Post the component event sem */
          TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
        }
      }
      break;
    
    case OMX_EventPortSettingsChanged: /* Port settings changed event */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventPortSettingsChanged\n",
                                    __func__);

        if(0 == Data2)
        {
          /* Perform the port settings as per params received from component */
        }
      }
      break;
    
    case OMX_EventBufferFlag:  /* Buffer related events */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventBufferFlag\n", __func__);
        if(OMX_BUFFERFLAG_EOS == (int)Data2)
        {
          //PRINT(PRINT_MUST, "OMX_EventBufferFlag - OMX_BUFFERFLAG_EOS\n");

          /* Commented temporarily as component sends EOS after each frame */
          //TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eofSem);
        }
      }
      break;
    
    case OMX_EventError:  /* Event error */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventError\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
      }
      break;
    
    case OMX_EventMark: /* Event mark buffer */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventMark\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
      }
      break;
    
    case OMX_EventResourcesAcquired:  /* Event resources acquired */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventResourcesAcquired\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
      }
      break;
    
    case OMX_EventComponentResumed: /* Event component resumed */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventComponentResumed\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
      }
      break;
    
    case OMX_EventDynamicResourcesAvailable:// Event dynamic resources available
      {
        PRINT(PRINT_SIMPLE_MESSAGE, 
              "In %s: OMX_EventDynamicResourcesAvailable\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eventSem);
      }
      break;
    
    case OMX_EventMax: // Event not implemented
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param1 is %i\n",
              __func__, (int)Data1);
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param2 is %i\n",
              __func__, (int)Data2);
        err = OMX_ErrorNotImplemented;
      }
      break;
    
    default:    // Event Bad parameter
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param1 is %i\n",
              __func__, (int)Data1);
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param2 is %i\n",
              __func__, (int)Data2);
        err = OMX_ErrorBadParameter;
      }
      break;

  } // switch(eEvent)
    
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Callback , instanc #> : <%s , %d>\n",
                              __func__, (int)vpssVfccClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_InputDataRead Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a VPSS VFCC thread which reads data from input file
 *
 *  @param in:
 *            threadArg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void VPSSVFCC_InputDataRead(void *threadsArg)
{
  unsigned int actualSize = 0,dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
  unsigned int numRemainingIn = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->vpssVfccClient);
    
  vpssVfccClient = strmDataThreadArg->vpssVfccClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == vpssVfccClient->inPortParams.flagInputEos)
  {
  
    /* waiting for empty input buffer via callback */
    TIMM_OSAL_SemaphoreObtain(vpssVfccClient->inPortParams.inputDataReady, 
                              TIMM_OSAL_SUSPEND);

    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (vpssVfccClient->inPortParams.ipBufPipe,
                                        (void*) &numRemainingIn);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Empty This buffer:%d\n",\
          (int) numRemainingIn);

    while(numRemainingIn)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering EmptyThisBuffer Loop\n");
    
      /* Read empty buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe (vpssVfccClient->inPortParams.ipBufPipe,
                              &pBufferIn,
                              sizeof(pBufferIn),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );

      /* Fill the data in the empty buffer */
      dataRead = VPSSVFCC_FillData(
                        vpssVfccClient,
                        pBufferIn,
                        vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount);

      /* Exit the loop if no data available */
      if(0 >= dataRead)
      {
          PRINT(PRINT_ERR_MSG, "No dataRead\n");
          vpssVfccClient->inPortParams.flagInputEos = OMX_TRUE;
          break;
      }
      else
      {
        /* increase the frame counter if some data is available */
        vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount++;
      }
    
      PRINT(PRINT_DEBUG_MESSAGE, "Address of pBufferIn is: %x\n",
                                  (unsigned int) pBufferIn);
      PRINT(PRINT_DEBUG_MESSAGE, "nAlloca length is: %d\n", \
                                  (int) pBufferIn->nAllocLen);
      PRINT(PRINT_DEBUG_MESSAGE, "nFilled length is: %d\n", \
                                  (int) pBufferIn->nFilledLen);
      
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
    
      /* Get the current state of the component */
      err = OMX_GetState(vpssVfccClient->handle, &pState);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetState");
      
      if(OMX_ErrorNone != err)
      {
        PRINT(PRINT_ERR_MSG, "%s: Error in fetching current state", __func__);
      }
      else
      {
        /* Don't feed buffers to the component if the component is not in */
        /* executing or pause state and empty the input pipe by fetching the */
        /* buffers */
        if((OMX_StateExecuting != pState ) && (OMX_StatePause != pState ))
        {
          /* decrease the counter of remaining buffers in input buffer pipe */
          numRemainingIn--;

          /* Reduce the frame counter & seek back as these frame is not supplied 
             to component and will be read again in VPSSVFCC_UseInitialResources
             on Func/API mode thread creation */
          if(0 == vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount)
            vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount = 0;
          else
            vpssVfccClient->inPortParams.comp.vpssVfcc.frameCount--;
          
          fseek(vpssVfccClient->inPortParams.fin, -dataRead, SEEK_CUR);

          continue;
        }
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the input buffer to the component */
      err = OMX_EmptyThisBuffer(vpssVfccClient->handle, pBufferIn);
    
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_EmptyThisBuffer");
    
      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }

      /* decrease the counter of remaining buffers in input buffer pipe */
      numRemainingIn--;
    
    } // while(numRemainingIn)

    /* Added to exit based on the EOS received from input read thread as */
    /* component is not reporting EOS event currently */
    if(OMX_BUFFERFLAG_EOS == pBufferIn->nFlags)
    {
      vpssVfccClient->inPortParams.flagInputEos = OMX_TRUE;
      vpssVfccClient->outPortParams.flagEos = OMX_TRUE;
    }
  
  }
  
  PRINT(DEFAULT_MESSAGES, "The execution of the video decoding\
                           input data read is terminated\n");

  /* post input buffer read thread exit */
  TIMM_OSAL_SemaphoreRelease(vpssVfccClient->inPortParams.inputDataReadThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(vpssVfccClient->inPortParams.inDataStrmThrdId, NULL);
}


/******************************************************************************\
 *      VPSSVFCC_OutputDataWrite Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a VPSS VFCC thread which writes data to output file
 *
 *  @param in:
 *            threadArg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void VPSSVFCC_OutputDataWrite(void *threadsArg)
{
  unsigned int actualSize = 0,framesDecoded = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
  unsigned int numRemainingOut = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;

  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->vpssVfccClient);
  
  vpssVfccClient = strmDataThreadArg->vpssVfccClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == vpssVfccClient->outPortParams.flagEos)
  {
    /* waiting for filled output buffer via callback */
    TIMM_OSAL_SemaphoreObtain(vpssVfccClient->outPortParams.outputDataReady, 
                              TIMM_OSAL_SUSPEND);
    
    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (vpssVfccClient->outPortParams.opBufPipe,
                                        (void*) &numRemainingOut);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Fill This buffer: %d\n",\
          (int) numRemainingOut);
    
    while(numRemainingOut)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering FillThisBuffer Loop\n");
    
      /* Read filled buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe( vpssVfccClient->outPortParams.opBufPipe,
                              &pBufferOut,
                              sizeof(pBufferOut),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );
      
      if(OMTB_FILE == vpssVfccClient->outPortParams.dataOutPutMode)
      {
        /* write data to output file */
        fwrite( pBufferOut->pBuffer,
                sizeof(char),
                vpssVfccClient->outPortParams.bufferOutSize,
                vpssVfccClient->outPortParams.fout);
        
        PRINT(PRINT_SIMPLE_MESSAGE, "Frame:%d Decoded\n",++framesDecoded );
        fflush (vpssVfccClient->outPortParams.fout);
      }
      else if(OMTB_DRIVER == vpssVfccClient->outPortParams.dataOutPutMode)
      {
      
      }

      /* reset the buffer filled length */
      pBufferOut->nFilledLen = 0;

      /* Exit the loop if EOS is reached */
      if(OMX_BUFFERFLAG_EOS == pBufferOut->nFlags)
      {
      
        PRINT(PRINT_MUST, "EOS reached ...\n");
        vpssVfccClient->outPortParams.flagEos = OMX_TRUE;
        break;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Get the current state of the component */
      err = OMX_GetState(vpssVfccClient->handle, &pState);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetState");
      
      if(OMX_ErrorNone != err)
      {
        PRINT(PRINT_ERR_MSG, "%s: Error in fetching current state", __func__);
      }
      else
      {
        /* Don't feed buffers to the component if the component is not in */
        /* executing or pause state and empty the output pipe by fetching the */
        /* buffers */
        if((OMX_StateExecuting != pState ) && (OMX_StatePause != pState ))
        {
          /* decrease the counter of remaining buffers in output buffer pipe */
          numRemainingOut--;
          continue;
        }
      }
    
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the output buffer to the component */
      err = OMX_FillThisBuffer(vpssVfccClient->handle, pBufferOut);
      
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_FillThisBuffer");
      
      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }

      /* decrease the counter of remaining buffers in output buffer pipe */
      numRemainingOut--;
    
    } // while(numRemainingOut)
  
  }
  
  PRINT(DEFAULT_MESSAGES, "The execution of the video decoding\
                           output data write is terminated\n");

  /* Post the EOS reached semaphore */
  TIMM_OSAL_SemaphoreRelease(vpssVfccClient->eofSem);

  /* waiting for output buffer write thread exit */
  TIMM_OSAL_SemaphoreRelease(
            vpssVfccClient->outPortParams.outputDataWriteThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)vpssVfccClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(vpssVfccClient->outPortParams.outDataStrmThrdId, NULL);
}


/******************************************************************************\
 *      VPSSVFCC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the HDVPSS VFCC component
 *
 *  @param in:
 *            hComponent: Component Handle
 *
 *            ptrAppData: Pointer to Private Data Structure
 *
 *            pBuffer   : Pointer to i/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE VPSSVFCC_EmptyBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_VPSS_VFCC_PRIVATE* pAppData = ptrAppData;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retVal = TIMM_OSAL_ERR_NONE;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuffer);
  
  /* API entry time calculation macro */
  //  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)pAppData->instanceNo);
  PRINT(PRINT_DEBUG_MESSAGE, "Address of pBuffer in emptybuff: %x\n",
                              (unsigned int) pBuffer);
  
  /* write the empty buffer pointer to input pipe */
  retVal = TIMM_OSAL_WriteToPipe( pAppData->inPortParams.ipBufPipe,
                                  &pBuffer,
                                  sizeof(pBuffer),
                                  TIMM_OSAL_SUSPEND);
  
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "Error writing to Input buffer i/p Pipe!\n");
    err = OMX_ErrorNotReady;
    return err;
  }
  
  /* Post the input data read thread */
  TIMM_OSAL_SemaphoreRelease(pAppData->inPortParams.inputDataReady);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)pAppData->instanceNo);
  
  /* API exit time calculation macro */
  //  OMTB_OMX_API_EXIT_TIME_CALC("VPSSVFCC_EmptyBufferDone");
  
  return err;
}


/******************************************************************************\
 *      VPSSVFCC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the HDVPSS VFCC component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE VPSSVFCC_FillBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_VPSS_VFCC_PRIVATE* pAppData = ptrAppData;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retVal = TIMM_OSAL_ERR_NONE;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuffer);
  
  /* API entry time calculation macro */
  //  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)pAppData->instanceNo);
  PRINT(PRINT_DEBUG_MESSAGE, "Address of pBuffer in fillbuff: %x\n",
                              (unsigned int) pBuffer);
  
  /* write the filled buffer pointer to output pipe */
  retVal = TIMM_OSAL_WriteToPipe( pAppData->outPortParams.opBufPipe,
                                  &pBuffer,
                                  sizeof(pBuffer),
                                  TIMM_OSAL_SUSPEND);
  
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "Error writing to Output buffer o/p Pipe!\n");
    err = OMX_ErrorNotReady;
    return err;
  }
  
  /* Post the output data write thread */
  TIMM_OSAL_SemaphoreRelease(pAppData->outPortParams.outputDataReady);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)pAppData->instanceNo);
  
  /* API exit time calculation macro */
  //  OMTB_OMX_API_EXIT_TIME_CALC("VPSSVFCC_FillBufferDone");
  
  return err;
}


/*----------------------------- end of file ----------------------------------*/

