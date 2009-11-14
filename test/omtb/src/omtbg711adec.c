/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/*  @file       omtbG711Adec.c
 *
 *  @brief      G711 Audio Decode IL Client Implementation.
 *
 * <B> History: </B>
 *
 *      # July-09-2009    AVME System Test    Initial Version
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

static OMX_CALLBACKTYPE G711ADEC_Callback = {
  .EventHandler = G711ADEC_EventHandler,      /* Event handler callback */
  .EmptyBufferDone = G711ADEC_EmptyBufferDone,/* Empty buffer done callback */
  .FillBufferDone = G711ADEC_FillBufferDone   /* Fill buffer done callback */
};


/******************************************************************************\
 *      Extern Object Definitions
\******************************************************************************/

// Streaming (I/P and O/P) thread arguments
extern OMTB_DATA_THREAD_ARG strmThreadArg[OMTB_MAX_INS];

// Component class structure which holds component instance handle, application
// private data etc
extern OMTB_COMP_HANDLE omtbClass[OMTB_MAX_CLASS];

/* Pointer to OMTB configuration template */
extern OMTB_TEMPLATE_INFO *omtbCfgTemplate[OMTB_MAX_INS];


/******************************************************************************\
 *      Globals
\******************************************************************************/

OMTB_G711_ADEC_CONFIG_PARAMS *g711AdecConfig;/* Pointer to G711 config params */

OmtbAudInfo *alsaConfig;                  /* Pointer to audio playback driver 
                                             configuration parameters */


/******************************************************************************\
 *      Static Globals
\******************************************************************************/

static OMX_U64 G711ADEC_funcCreateCount = 0;  /* Func create count */
static OMX_U64 G711ADEC_apiCreateCount  = 0;  /* API create count */

static OMX_U64 G711ADEC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U64 G711ADEC_apiDeleteCount  = 0;  /* API delete count */


/* Enum Entry for Supported cfg param indexs */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblG711AdecCfgParamIdx[] =
{
  /* ADD ENTRIES HERE for SetConfig/SetParam/GetConfig/GetParam */
  {"", 0, ""}
};


/******************************************************************************\
 *      Public Function Prototype
\******************************************************************************/


/******************************************************************************\
 *      Private Function Declarations
\******************************************************************************/


/******************************************************************************\
 *      Function Definitions
\******************************************************************************/

/******************************************************************************\
 *      G711ADEC_AllocateMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate memory for private data structure for g711 Audio decode
 *            component.
 *
 *  @param in:
 *            g711AdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB error codes
 *
 */

OmtbRet G711ADEC_AllocateMemory(OMTB_G711_ADEC_PRIVATE *g711AdecClient)
{
  OmtbRet err = OMTB_SUCCESS;
  unsigned int retVal = TIMM_OSAL_ERR_NONE;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  /* Allocating memory for callback structure */
  g711AdecClient->cb = (OMX_CALLBACKTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_CALLBACKTYPE),
                                      TIMM_OSAL_TRUE, 
                                      0, 
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->cb)
  {
    PRINT(PRINT_ERR_MSG, "Callback structure memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for input port definition structure */
  g711AdecClient->inPortParams.pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);

  if (!g711AdecClient->inPortParams.pInPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Input port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for output port definition structure */
  g711AdecClient->outPortParams.pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->outPortParams.pOutPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Output port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for component specific structures if any */
  if (OMX_AUDIO_CodingG711 == g711AdecClient->compressionFormat)
  {
    /* G711 specific memory allocation can be done here */
  }
  else
  {
    PRINT(PRINT_ERR_MSG, "Invalid compression format value\n");
    err = OMTB_NOT_SUPPORTED;
  }
  
  /* Create input pipe for input buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(g711AdecClient->inPortParams.ipBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      g711AdecClient->inPortParams.numInBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), 
                    OMX_TRUE );
  
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG,"TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Create output pipe for output buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(g711AdecClient->outPortParams.opBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      g711AdecClient->outPortParams.numOutBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), 
                    OMX_TRUE );
  
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG,"TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
#if 0
  /* Allocating memory for component event handling semaphore */
  g711AdecClient->eventSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->eventSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for eof handling semaphore */
  g711AdecClient->eofSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->eofSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for input data read thread exit semaphore */
  g711AdecClient->inPortParams.inputDataReadThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->inPortParams.inputDataReadThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  /* Allocating memory for input data read thread control semaphore */
  g711AdecClient->inPortParams.inputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->inPortParams.inputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for output data write thread exit semaphore */
  g711AdecClient->outPortParams.outputDataWriteThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!g711AdecClient->outPortParams.outputDataWriteThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  /* Allocating memory for output data write thread semaphore */
  g711AdecClient->outPortParams.outputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  if (!g711AdecClient->outPortParams.outputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
#endif
  /* Allocating memory for mono to stereo conversion in case of audio play */
  if(OMTB_DRIVER == g711AdecClient->outPortParams.dataOutPutMode)
  {
    /* Buffer to hold mono data given by the component */
    g711AdecClient->outPortParams.comp.g711Adec.tempBuff = (char *)
                    TIMM_OSAL_Malloc( OMTB_G711_ADEC_MAX_OUTBUFF_SIZE,
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
    if (!g711AdecClient->outPortParams.comp.g711Adec.tempBuff)
    {
      PRINT(PRINT_ERR_MSG, "Temporary mono buffer memory allocation failed\n");
      err = OMTB_INSUFFICIENT_RESOURCES;
    }
  
    /* Buffer to hold stereo data after mono to stereo conversion */
    g711AdecClient->outPortParams.comp.g711Adec.stereoBuff = (char *)
                    TIMM_OSAL_Malloc((OMTB_G711_ADEC_MAX_OUTBUFF_SIZE * 2),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
    if (!g711AdecClient->outPortParams.comp.g711Adec.stereoBuff)
    {
      PRINT(PRINT_ERR_MSG, "Stereo buffer memory allocation failed\n");
      err = OMTB_INSUFFICIENT_RESOURCES;
    }
  }

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      G711ADEC_AllocateResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate input port and output port buffers
 *
 *  @param in:
 *            g711AdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_AllocateResources(OMTB_G711_ADEC_PRIVATE* g711AdecClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  unsigned char *pBuffer = NULL;  
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  /* Input port buffer allocation */
  for(i = 0; i < g711AdecClient->inPortParams.pInPortDef->nBufferCountActual; 
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == g711AdecClient->inPortParams.flagUseInBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
  
      err = OMX_AllocateBuffer(
                        g711AdecClient->handle,
                        &g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i],
                        g711AdecClient->inPortParams.pInPortDef->nPortIndex, 
                        g711AdecClient,
                        g711AdecClient->inPortParams.pInPortDef->nBufferSize);
  
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
      /* Allocating memory for input port buffers */
      pBuffer = (unsigned char*)
        TIMM_OSAL_Malloc( g711AdecClient->inPortParams.pInPortDef->nBufferSize,
                          OMTB_CMEM_ALLOCATION,
                          0,
                          TIMMOSAL_MEM_SEGMENT_EXT);
      
      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for i/p port\n");

        /* Function to release the resources allocated by component */
        err = G711ADEC_FreeResources(g711AdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }
  
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
  
      err = OMX_UseBuffer(g711AdecClient->handle,
                        &g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i],
                        g711AdecClient->inPortParams.pInPortDef->nPortIndex, 
                        g711AdecClient,
                        g711AdecClient->inPortParams.pInPortDef->nBufferSize,
                        pBuffer);
  
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = G711ADEC_FreeResources(g711AdecClient);

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
  
      PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",\
          (unsigned int) g711AdecClient->handle);
      PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n", \
          (unsigned int) g711AdecClient->inPortParams.pInPortDef->nPortIndex);
      PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",\
          (unsigned int) g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]);
    }
  }
  
  /* Output port buffer allocation */
  for(i = 0; i < g711AdecClient->outPortParams.pOutPortDef->nBufferCountActual; 
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == g711AdecClient->outPortParams.flagUseOutBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
  
      err = OMX_AllocateBuffer(
                      g711AdecClient->handle,
                      &g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i],
                      g711AdecClient->outPortParams.pOutPortDef->nPortIndex,
                      g711AdecClient,
                      g711AdecClient->outPortParams.pOutPortDef->nBufferSize);
  
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
      /* Allocating memory for output port buffers */
      pBuffer = (unsigned char*)
      TIMM_OSAL_Malloc( g711AdecClient->outPortParams.pOutPortDef->nBufferSize,
                        OMTB_CMEM_ALLOCATION,
                        0,
                        TIMMOSAL_MEM_SEGMENT_EXT);
      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for o/p port\n");

        /* Function to release the resources allocated by component */
        err = G711ADEC_FreeResources(g711AdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }
  
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
  
      err = OMX_UseBuffer(g711AdecClient->handle,
                      &g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i],
                      g711AdecClient->outPortParams.pOutPortDef->nPortIndex,
                      g711AdecClient,
                      g711AdecClient->outPortParams.pOutPortDef->nBufferSize,
                      pBuffer);
  
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = G711ADEC_FreeResources(g711AdecClient);

      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }
      
      return tempErr;
    }
    else
    {
      PRINT(PRINT_DEBUG_MESSAGE,\
        "Buffers allocated successfully for o/p port\n");
  
      PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",
        (unsigned int) g711AdecClient->handle);
      PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n",
        (unsigned int) g711AdecClient->outPortParams.pOutPortDef->nPortIndex);
      PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",
        (unsigned int) g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i]);
    }
  }
  
  PRINT(PRINT_DEBUG_MESSAGE, "Before locking on idle semaphore\n");
  
  /* waiting for component to go to idle state */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem, TIMM_OSAL_SUSPEND);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      G711ADEC_UseInitialResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Tells G711 decode component to empty/fill the initial buffers.
 *
 *  @param in:
 *            g711AdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_UseInitialResources(
  OMTB_G711_ADEC_PRIVATE* g711AdecClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int dataRead = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  /* Give input buffers to component which is limited by no of input buffers 
     available. Rest of the data will be read on the callback from input data 
     read thread */
  for(i = 0; i < g711AdecClient->inPortParams.pInPortDef->nBufferCountActual; 
      i++)
  {
  
    dataRead = fread(
               g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->pBuffer,
               sizeof(char),
               g711AdecClient->inPortParams.bufferInSize,
               g711AdecClient->inPortParams.fin);
    
    g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->nFilledLen= dataRead;
    g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->nOffset = 0;
    g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->nAllocLen = dataRead;
    g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->nInputPortIndex = 0;
    
    PRINT(PRINT_DEBUG_MESSAGE, "--> Read %d bytes from file\n",dataRead);
    PRINT(PRINT_DEBUG_MESSAGE, "Empty first  buffer %x\n",
          (int)g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->pBuffer);
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer(
                      g711AdecClient->handle,
                      g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]);
    
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
  for(i = 0; i < g711AdecClient->outPortParams.pOutPortDef->nBufferCountActual; 
      i++)
  {
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer( 
                      g711AdecClient->handle,
                      g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i]);
    
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
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *                G711ADEC_SetPortDefinition Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for inport and outport port params
 *
 *  @param in:
 *            g711AdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_SetPortDefinition(OMTB_G711_ADEC_PRIVATE* g711AdecClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_PORT_PARAM_TYPE portInit;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader(&portInit,sizeof(OMX_PORT_PARAM_TYPE));
  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;
  
  /* output buffer size calculation based on input dimension speculation */
  g711AdecClient->inPortParams.bufferInSize   = OMTB_G711_ADEC_MAX_INBUFF_SIZE;
  g711AdecClient->outPortParams.bufferOutSize = OMTB_G711_ADEC_MAX_OUTBUFF_SIZE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Initialize the Port*/
  err = OMX_SetParameter( g711AdecClient->handle,
                          OMX_IndexParamAudioInit,
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
  OMTB_SetHeader( g711AdecClient->inPortParams.pInPortDef, 
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  g711AdecClient->inPortParams.pInPortDef->nPortIndex         = 0x0;
  g711AdecClient->inPortParams.pInPortDef->eDir               = OMX_DirInput;
  g711AdecClient->inPortParams.pInPortDef->nBufferCountActual = 
                                    g711AdecClient->inPortParams.numInBuf;
  g711AdecClient->inPortParams.pInPortDef->nBufferCountMin    = 1;
  g711AdecClient->inPortParams.pInPortDef->nBufferSize        = 
                                    g711AdecClient->inPortParams.bufferInSize;
  g711AdecClient->inPortParams.pInPortDef->bEnabled           = OMX_TRUE;
  g711AdecClient->inPortParams.pInPortDef->bPopulated         = OMX_FALSE;
  g711AdecClient->inPortParams.pInPortDef->eDomain            = 
                                    OMX_PortDomainAudio;
  g711AdecClient->inPortParams.pInPortDef->bBuffersContiguous = OMX_FALSE;
  g711AdecClient->inPortParams.pInPortDef->nBufferAlignment   = 0x0;
  
  /* OMX_AUDIO_PORTDEFINITIONTYPE values for input port */
  g711AdecClient->inPortParams.pInPortDef->format.audio.cMIMEType     = "ADEC";
  g711AdecClient->inPortParams.pInPortDef->format.audio.pNativeRender = NULL;
  g711AdecClient->inPortParams.pInPortDef->format.audio.bFlagErrorConcealment =
                                    OMX_FALSE;
  g711AdecClient->inPortParams.pInPortDef->format.audio.eEncoding     = 
                                    OMX_AUDIO_CodingG711;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the inPort Definition*/
  err = OMX_SetParameter( g711AdecClient->handle,
                          OMX_IndexParamPortDefinition,
                          g711AdecClient->inPortParams.pInPortDef);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  
  OMTB_SetHeader( g711AdecClient->outPortParams.pOutPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  g711AdecClient->outPortParams.pOutPortDef->nPortIndex         = 0x1;
  g711AdecClient->outPortParams.pOutPortDef->eDir               = OMX_DirOutput;
  g711AdecClient->outPortParams.pOutPortDef->nBufferCountActual = 
                                   g711AdecClient->outPortParams.numOutBuf;
  g711AdecClient->outPortParams.pOutPortDef->nBufferCountMin    = 1;
  g711AdecClient->outPortParams.pOutPortDef->nBufferSize        = 
                                   g711AdecClient->outPortParams.bufferOutSize;
  g711AdecClient->outPortParams.pOutPortDef->bEnabled           = OMX_TRUE;
  g711AdecClient->outPortParams.pOutPortDef->bPopulated         = OMX_FALSE;
  g711AdecClient->outPortParams.pOutPortDef->eDomain            = 
                                   OMX_PortDomainAudio;
  g711AdecClient->outPortParams.pOutPortDef->bBuffersContiguous = OMX_FALSE;
  g711AdecClient->outPortParams.pOutPortDef->nBufferAlignment   = 0x0;
  
  /* OMX_AUDIO_PORTDEFINITIONTYPE values for output port */
  g711AdecClient->outPortParams.pOutPortDef->format.audio.cMIMEType   = "ADEC";
  g711AdecClient->outPortParams.pOutPortDef->format.audio.pNativeRender = NULL;
  g711AdecClient->outPortParams.pOutPortDef->format.audio.bFlagErrorConcealment=
                                   OMX_FALSE;
  g711AdecClient->outPortParams.pOutPortDef->format.audio.eEncoding     = 
                                   OMX_AUDIO_CodingMax;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the OutPort Definition*/
  err = OMX_SetParameter (g711AdecClient->handle,
                          OMX_IndexParamPortDefinition,
                          g711AdecClient->outPortParams.pOutPortDef);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      G711ADEC_ApiTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    G711 Adec API mode Execute Thread
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

void G711ADEC_ApiTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OmtbAudInfo  *audPlayDrvClient = NULL;
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->g711AdecClient);
  ASSERT(NULL != funcDataThreadArg->audPlayDrvClient);

  g711AdecClient = funcDataThreadArg->g711AdecClient;
  audPlayDrvClient  = funcDataThreadArg->audPlayDrvClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* Update the api mode create count */
  G711ADEC_apiCreateCount++;
  PRINT(PRINT_MUST, "%s: G711 API Mode Create Count: %u\n", __func__,
                    (unsigned int) G711ADEC_apiCreateCount);

  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem, TIMM_OSAL_SUSPEND);
  
  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = G711ADEC_UseInitialResources(g711AdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Data streaming thread arguments */
  strmThreadArg[g711AdecClient->instanceNo].g711AdecClient = g711AdecClient;
  strmThreadArg[g711AdecClient->instanceNo].audPlayDrvClient = audPlayDrvClient;

  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&g711AdecClient->inPortParams.inDataStrmThrdId,
                    (void *)G711ADEC_InputDataRead,
                    0,
                    &strmThreadArg[g711AdecClient->instanceNo],
                    OMTB_G711_ADEC_TSK_STACK_SIZE,
                    OMTB_G711_ADEC_GROUP_PRIORITY,
                    (signed char*)"G711_ADEC_DATA_IN_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
                "Unable to start G711 audio decode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&g711AdecClient->outPortParams.outDataStrmThrdId,
                    (void *)G711ADEC_OutputDataWrite,
                    0,
                    &strmThreadArg[g711AdecClient->instanceNo],
                    OMTB_G711_ADEC_TSK_STACK_SIZE,
                    OMTB_G711_ADEC_GROUP_PRIORITY,
                    (signed char*)"G711_ADEC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
              "Unable to start G711 audio decode output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eofSem, TIMM_OSAL_SUSPEND);

  /* Update the api mode delete count */
  G711ADEC_apiDeleteCount++;
  PRINT(PRINT_MUST, "%s: G711 API Mode Delete Count: %u\n", __func__,
                    (unsigned int) G711ADEC_apiDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  g711AdecClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_G711_ADEC_CLASS].execThreadId[g711AdecClient->instanceNo], 
      NULL);
}


/******************************************************************************\
 *                G711ADEC_GetHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get the handle for G711 Audio Decode component
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

OMX_ERRORTYPE G711ADEC_GetHandle(
  OMTB_COMP_HANDLE* classPtr, 
  unsigned int instance)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  char inputFile[OMTB_MAX_LEN]  = {"\0"};
  char outputFile[OMTB_MAX_LEN] = {"\0"};
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
  int driverReturn = OMTB_FAIL;
  OMTB_OMX_API_TIMING_VAR_DECL;
  OmtbAudInfo  *audPlayDrvClient = NULL;

  /* validate the function arguments */
  ASSERT(NULL != classPtr);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)instance);
  
  /* Allocate memory for application private data */
  g711AdecClient = (OMTB_G711_ADEC_PRIVATE *)
                    TIMM_OSAL_Malloc( sizeof(OMTB_G711_ADEC_PRIVATE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  if (!g711AdecClient)
  {
    PRINT(PRINT_ERR_MSG, "Error allocating g711AdecClient structure \n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    return err;
  }
  else
  {
    /* Update the application instance management structure with app pvt data
       structure pointer */
    classPtr->g711AdecClient[instance] = 
                              (OMTB_G711_ADEC_PRIVATE *)g711AdecClient;
    memset(g711AdecClient, 0, sizeof(OMTB_G711_ADEC_PRIVATE));
    g711AdecClient->instanceNo = instance;
  }

  /* Make local copy of input and output file names */
  strcpy(outputFile, g711AdecConfig->outputFile);
  strcpy(inputFile,  g711AdecConfig->inputFile);
  strcpy(g711AdecClient->compName, g711AdecConfig->compName);

  PRINT(PRINT_DEBUG_MESSAGE, "Component Name is:%s\n", 
                              g711AdecClient->compName);
  PRINT(PRINT_DEBUG_MESSAGE, "Input File is:%s\n", inputFile);
  PRINT(PRINT_DEBUG_MESSAGE, "Output File is:%s\n", outputFile);
  
  /* Data output mode selection - File/Driver */
  g711AdecClient->outPortParams.dataOutPutMode = 
                                            g711AdecConfig->dataOutPutMode;
  
  /* Buffer allocation mode - OMX_UseBuffer or OMX_AllocateBuffer */
  g711AdecClient->inPortParams.flagUseInBuffer   = 
                                            g711AdecConfig->flagUseInBuffer;
  g711AdecClient->outPortParams.flagUseOutBuffer = 
                                            g711AdecConfig->flagUseOutBuffer;

  /* no of i/p and o/p buffers */
  g711AdecClient->inPortParams.numInBuf   = g711AdecConfig->numInBuf;
  g711AdecClient->outPortParams.numOutBuf = g711AdecConfig->numOutBuf;
  
  /* Open the input file */
  g711AdecClient->inPortParams.fin = fopen(inputFile, "rb");
  if(NULL == g711AdecClient->inPortParams.fin)
  {
    PRINT(PRINT_ERR_MSG, "Error in opening input file %s\n",inputFile);
    err = OMTB_INSUFFICIENT_RESOURCES;
    G711ADEC_FreeMemory(g711AdecClient);
    return err;
  }
  
  if(OMTB_FILE == g711AdecClient->outPortParams.dataOutPutMode)
  {
    /* Open the output file */
    g711AdecClient->outPortParams.fout = fopen(outputFile, "wb");
    if(NULL == g711AdecClient->outPortParams.fout)
    {
      PRINT(PRINT_ERR_MSG,"Error in opening output file %s\n",outputFile);
      err = OMTB_INSUFFICIENT_RESOURCES;
      G711ADEC_FreeMemory(g711AdecClient);
      return err;
    }
  }
  else if(OMTB_DRIVER == g711AdecClient->outPortParams.dataOutPutMode)
  {
    /* Allocate memory for play driver cfg params */
    audPlayDrvClient = (OmtbAudInfo*)
                    TIMM_OSAL_Malloc( sizeof(OmtbAudInfo),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
    if(!audPlayDrvClient)
    {
      PRINT(PRINT_ERR_MSG, "Error allocating dssClient structure \n");
      err = OMTB_INSUFFICIENT_RESOURCES;
      G711ADEC_FreeMemory(g711AdecClient);
      return err;
    }
    else
    {
      classPtr->audPlayDrvClient[instance] = audPlayDrvClient;
    }
    
    /* Copy the driver configuration from the template */
    memset(audPlayDrvClient, 0, sizeof(OmtbAudInfo));
    memcpy(audPlayDrvClient, alsaConfig, sizeof(OmtbAudInfo));
    
    /* Initialize the ALSA playback driver */
    driverReturn = OMTB_AudInit(audPlayDrvClient);
    if (OMTB_FAIL == driverReturn)
    {
      PRINT(PRINT_ERR_MSG, "Unable to setup Audio device\n");
      err = OMTB_INSUFFICIENT_RESOURCES;
      G711ADEC_FreeMemory(g711AdecClient);
      return err;
    }
    else
       PRINT(PRINT_SIMPLE_MESSAGE, "Audio device initialized\n");
  }
  
  g711AdecClient->compressionFormat = OMX_AUDIO_CodingG711;
  
  /* output buffer size calculation based on input dimension speculation */
  g711AdecClient->inPortParams.bufferInSize   = OMTB_G711_ADEC_MAX_INBUFF_SIZE;
  g711AdecClient->outPortParams.bufferOutSize = OMTB_G711_ADEC_MAX_OUTBUFF_SIZE;

  /*Function to allocate memory for app pvt struct members*/
  err = G711ADEC_AllocateMemory(g711AdecClient);

  if(OMTB_SUCCESS != err)
  {
    PRINT(PRINT_ERR_MSG, "Unable to allocate memory for app pvt structure\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    G711ADEC_FreeMemory(g711AdecClient);
    return err;
  }

  /* Initialize the semaphores */
  TIMM_OSAL_SemaphoreCreate(&g711AdecClient->eventSem, 0);
  TIMM_OSAL_SemaphoreCreate(&g711AdecClient->eofSem, 0);
  TIMM_OSAL_SemaphoreCreate(&g711AdecClient->inPortParams.inputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&g711AdecClient->outPortParams.outputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&g711AdecClient->inPortParams.inputDataReadThrExit, 
                            0);
  TIMM_OSAL_SemaphoreCreate(
                  &g711AdecClient->outPortParams.outputDataWriteThrExit,
                  0);

  /*Assign the callbacks*/
  *g711AdecClient->cb = G711ADEC_Callback;

  /* Initialize the EOS flag */
  g711AdecClient->outPortParams.flagEos = OMX_FALSE;
  g711AdecClient->inPortParams.flagInputEos = (unsigned int) OMX_FALSE;

  /* Initialize the flag used to determine whether Func/API thread is created */
  g711AdecClient->flagThrCreated = OMX_FALSE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* get component handle */
  err = OMX_GetHandle(&g711AdecClient->handle,
                      (OMX_STRING)g711AdecClient->compName,
                      g711AdecClient,
                      g711AdecClient->cb);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetHandle");
  
  if(OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, "No component named \"%s\" found\n",
                          g711AdecClient->compName);
    OMTB_OmxReturnStatus(err);
    G711ADEC_FreeMemory(g711AdecClient);
    return err;
  }
  else
  {
    /* Update the application instance management strucutre with handle */
    classPtr->handle[instance] = g711AdecClient->handle;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-G711adec Instance #: %d\n\n", instance);
  
    PRINT(PRINT_SIMPLE_MESSAGE, "Component Handle Available\n");
  }
  
  PRINT(PRINT_DEBUG_MESSAGE, "bufferOutSize:%d \n",\
        (int)g711AdecClient->outPortParams.bufferOutSize);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      G711ADEC_GetStructIdx Function Definition
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

OmtbRet G711ADEC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct)
{
  unsigned int enumCount = 0;
  
  /* Use the specified template structure */
  g711AdecConfig = (OMTB_G711_ADEC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->g711AdecTemplate);

  /* Get the corresponding string for enum values from enum table 
     Set structure index
     Set structure pointer
     Set port index for which parameter to be updated 
     Set structure header info */
  while(strcmp(enumTblG711AdecCfgParamIdx[enumCount].name , ""))
  {
    if(0 == strcmp(enumTblG711AdecCfgParamIdx[enumCount].name,
                   indexStr))
    {
      switch(enumTblG711AdecCfgParamIdx[enumCount].value)
      {
        /* ADD Similar implementation below for supported indexes */
/*        case OMX_IndexConfigVideoFramerate:
          {
            *nConfigIndex   = OMX_IndexConfigVideoFramerate;
            *pCompCfgStruct = &g711AdecConfig->frameRate;
            g711AdecConfig->frameRate.nPortIndex = portIdx;
            OMTB_SetHeader( &g711AdecConfig->frameRate,
                            sizeof(OMX_CONFIG_FRAMERATETYPE));
          }
          return OMTB_SUCCESS;
*/           
        
          
        default:
          {
            return OMTB_FAIL;
          }

      } // switch(enumTblG711AdecCfgParamIdx[enumCount].value)
     
    } // if(0 == strcmp(enumTblG711AdecCfgParamIdx[enumCount].name, ...
      
    enumCount++;
  }

  return OMTB_SUCCESS;  
}


/******************************************************************************\
 *      G711ADEC_SetParameter Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for the appropriate index passsed
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

OMX_ERRORTYPE G711ADEC_SetParameter(
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
  err = G711ADEC_GetStructIdx(indexStr, 
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
 *      G711ADEC_SetConfig Function Definition
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

OMX_ERRORTYPE G711ADEC_SetConfig(
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
  err = G711ADEC_GetStructIdx(indexStr, 
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
 *      G711ADEC_GetParameter Function Definition
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

OMX_ERRORTYPE G711ADEC_GetParameter(
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
  err = G711ADEC_GetStructIdx(indexStr, 
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
 *      G711ADEC_GetConfig Function Definition
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

OMX_ERRORTYPE G711ADEC_GetConfig(
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
  err = G711ADEC_GetStructIdx(indexStr, 
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
 *      G711ADEC_FillData Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *            OMX_BUFFERHEADERTYPE *: Pointer of I/P filled Buffer
 *
 *  @param Out:
 *            None
 *
 *  @return   No of bytes read
 *
 */

unsigned int G711ADEC_FillData(
  OMTB_G711_ADEC_PRIVATE* pAppData, 
  OMX_BUFFERHEADERTYPE *pBuf)
{
  unsigned int dataRead = 0;

  /* validate the function arguments */
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuf);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
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

    PRINT(PRINT_DEBUG_MESSAGE, "Bytes read are:%d\n",dataRead);

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
    PRINT(PRINT_MUST, "%s: End of File reached\n", __func__);
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)pAppData->instanceNo);
  
  return dataRead;
}


/******************************************************************************\
 *      G711ADEC_FuncTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a G711 thread which invokes during func command
 *
 *  @param in:
 *            void *threadsArg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void G711ADEC_FuncTsk(void *threadsArg)
{
  int  threadRes = TIMM_OSAL_ERR_NONE;
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OmtbAudInfo  *audPlayDrvClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->g711AdecClient);
  ASSERT(NULL != funcDataThreadArg->audPlayDrvClient);
  
  g711AdecClient = funcDataThreadArg->g711AdecClient;
  audPlayDrvClient  = funcDataThreadArg->audPlayDrvClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  PRINT(PRINT_SIMPLE_MESSAGE, "nbufferOutSize:%d\n",\
        (int)g711AdecClient->outPortParams.bufferOutSize);

  /* Set the port params */
  err = G711ADEC_SetPortDefinition(g711AdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* send command to component to go to idle state */
  err = OMX_SendCommand(g711AdecClient->handle,
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
  err = G711ADEC_AllocateResources(g711AdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* send command to component to go to executing state */
  err = OMX_SendCommand(g711AdecClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem, TIMM_OSAL_SUSPEND);

  /* Update the func create count */
  G711ADEC_funcCreateCount++;
  PRINT(PRINT_MUST, "%s: G711 FUNC Create Count: %u\n", __func__,
                    (unsigned int) G711ADEC_funcCreateCount);

  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = G711ADEC_UseInitialResources(g711AdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* Update the data streaming thread arguments */
  strmThreadArg[g711AdecClient->instanceNo].g711AdecClient = g711AdecClient;
  strmThreadArg[g711AdecClient->instanceNo].audPlayDrvClient = audPlayDrvClient;
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&g711AdecClient->inPortParams.inDataStrmThrdId,
                    (void *)G711ADEC_InputDataRead,
                    0,
                    &strmThreadArg[g711AdecClient->instanceNo],
                    OMTB_G711_ADEC_TSK_STACK_SIZE,
                    OMTB_G711_ADEC_GROUP_PRIORITY,
                    (signed char*)"G711_ADEC_DATA_IN_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
              "Unable to start G711 audio decode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&g711AdecClient->outPortParams.outDataStrmThrdId,
                    (void *)G711ADEC_OutputDataWrite,
                    0,
                    &strmThreadArg[g711AdecClient->instanceNo],
                    OMTB_G711_ADEC_TSK_STACK_SIZE,
                    OMTB_G711_ADEC_GROUP_PRIORITY,
                    (signed char*)"G711_ADEC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
              "Unable to start G711 audio decode output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eofSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "The execution of the Audio decoding\
        process is terminated\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* state change of component from executing to idle */
  err = OMX_SendCommand(g711AdecClient->handle,
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
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "State transitioned to Idle\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to go to loaded state */
  err = OMX_SendCommand(g711AdecClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateLoaded,
                        NULL);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SendCommand");

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* reset the EOS flag */
  g711AdecClient->outPortParams.flagEos = OMX_FALSE;
  g711AdecClient->inPortParams.flagInputEos = (unsigned int) OMX_FALSE;
  
  /* Function to release the resources allocated by component */
  err = G711ADEC_FreeResources(g711AdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "State transitioned to loaded\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Delete the component instance */
  err = OMX_FreeHandle(g711AdecClient->handle);

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
    g711AdecClient->handle = NULL;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted G711adec Instance #: %d\n\n", 
          (int) g711AdecClient->instanceNo);
    omtbClass[OMTB_G711_ADEC_CLASS].handle[g711AdecClient->instanceNo] = NULL;
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "Audio dec freed\n");

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
  G711ADEC_FreeMemory(g711AdecClient);

  /* Update the func delete count */
  G711ADEC_funcDeleteCount++;
  PRINT(PRINT_MUST, "%s: G711 FUNC Delete Count: %u\n", __func__,
                      (unsigned int) G711ADEC_funcDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  g711AdecClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_G711_ADEC_CLASS].execThreadId[g711AdecClient->instanceNo], 
      NULL);
}


/******************************************************************************\
 *      G711ADEC_FreeResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            g711AdecClient: Pointer to OMTB_G711_ADEC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_FreeResources(OMTB_G711_ADEC_PRIVATE *g711AdecClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* Ask component to Free input port buffers */
  for(i = 0; i < g711AdecClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Audio dec I/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",\
      (unsigned int) g711AdecClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n", \
      (unsigned int) g711AdecClient->inPortParams.pInPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",\
      (unsigned int) g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]);

    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == g711AdecClient->inPortParams.flagUseInBuffer)
    {
      TIMM_OSAL_CMEM_Free (
              g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]->pBuffer, 
              g711AdecClient->inPortParams.pInPortDef->nBufferSize);
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* freeing buffers of component input ports */
    err = OMX_FreeBuffer( g711AdecClient->handle,
                        g711AdecClient->inPortParams.pInPortDef->nPortIndex,
                        g711AdecClient->inPortParams.comp.g711Adec.pInBuff[i]);

    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");

    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
  }

  /* Ask component to Free output port buffers */
  for(i = 0; i < g711AdecClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Audio dec O/P ports\n");

    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",
      (unsigned int) g711AdecClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n",
      (unsigned int) g711AdecClient->outPortParams.pOutPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",
      (unsigned int) g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i]);

    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == g711AdecClient->outPortParams.flagUseOutBuffer)
    {
      TIMM_OSAL_CMEM_Free (
              g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i]->pBuffer, 
              g711AdecClient->outPortParams.pOutPortDef->nBufferSize);
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* freeing buffers of component output ports */
    err = OMX_FreeBuffer( g711AdecClient->handle,
                      g711AdecClient->outPortParams.pOutPortDef->nPortIndex,
                      g711AdecClient->outPortParams.comp.g711Adec.pOutBuff[i]);

    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");

    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }

  }

  /* wait for event handler to report completion of loaded command */
  TIMM_OSAL_SemaphoreObtain(g711AdecClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      G711ADEC_FreeMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            g711AdecClient: Pointer to OMTB_G711_ADEC_PRIVATE struct
 *
 *  @return   None
 *
 */

void G711ADEC_FreeMemory(OMTB_G711_ADEC_PRIVATE *g711AdecClient)
{
  int instanceNo = g711AdecClient->instanceNo;

  /* validate the function arguments */
  ASSERT(NULL != g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  /* Release callback structure memory */
  if(NULL != g711AdecClient->cb)
    TIMM_OSAL_Free(g711AdecClient->cb);
  
  /* Release input port memory */
  if(NULL != g711AdecClient->inPortParams.pInPortDef)
    TIMM_OSAL_Free(g711AdecClient->inPortParams.pInPortDef);
  
  /* Release output port memory */
  if(NULL != g711AdecClient->outPortParams.pOutPortDef)
    TIMM_OSAL_Free(g711AdecClient->outPortParams.pOutPortDef);
  
  /* Release component event handling semaphore memory */
  if(NULL != g711AdecClient->eventSem)
    TIMM_OSAL_SemaphoreDelete(g711AdecClient->eventSem);
  
  /* Release EOF handling semaphore memory */
  if(NULL != g711AdecClient->eofSem)
    TIMM_OSAL_SemaphoreDelete(g711AdecClient->eofSem);
  
  /* Release input data read thread control semaphore memory */
  if(NULL != g711AdecClient->inPortParams.inputDataReady)
    TIMM_OSAL_SemaphoreDelete(g711AdecClient->inPortParams.inputDataReady);
  
  /* Release output data write thread control semaphore memory */
  if(NULL != g711AdecClient->outPortParams.outputDataReady)
    TIMM_OSAL_SemaphoreDelete(g711AdecClient->outPortParams.outputDataReady);

  /* Release input data read thread exit semaphore memory */
  if(NULL != g711AdecClient->inPortParams.inputDataReadThrExit)
    TIMM_OSAL_SemaphoreDelete(
              g711AdecClient->inPortParams.inputDataReadThrExit);
  
  /* Release output data write thread exit semaphore memory */
  if(NULL != g711AdecClient->outPortParams.outputDataWriteThrExit)
    TIMM_OSAL_SemaphoreDelete(
              g711AdecClient->outPortParams.outputDataWriteThrExit);
  
  if(OMTB_FILE == g711AdecClient->outPortParams.dataOutPutMode )
  {
    /** closing the output file */
    if(NULL != g711AdecClient->outPortParams.fout)
      fclose(g711AdecClient->outPortParams.fout);
  }
  else if(OMTB_DRIVER == g711AdecClient->outPortParams.dataOutPutMode)
  {
    OmtbAudInfo  *audPlayDrvClient;
    audPlayDrvClient = 
      omtbClass[OMTB_G711_ADEC_CLASS].audPlayDrvClient[\
                g711AdecClient->instanceNo];
    
    /* close the audio device */
    if(NULL != audPlayDrvClient)
      OMTB_AudioClose(audPlayDrvClient);

    /* Release the stereo buffer memory */
    if(NULL != g711AdecClient->outPortParams.comp.g711Adec.stereoBuff)
      TIMM_OSAL_Free(g711AdecClient->outPortParams.comp.g711Adec.stereoBuff);

    /* Release the temporary mono buffer memory */
    if(NULL != g711AdecClient->outPortParams.comp.g711Adec.tempBuff)
      TIMM_OSAL_Free(g711AdecClient->outPortParams.comp.g711Adec.tempBuff);
    
    /* Release play driver cfg param memory */
    if(NULL != audPlayDrvClient)
    {
      TIMM_OSAL_Free(audPlayDrvClient);
        omtbClass[OMTB_G711_ADEC_CLASS].audPlayDrvClient[\
                  g711AdecClient->instanceNo] = NULL;
    }
  }
  
  /* closing the input file */
  if(NULL != g711AdecClient->inPortParams.fin)
    fclose(g711AdecClient->inPortParams.fin);
  
  
  /* Release app pvt data private structure */
  if(NULL != g711AdecClient)
    TIMM_OSAL_Free(g711AdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instance #> : <%s , %d>\n",
                     __func__, (int)instanceNo);
}


/******************************************************************************\
 *      G711ADEC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the g711 Audio decoder component
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

OMX_ERRORTYPE G711ADEC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;

  /* thread argument validation */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  
  g711AdecClient = pAppData;
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

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
          TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
        }
        /* port enable or disable command related event */
        else if ( OMX_CommandPortEnable   == Data1  || 
                  OMX_CommandPortDisable  == Data1  )
        {
          PRINT(PRINT_SIMPLE_MESSAGE,
                "In %s: OMX_CommandPortEnable/OMX_CommandPortDisable\n",
                __func__);

          /* Post the component event sem */
          TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
        }
      }
      break;
    
    case OMX_EventPortSettingsChanged:  /* Port settings changed event */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventPortSettingsChanged\n",
                                    __func__);

        if(0 == Data2)
        {
          /* Perform the port settings as per params received from component */
        }
      }
      break;
      
    case OMX_EventBufferFlag: /* Buffer related events */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventBufferFlag\n", __func__);
        if(OMX_BUFFERFLAG_EOS == (int)Data2)
        {
          PRINT(PRINT_MUST, "OMX_EventBufferFlag - OMX_BUFFERFLAG_EOS\n");
          TIMM_OSAL_SemaphoreRelease(g711AdecClient->eofSem);
        }
      }
      break;
      
    case OMX_EventError:  /* Event error */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventError\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
      }
      break;
    
    case OMX_EventMark:   /* Event mark buffer */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventMark\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
      }
      break;
    
    case OMX_EventResourcesAcquired:  /* Event resources acquired */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventResourcesAcquired\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
      }
      break;
    
    case OMX_EventComponentResumed:   /* Event component resumed */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventComponentResumed\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
      }
      break;
    
    case OMX_EventDynamicResourcesAvailable:// Event dynamic resources available
      {
        PRINT(PRINT_SIMPLE_MESSAGE, 
              "In %s: OMX_EventDynamicResourcesAvailable\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(g711AdecClient->eventSem);
      }
      break;
      
    case OMX_EventMax:  // Event not implemented
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
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Callback , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *  G711ADEC_InputDataRead Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a G711 thread which reads data from input file.
 *
 *  @param in:
 *            thread_arg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void G711ADEC_InputDataRead(void *threadsArg)
{
  unsigned int actualSize = 0, dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
  unsigned int numRemainingIn = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->g711AdecClient);
  
  g711AdecClient = strmDataThreadArg->g711AdecClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == g711AdecClient->inPortParams.flagInputEos)
  {
    /* waiting for empty input buffer via callback */
    TIMM_OSAL_SemaphoreObtain(g711AdecClient->inPortParams.inputDataReady, 
                              TIMM_OSAL_SUSPEND);
    
    TIMM_OSAL_GetPipeReadyMessageCount (g711AdecClient->inPortParams.ipBufPipe,
                                        (void*) &numRemainingIn);

    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Empty This buffer:%d\n",\
              (int) numRemainingIn);

    while(numRemainingIn)
    {
      PRINT(PRINT_SIMPLE_MESSAGE,"Entering EmptyThisBuffer Loop\n");
    
      /* Read empty buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe (g711AdecClient->inPortParams.ipBufPipe,
                              &pBufferIn,
                              sizeof(pBufferIn),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );

      /* Fill the data in the empty buffer */
      dataRead = G711ADEC_FillData(g711AdecClient, pBufferIn);

      /* Exit the loop if no data available */
      if(0 >= dataRead)
      {
          PRINT(PRINT_ERR_MSG, "No dataRead\n");
          g711AdecClient->inPortParams.flagInputEos = OMX_TRUE;
          break;
      }
      
      PRINT(PRINT_DEBUG_MESSAGE, "Address of pBufferIn is: %x\n",
                                  (unsigned int) pBufferIn);
      PRINT(PRINT_DEBUG_MESSAGE, "nAlloc length is: %d\n", \
                                  (int) pBufferIn->nAllocLen);
      PRINT(PRINT_DEBUG_MESSAGE, "nFilled length is: %d\n", \
                                  (int) pBufferIn->nFilledLen);
      
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
    
      /* Get the current state of the component */
      err = OMX_GetState(g711AdecClient->handle, &pState);

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

          /* Seek back to the same frame as these frame is not supplied to 
             component and will be read again in G711ADEC_UseInitialResources on
             Func/API mode thread creation */
          fseek(g711AdecClient->inPortParams.fin, -dataRead, SEEK_CUR);

          continue;
        }
      }
      
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the input buffer to the component */
      err = OMX_EmptyThisBuffer(g711AdecClient->handle, pBufferIn);
    
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_EmptyThisBuffer");
    
      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }

      /* decrease the counter of remaining buffers in input buffer pipe */
      numRemainingIn--;
    }

    /* Added to exit based on the EOS received from input read thread as */
    /* component is not reporting EOS event currently */
    if(OMX_BUFFERFLAG_EOS == pBufferIn->nFlags)
    {
      g711AdecClient->inPortParams.flagInputEos = OMX_TRUE;
      g711AdecClient->outPortParams.flagEos = OMX_TRUE;
    }
  }
  
  PRINT(DEFAULT_MESSAGES, "The execution of the Audio decoding\
                           input data read is terminated\n");

  /* post input buffer read thread exit */
  TIMM_OSAL_SemaphoreRelease(g711AdecClient->inPortParams.inputDataReadThrExit);

  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* exit the thread */  
  TIMM_OSAL_ExitTask(g711AdecClient->inPortParams.inDataStrmThrdId, NULL);
}


/******************************************************************************\
 *      G711_AudecFunc Output Data Read Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a G711 thread which writes data to output file.
 *
 *  @param in:
 *            thread_arg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void G711ADEC_OutputDataWrite(void *threadsArg)
{
  unsigned int actualSize = 0,framesDecoded = 0, k = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_G711_ADEC_PRIVATE* g711AdecClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
  unsigned int numRemainingOut = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OmtbAudInfo  *audPlayDrvClient = NULL;
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->g711AdecClient);
  ASSERT(NULL != strmDataThreadArg->audPlayDrvClient);
  
  g711AdecClient = strmDataThreadArg->g711AdecClient;
  audPlayDrvClient = strmDataThreadArg->audPlayDrvClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == g711AdecClient->outPortParams.flagEos)
  {
  
    /* waiting for filled output buffer via callback */
    TIMM_OSAL_SemaphoreObtain(g711AdecClient->outPortParams.outputDataReady, 
                              TIMM_OSAL_SUSPEND);
    
    TIMM_OSAL_GetPipeReadyMessageCount (g711AdecClient->outPortParams.opBufPipe,
                                        (void*) &numRemainingOut);
    
    while(numRemainingOut)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering FillThisBuffer Loop\n");
    
      /* Read empty buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe( g711AdecClient->outPortParams.opBufPipe,
                              &pBufferOut,
                              sizeof(pBufferOut),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );
    
      if(OMTB_FILE == g711AdecClient->outPortParams.dataOutPutMode )
      {
        /* write data to output file */
        fwrite( pBufferOut->pBuffer,
                sizeof(char),
                pBufferOut->nFilledLen,
                g711AdecClient->outPortParams.fout);
    
        PRINT(PRINT_SIMPLE_MESSAGE, "Frame:%d Decoded\n",++framesDecoded );
        fflush (g711AdecClient->outPortParams.fout);
      }
      else if(OMTB_DRIVER == g711AdecClient->outPortParams.dataOutPutMode)
      {
        /* copy data to temporary buffer for mono to stereo conversion */
        memcpy( g711AdecClient->outPortParams.comp.g711Adec.tempBuff,
                pBufferOut->pBuffer,
                pBufferOut->nFilledLen);

        /* mono to stereo conversion */
        for(k = 0; k < pBufferOut->nFilledLen; k += 2)
        {
          g711AdecClient->outPortParams.comp.g711Adec.stereoBuff[k * 2] =  
          g711AdecClient->outPortParams.comp.g711Adec.stereoBuff[(k * 2) + 2] =
            g711AdecClient->outPortParams.comp.g711Adec.tempBuff[k];
        
          g711AdecClient->outPortParams.comp.g711Adec.stereoBuff[(k * 2) + 1] = 
          g711AdecClient->outPortParams.comp.g711Adec.stereoBuff[(k * 2) + 3] =
            g711AdecClient->outPortParams.comp.g711Adec.tempBuff[k + 1];
        }

        /* Give data to driver for playback */
        if(OMTB_FAIL == OMTB_AudioOut(
                      audPlayDrvClient,
                      g711AdecClient->outPortParams.comp.g711Adec.stereoBuff,
                      (pBufferOut->nFilledLen * 2)))
        {
          PRINT(PRINT_ERR_MSG, "Unable to Play Audio buffer\n");
        }
        else
        {
          PRINT(PRINT_SIMPLE_MESSAGE,"Played Frame# %d\n",++framesDecoded);
        }
      }

      /* reset the buffer filled length */
      pBufferOut->nFilledLen = 0;

      /* Exit the loop if EOS is reached */
      if(OMX_BUFFERFLAG_EOS == pBufferOut->nFlags)
      {
      
        PRINT(PRINT_MUST, "EOS reached...\n");
        g711AdecClient->outPortParams.flagEos = OMX_TRUE;
        break;
      }
      
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Get the current state of the component */
      err = OMX_GetState(g711AdecClient->handle, &pState);

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
      err = OMX_FillThisBuffer(g711AdecClient->handle, pBufferOut);
      
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_FillThisBuffer");
      
      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }

      /* decrease the counter of remaining buffers in output buffer pipe */
      numRemainingOut--;
    
    }  // while(numRemainingOut)
  
  }  // while(OMX_ErrorNone == err && OMX_FALSE == g711AdecClient->outPortPa ...
  
  PRINT(DEFAULT_MESSAGES, "The execution of the Audio decoding\
                           output data write is terminated\n");

  /* Post the EOS reached semaphore */
  TIMM_OSAL_SemaphoreRelease(g711AdecClient->eofSem);

  /* waiting for output buffer write thread exit */
  TIMM_OSAL_SemaphoreRelease(
            g711AdecClient->outPortParams.outputDataWriteThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)g711AdecClient->instanceNo);

  /* exit the thread */
  TIMM_OSAL_ExitTask(g711AdecClient->outPortParams.outDataStrmThrdId, NULL);
}


/******************************************************************************\
 *      G711ADEC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the g711 Audio decoder component
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

OMX_ERRORTYPE G711ADEC_EmptyBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{
  OMTB_G711_ADEC_PRIVATE* pAppData = ptrAppData;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retVal = TIMM_OSAL_ERR_NONE;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuffer);
  
  /* API entry time calculation macro */
  //OMTB_OMX_API_ENTRY_TIME_CALC;

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
  //OMTB_OMX_API_EXIT_TIME_CALC("G711ADEC_EmptyBufferDone");
  
  return err;
}


/******************************************************************************\
 *      G711ADEC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the g711 Audio decoder component
 *
 *  @param in:
 *            hComponent: Component Handle
 *
 *            ptrAppData: Pointer to Private Data Structure
 *
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE G711ADEC_FillBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{
  OMTB_G711_ADEC_PRIVATE* pAppData = ptrAppData;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retVal = TIMM_OSAL_ERR_NONE;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuffer);
  
  /* API entry time calculation macro */
  //OMTB_OMX_API_ENTRY_TIME_CALC;
  
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
  //OMTB_OMX_API_EXIT_TIME_CALC("G711ADEC_FillBufferDone");
  
  return err;
}


/*----------------------------- end of file ----------------------------------*/

