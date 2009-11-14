/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file      omtbH264Vdec.c
 *
 *  @brief     H264 Video Decode IL Client Implementation.
 *
 * <B> History: </B>
 *
 *      # June-1-2009   AVME System Test    Initial Version
 */

/******************************************************************************\
 *      Standard Includes
\******************************************************************************/

#include <stdio.h>
#ifdef WTSD
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#endif
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
#include "timm_osal_trace.h"
#else
#include "platform/osal/timm_osal_interfaces.h"
#endif
#include "omtbCommon.h"
#include "omtbCmd.h"

#ifdef SHARED_MEM
#include <unistd.h>
#include <mmplatform.h>
#include <RcmClient.h>
#include <HeapBuf.h>
#include <SharedRegion.h>
#endif

#ifdef TILER
#include <memmgr/memmgr.h>
#include <mmplatform.h>
#endif
/******************************************************************************\
 *      Local Defines
\******************************************************************************/
#ifdef TILER
#define STRIDE_8BIT (4 * 1024)
#define STRIDE_16BIT (4 * 1024)
#endif

#ifdef WTSD
#define     PADX    32
#define     PADY    24
#endif

/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Callback structure Definition */

static OMX_CALLBACKTYPE H264VDEC_Callback = {
  .EventHandler = H264VDEC_EventHandler,      /* Event handler callback */    
  .EmptyBufferDone = H264VDEC_EmptyBufferDone,/* Empty buffer done callback */
  .FillBufferDone = H264VDEC_FillBufferDone   /* Fill buffer done callback */ 
};


/******************************************************************************\
 *      Extern Object Definitions
\******************************************************************************/

#ifdef SHARED_MEM
extern HeapBuf_Handle heapHandle;
#endif

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

/* Pointer to H264 config params */
OMTB_H264_VDEC_CONFIG_PARAMS *h264VdecConfig = NULL;

#ifdef OMTB_FF_DRIVER_DISPLAY
OmtbDispInfo *dssConfig = NULL;   /* Pointer to DSS configuration parameter */
#endif

/******************************************************************************\
 *      Static Globals
\******************************************************************************/
#ifdef WTSD
static OMX_U32 H264VDEC_funcCreateCount = 0;  /* Func create count */
static OMX_U32 H264VDEC_apiCreateCount = 0;   /* API create count */ 
                                                                 
static OMX_U32 H264VDEC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U32 H264VDEC_apiDeleteCount = 0;   /* API delete count */ 
#else
static OMX_U64 H264VDEC_funcCreateCount = 0;  /* Func create count */
static OMX_U64 H264VDEC_apiCreateCount = 0;   /* API create count */ 
                                                                 
static OMX_U64 H264VDEC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U64 H264VDEC_apiDeleteCount = 0;   /* API delete count */ 
#endif

/* Enum Entry for Supported cfg param indexs */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VdecCfgParamIdx[] =
{
  /* ADD ENTRIES HERE for SetConfig/SetParam/GetConfig/GetParam */
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
 *      H264VDEC_AllocateMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate memory for private data structure for h264 video decode
 *            component
 *
 *  @param in:
 *            h264VdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */

OmtbRet H264VDEC_AllocateMemory(OMTB_H264_VDEC_PRIVATE *h264VdecClient)
{
  OmtbRet err = OMTB_SUCCESS;
  unsigned int retVal = TIMM_OSAL_ERR_NONE;

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  /* Allocating memory for callback structure */
  h264VdecClient->cb=(OMX_CALLBACKTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_CALLBACKTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->cb)
  {
    PRINT(PRINT_ERR_MSG, "Callback structure memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for input port definition structure */
  h264VdecClient->inPortParams.pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->inPortParams.pInPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Input port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for output port definition structure */
  h264VdecClient->outPortParams.pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                            TIMM_OSAL_TRUE,
                                            0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->outPortParams.pOutPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Output port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for component specific structures if any */
  if (OMX_VIDEO_CodingAVC == h264VdecClient->compressionFormat)
  {
    h264VdecClient->pH264  = (OMX_VIDEO_PARAM_AVCTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_VIDEO_PARAM_AVCTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
    
    if (!h264VdecClient->pH264)
    {
      PRINT(PRINT_ERR_MSG, "component specific memory allocation failed\n");
      err = OMTB_INSUFFICIENT_RESOURCES;
    }
  }
  else
  {
    PRINT(PRINT_ERR_MSG, "Invalid compression format value\n");
    err = OMX_ErrorUnsupportedSetting;
  }
  
  /* Create input pipe for input buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(h264VdecClient->inPortParams.ipBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      h264VdecClient->inPortParams.numInBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Create output pipe for output buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(h264VdecClient->outPortParams.opBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      h264VdecClient->outPortParams.numOutBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
#if 0
  /* Allocating memory for component event handling semaphore */
  h264VdecClient->eventSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VdecClient->eventSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for eof handling semaphore */
  h264VdecClient->eofSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VdecClient->eofSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for input data read thread exit semaphore */
  h264VdecClient->inPortParams.inputDataReadThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->inPortParams.inputDataReadThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  
  /* Allocating memory for output data write thread exit semaphore */
  h264VdecClient->outPortParams.outputDataWriteThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->outPortParams.outputDataWriteThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  /* Allocating memory for input data read thread control semaphore */
  h264VdecClient->inPortParams.inputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VdecClient->inPortParams.inputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for output data write thread semaphore */
  h264VdecClient->outPortParams.outputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VdecClient->outPortParams.outputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
#endif
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VDEC_AllocateResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for H264 decoder.
 *
 *  @param in:
 *            h264VdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_AllocateResources(OMTB_H264_VDEC_PRIVATE* h264VdecClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  unsigned char *pBuffer = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

#ifdef TILER
  MemAllocBlock *MemReqDescTiler;
  OMX_PTR TilerAddr=NULL;

 
  MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
  if( MemReqDescTiler == TIMM_OSAL_NULL )
  {
    PRINT(PRINT_ERR_MSG, \
            "Error in MemReqDescTiler\n");

    /* Function to release the resources allocated by component */
    err = H264VDEC_FreeResources(h264VdecClient);

    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
    }
        
    err = OMTB_INSUFFICIENT_RESOURCES;
    return err;
		
  }
#endif

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  /* Input port buffer allocation */
  for(i = 0; i < h264VdecClient->inPortParams.pInPortDef->nBufferCountActual; 
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VdecClient->inPortParams.flagUseInBuffer)
    {

#ifdef WTSD_TEMP
      PRINT(PRINT_DEBUG_MESSAGE, "Before IP OMX_AllocateBuffer count %d\n",i);
#endif

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
   
      err = OMX_AllocateBuffer( 
                        h264VdecClient->handle,
                        &h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i],
                        h264VdecClient->inPortParams.pInPortDef->nPortIndex, 
                        h264VdecClient,
                        h264VdecClient->inPortParams.pInPortDef->nBufferSize);
   
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
#ifdef SHARED_MEM
      pBuffer = HeapBuf_alloc(heapHandle,
                                        h264VdecClient->inPortParams.pInPortDef->nBufferSize, 0);

	    PRINT(PRINT_DEBUG_MESSAGE,"\npBuffer before UB = %x\n",pBuffer);
      pBuffer = (char *)SharedRegion_getSRPtr(pBuffer, 2);
	    PRINT(PRINT_DEBUG_MESSAGE,"\npBuffer SR before UB = %x\n",pBuffer);

#elif defined (TILER)
	  	MemReqDescTiler[0].pixelFormat = PIXEL_FMT_PAGE;
    	MemReqDescTiler[0].dim.len = 
      h264VdecClient->inPortParams.pInPortDef->nBufferSize;
		  MemReqDescTiler[0].stride = 0;

		  /*call to tiler Alloc*/
		  PRINT(PRINT_DEBUG_MESSAGE, "Before 1D Tiler buffer allocation for BUF %d \n",i);
		  TilerAddr=MemMgr_Alloc(MemReqDescTiler,1);		
		  PRINT(PRINT_DEBUG_MESSAGE, "1D Tiler buffer allocated :TilerAddr\n");

      pBuffer=(OMX_U8*)TilerAddr;
#else        
      pBuffer= TIMM_OSAL_Malloc((h264VdecClient->inPortParams.pInPortDef->nBufferSize),
		                    OMTB_CMEM_ALLOCATION, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif        
      
      if (!pBuffer)
      {
         PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for i/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VDEC_FreeResources(h264VdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
#ifdef TILER
        if(MemReqDescTiler)
          TIMM_OSAL_Free(MemReqDescTiler);
#endif
        return err;
      }

#ifdef WTSD_TEMP
      PRINT(PRINT_DEBUG_MESSAGE, "Before IP OMX_UseBuffer count %d\n",i);
#endif

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;
    
      err = OMX_UseBuffer(h264VdecClient->handle,
                      &h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i],
                      h264VdecClient->inPortParams.pInPortDef->nPortIndex, 
                      h264VdecClient,
                      h264VdecClient->inPortParams.pInPortDef->nBufferSize,
                      pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");

#ifdef SHARED_MEM
	    TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",
	        h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);
      h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer = 
                SharedRegion_getPtr(
                h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);
	    TIMM_OSAL_Trace("\npBuffer after UB = %x\n",h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer); 
#endif
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = H264VDEC_FreeResources(h264VdecClient);

      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }
      
#ifdef TILER
      if(MemReqDescTiler)
          TIMM_OSAL_Free(MemReqDescTiler);
#endif

      return tempErr;
    }
    else
    {
      PRINT(PRINT_DEBUG_MESSAGE, \
            "Buffers allocated successfully for i/p port\n");
    }
  }


  /* Output port buffer allocation */
  for(i = 0; i < h264VdecClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VdecClient->outPortParams.flagUseOutBuffer)
    {
  #ifdef WTSD_TEMP
      PRINT(PRINT_DEBUG_MESSAGE, "Before OP OMX_AllocateBuffer count %d\n",i);
  #endif
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                    h264VdecClient->handle,
                    &h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i],
                    h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
                    h264VdecClient,
                    h264VdecClient->outPortParams.pOutPortDef->nBufferSize);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
      /* Allocating memory for output port buffer */
#ifdef SHARED_MEM
      pBuffer = HeapBuf_alloc(heapHandle,
                                        h264VdecClient->outPortParams.pOutPortDef->nBufferSize, 0);

	    TIMM_OSAL_Trace("\npBuffer before UB = %x\n",pBuffer);
            pBuffer = (char *)SharedRegion_getSRPtr(pBuffer, 2);
	    TIMM_OSAL_Trace("\npBuffer SR before UB = %x\n",pBuffer);

#elif defined (TILER)
      MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
		  MemReqDescTiler[0].dim.area.width= h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth;//256
		  MemReqDescTiler[0].dim.area.height= h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight;//240;
		  MemReqDescTiler[0].stride=STRIDE_8BIT;
					  
		  MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
		  MemReqDescTiler[1].dim.area.width=h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth/2;//128;
		  MemReqDescTiler[1].dim.area.height=h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight/2;//120;
		  MemReqDescTiler[1].stride=STRIDE_16BIT;
					  
		  //MemReqDescTiler.reserved
		  /*call to tiler Alloc*/
		  PRINT(PRINT_MUST, "Before 2D Tiler buffer allocation for BUF %d \n",i);
		  TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
		  PRINT(PRINT_MUST, "2D Tiler buffer allocated :TilerAddr\n");

      pBuffer=(OMX_U8*)TilerAddr;
#else        
	    pBuffer= TIMM_OSAL_Malloc((h264VdecClient->outPortParams.pOutPortDef->nBufferSize),
		                    OMTB_CMEM_ALLOCATION, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif        

      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for o/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VDEC_FreeResources(h264VdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
#ifdef TILER
        if(MemReqDescTiler)
          TIMM_OSAL_Free(MemReqDescTiler);
#endif

        return err;
      }
  #ifdef WTSD_TEMP
      PRINT(PRINT_DEBUG_MESSAGE, "Before OP OMX_UseBuffer count %d\n",i);
  #endif
  
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer( h264VdecClient->handle,
                     &h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i],
                     h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
                     h264VdecClient,
                     h264VdecClient->outPortParams.pOutPortDef->nBufferSize,
                     pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");

#ifdef SHARED_MEM
	TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",
	        h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);
       h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer = 
                SharedRegion_getPtr(
                h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);
	TIMM_OSAL_Trace("\npBuffer after UB = %x\n",
                h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer); 
#endif
    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = H264VDEC_FreeResources(h264VdecClient);

      if(OMX_ErrorNone != err)
      {
        OMTB_OmxReturnStatus(err);
      }
#ifdef TILER
      if(MemReqDescTiler)
          TIMM_OSAL_Free(MemReqDescTiler);
#endif
      
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
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

#ifdef TILER
  if(MemReqDescTiler)
          TIMM_OSAL_Free(MemReqDescTiler);
#endif

  return err;
}

#if 0
/******************************************************************************\
 *      H264VDEC_AllocateResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for H264 decoder.
 *
 *  @param in:
 *            h264VdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */
OMX_ERRORTYPE H264VDEC_AllocateResources(OMTB_H264_VDEC_PRIVATE* h264VdecClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  unsigned char *pBuffer = NULL;
  OMX_U8 *pTmpBuffer; /*Used with Use Buffer calls*/  //Brijesh
  OMTB_OMX_API_TIMING_VAR_DECL;

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
	  MemAllocBlock *MemReqDescTiler;
	  OMX_PTR TilerAddr=NULL;
#endif

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
	  MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	  if(MemReqDescTiler == NULL)//???
	  {
		  PRINT(PRINT_ERR_MSG, \
		      "No Memory for MemReqDescTiler\n");

		  /* Function to release the resources allocated by component */
		  err = H264VDEC_FreeResources(h264VdecClient);

		  if(OMX_ErrorNone != err)
		  {
		    OMTB_OmxReturnStatus(err);
		  }
		    
		  err = OMTB_INSUFFICIENT_RESOURCES;
		  return err;
		}	  
#endif
  
  /* Input port buffer allocation */
  for(i = 0; i < h264VdecClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VdecClient->inPortParams.flagUseInBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                        h264VdecClient->handle,
                        &h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i], //Brijesh where do we allocate for buffer header type????
                        h264VdecClient->inPortParams.pInPortDef->nPortIndex, 
                        h264VdecClient,
                        h264VdecClient->inPortParams.pInPortDef->nBufferSize);  //Brijesh is buffer size OK???

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */ 
    else
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh

		MemReqDescTiler[0].pixelFormat = PIXEL_FMT_PAGE;
		MemReqDescTiler[0].dim.len = 
      h264VdecClient->inPortParams.pInPortDef->nBufferSize;
		MemReqDescTiler[0].stride = 0;

		/*call to tiler Alloc*/
		PRINT(PRINT_MUST, "Before 1D Tiler buffer allocation for BUF %d \n",i);
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,1);		
		PRINT(PRINT_MUST, "1D Tiler buffer allocated :TilerAddr\n");

    pTmpBuffer = (OMX_U8 *)TilerAddr;
		if(pTmpBuffer == NULL)//???
		{
		   PRINT(PRINT_ERR_MSG, \
		       "Error in TILER Buffer alloc for 2D pTmpBuffer\n");

		   /* Function to release the resources allocated by component */
		   err = H264VDEC_FreeResources(h264VdecClient);

		   if(OMX_ErrorNone != err)
		   {
		     OMTB_OmxReturnStatus(err);
		   }
		    
		   err = OMTB_INSUFFICIENT_RESOURCES;
		   return err;
		}

		/* API entry time calculation macro */
		OMTB_OMX_API_ENTRY_TIME_CALC;
		
		err = OMX_UseBuffer(h264VdecClient->handle,
					  &h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i],
					  h264VdecClient->inPortParams.pInPortDef->nPortIndex, 
					  h264VdecClient,
					  h264VdecClient->inPortParams.pInPortDef->nBufferSize,
					  pTmpBuffer);
		
		/* API exit time calculation macro */
		OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
		
		
#else  //Brijesh  

      /* Allocating memory for input port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                        h264VdecClient->inPortParams.pInPortDef->nBufferSize,
                        OMTB_CMEM_ALLOCATION,
                        0,
                        TIMMOSAL_MEM_SEGMENT_EXT);
      
      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for i/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VDEC_FreeResources(h264VdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer(h264VdecClient->handle,
                    &h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i],//Brijesh is it ok???
                    h264VdecClient->inPortParams.pInPortDef->nPortIndex, 
                    h264VdecClient,
                    h264VdecClient->inPortParams.pInPortDef->nBufferSize, //Brijesh is buffer size ok???
                    pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");

#endif //Brijesh	  

    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = H264VDEC_FreeResources(h264VdecClient);

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
  for(i = 0; i < h264VdecClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VdecClient->outPortParams.flagUseOutBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                    h264VdecClient->handle,
                    &h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i], 
                    h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
                    h264VdecClient,
                    h264VdecClient->outPortParams.pOutPortDef->nBufferSize); 

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh


    MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
		MemReqDescTiler[0].dim.area.width= h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth;//256
		MemReqDescTiler[0].dim.area.height= h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight;//240;
		MemReqDescTiler[0].stride=STRIDE_8BIT;
					
		MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
		MemReqDescTiler[1].dim.area.width=h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth/2;//128;
		MemReqDescTiler[1].dim.area.height=h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight/2;//120;
		MemReqDescTiler[1].stride=STRIDE_16BIT;
					
		//MemReqDescTiler.reserved
		/*call to tiler Alloc*/
		PRINT(PRINT_MUST, "Before 2D Tiler buffer allocation for BUF %d \n",i);
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
		PRINT(PRINT_MUST, "2D Tiler buffer allocated :TilerAddr\n");

    pTmpBuffer = (OMX_U8 *)TilerAddr;
		if(pTmpBuffer == NULL)//???
		{
		   PRINT(PRINT_ERR_MSG, \
			   "Error in TILER Buffer alloc for 2D pTmpBuffer\n");

		   /* Function to release the resources allocated by component */
		   err = H264VDEC_FreeResources(h264VdecClient);

		   if(OMX_ErrorNone != err)
		   {
			 OMTB_OmxReturnStatus(err);
		   }
			
		   err = OMTB_INSUFFICIENT_RESOURCES;
		   return err;
		}

		/* API entry time calculation macro */
		OMTB_OMX_API_ENTRY_TIME_CALC;

		err = OMX_UseBuffer( h264VdecClient->handle,
					   &h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i],
					   h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
					   h264VdecClient,
					   h264VdecClient->outPortParams.pOutPortDef->nBufferSize,
					   pTmpBuffer);
		
		/* API exit time calculation macro */
		OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
				
				
#else  //Brijesh  
    
      /* Allocating memory for output port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                      h264VdecClient->outPortParams.pOutPortDef->nBufferSize,
                      OMTB_CMEM_ALLOCATION,
                      0,
                      TIMMOSAL_MEM_SEGMENT_EXT);

      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for o/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VDEC_FreeResources(h264VdecClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer( h264VdecClient->handle,
                     &h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i],
                     h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
                     h264VdecClient,
                     h264VdecClient->outPortParams.pOutPortDef->nBufferSize,
                     pBuffer);

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
#endif //Brijesh

    }
    
    if(OMX_ErrorNone != err)
    {
      OMX_ERRORTYPE tempErr = err;
      OMTB_OmxReturnStatus(err);

      /* Function to release the resources allocated by component */
      err = H264VDEC_FreeResources(h264VdecClient);

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
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
  if(MemReqDescTiler)
	TIMM_OSAL_Free(MemReqDescTiler); //Brijesh last update from office
	  
#endif //Brijesh

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

  return err;
}

#endif
/******************************************************************************\
 *      H264VDEC_UseInitialResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Tells H264 decode component to empty/fill the initial buffers.
 *
 *  @param in:
 *            h264VdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_UseInitialResources(
  OMTB_H264_VDEC_PRIVATE* h264VdecClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int dataRead = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  /* Give input buffers to component which is limited by no of input buffers 
     available. Rest of the data will be read on the callback from input data 
     read thread */
  for(i = 0; i < h264VdecClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
  
    dataRead = fread(
                h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer,
                sizeof(char),
                h264VdecClient->inPortParams.comp.h264Vdec.frameSize[\
                h264VdecClient->inPortParams.comp.h264Vdec.frameCount++],
                h264VdecClient->inPortParams.fin);
                      
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->nFilledLen =dataRead;
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->nOffset = 0;
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->nAllocLen = dataRead;
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->nInputPortIndex = 0;
    
    PRINT(PRINT_DEBUG_MESSAGE, "--> Read %d bytes from file\n",dataRead);
    PRINT(PRINT_DEBUG_MESSAGE, "Empty first  buffer %x\n",
          (int)h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);

#ifdef SHARED_MEM
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer = 
            (char *)SharedRegion_getSRPtr(
            h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer, 2);
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer SR before ETB = %x\n",
            h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);        
#endif
            
  
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer(
                        h264VdecClient->handle,
                        h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]);
    
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_EmptyThisBuffer");

#ifdef SHARED_MEM
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer SR after ETB = %x\n",
            h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);
    h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer = 
            SharedRegion_getPtr(h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer after ETB = %x\n",
            h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);

#endif        
    PRINT(PRINT_DEBUG_MESSAGE,"ERR STATE after initial ETB = %d\n",err);

    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }

  }
  
  /* Give output buffers to component which is limited by no of output buffers 
     available. Rest of the data will be written on the callback from output  
     data write thread */
  for(i = 0; i < h264VdecClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {

#ifdef SHARED_MEM
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer before FTB = %x\n",
            h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);        
    h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer = 
            (char *)SharedRegion_getSRPtr(
            h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer, 2);
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer SR before FTB = %x\n",
            h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);        
#endif

    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer( 
                      h264VdecClient->handle,
                      h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]);
    
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FillThisBuffer");
    
#ifdef SHARED_MEM
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer SR after FTB = %x\n",
            h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);
    h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer = 
            SharedRegion_getPtr(h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);
    PRINT(PRINT_DEBUG_MESSAGE,"pBuffer after FTB = %x\n",
            h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);

#endif        

    PRINT(PRINT_DEBUG_MESSAGE,"ERR STATE after initial FTB = %d\n",err);

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
                              __func__, (int)h264VdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VDEC_SetPortDefinition Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for H264 decoder.
 *
 *  @param in:
 *            h264VdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_SetPortDefinition(OMTB_H264_VDEC_PRIVATE* h264VdecClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_PORT_PARAM_TYPE portInit;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader(&portInit,sizeof(OMX_PORT_PARAM_TYPE));
  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;

#ifndef WTSD
  
/* For WTSD, this is set in Get Handle */

/* output buffer size calculation based on input dimension speculation */
  h264VdecClient->inPortParams.bufferInSize   = OMTB_H264_VDEC_MAX_INBUFF_SIZE;

  h264VdecClient->outPortParams.bufferOutSize = 
                      (h264VdecClient->outPortParams.comp.h264Vdec.nWidth *\
                      h264VdecClient->outPortParams.comp.h264Vdec.nHeight * 2);
  
#endif
  
#ifdef WTSD_TEMP
  PRINT(PRINT_DEBUG_MESSAGE, "Before OSP, OMX_IndexParamVideoInit \n");
#endif

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  err = OMX_SetParameter( h264VdecClient->handle,
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
    PRINT(DEFAULT_MESSAGES, "Found The The component for decoding\n");
  }
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader( h264VdecClient->inPortParams.pInPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  h264VdecClient->inPortParams.pInPortDef->nPortIndex          = 0x0;
  h264VdecClient->inPortParams.pInPortDef->eDir                = OMX_DirInput;
  h264VdecClient->inPortParams.pInPortDef->nBufferCountActual  = 
                                  h264VdecClient->inPortParams.numInBuf;
  h264VdecClient->inPortParams.pInPortDef->nBufferCountMin     = 1;
  h264VdecClient->inPortParams.pInPortDef->nBufferSize         = 
                                  h264VdecClient->inPortParams.bufferInSize;
  h264VdecClient->inPortParams.pInPortDef->bEnabled            = OMX_TRUE;
  h264VdecClient->inPortParams.pInPortDef->bPopulated          = OMX_FALSE;
  h264VdecClient->inPortParams.pInPortDef->eDomain             = 
                                  OMX_PortDomainVideo;
  h264VdecClient->inPortParams.pInPortDef->bBuffersContiguous  = OMX_FALSE;
  h264VdecClient->inPortParams.pInPortDef->nBufferAlignment    = 0x0;
  
  /* OMX_VIDEO_PORTDEFINITION values for input port */
  h264VdecClient->inPortParams.pInPortDef->format.video.pNativeRender  = NULL;
  h264VdecClient->inPortParams.pInPortDef->format.video.cMIMEType      = "H264";
  h264VdecClient->inPortParams.pInPortDef->format.video.nStride        = -1;
  h264VdecClient->inPortParams.pInPortDef->format.video.nSliceHeight   = 0;
#ifdef WTSD
  h264VdecClient->inPortParams.pInPortDef->format.video.nBitrate       = 10485760;
  h264VdecClient->inPortParams.pInPortDef->format.video.xFramerate     = 30;
#else
  h264VdecClient->inPortParams.pInPortDef->format.video.nBitrate       = 0;
  h264VdecClient->inPortParams.pInPortDef->format.video.xFramerate     = 0;
#endif
  h264VdecClient->inPortParams.pInPortDef->format.video.eColorFormat   =
                      h264VdecClient->outPortParams.comp.h264Vdec.colorFormat;
  h264VdecClient->inPortParams.pInPortDef->format.video.nFrameWidth    = 
                      h264VdecClient->outPortParams.comp.h264Vdec.nWidth;
  h264VdecClient->inPortParams.pInPortDef->format.video.nFrameHeight   = 
                      h264VdecClient->outPortParams.comp.h264Vdec.nHeight;
  h264VdecClient->inPortParams.pInPortDef->format.video.eCompressionFormat = 
                      OMX_VIDEO_CodingAVC;
  h264VdecClient->inPortParams.pInPortDef->format.video.bFlagErrorConcealment = 
                      OMX_FALSE;

#ifdef WTSD_TEMP
  PRINT(PRINT_DEBUG_MESSAGE, "Before OSP, IP OMX_IndexParamPortDefinition, %d =  \n",
      h264VdecClient->inPortParams.pInPortDef->nBufferSize);
#endif

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the inPort Definition*/
  err = OMX_SetParameter( h264VdecClient->handle,
                          OMX_IndexParamPortDefinition,
                          h264VdecClient->inPortParams.pInPortDef);

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  
  OMTB_SetHeader( h264VdecClient->outPortParams.pOutPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  
  h264VdecClient->outPortParams.pOutPortDef->nPortIndex         = 0x1;
  h264VdecClient->outPortParams.pOutPortDef->eDir               = OMX_DirOutput;
  h264VdecClient->outPortParams.pOutPortDef->nBufferCountActual = 
                                  h264VdecClient->outPortParams.numOutBuf;
  h264VdecClient->outPortParams.pOutPortDef->nBufferCountMin    = 1;
#ifdef WTSD
  h264VdecClient->outPortParams.pOutPortDef->nBufferSize        = 
                                  65280 + 48000; // (h * w * 5/3/2);  //sujatha. Is this used any where / use buf? How to make it generic? Atleast used by HEAPBUF                             
#else
    h264VdecClient->outPortParams.pOutPortDef->nBufferSize        = 
                                  h264VdecClient->outPortParams.bufferOutSize;
#endif
  h264VdecClient->outPortParams.pOutPortDef->bEnabled           = OMX_TRUE;
  h264VdecClient->outPortParams.pOutPortDef->bPopulated         = OMX_FALSE;
  h264VdecClient->outPortParams.pOutPortDef->eDomain            = 
                                  OMX_PortDomainVideo;
  h264VdecClient->outPortParams.pOutPortDef->bBuffersContiguous = OMX_FALSE;
  h264VdecClient->outPortParams.pOutPortDef->nBufferAlignment   = 0x0;
  
  /* OMX_VIDEO_PORTDEFINITION values for output port */
  h264VdecClient->outPortParams.pOutPortDef->format.video.cMIMEType  = "H264";
  h264VdecClient->outPortParams.pOutPortDef->format.video.pNativeRender = NULL;
#ifdef WTSD
  h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth   = 
      ((h264VdecClient->outPortParams.comp.h264Vdec.nWidth + (2*PADX) + 127) & 0xFFFFFF80);//pAppData->nWidth; //256
  h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight  = 
      ((h264VdecClient->outPortParams.comp.h264Vdec.nHeight + (4*PADY))); //pAppData->nHeight; //240
#else
  h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameWidth   = 
                       h264VdecClient->outPortParams.comp.h264Vdec.nWidth;
  h264VdecClient->outPortParams.pOutPortDef->format.video.nFrameHeight  = 
                       h264VdecClient->outPortParams.comp.h264Vdec.nHeight;
#endif
  h264VdecClient->outPortParams.pOutPortDef->format.video.eColorFormat  = 
                       h264VdecClient->outPortParams.comp.h264Vdec.colorFormat;
  h264VdecClient->outPortParams.pOutPortDef->format.video.nStride       = 0;
  h264VdecClient->outPortParams.pOutPortDef->format.video.nSliceHeight  = 0;
#ifdef WTSD
  h264VdecClient->outPortParams.pOutPortDef->format.video.nBitrate      = 10485760;
  h264VdecClient->outPortParams.pOutPortDef->format.video.xFramerate    = 30;
#else 
  h264VdecClient->outPortParams.pOutPortDef->format.video.nBitrate      = 0;
  h264VdecClient->outPortParams.pOutPortDef->format.video.xFramerate    = 0;
#endif  
  h264VdecClient->outPortParams.pOutPortDef->format.video.bFlagErrorConcealment=
                       OMX_FALSE;
  h264VdecClient->outPortParams.pOutPortDef->format.video.eCompressionFormat   =
                       OMX_VIDEO_CodingAVC;
  
#ifdef WTSD_TEMP
  PRINT(PRINT_DEBUG_MESSAGE, "Before OSP, OP OMX_IndexParamPortDefinition \n");
#endif

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /*Set the OutPort Definition*/
  err = OMX_SetParameter( h264VdecClient->handle,
                          OMX_IndexParamPortDefinition,
                          h264VdecClient->outPortParams.pOutPortDef);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VDEC_ApiTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    H264 Vdec API mode Execute Thread
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

void H264VDEC_ApiTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->h264VdecClient);
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != funcDataThreadArg->dssClient);
  dssClient  = funcDataThreadArg->dssClient;
#endif
  h264VdecClient = funcDataThreadArg->h264VdecClient;
 

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);

  /* Update the api mode create count */
  H264VDEC_apiCreateCount++;
  PRINT(PRINT_MUST, "%s: H264 API Mode Create Count: %u\n", __func__,
                     (unsigned int) H264VDEC_apiCreateCount);
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);
  
  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = H264VDEC_UseInitialResources(h264VdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* Data streaming thread arguments */
  strmThreadArg[h264VdecClient->instanceNo].h264VdecClient = h264VdecClient;
#ifdef OMTB_FF_DRIVER_DISPLAY
  strmThreadArg[h264VdecClient->instanceNo].dssClient  = dssClient;
#endif
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VdecClient->inPortParams.inDataStrmThrdId,
                    (void *)H264VDEC_InputDataRead,
                    0,
                    &strmThreadArg[h264VdecClient->instanceNo],
                    OMTB_H264_VDEC_TSK_STACK_SIZE,
                    OMTB_H264_VDEC_GROUP_PRIORITY,
                    (signed char*)"H264_VDEC_DATA_IN_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video decode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VdecClient->outPortParams.outDataStrmThrdId,
                    (void *)H264VDEC_OutputDataWrite,
                    0,
                    &strmThreadArg[h264VdecClient->instanceNo],
                    OMTB_H264_VDEC_TSK_STACK_SIZE,
                    OMTB_H264_VDEC_GROUP_PRIORITY,
                    (signed char*)"H264_VDEC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video decode output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eofSem, TIMM_OSAL_SUSPEND);

  /* Update the api mode delete count */
  H264VDEC_apiDeleteCount++;
  PRINT(PRINT_MUST, "%s: H264 API Mode Delete Count: %u\n", __func__,
                    (unsigned int) H264VDEC_apiDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  h264VdecClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[h264VdecClient->instanceNo]);
#else
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[h264VdecClient->instanceNo],
      NULL);
#endif
}


/******************************************************************************\
 *      H264VDEC_GetHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get the handle for H264 Video Decode component
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

OMX_ERRORTYPE H264VDEC_GetHandle(
  OMTB_COMP_HANDLE * classPtr,
  unsigned int instance)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  char inputFile[OMTB_MAX_LEN]  = {"\0"};
  char outputFile[OMTB_MAX_LEN] = {"\0"};
  char frameData[OMTB_MAX_LEN]  = {"\0"};
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
  unsigned char input[OMTB_MAX_FRM_SZ_BYTES] = {"\0"};
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  unsigned int frameCount = 0; /* keeps count of total i/p frames */
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != classPtr);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instance);

  /* Allocate memory for application private data */
  h264VdecClient = (OMTB_H264_VDEC_PRIVATE *)
                    TIMM_OSAL_Malloc( sizeof(OMTB_H264_VDEC_PRIVATE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);

  if (!h264VdecClient)
  {
    PRINT(PRINT_ERR_MSG, "Error allocating h264VdecClient structure \n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    return err;
  }
  else
  {
    /* Update the application instance management structure with app pvt data
       structure pointer */
    classPtr->h264VdecClient[instance] = 
                          (OMTB_H264_VDEC_PRIVATE *)h264VdecClient;
    memset(h264VdecClient, 0, sizeof(OMTB_H264_VDEC_PRIVATE));
    h264VdecClient->instanceNo = instance;
  }

  /* Make local copy of input, output and framesize file names */
  strcpy(outputFile, h264VdecConfig->outputFile);
  strcpy(inputFile,  h264VdecConfig->inputFile);
  strcpy(frameData,  h264VdecConfig->frameSizeFile);
  strcpy(h264VdecClient->compName,  h264VdecConfig->compName);

  PRINT(PRINT_DEBUG_MESSAGE, "Component Name is:%s\n", 
                              h264VdecClient->compName);
  PRINT(PRINT_DEBUG_MESSAGE, "Input File is:%s\n", inputFile);
  PRINT(PRINT_DEBUG_MESSAGE, "Output File is:%s\n", outputFile);
  PRINT(PRINT_DEBUG_MESSAGE, "Frame Size File is:%s\n", frameData);
  
  /* Data output mode selection - File/Driver */
  h264VdecClient->outPortParams.dataOutPutMode = 
                                            h264VdecConfig->dataOutPutMode;
  
  /* Buffer allocation mode - OMX_UseBuffer or OMX_AllocateBuffer */
  h264VdecClient->inPortParams.flagUseInBuffer   = 
                                            h264VdecConfig->flagUseInBuffer;
  h264VdecClient->outPortParams.flagUseOutBuffer = 
                                            h264VdecConfig->flagUseOutBuffer;

  /* no of i/p and o/p buffers */
  h264VdecClient->inPortParams.numInBuf   = h264VdecConfig->numInBuf;
  h264VdecClient->outPortParams.numOutBuf = h264VdecConfig->numOutBuf;
  
  /* Open the input file */
  h264VdecClient->inPortParams.fin = fopen(inputFile, "rb");
  if(NULL == h264VdecClient->inPortParams.fin)
  {
    PRINT(PRINT_ERR_MSG, "Error in opening input file %s\n",inputFile);
    err = OMTB_INSUFFICIENT_RESOURCES;
    H264VDEC_FreeMemory(h264VdecClient);
    return err;
  }

  /* Initialize the counter to get frame size from input data read thread */
  h264VdecClient->inPortParams.comp.h264Vdec.frameCount = 0;
  
  if(OMTB_FILE == h264VdecClient->outPortParams.dataOutPutMode )
  {
    /* Open the output file */
    h264VdecClient->outPortParams.fout = fopen(outputFile, "wb");
    if(NULL == h264VdecClient->outPortParams.fout)
    {
      PRINT(PRINT_ERR_MSG, "Error opening output file %s\n",outputFile);
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VDEC_FreeMemory(h264VdecClient);
      return err;
    }
  }
#ifdef OMTB_FF_DRIVER_DISPLAY
  else if(OMTB_DRIVER == h264VdecClient->outPortParams.dataOutPutMode)
  {
    /* Allocate memory for diplay driver cfg params */
    dssClient = (OmtbDispInfo *)
                    TIMM_OSAL_Malloc( sizeof(OmtbDispInfo),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);

    if(!dssClient)
    {
      PRINT(PRINT_ERR_MSG, "Error allocating dssClient structure \n");
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VDEC_FreeMemory(h264VdecClient);
      return err;
    }
    else
    {
      classPtr->dssClient[instance] = dssClient;
    }
  
    /* Copy the driver configuration from the template */
    memset(dssClient, 0, sizeof(OmtbDispInfo));
    memcpy(dssClient, dssConfig, sizeof(OmtbDispInfo));
    
    /* Initialize the V4L2 display driver */
    if(OMTB_FAIL == OMTB_DispInit(dssClient))
    {
      PRINT(PRINT_ERR_MSG, "Unable to setup Video device\n");
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VDEC_FreeMemory(h264VdecClient);
      return err;
    }
    else
    {
      PRINT(PRINT_SIMPLE_MESSAGE,"Video device initialized\n");
    }
  }
#endif
  /* Open the Frame Size File */
  h264VdecClient->inPortParams.comp.h264Vdec.frameFile = fopen(frameData, "rb");
  if(NULL == h264VdecClient->inPortParams.comp.h264Vdec.frameFile)
  {
    PRINT(PRINT_ERR_MSG, "Error in opening frame size data file %s\n", 
                          frameData);
    err = OMTB_INSUFFICIENT_RESOURCES;
    H264VDEC_FreeMemory(h264VdecClient);
    return err;
  }

#ifdef WTSD
  /* output buffer size calculation based on input dimension speculation. If Frame Size txt 
        is used, it is later set to max frame Size */
  h264VdecClient->inPortParams.bufferInSize   = 0;

#endif
  
  /* Read the frame Size from the frame size file */
  while(NULL != 
        fgets((char *)input,
              OMTB_MAX_FRM_SZ_BYTES,
              h264VdecClient->inPortParams.comp.h264Vdec.frameFile))
  {
    h264VdecClient->inPortParams.comp.h264Vdec.frameSize[frameCount] = \
                                                        atoi((char *)input);
#ifdef WTSD
    /* Initialising bufferSize to Max */
    if (h264VdecClient->inPortParams.comp.h264Vdec.frameSize[frameCount] >  h264VdecClient->inPortParams.bufferInSize)
        h264VdecClient->inPortParams.bufferInSize = 
            h264VdecClient->inPortParams.comp.h264Vdec.frameSize[frameCount] ;
    
    if(frameCount > OMTB_MAX_FRAME)
    {
          
      PRINT(PRINT_ERR_MSG, "Num Frames %d excceded MAX limit %d \n",frameCount, OMTB_MAX_FRAME);
                          
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VDEC_FreeMemory(h264VdecClient);
      return err;
    }
#endif
    frameCount++;
    
  }

  /* Initialize app pvt structure params from the template */
  h264VdecClient->outPortParams.comp.h264Vdec.nWidth = 
                                              h264VdecConfig->frameWidth;
  h264VdecClient->outPortParams.comp.h264Vdec.nHeight = 
                                              h264VdecConfig->frameHeight;
  h264VdecClient->compressionFormat = OMX_VIDEO_CodingAVC;
  h264VdecClient->outPortParams.comp.h264Vdec.colorFormat = \
                                                OMX_COLOR_FormatYUV420Planar;
  
#ifdef WTSD
  PRINT(PRINT_DEBUG_MESSAGE, "Size of H264 DECODER Input Buffer = %d",  
  h264VdecClient->inPortParams.bufferInSize )
    
  h264VdecClient->outPortParams.bufferOutSize = 
                      (h264VdecClient->outPortParams.comp.h264Vdec.nWidth *\
                      h264VdecClient->outPortParams.comp.h264Vdec.nHeight * 1.5);
#else
  /* output buffer size calculation based on input dimension speculation */
  h264VdecClient->inPortParams.bufferInSize   = OMTB_H264_VDEC_MAX_INBUFF_SIZE;

  h264VdecClient->outPortParams.bufferOutSize = 
                      (h264VdecClient->outPortParams.comp.h264Vdec.nWidth *\
                      h264VdecClient->outPortParams.comp.h264Vdec.nHeight * 2);
#endif

/*Function to allocate memory for app pvt struct members*/
  err = H264VDEC_AllocateMemory(h264VdecClient);

  if(OMTB_SUCCESS != err)
  {
    PRINT(PRINT_ERR_MSG, "Unable to allocate memory for app pvt structure\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    H264VDEC_FreeMemory(h264VdecClient);
    return err;
  }
  
  /* Initialize the semaphores */
  TIMM_OSAL_SemaphoreCreate(&h264VdecClient->eventSem, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VdecClient->eofSem, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VdecClient->inPortParams.inputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VdecClient->outPortParams.outputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VdecClient->inPortParams.inputDataReadThrExit, 
                            0);
  TIMM_OSAL_SemaphoreCreate(
                &h264VdecClient->outPortParams.outputDataWriteThrExit,
                0);
  
  /*Assign the callbacks*/
  *h264VdecClient->cb = H264VDEC_Callback;

  /* Initialize the EOS flag */
  h264VdecClient->outPortParams.flagEos = OMX_FALSE;
  h264VdecClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Initialize the flag used to determine whether Func/API thread is created */
  h264VdecClient->flagThrCreated = OMX_FALSE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* get component handle */
  err = OMX_GetHandle(&h264VdecClient->handle,
                      (OMX_STRING)h264VdecClient->compName,
                      h264VdecClient,
                      h264VdecClient->cb);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetHandle");
  
  if(OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, "No component named \"%s\" found\n",
                          h264VdecClient->compName);
    OMTB_OmxReturnStatus(err);
    H264VDEC_FreeMemory(h264VdecClient);
    return err;
  }
  else
  {
    /* Update the application instance management strucutre with handle */
    classPtr->handle[instance] = h264VdecClient->handle;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-H264vdec Instance #: %d\n\n", instance);
    PRINT(PRINT_SIMPLE_MESSAGE, "Component Handle Available %x \n",h264VdecClient->handle);
  }
  
  PRINT(PRINT_DEBUG_MESSAGE, "\nbufferOutSize: %d\n",\
        (int)h264VdecClient->outPortParams.bufferOutSize);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VDEC_GetStructIdx Function Definition
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

OmtbRet H264VDEC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct)
{
  unsigned int enumCount = 0;
  
  /* Use the specified template structure */
  h264VdecConfig = (OMTB_H264_VDEC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->h264VdecTemplate);

  /* Get the corresponding string for enum values from enum table 
     Set structure index
     Set structure pointer
     Set port index for which parameter to be updated 
     Set structure header info */
  while(strcmp(enumTblH264VdecCfgParamIdx[enumCount].name , ""))
  {
    if(0 == strcmp(enumTblH264VdecCfgParamIdx[enumCount].name,
                   indexStr))
    {
      switch(enumTblH264VdecCfgParamIdx[enumCount].value)
      {
        /* ADD Similar implementation below for supported indexes */
/*        case OMX_IndexConfigVideoFramerate:
          {
            *nConfigIndex   = OMX_IndexConfigVideoFramerate;
            *pCompCfgStruct = &h264VdecConfig->frameRate;
            h264VdecConfig->frameRate.nPortIndex = portIdx;
            OMTB_SetHeader( &h264VdecConfig->frameRate,
                            sizeof(OMX_CONFIG_FRAMERATETYPE));
          }
          return OMTB_SUCCESS;
*/           
        
          
        default:
          {
            return OMTB_FAIL;
          }

      } // switch(enumTblH264VdecCfgParamIdx[enumCount].value)
     
    } // if(0 == strcmp(enumTblH264VdecCfgParamIdx[enumCount].name, ...
      
    enumCount++;
  }

  return OMTB_SUCCESS;  
}


/******************************************************************************\
 *      H264VDEC_SetParameter Function Definition
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

OMX_ERRORTYPE H264VDEC_SetParameter(
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
  err = H264VDEC_GetStructIdx(indexStr, 
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
 *      H264VDEC_SetConfig Function Definition
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

OMX_ERRORTYPE H264VDEC_SetConfig(
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
  err = H264VDEC_GetStructIdx(indexStr, 
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
 *      H264VDEC_GetParameter Function Definition
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

OMX_ERRORTYPE H264VDEC_GetParameter(
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
  err = H264VDEC_GetStructIdx(indexStr, 
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
 *      H264VDEC_GetConfig Function Definition
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

OMX_ERRORTYPE H264VDEC_GetConfig(
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
  err = H264VDEC_GetStructIdx(indexStr, 
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
 *      H264VDEC_FillData Function Definition
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

unsigned int H264VDEC_FillData(
  OMTB_H264_VDEC_PRIVATE* pAppData,
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
                      pAppData->inPortParams.comp.h264Vdec.frameSize[count],
                      pAppData->inPortParams.fin);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Bytes read are:%d\n", dataRead);

    /* Update the buffer header with buffer filled length and alloc length */
    pBuf->nFilledLen = dataRead;
    pBuf->nAllocLen = pAppData->inPortParams.comp.h264Vdec.frameSize[count];

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
 *      H264VDEC_FuncTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a H264 thread which invokes during func command
 *
 *  @param in:
 *            void *threadsArg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void H264VDEC_FuncTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->h264VdecClient);
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != funcDataThreadArg->dssClient);
  dssClient  = funcDataThreadArg->dssClient;
#endif
  h264VdecClient = funcDataThreadArg->h264VdecClient;
  

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);
  
  PRINT(PRINT_SIMPLE_MESSAGE, "\nbufferOutSize:%d\n",\
        (int)h264VdecClient->outPortParams.bufferOutSize);
  
  /* Set the port params */
  err = H264VDEC_SetPortDefinition(h264VdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to idle state */
  err = OMX_SendCommand(h264VdecClient->handle,
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
  err = H264VDEC_AllocateResources(h264VdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to executing state */
  err = OMX_SendCommand(h264VdecClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL);
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);

  /* Update the func create count */
  H264VDEC_funcCreateCount++;
  PRINT(PRINT_MUST, "%s: H264 FUNC Create Count: %u\n", __func__,
                    (unsigned int) H264VDEC_funcCreateCount);

  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = H264VDEC_UseInitialResources(h264VdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Update the data streaming thread arguments */
  strmThreadArg[h264VdecClient->instanceNo].h264VdecClient = h264VdecClient;
#ifdef OMTB_FF_DRIVER_DISPLAY
  strmThreadArg[h264VdecClient->instanceNo].dssClient  = dssClient;
#endif  
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VdecClient->inPortParams.inDataStrmThrdId,
                    (void *)H264VDEC_InputDataRead,
                    0,
                    &strmThreadArg[h264VdecClient->instanceNo],
                    OMTB_H264_VDEC_TSK_STACK_SIZE,
                    OMTB_H264_VDEC_GROUP_PRIORITY,
                    (signed char*)"H264_VDEC_DATA_IN_TSK");
    
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video decode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VdecClient->outPortParams.outDataStrmThrdId,
                    (void *)H264VDEC_OutputDataWrite,
                    0,
                    &strmThreadArg[h264VdecClient->instanceNo],
                    OMTB_H264_VDEC_TSK_STACK_SIZE,
                    OMTB_H264_VDEC_GROUP_PRIORITY,
                    (signed char*)"H264_VDEC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video decode output data write thread\n");
  }
  

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eofSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "The execution of the video decoding\
                           process is terminated\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* state change of component from executing to idle */
  err = OMX_SendCommand(h264VdecClient->handle,
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
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "State transitioned to Idle\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to go to loaded state */
  err = OMX_SendCommand(h264VdecClient->handle,
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
  h264VdecClient->outPortParams.flagEos = OMX_FALSE;
  h264VdecClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Function to reelase the resources allocated by component */
  err = H264VDEC_FreeResources(h264VdecClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "State transitioned to loaded\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Delete the component instance */
  err = OMX_FreeHandle(h264VdecClient->handle);

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
    h264VdecClient->handle = NULL;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted H264Vdec Instance #: %d\n\n", 
          (int) h264VdecClient->instanceNo);
    omtbClass[OMTB_H264_VDEC_CLASS].handle[h264VdecClient->instanceNo] = NULL;
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
  H264VDEC_FreeMemory(h264VdecClient);

  /* Update the func delete count */
  H264VDEC_funcDeleteCount++;
  PRINT(PRINT_MUST, "%s: H264 FUNC Delete Count: %u\n", __func__,
                    (unsigned int) H264VDEC_funcDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  h264VdecClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(
      omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[h264VdecClient->instanceNo]);

#else
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_H264_VDEC_CLASS].execThreadId[h264VdecClient->instanceNo],
      NULL);
#endif
}


/******************************************************************************\
 *      H264VDEC_FreeResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            h264VdecClient: Pointer to OMTB_H264_VDEC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_FreeResources(OMTB_H264_VDEC_PRIVATE *h264VdecClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  /* Ask component to Free input port buffers */
  
  for(i = 0; i < h264VdecClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video dec I/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",\
          (unsigned int) h264VdecClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n", \
          (unsigned int) h264VdecClient->inPortParams.pInPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",\
          (unsigned int) h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == h264VdecClient->inPortParams.flagUseInBuffer)
    {

#ifdef SHARED_MEM
       HeapBuf_free(heapHandle, h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer,
                h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->nAllocLen);
#elif defined (TILER)
	MemMgr_Free(h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);   
#elif defined (WTSD_TEMP)
      TIMM_OSAL_Free (
              h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer);
#else
      TIMM_OSAL_CMEM_Free (
              h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]->pBuffer, 
              h264VdecClient->inPortParams.pInPortDef->nBufferSize);
#endif
      
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
  
    /* freeing buffers of component input ports */
    err = OMX_FreeBuffer( h264VdecClient->handle,
                      h264VdecClient->inPortParams.pInPortDef->nPortIndex,
                      h264VdecClient->inPortParams.comp.h264Vdec.pInBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
    
  } // for(i = 0; i < h264VdecClient->inPortParams.pInPortDef
  
  /* Ask component to Free output port buffers */  
  for(i = 0; i < h264VdecClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video dec O/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",
          (unsigned int) h264VdecClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n",
          (unsigned int) h264VdecClient->outPortParams.pOutPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",
          (unsigned int) h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == h264VdecClient->outPortParams.flagUseOutBuffer)
    {

#ifdef SHARED_MEM
       HeapBuf_free(heapHandle, h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer,
                h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->nAllocLen);
#elif defined (TILER)
	MemMgr_Free(h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);   
#elif defined (WTSD_TEMP)
      TIMM_OSAL_Free (
              h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer);
#else
      TIMM_OSAL_CMEM_Free (
              h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]->pBuffer, 
              h264VdecClient->outPortParams.pOutPortDef->nBufferSize);
#endif
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* freeing buffers of component output ports */
    err = OMX_FreeBuffer( h264VdecClient->handle,
                      h264VdecClient->outPortParams.pOutPortDef->nPortIndex,
                      h264VdecClient->outPortParams.comp.h264Vdec.pOutBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
  
  } // for(i = 0; i < h264VdecClient->outPortParams.pOutPortDef ...
  
  /*wait for event handler to report completion of loaded command */
  TIMM_OSAL_SemaphoreObtain(h264VdecClient->eventSem, TIMM_OSAL_SUSPEND);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VDEC_FreeMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            h264VdecClient: Pointer to OMTB_H264_VDEC_PRIVATE struct
 *
 *  @return   None
 *
 */

void H264VDEC_FreeMemory(OMTB_H264_VDEC_PRIVATE *h264VdecClient)
{
  int instanceNo = h264VdecClient->instanceNo;

  /* validate the function arguments */
  ASSERT(NULL != h264VdecClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);
  
  /* Release callback structure memory */
  if(NULL != h264VdecClient->cb)
    TIMM_OSAL_Free(h264VdecClient->cb);
  
  /* Release input port memory */
  if(NULL != h264VdecClient->inPortParams.pInPortDef)
    TIMM_OSAL_Free(h264VdecClient->inPortParams.pInPortDef);
  
  /* Release output port memory */
  if(NULL != h264VdecClient->outPortParams.pOutPortDef)
    TIMM_OSAL_Free(h264VdecClient->outPortParams.pOutPortDef);
  
  /* Release component event handling semaphore memory */
  if(NULL != h264VdecClient->eventSem)
    TIMM_OSAL_SemaphoreDelete(h264VdecClient->eventSem);
  
  /* Release EOF handling semaphore memory */
  if(NULL != h264VdecClient->eofSem)
    TIMM_OSAL_SemaphoreDelete(h264VdecClient->eofSem);
  
  /* Release input data read thread control semaphore memory */
  if(NULL != h264VdecClient->inPortParams.inputDataReady)
    TIMM_OSAL_SemaphoreDelete(h264VdecClient->inPortParams.inputDataReady);
  
  /* Release output data write thread control semaphore memory */
  if(NULL != h264VdecClient->outPortParams.outputDataReady)
    TIMM_OSAL_SemaphoreDelete(h264VdecClient->outPortParams.outputDataReady);

  /* Release input data read thread exit semaphore memory */
  if(NULL != h264VdecClient->inPortParams.inputDataReadThrExit)
    TIMM_OSAL_SemaphoreDelete(
              h264VdecClient->inPortParams.inputDataReadThrExit);
  
  /* Release output data write thread exit semaphore memory */
  if(NULL != h264VdecClient->outPortParams.outputDataWriteThrExit)
    TIMM_OSAL_SemaphoreDelete(
              h264VdecClient->outPortParams.outputDataWriteThrExit);
  
  if(OMTB_FILE == h264VdecClient->outPortParams.dataOutPutMode)
  {
    /* closing the output file */
    if(NULL != h264VdecClient->outPortParams.fout)
      fclose(h264VdecClient->outPortParams.fout);
  }
#ifdef OMTB_FF_DRIVER_DISPLAY
  else if(OMTB_DRIVER == h264VdecClient->outPortParams.dataOutPutMode)
  {
    OmtbDispInfo *dssClient;
    dssClient = omtbClass[OMTB_H264_VDEC_CLASS].dssClient[\
                          h264VdecClient->instanceNo];
    
    /* Close the display device */
    if(NULL != dssClient)
      OMTB_DispClose (dssClient);
    
    /* Release display driver cfg param memory */
    if(NULL != dssClient)
    {
      TIMM_OSAL_Free(dssClient);
      omtbClass[OMTB_H264_VDEC_CLASS].dssClient[\
                h264VdecClient->instanceNo] = NULL;
    }
  }
#endif
  
  /* closing the input file */
  if(NULL != h264VdecClient->inPortParams.fin)
    fclose(h264VdecClient->inPortParams.fin);
  
  /* closing the stream_size file */
  if(NULL != h264VdecClient->inPortParams.comp.h264Vdec.frameFile)
    fclose(h264VdecClient->inPortParams.comp.h264Vdec.frameFile);
  
  /* Release component specific memory */
  if(NULL != h264VdecClient->pH264)
    TIMM_OSAL_Free(h264VdecClient->pH264);
  
  /* Release app pvt data private structure */
  if(NULL != h264VdecClient)
    TIMM_OSAL_Free(h264VdecClient);

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instanceNo);
}


/******************************************************************************\
 *      H264VDEC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the h264 video decoder component
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

OMX_ERRORTYPE H264VDEC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;

  /* thread argument validation */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  
  h264VdecClient = pAppData;

  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VdecClient->instanceNo);

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
          TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
        }
        /* port enable or disable command related event */
        else if ( OMX_CommandPortEnable   == Data1  || 
                  OMX_CommandPortDisable  == Data1  )
        {
          PRINT(PRINT_SIMPLE_MESSAGE,
                "In %s: OMX_CommandPortEnable/OMX_CommandPortDisable\n",
                __func__);

          /* Post the component event sem */
          TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
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
          //TIMM_OSAL_SemaphoreRelease(h264VdecClient->eofSem);
        }
      }
      break;
    
    case OMX_EventError:  /* Event error */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventError\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
      }
      break;
    
    case OMX_EventMark: /* Event mark buffer */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventMark\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
      }
      break;
    
    case OMX_EventResourcesAcquired:  /* Event resources acquired */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventResourcesAcquired\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
      }
      break;
    
    case OMX_EventComponentResumed: /* Event component resumed */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventComponentResumed\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
      }
      break;
    
    case OMX_EventDynamicResourcesAvailable:// Event dynamic resources available
      {
        PRINT(PRINT_SIMPLE_MESSAGE, 
              "In %s: OMX_EventDynamicResourcesAvailable\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VdecClient->eventSem);
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
                              __func__, (int)h264VdecClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VDEC_InputDataRead Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a H264 thread which reads data from input file
 *
 *  @param in:
 *            threadArg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void H264VDEC_InputDataRead(void *threadsArg)
{
  unsigned int actualSize = 0,dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
  unsigned int numRemainingIn = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->h264VdecClient);
    
  h264VdecClient = strmDataThreadArg->h264VdecClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == h264VdecClient->inPortParams.flagInputEos)
  {
  
    /* waiting for empty input buffer via callback */
    TIMM_OSAL_SemaphoreObtain(h264VdecClient->inPortParams.inputDataReady, 
                              TIMM_OSAL_SUSPEND);

    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (h264VdecClient->inPortParams.ipBufPipe,
                                        (void*) &numRemainingIn);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Empty This buffer:%d\n",\
          (int) numRemainingIn);

    while(numRemainingIn)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering EmptyThisBuffer Loop\n");
    
      /* Read empty buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe (h264VdecClient->inPortParams.ipBufPipe,
                              &pBufferIn,
                              sizeof(pBufferIn),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );

      /* Fill the data in the empty buffer */
      dataRead = H264VDEC_FillData(
                        h264VdecClient,
                        pBufferIn,
                        h264VdecClient->inPortParams.comp.h264Vdec.frameCount);

      /* Exit the loop if no data available */
      if(0 >= dataRead)
      {
          PRINT(PRINT_ERR_MSG, "No dataRead\n");
          h264VdecClient->inPortParams.flagInputEos = OMX_TRUE;
          break;
      }
      else
      {
        /* increase the frame counter if some data is available */
        h264VdecClient->inPortParams.comp.h264Vdec.frameCount++;
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
      err = OMX_GetState(h264VdecClient->handle, &pState);

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
             to component and will be read again in H264VDEC_UseInitialResources
             on Func/API mode thread creation */
          if(0 == h264VdecClient->inPortParams.comp.h264Vdec.frameCount)
            h264VdecClient->inPortParams.comp.h264Vdec.frameCount = 0;
          else
            h264VdecClient->inPortParams.comp.h264Vdec.frameCount--;
          
          fseek(h264VdecClient->inPortParams.fin, -dataRead, SEEK_CUR);

          continue;
        }
      }
#ifdef SHARED_MEM
      printf("\npBuffer before ETB = %x\n",pBufferIn->pBuffer);
      pBufferIn->pBuffer = (char *)SharedRegion_getSRPtr(pBufferIn->pBuffer, 2);
      printf("\npBuffer SR before ETB = %x\n",pBufferIn->pBuffer);        
#endif
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the input buffer to the component */
      err = OMX_EmptyThisBuffer(h264VdecClient->handle, pBufferIn);
    
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_EmptyThisBuffer");
    
#ifdef SHARED_MEM
      printf("\npBuffer SR after ETB = %x\n",pBufferIn->pBuffer);
      pBufferIn->pBuffer = SharedRegion_getPtr(pBufferIn->pBuffer);
      printf("\npBuffer after ETB = %x\n",pBufferIn->pBuffer);
#endif 
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
      h264VdecClient->inPortParams.flagInputEos = OMX_TRUE;
      h264VdecClient->outPortParams.flagEos = OMX_TRUE;
    }
  
  }
  
  PRINT(DEFAULT_MESSAGES, "The execution of the video decoding\
                           input data read is terminated\n");

  /* post input buffer read thread exit */
  TIMM_OSAL_SemaphoreRelease(h264VdecClient->inPortParams.inputDataReadThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);

  /* exit the thread */
#ifdef WTSD
  TIMM_OSAL_DeleteTask(h264VdecClient->inPortParams.inDataStrmThrdId);
#else
  TIMM_OSAL_ExitTask(h264VdecClient->inPortParams.inDataStrmThrdId, NULL);
#endif
}


/******************************************************************************\
 *      H264VDEC_OutputDataWrite Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    This is a H264 thread which writes data to output file
 *
 *  @param in:
 *            threadArg - Parameters which we need to pass from func function.
 *
 *  @return   None
 *
 */

void H264VDEC_OutputDataWrite(void *threadsArg)
{
  unsigned int actualSize = 0,framesDecoded = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VDEC_PRIVATE* h264VdecClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
  unsigned int numRemainingOut = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
#ifdef OMTB_FF_DRIVER_DISPLAY
  OMX_PTR *displayBuffer = NULL;
  OmtbDispInfo *dssClient = NULL;
#endif
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->h264VdecClient);
  
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != strmDataThreadArg->dssClient);
  dssClient  = strmDataThreadArg->dssClient;
#endif

  h264VdecClient = strmDataThreadArg->h264VdecClient;
  

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == h264VdecClient->outPortParams.flagEos)
  {
    /* waiting for filled output buffer via callback */
    TIMM_OSAL_SemaphoreObtain(h264VdecClient->outPortParams.outputDataReady, 
                              TIMM_OSAL_SUSPEND);
    
    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (h264VdecClient->outPortParams.opBufPipe,
                                        (void*) &numRemainingOut);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Fill This buffer: %d\n",\
          (int) numRemainingOut);
    
    while(numRemainingOut)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering FillThisBuffer Loop\n");
    
      /* Read filled buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe( h264VdecClient->outPortParams.opBufPipe,
                              &pBufferOut,
                              sizeof(pBufferOut),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );
      
      if(OMTB_FILE == h264VdecClient->outPortParams.dataOutPutMode)
      {

        /* write data to output file */
        fwrite( pBufferOut->pBuffer,
                sizeof(char),
                h264VdecClient->outPortParams.bufferOutSize,
                h264VdecClient->outPortParams.fout);        
        
        PRINT(PRINT_SIMPLE_MESSAGE, "Frame:%d Decoded\n",++framesDecoded );
        fflush (h264VdecClient->outPortParams.fout);
      }
#ifdef OMTB_FF_DRIVER_DISPLAY
      else if(OMTB_DRIVER == h264VdecClient->outPortParams.dataOutPutMode)
      {
        /* Get empty buffer from the driver */
        displayBuffer = (OMX_PTR *) OMTB_GetDispBuff(dssClient);
        if(NULL == displayBuffer)
        {
           PRINT(PRINT_ERR_MSG, "Unable to get the Display buffer\n");
        }
        else
        {
          /* Copy the data in the driver buffer */
          memcpy( displayBuffer, pBufferOut->pBuffer,
                  h264VdecClient->outPortParams.bufferOutSize);
        }

        /* Give data to driver for playback */
        if(OMTB_FAIL == OMTB_DisplayBuf(dssClient, displayBuffer))
        {
           PRINT(PRINT_ERR_MSG, "Unable to display buffer \n");
        }
        else
        {
           PRINT(PRINT_SIMPLE_MESSAGE,"Displayed Frame# %d\n",++framesDecoded);
        }
      
      }
#endif
      /* reset the buffer filled length */
      pBufferOut->nFilledLen = 0;

      /* Exit the loop if EOS is reached */
      if(OMX_BUFFERFLAG_EOS == pBufferOut->nFlags)
      {
      
        PRINT(PRINT_MUST, "EOS reached ...\n");
        h264VdecClient->outPortParams.flagEos = OMX_TRUE;
        break;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Get the current state of the component */
      err = OMX_GetState(h264VdecClient->handle, &pState);

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
    
#ifdef SHARED_MEM
      pBufferOut->pBuffer = 
          (char *)SharedRegion_getSRPtr(pBufferOut->pBuffer, 2);    
#endif
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the output buffer to the component */
      err = OMX_FillThisBuffer(h264VdecClient->handle, pBufferOut);
      
      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_FillThisBuffer");

#ifdef SHARED_MEM
      pBufferOut->pBuffer = SharedRegion_getPtr(pBufferOut->pBuffer);  
#endif

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
  TIMM_OSAL_SemaphoreRelease(h264VdecClient->eofSem);

  /* waiting for output buffer write thread exit */
  TIMM_OSAL_SemaphoreRelease(
            h264VdecClient->outPortParams.outputDataWriteThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VdecClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(h264VdecClient->outPortParams.outDataStrmThrdId);  
#else
  TIMM_OSAL_ExitTask(h264VdecClient->outPortParams.outDataStrmThrdId, NULL);
#endif
}


/******************************************************************************\
 *      H264VDEC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the h264 video decoder component
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

OMX_ERRORTYPE H264VDEC_EmptyBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_H264_VDEC_PRIVATE* pAppData = ptrAppData;
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
  //  OMTB_OMX_API_EXIT_TIME_CALC("H264VDEC_EmptyBufferDone");
  
  return err;
}


/******************************************************************************\
 *      H264VDEC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the h264 video decoder component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE H264VDEC_FillBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_H264_VDEC_PRIVATE* pAppData = ptrAppData;
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
  //  OMTB_OMX_API_EXIT_TIME_CALC("H264VDEC_FillBufferDone");
  
  return err;
}

/*----------------------------- end of file ----------------------------------*/

