/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file      omtbH264Venc.c
 *
 *  @brief     H264 Video Encode IL Client Implementation.
 *
 * <B> History: </B>
 *
 *      # October-2-2009   AVME System Test    Initial Version
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
#ifdef WTSD
#include "omx_ti_index.h"
#include "omx_ti_video.h" //Brijesh: OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE
#include "omx_video.h"// Brijesh: OMX_TI_IndexParamVideoBitStreamFormatSelect
#else
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

#include "omtbCommon.h"
#include "omtbCmd.h"

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
/*Tiler APIs*/
#include <memmgr/memmgr.h>
#include <mmplatform.h>
#endif

/******************************************************************************\
 *      Local Defines
\******************************************************************************/
#ifdef TILER //OMX_H264E_LINUX_TILERTEST
#define STRIDE_8BIT (4 * 1024)
#define STRIDE_16BIT (4 * 1024)
#endif

// Brijesh added below
#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)   \
 TIMM_OSAL_Memset((_s_), 0x0, sizeof(_name_)); 			\
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       	\
    (_s_)->nVersion.s.nStep   = 0x0;
#define H264_APP_OUTPUTPORT 1


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Callback structure Definition */

static OMX_CALLBACKTYPE H264VENC_Callback = {
  .EventHandler = H264VENC_EventHandler,      /* Event handler callback */    
  .EmptyBufferDone = H264VENC_EmptyBufferDone,/* Empty buffer done callback */
  .FillBufferDone = H264VENC_FillBufferDone   /* Fill buffer done callback */ 
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
extern OMTB_TEMPLATE_INFO    *omtbCfgTemplate[OMTB_MAX_INS];


/******************************************************************************\
 *      Globals
\******************************************************************************/

/* Pointer to H264 venc config params */
OMTB_H264_VENC_CONFIG_PARAMS *h264VencConfig = NULL;


/******************************************************************************\
 *      Static Globals
\******************************************************************************/
#ifdef WTSD_TEMP
static OMX_U32 H264VENC_funcCreateCount = 0;  /* Func create count */
static OMX_U32 H264VENC_apiCreateCount = 0;   /* API create count */ 
                                                                 
static OMX_U32 H264VENC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U32 H264VENC_apiDeleteCount = 0;   /* API delete count */ 
#else
static OMX_U64 H264VENC_funcCreateCount = 0;  /* Func create count */
static OMX_U64 H264VENC_apiCreateCount = 0;   /* API create count */ 
                                                                 
static OMX_U64 H264VENC_funcDeleteCount = 0;  /* Func delete count */
static OMX_U64 H264VENC_apiDeleteCount = 0;   /* API delete count */ 
#endif
/* Enum Entry for Supported cfg param indexs */

PRIVATE MX_DBG_ENUM_ENTRY_T enumTblH264VencCfgParamIdx[] =
{
  /* Dynamic configuration params */
  {"OMX_IndexConfigVideoFramerate",
    OMX_IndexConfigVideoFramerate,
    ""},
  {"OMX_IndexConfigVideoBitrate",
    OMX_IndexConfigVideoBitrate,
    ""},
  {"OMX_TI_IndexConfigVideoMESearchRange",
    OMX_TI_IndexConfigVideoMESearchRange,
    ""},
  {"OMX_IndexConfigVideoIntraVOPRefresh",
    OMX_IndexConfigVideoIntraVOPRefresh,
    ""},
  {"OMX_TI_IndexConfigVideoQPSettings",
    OMX_TI_IndexConfigVideoQPSettings,
    ""},
  {"OMX_IndexConfigVideoAVCIntraPeriod",
    OMX_IndexConfigVideoAVCIntraPeriod,
    ""},
  {"OMX_TI_IndexConfigSliceSettings",
    OMX_TI_IndexConfigSliceSettings,
    ""},
  {"OMX_TI_IndexConfigVideoPixelInfo",
    OMX_TI_IndexConfigVideoPixelInfo,
    ""},

  /* Advanced configuration params */
  {"OMX_TI_IndexParamVideoNALUsettings",
    OMX_TI_IndexParamVideoNALUsettings,
    ""},
  {"OMX_TI_IndexParamVideoAdvancedFMO",
    OMX_TI_IndexParamVideoAdvancedFMO,
    ""},
  {"OMX_TI_IndexParamVideoMEBlockSize",
    OMX_TI_IndexParamVideoMEBlockSize,
    ""},
  {"OMX_IndexParamVideoIntraRefresh",
    OMX_IndexParamVideoIntraRefresh,
    ""},
  {"OMX_TI_IndexParamVideoVUIsettings",
    OMX_TI_IndexParamVideoVUIsettings,
    ""},
  {"OMX_TI_IndexParamVideoIntraPredictionSettings",
    OMX_TI_IndexParamVideoIntraPredictionSettings,
    ""},
  {"OMX_TI_IndexParamVideoDataSyncMode",
    OMX_TI_IndexParamVideoDataSyncMode,
    ""},

  /* Basic configuration params */
  {"OMX_IndexParamVideoAvc",
    OMX_IndexParamVideoAvc,
    ""},
  {"OMX_IndexParamVideoBitrate",
    OMX_IndexParamVideoBitrate,
    ""},
  
  {"", 0, ""}
};


/******************************************************************************\
 *      Public Function Prototypes
\******************************************************************************/
OMX_ERRORTYPE H264ENC_SetAllParams(OMTB_H264_VENC_PRIVATE* h264Client); //Brijesh
OMX_ERRORTYPE H264ENC_SetParamsFromInitialDynamicParams(OMTB_H264_VENC_PRIVATE* pAppData);//Brijesh

/******************************************************************************\
 *      Private Function Prototypes
\******************************************************************************/
#ifdef TILER //Brijesh
OMX_ERRORTYPE OMXH264_Util_Memcpy_1Dto2D_ENC(OMX_PTR pDst2D, OMX_PTR pSrc1D, OMX_U32 nSize1D, OMX_U32 nHeight2D, OMX_U32 nWidth2D, OMX_U32 nStride2D);
//TODO: Brijesh to have a proper deination for this function
#endif //Brijesh

/******************************************************************************\
 *      Function Definitions                                                    
\******************************************************************************/

/******************************************************************************\
 *      H264VENC_AllocateMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate memory for private data structure for h264 video encode
 *            component
 *
 *  @param in:
 *            h264VencClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */

OmtbRet H264VENC_AllocateMemory(OMTB_H264_VENC_PRIVATE *h264VencClient)
{
  OmtbRet err = OMTB_SUCCESS;
  unsigned int retVal = TIMM_OSAL_ERR_NONE;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  /* Allocating memory for callback structure */
  h264VencClient->cb=(OMX_CALLBACKTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_CALLBACKTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->cb)
  {
    PRINT(PRINT_ERR_MSG, "Callback structure memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for input port definition structure */
  h264VencClient->inPortParams.pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->inPortParams.pInPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Input port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for output port definition structure */
  h264VencClient->outPortParams.pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)
                    TIMM_OSAL_Malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE),
                                            TIMM_OSAL_TRUE,
                                            0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->outPortParams.pOutPortDef)
  {
    PRINT(PRINT_ERR_MSG, "Output port struct memory allocation failed\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Allocating memory for component specific structures if any */
  if (OMX_VIDEO_CodingAVC == h264VencClient->compressionFormat)
  {
    h264VencClient->pH264  = (OMX_VIDEO_PARAM_AVCTYPE*)
                    TIMM_OSAL_Malloc( sizeof(OMX_VIDEO_PARAM_AVCTYPE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
    
    if (!h264VencClient->pH264)
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
                    &(h264VencClient->inPortParams.ipBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      h264VencClient->inPortParams.numInBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
  
  /* Create output pipe for output buffers */
  retVal = TIMM_OSAL_CreatePipe(
                    &(h264VencClient->outPortParams.opBufPipe),
                    ((sizeof(OMX_BUFFERHEADERTYPE*)) * 
                      h264VencClient->outPortParams.numOutBuf),
                    sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
                    
  if (TIMM_OSAL_ERR_NONE != retVal)
  {
    PRINT(PRINT_ERR_MSG, "TIMM_OSAL_CreatePipe failed to open\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
  }
#if 0
  /* Allocating memory for component event handling semaphore */
  h264VencClient->eventSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VencClient->eventSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for eof handling semaphore */
  h264VencClient->eofSem = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VencClient->eofSem)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for input data read thread exit semaphore */
  h264VencClient->inPortParams.inputDataReadThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->inPortParams.inputDataReadThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  
  /* Allocating memory for output data write thread exit semaphore */
  h264VencClient->outPortParams.outputDataWriteThrExit= (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->outPortParams.outputDataWriteThrExit)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }

  /* Allocating memory for input data read thread control semaphore */
  h264VencClient->inPortParams.inputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
  
  if (!h264VencClient->inPortParams.inputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
  
  /* Allocating memory for output data write thread semaphore */
  h264VencClient->outPortParams.outputDataReady = (sem_p*)
                    TIMM_OSAL_Malloc( sizeof(sem_p),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);
                                      
  if (!h264VencClient->outPortParams.outputDataReady)
  {
    err = OMTB_INSUFFICIENT_RESOURCES;
    OMTB_OmxReturnStatus(err);
  }
#endif
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VENC_AllocateResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for H264 encoder.
 *
 *  @param in:
 *            h264VencClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VENC_AllocateResources(OMTB_H264_VENC_PRIVATE* h264VencClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  unsigned char *pBuffer = NULL;
  OMX_U8 *pTmpBuffer; /*Used with Use Buffer calls*/  //Brijesh

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
	  MemAllocBlock *MemReqDescTiler;
	  OMX_PTR TilerAddr=NULL;
#endif

  
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
	  MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	  if(MemReqDescTiler == NULL)//???
	  {
		  PRINT(PRINT_ERR_MSG, \
		      "No Memory for MemReqDescTiler\n");

		  /* Function to release the resources allocated by component */
		  err = H264VENC_FreeResources(h264VencClient);

		  if(OMX_ErrorNone != err)
		  {
		    OMTB_OmxReturnStatus(err);
		  }
		    
		  err = OMTB_INSUFFICIENT_RESOURCES;
		  return err;
		}	  
#endif
  
  /* Input port buffer allocation */
  for(i = 0; i < h264VencClient->inPortParams.pInPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VencClient->inPortParams.flagUseInBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                        h264VencClient->handle,
                        &h264VencClient->inPortParams.comp.h264Venc.pInBuff[i], //Brijesh where do we allocate for buffer header type????
                        h264VencClient->inPortParams.pInPortDef->nPortIndex, 
                        h264VencClient,
                        h264VencClient->inPortParams.pInPortDef->nBufferSize);  //Brijesh is buffer size OK???

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */ 
    else
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh

		MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
		MemReqDescTiler[0].dim.area.width=h264VencClient->inPortParams.comp.h264Venc.nWidth;/*width*/
		MemReqDescTiler[0].dim.area.height=h264VencClient->inPortParams.comp.h264Venc.nHeight;/*height*/
		MemReqDescTiler[0].stride=STRIDE_8BIT;
					
		MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
		MemReqDescTiler[1].dim.area.width=h264VencClient->inPortParams.comp.h264Venc.nWidth/2;/*width*/
		MemReqDescTiler[1].dim.area.height=h264VencClient->inPortParams.comp.h264Venc.nHeight/2;/*height*/
		MemReqDescTiler[1].stride=STRIDE_16BIT;
					
		//MemReqDescTiler.reserved
		/*call to tiler Alloc*/
		PRINT(PRINT_MUST, "Before 2D Tiler buffer allocation \n");
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
		PRINT(PRINT_MUST, "2D Tiler buffer allocated :TilerAddr\n");
        pTmpBuffer = (OMX_U8 *)TilerAddr;
		if(pTmpBuffer == NULL)//???
		{
		   PRINT(PRINT_ERR_MSG, \
		       "Error in TILER Buffer alloc for 2D pTmpBuffer\n");

		   /* Function to release the resources allocated by component */
		   err = H264VENC_FreeResources(h264VencClient);

		   if(OMX_ErrorNone != err)
		   {
		     OMTB_OmxReturnStatus(err);
		   }
		    
		   err = OMTB_INSUFFICIENT_RESOURCES;
		   return err;
		}

		/* API entry time calculation macro */
		OMTB_OMX_API_ENTRY_TIME_CALC;
		
		err = OMX_UseBuffer(h264VencClient->handle,
					  &h264VencClient->inPortParams.comp.h264Venc.pInBuff[i],
					  h264VencClient->inPortParams.pInPortDef->nPortIndex, 
					  h264VencClient,
					  ((h264VencClient->inPortParams.comp.h264Venc.nWidth * h264VencClient->inPortParams.comp.h264Venc.nHeight *3/2)),		  
					  //h264VencClient->inPortParams.pInPortDef->nBufferSize,
					  pTmpBuffer);
		
		/* API exit time calculation macro */
		OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
		
		
#else  //Brijesh  

      /* Allocating memory for input port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                        h264VencClient->inPortParams.pInPortDef->nBufferSize,
                        OMTB_CMEM_ALLOCATION,
                        0,
                        TIMMOSAL_MEM_SEGMENT_EXT);
      
      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for i/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VENC_FreeResources(h264VencClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer(h264VencClient->handle,
                    &h264VencClient->inPortParams.comp.h264Venc.pInBuff[i],//Brijesh is it ok???
                    h264VencClient->inPortParams.pInPortDef->nPortIndex, 
                    h264VencClient,
                    h264VencClient->inPortParams.pInPortDef->nBufferSize, //Brijesh is buffer size ok???
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
      err = H264VENC_FreeResources(h264VencClient);

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
  for(i = 0; i < h264VencClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* OMX_AllocateBuffer */
    if(OMX_FALSE == h264VencClient->outPortParams.flagUseOutBuffer)
    {
      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_AllocateBuffer( 
                    h264VencClient->handle,
                    &h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i], //Brijesh where do we allocate for buffer header type????
                    h264VencClient->outPortParams.pOutPortDef->nPortIndex,
                    h264VencClient,
                    h264VencClient->outPortParams.pOutPortDef->nBufferSize); //Brijesh is buffer size ok???

      /* API exit time calculation macro */
      OMTB_OMX_API_EXIT_TIME_CALC("OMX_AllocateBuffer");
    }
    /* OMX_UseBuffer */
    else
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh

		MemReqDescTiler[0].pixelFormat = PIXEL_FMT_PAGE;
		MemReqDescTiler[0].dim.len = h264VencClient->inPortParams.comp.h264Venc.nWidth * h264VencClient->inPortParams.comp.h264Venc.nHeight;
		MemReqDescTiler[0].stride = 0;

		//MemReqDescTiler.stride
		//MemReqDescTiler.reserved
		/*call to tiler Alloc*/
		PRINT(PRINT_MUST, "Before 1D Tiler buffer allocation \n");		
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,1);		
		PRINT(PRINT_MUST, "1D Tiler buffer allocated :TilerAddr\n");
		pTmpBuffer = (OMX_U8 *)TilerAddr;
		if(pTmpBuffer == NULL)//???
		{
		   PRINT(PRINT_ERR_MSG, \
			   "Error in TILER Buffer alloc for 1D pTmpBuffer\n");

		   /* Function to release the resources allocated by component */
		   err = H264VENC_FreeResources(h264VencClient);

		   if(OMX_ErrorNone != err)
		   {
			 OMTB_OmxReturnStatus(err);
		   }
			
		   err = OMTB_INSUFFICIENT_RESOURCES;
		   return err;
		}

		/* API entry time calculation macro */
		OMTB_OMX_API_ENTRY_TIME_CALC;

		err = OMX_UseBuffer( h264VencClient->handle,
					   &h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i],
					   h264VencClient->outPortParams.pOutPortDef->nPortIndex,
					   h264VencClient,
					   ((h264VencClient->inPortParams.comp.h264Venc.nWidth * h264VencClient->inPortParams.comp.h264Venc.nHeight)),
					   pTmpBuffer);
		
		/* API exit time calculation macro */
		OMTB_OMX_API_EXIT_TIME_CALC("OMX_UseBuffer");
				
				
#else  //Brijesh  
    
      /* Allocating memory for output port buffer */
      pBuffer = (unsigned char*)TIMM_OSAL_Malloc(
                      h264VencClient->outPortParams.pOutPortDef->nBufferSize,
                      OMTB_CMEM_ALLOCATION,
                      0,
                      TIMMOSAL_MEM_SEGMENT_EXT);

      if (!pBuffer)
      {
        PRINT(PRINT_ERR_MSG, \
            "Error in Buffer allocation for o/p port\n");

        /* Function to release the resources allocated by component */
        err = H264VENC_FreeResources(h264VencClient);

        if(OMX_ErrorNone != err)
        {
          OMTB_OmxReturnStatus(err);
        }
        
        err = OMTB_INSUFFICIENT_RESOURCES;
        return err;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      err = OMX_UseBuffer( h264VencClient->handle,
                     &h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i],
                     h264VencClient->outPortParams.pOutPortDef->nPortIndex,
                     h264VencClient,
                     h264VencClient->outPortParams.pOutPortDef->nBufferSize,
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
      err = H264VENC_FreeResources(h264VencClient);

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
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem, TIMM_OSAL_SUSPEND);

#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
  if(MemReqDescTiler)
	TIMM_OSAL_Free(MemReqDescTiler); //Brijesh last update from office
	  
#endif //Brijesh

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VENC_UseInitialResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Tells H264 encode component to empty/fill the initial buffers.
 *
 *  @param in:
 *            h264VencClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VENC_UseInitialResources(
  OMTB_H264_VENC_PRIVATE* h264VencClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int dataRead = 0;

#ifdef TILER //OMX_H264E_TILERTEST	//Brijesh
    OMX_U8 *pTmpBuffer = NULL;
	OMX_U32 size1D = 0;
	OMX_U32 framesizetoread = 0;
	OMX_U32 Addoffset=0;	
#endif
  
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

#ifdef TILER //OMX_H264E_TILERTEST	//Brijesh: Used for 1D buffers which are read from input file.
  framesizetoread = h264VencClient->inPortParams.comp.h264Venc.nWidth * h264VencClient->inPortParams.comp.h264Venc.nHeight * 3  / 2;
  size1D=framesizetoread;
  pTmpBuffer = (OMX_U8 *)TIMM_OSAL_Malloc(framesizetoread,OMX_FALSE,0,0);
  // Brijesh: TODO: need to pTmpBuffer FREE at the end??? For THC
#endif 

  /* Give input buffers to component which is limited by no of input buffers 
     available. Rest of the data will be read on the callback from input data 
     read thread */
  for(i = 0; i < h264VencClient->inPortParams.pInPortDef->nBufferCountActual;//buffercnt=1 Brijesh
      i++)
  {

#ifdef TILER //OMX_H264E_TILERTEST  //Brijesh
    dataRead = fread( //Brijesh reading 1D buffer
               pTmpBuffer, //Brijesh : h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer,
               sizeof(char),
               framesizetoread, //h264VencClient->inPortParams.bufferInSize, //Brijesh Home for OMX_COLOR_FormatYUV420SemiPlanar=ht*wdth *3/2
               h264VencClient->inPortParams.fin); //above already set in port defination

    //Brijesh TODO: Should do a error check for end of file OR no bytes read or just plain error.   Pending on OMTB                   

   // Brijesh: Convert the 1D buffer from read file to 2D buffer for OMX component consumption
   err = OMXH264_Util_Memcpy_1Dto2D_ENC(
   		h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer, 
   		pTmpBuffer, 
   		size1D, //h264VencClient->inPortParams.bufferInSize, //Brijesh TODO: cross check it is constant
   		h264VencClient->inPortParams.comp.h264Venc.nHeight, 
   		h264VencClient->inPortParams.comp.h264Venc.nWidth, 
   		STRIDE_8BIT); 
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }

	// Error check if data read from file is less than 1 frame data size: TODO maybe we might not want to do this since 
	// we are solving cases which the OMX component should take care of. TODO : Action item on THC to sort this out.
	// May be we should comment this below code.
	// TODO : bug in THC code : below if condition should contain "dataRead" check and not "framesizetoread" check
	if(dataRead/*framesizetoread*/< (h264VencClient->inPortParams.comp.h264Venc.nHeight * h264VencClient->inPortParams.comp.h264Venc.nWidth))
	   pTmpBuffer += size1D;
	else{
	   pTmpBuffer += (h264VencClient->inPortParams.comp.h264Venc.nHeight * h264VencClient->inPortParams.comp.h264Venc.nWidth);
	   size1D-=(h264VencClient->inPortParams.comp.h264Venc.nHeight  * h264VencClient->inPortParams.comp.h264Venc.nWidth);
	}

	// Is there one more copy of 1D to 2D needed as per THC in OMTB : Brijesh
	// I think the above one is for Y and one more below should be for UV data.: Brijesh
#ifdef TILER //OMX_H264E_LINUX_TILERTEST
	Addoffset=(h264VencClient->inPortParams.comp.h264Venc.nHeight * STRIDE_8BIT);
	err = OMXH264_Util_Memcpy_1Dto2D_ENC(
			(h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer + Addoffset), 
			pTmpBuffer, 
			size1D, 
			(h264VencClient->inPortParams.comp.h264Venc.nHeight/2), 
			h264VencClient->inPortParams.comp.h264Venc.nWidth, 
			STRIDE_16BIT); 
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
#endif
	
#endif //Brijesh

    h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->nFilledLen = 
                                                                    dataRead;
    h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->nOffset = 0;
    h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->nAllocLen = dataRead; //TODO Brijesh: Need to check
    h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->nInputPortIndex = 0;
    
    PRINT(PRINT_DEBUG_MESSAGE, "--> Read %d bytes from file\n",dataRead);
    PRINT(PRINT_DEBUG_MESSAGE, "Empty first  buffer %x\n",
          (int)h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer);

    h264VencClient->inPortParams.comp.h264Venc.frameCount++;
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer(
                        h264VencClient->handle,
                        h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]);
    
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
  for(i = 0; i < h264VencClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer( 
                      h264VencClient->handle,
                      h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i]);
    
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
                              __func__, (int)h264VencClient->instanceNo);

  // Brijesh: TODO: need to pTmpBuffer FREE at the end??? For THC

  return err;
}


/******************************************************************************\
 *      H264VENC_SetPortDefinition Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for H264 encoder.
 *
 *  @param in:
 *            h264VencClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VENC_SetPortDefinition(OMTB_H264_VENC_PRIVATE* h264VencClient)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_PORT_PARAM_TYPE portInit;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader(&portInit,sizeof(OMX_PORT_PARAM_TYPE));
  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;

  /* input buffer size calculation based on frame dimension speculation */
  switch(h264VencClient->inPortParams.comp.h264Venc.colorFormat)
  {
    case OMX_COLOR_FormatYUV420Planar:
    case OMX_COLOR_FormatYUV420SemiPlanar://Brijesh we use this type
      
      h264VencClient->inPortParams.bufferInSize   = 
                    ((h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 3)
                    >> 1); //Brijesh (ht *wdth * 3/2)

      break;
      
    case OMX_COLOR_FormatYUV422Planar:
    case OMX_COLOR_FormatYCbYCr:
      
      h264VencClient->inPortParams.bufferInSize   = 
                    (h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 2);

      break;

    default:

      PRINT(PRINT_MUST, "Undefined chroma format. Setting i/p buffer size for \
420p \n");
      h264VencClient->inPortParams.bufferInSize   = 
                    ((h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 3)
                    >> 1);

      break;
  }

//Brijesh start  
//  h264VencClient->outPortParams.bufferOutSize = OMTB_H264_VENC_MAX_OUTBUF_SIZE;
   h264VencClient->outPortParams.bufferOutSize = (h264VencClient->inPortParams.comp.h264Venc.nWidth *
				  h264VencClient->inPortParams.comp.h264Venc.nHeight);//Brijesh added should it be outPortParams????
//Brijesh end
                      
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* Initialize the Port*/
  // Brijesh THC doesnt set it just uses OMX_GetParameter to get the number of ports for looping on component
/*  
  err = OMX_SetParameter( h264VencClient->handle,
                          OMX_IndexParamVideoInit,
                          &portInit);
*/

  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
/*  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  else
  {
    PRINT(DEFAULT_MESSAGES, "Found The component for encoding\n");
  }
*/  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */
  OMTB_SetHeader( h264VencClient->inPortParams.pInPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  h264VencClient->inPortParams.pInPortDef->nPortIndex          = 0x0;
  h264VencClient->inPortParams.pInPortDef->eDir                = OMX_DirInput;
  h264VencClient->inPortParams.pInPortDef->nBufferCountActual  = 
                                  h264VencClient->inPortParams.numInBuf; //Brijesh will be 1
  h264VencClient->inPortParams.pInPortDef->nBufferCountMin     = 1;
  h264VencClient->inPortParams.pInPortDef->nBufferSize         = 
                                  h264VencClient->inPortParams.bufferInSize;
  h264VencClient->inPortParams.pInPortDef->bEnabled            = OMX_TRUE;
  h264VencClient->inPortParams.pInPortDef->bPopulated          = OMX_FALSE;
  h264VencClient->inPortParams.pInPortDef->eDomain             = 
                                  OMX_PortDomainVideo;
  h264VencClient->inPortParams.pInPortDef->bBuffersContiguous  = OMX_FALSE;
  h264VencClient->inPortParams.pInPortDef->nBufferAlignment    = 0x0;
  
  /* OMX_VIDEO_PORTDEFINITION values for input port */
  h264VencClient->inPortParams.pInPortDef->format.video.pNativeRender  = NULL;
  h264VencClient->inPortParams.pInPortDef->format.video.cMIMEType      = "H264";
  
//Brijesh commented  h264VencClient->inPortParams.pInPortDef->format.video.nStride        = -1;
  h264VencClient->inPortParams.pInPortDef->format.video.nStride        = 
     h264VencClient->inPortParams.comp.h264Venc.nWidth;// Brijesh stride=width

  h264VencClient->inPortParams.pInPortDef->format.video.nSliceHeight   = 0;
  h264VencClient->inPortParams.pInPortDef->format.video.nBitrate       = 0;
  h264VencClient->inPortParams.pInPortDef->format.video.xFramerate     = 0;
  h264VencClient->inPortParams.pInPortDef->format.video.eColorFormat   =
                    h264VencClient->inPortParams.comp.h264Venc.colorFormat;
  h264VencClient->inPortParams.pInPortDef->format.video.nFrameWidth    = 
                    h264VencClient->inPortParams.comp.h264Venc.nWidth;
  h264VencClient->inPortParams.pInPortDef->format.video.nFrameHeight   = 
                    h264VencClient->inPortParams.comp.h264Venc.nHeight;
  h264VencClient->inPortParams.pInPortDef->format.video.eCompressionFormat = 
                    OMX_VIDEO_CodingAVC;
  h264VencClient->inPortParams.pInPortDef->format.video.bFlagErrorConcealment =
                    OMX_FALSE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the inPort Definition*/
  /*
  err = OMX_SetParameter( h264VencClient->handle,
                          OMX_IndexParamPortDefinition,
                          h264VencClient->inPortParams.pInPortDef);
  */
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
/* BRIJESH TEMPORARILY commented
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  */
  OMTB_SetHeader( h264VencClient->outPortParams.pOutPortDef,
                  sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
  
  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  
  h264VencClient->outPortParams.pOutPortDef->nPortIndex         = 0x1;
  h264VencClient->outPortParams.pOutPortDef->eDir               = OMX_DirOutput;
  h264VencClient->outPortParams.pOutPortDef->nBufferCountActual = 
                                  h264VencClient->outPortParams.numOutBuf;//Brijesh will be 1
  h264VencClient->outPortParams.pOutPortDef->nBufferCountMin    = 1;
  h264VencClient->outPortParams.pOutPortDef->nBufferSize        = 
                                  h264VencClient->outPortParams.bufferOutSize;
  h264VencClient->outPortParams.pOutPortDef->bEnabled           = OMX_TRUE;
  h264VencClient->outPortParams.pOutPortDef->bPopulated         = OMX_FALSE;
  h264VencClient->outPortParams.pOutPortDef->eDomain            = 
                                         OMX_PortDomainVideo;
  h264VencClient->outPortParams.pOutPortDef->bBuffersContiguous = OMX_FALSE;
  h264VencClient->outPortParams.pOutPortDef->nBufferAlignment   = 0x0;
  
  /* OMX_VIDEO_PORTDEFINITION values for output port */
  h264VencClient->outPortParams.pOutPortDef->format.video.cMIMEType   = "H264";
  h264VencClient->outPortParams.pOutPortDef->format.video.pNativeRender = NULL;

//Brijesh start  
//  h264VencClient->outPortParams.pOutPortDef->format.video.nStride       = 0
  h264VencClient->outPortParams.pOutPortDef->format.video.nStride       =
  	h264VencClient->inPortParams.comp.h264Venc.nWidth;// Brijesh stride=width0;
//added the 2 new below
  h264VencClient->outPortParams.pOutPortDef->format.video.nFrameWidth  =
  		h264VencClient->inPortParams.comp.h264Venc.nWidth; // should it be outPortParams??? Brijesh
  h264VencClient->outPortParams.pOutPortDef->format.video.nFrameHeight =
  		h264VencClient->inPortParams.comp.h264Venc.nHeight; // should it be outPortParams??? Brijesh
  // Pending to set frame rate or bit rate on output port defination, Brijesh
  // Pending to set frame rate or bit rate on OMX_IndexParamVideoPortFormat, Brijesh
//Brijesh end

  h264VencClient->outPortParams.pOutPortDef->format.video.nSliceHeight  = 0;
  h264VencClient->outPortParams.pOutPortDef->format.video.nBitrate      = 0;
  h264VencClient->outPortParams.pOutPortDef->format.video.xFramerate    = 0;
  h264VencClient->outPortParams.pOutPortDef->format.video.bFlagErrorConcealment=
                            OMX_FALSE;
  h264VencClient->outPortParams.pOutPortDef->format.video.eCompressionFormat   =
                            OMX_VIDEO_CodingAVC;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /*Set the OutPort Definition*/
  /*
  err = OMX_SetParameter( h264VencClient->handle,
                          OMX_IndexParamPortDefinition,
                          h264VencClient->outPortParams.pOutPortDef);
  */
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_SetParameter");
  
/* BRIJESH TEMPORARILY commented
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
    return err;
  }
  */
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VENC_ApiTsk Thread Definition
\******************************************************************************/
/**
 *
 *  @brief    H264 Venc API mode Execute Thread
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

void H264VENC_ApiTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
 #ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->h264VencClient);
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != funcDataThreadArg->dssClient);
  dssClient  = funcDataThreadArg->dssClient;
#endif
  h264VencClient = funcDataThreadArg->h264VencClient;
  

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);

  /* Update the api mode create count */
  H264VENC_apiCreateCount++;
  PRINT(PRINT_MUST, "%s: H264 API Mode Create Count: %u\n", __func__,
                     (unsigned int) H264VENC_apiCreateCount);
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem, TIMM_OSAL_SUSPEND);
  
  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = H264VENC_UseInitialResources(h264VencClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* Data streaming thread arguments */
  strmThreadArg[h264VencClient->instanceNo].h264VencClient = h264VencClient;
#ifdef OMTB_FF_DRIVER_DISPLAY
  strmThreadArg[h264VencClient->instanceNo].dssClient  = dssClient;
#endif
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VencClient->inPortParams.inDataStrmThrdId,
                    (void *)H264VENC_InputDataRead,
                    0,
                    &strmThreadArg[h264VencClient->instanceNo],
                    OMTB_H264_VENC_TSK_STACK_SIZE,
                    OMTB_H264_VENC_GROUP_PRIORITY,
                    (signed char*)"H264_VENC_DATA_IN_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video encode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VencClient->outPortParams.outDataStrmThrdId,
                    (void *)H264VENC_OutputDataWrite,
                    0,
                    &strmThreadArg[h264VencClient->instanceNo],
                    OMTB_H264_VENC_TSK_STACK_SIZE,
                    OMTB_H264_VENC_GROUP_PRIORITY,
                    (signed char*)"H264_VENC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video encode output data write thread\n");
  }

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eofSem, TIMM_OSAL_SUSPEND);

  /* Update the api mode delete count */
  H264VENC_apiDeleteCount++;
  PRINT(PRINT_MUST, "%s: H264 API Mode Delete Count: %u\n", __func__,
                    (unsigned int) H264VENC_apiDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  h264VencClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(
      omtbClass[OMTB_H264_VENC_CLASS].execThreadId[h264VencClient->instanceNo]);
#else
  TIMM_OSAL_ExitTask(
      omtbClass[OMTB_H264_VENC_CLASS].execThreadId[h264VencClient->instanceNo],
      NULL);
#endif
}


/******************************************************************************\
 *      H264VENC_GetHandle Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Get the handle for H264 Video encode component
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

OMX_ERRORTYPE H264VENC_GetHandle(
  OMTB_COMP_HANDLE * classPtr,
  unsigned int instance)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  char inputFile[OMTB_MAX_LEN]  = {"\0"};
  char outputFile[OMTB_MAX_LEN] = {"\0"};
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != classPtr);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instance);

  /* Allocate memory for application private data */
  h264VencClient = (OMTB_H264_VENC_PRIVATE *)
                    TIMM_OSAL_Malloc( sizeof(OMTB_H264_VENC_PRIVATE),
                                      TIMM_OSAL_TRUE,
                                      0,
                                      TIMMOSAL_MEM_SEGMENT_EXT);

  if (!h264VencClient)
  {
    PRINT(PRINT_ERR_MSG, "Error allocating h264VencClient structure \n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    return err;
  }
  else
  {
    /* Update the application instance management structure with app pvt data
       structure pointer */
    classPtr->h264VencClient[instance] = 
                            (OMTB_H264_VENC_PRIVATE *)h264VencClient;
    memset(h264VencClient, 0, sizeof(OMTB_H264_VENC_PRIVATE));
    h264VencClient->instanceNo = instance;
  }

  /* Make local copy of input, output and framesize file names */
  strcpy(outputFile, h264VencConfig->outputFile);
  strcpy(inputFile,  h264VencConfig->inputFile);
  strcpy(h264VencClient->compName,  h264VencConfig->compName);

  PRINT(PRINT_DEBUG_MESSAGE, "Component Name is:%s\n", 
                              h264VencClient->compName);
  PRINT(PRINT_DEBUG_MESSAGE, "Input File is:%s\n", inputFile);
  PRINT(PRINT_DEBUG_MESSAGE, "Output File is:%s\n", outputFile);
  
  /* Data output mode selection - File/Driver */
  h264VencClient->outPortParams.dataOutPutMode = 
                                        h264VencConfig->dataOutPutMode;
  
  /* Buffer allocation mode - OMX_UseBuffer or OMX_AllocateBuffer */
  h264VencClient->inPortParams.flagUseInBuffer   = 
                                        h264VencConfig->flagUseInBuffer;
  h264VencClient->outPortParams.flagUseOutBuffer = 
                                        h264VencConfig->flagUseOutBuffer;

  /* no of i/p and o/p buffers */
  h264VencClient->inPortParams.numInBuf   = h264VencConfig->numInBuf;
  h264VencClient->outPortParams.numOutBuf = h264VencConfig->numOutBuf;
  
  /* Open the input file */
  h264VencClient->inPortParams.fin = fopen(inputFile, "rb");
  if(NULL == h264VencClient->inPortParams.fin)
  {
    PRINT(PRINT_ERR_MSG, "Error in opening input file %s\n",inputFile);
    err = OMTB_INSUFFICIENT_RESOURCES;
    H264VENC_FreeMemory(h264VencClient);
    return err;
  }

  /* Initialize the counter to get frame size from input data read thread */
  h264VencClient->inPortParams.comp.h264Venc.frameCount = 0;
  
  if(OMTB_FILE == h264VencClient->outPortParams.dataOutPutMode )
  {
    /* Open the output file */
    h264VencClient->outPortParams.fout = fopen(outputFile, "wb");
    if(NULL == h264VencClient->outPortParams.fout)
    {
      PRINT(PRINT_ERR_MSG, "Error opening output file %s\n",outputFile);
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VENC_FreeMemory(h264VencClient);
      return err;
    }
  }
  else if(OMTB_DRIVER == h264VencClient->outPortParams.dataOutPutMode)
  {
    /* TBD - Driver interface */
    /* Open the output file */
    h264VencClient->outPortParams.fout = fopen(outputFile, "wb");
    if(NULL == h264VencClient->outPortParams.fout)
    {
      PRINT(PRINT_ERR_MSG, "Error opening output file %s\n",outputFile);
      err = OMTB_INSUFFICIENT_RESOURCES;
      H264VENC_FreeMemory(h264VencClient);
      return err;
    }
  }
  
  /* Initialize app pvt structure params from the template */
  h264VencClient->inPortParams.comp.h264Venc.nWidth       = 
                  h264VencConfig->frameWidth;
  h264VencClient->inPortParams.comp.h264Venc.nHeight      = 
                  h264VencConfig->frameHeight;
  h264VencClient->compressionFormat = OMX_VIDEO_CodingAVC;
  h264VencClient->inPortParams.comp.h264Venc.colorFormat  = 
                  h264VencConfig->colorFormat;

  h264VencClient->profileLevel.eProfile                   = 
                  h264VencConfig->profileLevel.eProfile;
  h264VencClient->profileLevel.eLevel                     = 
                  h264VencConfig->profileLevel.eLevel;

  h264VencClient->inputImgFmt.eContentType                = 
                  h264VencConfig->inputImgFmt.eContentType;
  h264VencClient->inputImgFmt.eInterlaceCodingType        = 
                  h264VencConfig->inputImgFmt.eInterlaceCodingType;

  h264VencClient->avcParams.eLoopFilterMode               = 
                  h264VencConfig->avcParams.eLoopFilterMode;
  h264VencClient->avcParams.bEntropyCodingCABAC           = 
                  h264VencConfig->avcParams.bEntropyCodingCABAC;
  h264VencClient->avcParams.bEnableFMO                    = 
                  h264VencConfig->avcParams.bEnableFMO;
  h264VencClient->avcParams.bconstIpred                   = 
                  h264VencConfig->avcParams.bconstIpred;
  h264VencClient->avcParams.bUseHadamard                  = 
                  h264VencConfig->avcParams.bUseHadamard;
  h264VencClient->avcParams.bEnableASO                    =
                  h264VencConfig->avcParams.bEnableASO;
  h264VencClient->avcParams.bEnableRS                     =
                  h264VencConfig->avcParams.bEnableRS;
  h264VencClient->avcParams.bEnableUEP                    =
                  h264VencConfig->avcParams.bEnableUEP;
  h264VencClient->avcParams.bFrameMBsOnly                 = 
                  h264VencConfig->avcParams.bFrameMBsOnly;
  h264VencClient->avcParams.bMBAFF                        = 
                  h264VencConfig->avcParams.bMBAFF;
  h264VencClient->avcParams.bWeightedPPrediction          =
                  h264VencConfig->avcParams.bWeightedPPrediction;
  h264VencClient->avcParams.bDirect8x8Inference           =
                  h264VencConfig->avcParams.bDirect8x8Inference;
  h264VencClient->avcParams.bDirectSpatialTemporal        =
                  h264VencConfig->avcParams.bDirectSpatialTemporal;
  h264VencClient->avcParams.nSliceHeaderSpacing           = 
                  h264VencConfig->avcParams.nSliceHeaderSpacing;
  h264VencClient->avcParams.nPFrames                      = 
                  h264VencConfig->avcParams.nPFrames;
  h264VencClient->avcParams.nBFrames                      = 
                  h264VencConfig->avcParams.nBFrames;
  h264VencClient->avcParams.nRefFrames                    = 
                  h264VencConfig->avcParams.nRefFrames;
  h264VencClient->avcParams.nRefIdx10ActiveMinus1         = 
                  h264VencConfig->avcParams.nRefIdx10ActiveMinus1;
  h264VencClient->avcParams.nRefIdx11ActiveMinus1         = 
                  h264VencConfig->avcParams.nRefIdx11ActiveMinus1;
  h264VencClient->avcParams.nAllowedPictureTypes          = 
                  h264VencConfig->avcParams.nAllowedPictureTypes;
  h264VencClient->avcParams.nWeightedBipredicitonMode     = 
                  h264VencConfig->avcParams.nWeightedBipredicitonMode;
  h264VencClient->avcParams.nCabacInitIdc                 = 
                  h264VencConfig->avcParams.nCabacInitIdc;
  
  h264VencClient->avcFMO.nNumSliceGroups                  = 
                  h264VencConfig->avcFMO.nNumSliceGroups;
  h264VencClient->avcFMO.nSliceGroupMapType               = 
                  h264VencConfig->avcFMO.nSliceGroupMapType;
  h264VencClient->avcFMO.eSliceMode                       = 
                  h264VencConfig->avcFMO.eSliceMode;
  
  h264VencClient->rateControlAlg                          = 
                  h264VencConfig->rateControlAlg;
  h264VencClient->transformBlockSize                      = 
                  h264VencConfig->transformBlockSize;
  h264VencClient->encoderPreset.eEncodingModePreset       = 
                  h264VencConfig->encoderPreset.eEncodingModePreset;
  h264VencClient->encoderPreset.eRateControlPreset        = 
                  h264VencConfig->encoderPreset.eRateControlPreset;
  h264VencClient->bitStreamFormat                         = 
                  h264VencConfig->bitStreamFormat;
  
  
  /* input buffer size calculation based on frame dimension speculation */
  switch(h264VencClient->inPortParams.comp.h264Venc.colorFormat)
  {
    case OMX_COLOR_FormatYUV420Planar:
    case OMX_COLOR_FormatYUV420SemiPlanar: //Brijesh we use this one
      
      h264VencClient->inPortParams.bufferInSize   = 
                    ((h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 3)
                    >> 1); //Brijesh (ht * wdth * 3/2)

      break;
      
    case OMX_COLOR_FormatYUV422Planar:
    case OMX_COLOR_FormatYCbYCr:
      
      h264VencClient->inPortParams.bufferInSize   = 
                    (h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 2);

      break;

    default:

      PRINT(PRINT_MUST, "Undefined chroma format. Setting i/p buffer size \
according to 420P format.\n");
      h264VencClient->inPortParams.bufferInSize   = 
                    ((h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight * 3)
                    >> 1);

      break;
  }

  // Brijesh commented h264VencClient->outPortParams.bufferOutSize = OMTB_H264_VENC_MAX_OUTBUF_SIZE;
  //and added below where output size = (ht * wdth)
  h264VencClient->outPortParams.bufferOutSize = (h264VencClient->inPortParams.comp.h264Venc.nWidth *
                    h264VencClient->inPortParams.comp.h264Venc.nHeight);//Brijesh added
  
  /*Function to allocate memory for app pvt struct members*/
  err = H264VENC_AllocateMemory(h264VencClient);

  if(OMTB_SUCCESS != err)
  {
    PRINT(PRINT_ERR_MSG, "Unable to allocate memory for app pvt structure\n");
    err = OMTB_INSUFFICIENT_RESOURCES;
    H264VENC_FreeMemory(h264VencClient);
    return err;
  }
  
  /* Initialize the semaphores */
  TIMM_OSAL_SemaphoreCreate(&h264VencClient->eventSem, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VencClient->eofSem, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VencClient->inPortParams.inputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VencClient->outPortParams.outputDataReady, 0);
  TIMM_OSAL_SemaphoreCreate(&h264VencClient->inPortParams.inputDataReadThrExit,
                            0);
  TIMM_OSAL_SemaphoreCreate(
            &h264VencClient->outPortParams.outputDataWriteThrExit,
            0);
  
  /*Assign the callbacks*/
  *h264VencClient->cb = H264VENC_Callback;

  /* Initialize the EOS flag */
  h264VencClient->outPortParams.flagEos = OMX_FALSE;
  h264VencClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Initialize the flag used to determine whether Func/API thread is created */
  h264VencClient->flagThrCreated = OMX_FALSE;
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* get component handle */
  err = OMX_GetHandle(&h264VencClient->handle,
                      (OMX_STRING)h264VencClient->compName,
                      h264VencClient,
                      h264VencClient->cb);
  
  /* API exit time calculation macro */
  OMTB_OMX_API_EXIT_TIME_CALC("OMX_GetHandle");
  
  if(OMX_ErrorNone != err)
  {
    PRINT(PRINT_ERR_MSG, "No component named \"%s\" found\n",
                          h264VencClient->compName);
    OMTB_OmxReturnStatus(err);
    H264VENC_FreeMemory(h264VencClient);
    return err;
  }
  else
  {
    /* Update the application instance management strucutre with handle */
    classPtr->handle[instance] = h264VencClient->handle;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-h264Venc Instance #: %d\n\n", instance);
    PRINT(PRINT_SIMPLE_MESSAGE, "Component Handle Available\n");
  }
  
  PRINT(PRINT_DEBUG_MESSAGE, "\nbufferOutSize: %d\n",\
        (int)h264VencClient->outPortParams.bufferOutSize);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VENC_GetStructIdx Function Definition
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

OmtbRet H264VENC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct)
{
  unsigned int enumCount = 0;
  
  /* Use the specified template structure */
  h264VencConfig = (OMTB_H264_VENC_CONFIG_PARAMS*)
                     &(omtbCfgTemplate[template]->h264VencTemplate);

  /* Get the corresponding string for enum values from enum table 
     Set structure index
     Set structure pointer
     Set port index for which parameter to be updated 
     Set structure header info */
  while(strcmp(enumTblH264VencCfgParamIdx[enumCount].name , ""))
  {
    if(0 == strcmp(enumTblH264VencCfgParamIdx[enumCount].name,
                   indexStr))
    {
      switch(enumTblH264VencCfgParamIdx[enumCount].value)
      {
        case OMX_IndexConfigVideoFramerate:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->frameRate;
            h264VencConfig->frameRate.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_CONFIG_FRAMERATETYPE));
          }
          return OMTB_SUCCESS;

        case OMX_IndexParamVideoBitrate:
        case OMX_IndexConfigVideoBitrate:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->bitRate;
            h264VencConfig->bitRate.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_BITRATETYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexConfigVideoMESearchRange:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->meSearchrange;
            h264VencConfig->meSearchrange.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_MESEARCHRANGETYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_IndexConfigVideoIntraVOPRefresh:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->forceFrame;
            h264VencConfig->forceFrame.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_CONFIG_INTRAREFRESHVOPTYPE));
          }
          return OMTB_SUCCESS;
        
        case OMX_TI_IndexConfigVideoQPSettings:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->qpSettings;
            h264VencConfig->qpSettings.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_QPSETTINGSTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_IndexConfigVideoAVCIntraPeriod:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->avcIntraPeriod;
            h264VencConfig->avcIntraPeriod.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_AVCINTRAPERIOD));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexConfigSliceSettings:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->sliceSettings;
            h264VencConfig->sliceSettings.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_SLICECODINGTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexConfigVideoPixelInfo:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->pixelInfo;
            h264VencConfig->pixelInfo.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_CONFIG_PIXELINFOTYPE));
          }
          return OMTB_SUCCESS;


        case OMX_TI_IndexParamVideoNALUsettings:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->naluSettings;
            h264VencConfig->naluSettings.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexParamVideoAdvancedFMO:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->advancedFMO;
            h264VencConfig->advancedFMO.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_AVCADVANCEDFMOTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexParamVideoMEBlockSize:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->meBlockSize;
            h264VencConfig->meBlockSize.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_MEBLOCKSIZETYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_IndexParamVideoIntraRefresh:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->intraRefresh;
            h264VencConfig->intraRefresh.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_INTRAREFRESHTYPE));
          }
          return OMTB_SUCCESS;
        
        case OMX_TI_IndexParamVideoVUIsettings:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->vuiSettings;
            h264VencConfig->vuiSettings.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_VUIINFOTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexParamVideoIntraPredictionSettings:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->intraPred;
            h264VencConfig->intraPred.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_INTRAPREDTYPE));
          }
          return OMTB_SUCCESS;
          
        case OMX_TI_IndexParamVideoDataSyncMode:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->vidDataMode;
            h264VencConfig->vidDataMode.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_DATASYNCMODETYPE));
          }
          return OMTB_SUCCESS;

        case OMX_IndexParamVideoAvc:
          {
            *nConfigIndex   = enumTblH264VencCfgParamIdx[enumCount].value;
            *pCompCfgStruct = &h264VencConfig->avcParams;
            h264VencConfig->avcParams.nPortIndex = portIdx;
            OMTB_SetHeader( *pCompCfgStruct,
                            sizeof(OMX_VIDEO_PARAM_AVCTYPE));
          }
          return OMTB_SUCCESS;
          
        default:
          {
            return OMTB_FAIL;
          }

      } // switch(enumTblH264VencCfgParamIdx[enumCount].value)
     
    } // if(0 == strcmp(enumTblH264VencCfgParamIdx[enumCount].name, ...
      
    enumCount++;
  }

  return OMTB_SUCCESS;  
}


/******************************************************************************\
 *      H264VENC_SetParameter Function Definition
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

OMX_ERRORTYPE H264VENC_SetParameter(
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
  err = H264VENC_GetStructIdx(indexStr, 
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
 *      H264VENC_SetConfig Function Definition
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

OMX_ERRORTYPE H264VENC_SetConfig(
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
  err = H264VENC_GetStructIdx(indexStr, 
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
 *      H264VENC_GetParameter Function Definition
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

OMX_ERRORTYPE H264VENC_GetParameter(
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
  err = H264VENC_GetStructIdx(indexStr, 
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
 *      H264VENC_GetConfig Function Definition
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

OMX_ERRORTYPE H264VENC_GetConfig(
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
  err = H264VENC_GetStructIdx(indexStr, 
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
 *      H264VENC_FillData Function Definition
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *
 *            pBuf - Pointer to buffer to be filled
 *
 *  @param Out:
 *            OMX_BUFFERHEADERTYPE * - Pointer of I/P filled Buffer
 *  @return   No of bytes read
 *
 */

unsigned int H264VENC_FillData(OMTB_H264_VENC_PRIVATE* pAppData,
                              OMX_BUFFERHEADERTYPE *pBuf)
{
  unsigned int dataRead = 0;

#ifdef TILER //OMX_H264E_TILERTEST	//Brijesh
	  OMX_ERRORTYPE err = OMX_ErrorNone;
	  OMX_U8 *pTmpBuffer = NULL;
	  OMX_U32 size1D = 0;
	  OMX_U32 framesizetoread = 0;
	  OMX_U32 Addoffset=0;	  
#endif
	
  

  /* validate the function arguments */
  ASSERT(NULL != pAppData);
  ASSERT(NULL != pBuf);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)pAppData->instanceNo);

#ifdef TILER // OMX_H264E_TILERTEST	//Brijesh: Used for 1D buffers which are read from input file.
	framesizetoread = pAppData->inPortParams.comp.h264Venc.nWidth * pAppData->inPortParams.comp.h264Venc.nHeight * 3  / 2;
	size1D=framesizetoread;
	pTmpBuffer = (OMX_U8 *)TIMM_OSAL_Malloc(framesizetoread,OMX_FALSE,0,0);
	// Brijesh: TODO: need to pTmpBuffer FREE at the end??? TODO for THC
#endif 
  
  if(!feof(pAppData->inPortParams.fin))
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Entering Function:%s\n",__func__);
    
    PRINT(PRINT_DEBUG_MESSAGE, "nAllocLen is %d\n",
                                (int) pBuf->nAllocLen);
    PRINT(PRINT_DEBUG_MESSAGE, "nFilledLen is: %d\n",
                                (int) pBuf->nFilledLen);

#ifdef TILER //OMX_H264E_TILERTEST  //Brijesh 
     /* Read from I/P file and Fill data in i/p Buffer */
    dataRead = fread( pTmpBuffer, //pBuf->pBuffer, 
                      sizeof(char), 
                      framesizetoread, // pAppData->inPortParams.bufferInSize,
                      pAppData->inPortParams.fin);

	//Brijesh TODO: Should do a error check for end of file OR no bytes read or just plain error.   Pending on OMTB
	
    PRINT(PRINT_DEBUG_MESSAGE, "Bytes read are:%d\n", dataRead);

	// Brijesh: Convert the 1D buffer from read file to 2D buffer for OMX component consumption
	err = OMXH264_Util_Memcpy_1Dto2D_ENC(
		 pBuf->pBuffer, 
		 pTmpBuffer, 
		 size1D, //h264VencClient->inPortParams.bufferInSize, //Brijesh TODO: cross check it is constant
		 pAppData->inPortParams.comp.h264Venc.nHeight, 
		 pAppData->inPortParams.comp.h264Venc.nWidth, 
		 STRIDE_8BIT); 
	 if(OMX_ErrorNone != err)
	 {
	   OMTB_OmxReturnStatus(err);
	   return err;
	 }
	
	 // Error check if data read from file is less than 1 frame data size: TODO maybe we might not want to do this since 
	 // we are solving cases which the OMX component should take care of. TODO : Action item on THC to sort this out.
	 // May be we should comment this below code.
	 // TODO : bug in THC code : below if condition should contain "dataRead" check and not "framesizetoread" check
	 if(dataRead/*framesizetoread*/< (pAppData->inPortParams.comp.h264Venc.nHeight * pAppData->inPortParams.comp.h264Venc.nWidth))
		   pTmpBuffer += size1D; //should be end of file and not pointer movement TODO: for THC and OMTB
		else{
		   pTmpBuffer += (pAppData->inPortParams.comp.h264Venc.nHeight * pAppData->inPortParams.comp.h264Venc.nWidth);
		   size1D-=(pAppData->inPortParams.comp.h264Venc.nHeight	* pAppData->inPortParams.comp.h264Venc.nWidth);
		}
	
		// Is there one more copy of 1D to 2D needed as per THC in OMTB : Brijesh
		// I think the above one is for Y and one more below should be for UV data.: Brijesh
#ifdef TILER //OMX_H264E_LINUX_TILERTEST
		Addoffset=(pAppData->inPortParams.comp.h264Venc.nHeight * STRIDE_8BIT);
		err = OMXH264_Util_Memcpy_1Dto2D_ENC(
				(pBuf->pBuffer + Addoffset), 
				pTmpBuffer, 
				size1D, 
				(pAppData->inPortParams.comp.h264Venc.nHeight/2), 
				pAppData->inPortParams.comp.h264Venc.nWidth, 
				STRIDE_16BIT); 
		if(OMX_ErrorNone != err)
		{
		  OMTB_OmxReturnStatus(err);
		  return err;
		}
#endif
		
#endif //Brijesh
	

    /* Update the buffer header with buffer filled length and alloc length */
    pBuf->nFilledLen = dataRead;
    pBuf->nAllocLen = pAppData->inPortParams.bufferInSize; //Brijesh should be the same as above

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
	
	// Brijesh: TODO: need to pTmpBuffer FREE at the end??? For THC: Must be done here
	// If nto done at the end of the function: Brijesh
  }
  
  TIMM_OSAL_Free(pTmpBuffer);

  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)pAppData->instanceNo);

  // Brijesh: TODO: need to pTmpBuffer FREE at the end??? For THC
  
  return dataRead;
}


/******************************************************************************\
 *      H264VENC_FuncTsk Thread Definition
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

void H264VENC_FuncTsk(void *threadsArg)
{
  int threadRes = TIMM_OSAL_ERR_NONE;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  OMTB_DATA_THREAD_ARG *funcDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != funcDataThreadArg->h264VencClient);
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != funcDataThreadArg->dssClient);
  dssClient  = funcDataThreadArg->dssClient;
#endif
  h264VencClient = funcDataThreadArg->h264VencClient;
  

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);
  
  PRINT(PRINT_SIMPLE_MESSAGE, "\nbufferOutSize:%d\n",\
        (int)h264VencClient->outPortParams.bufferOutSize);
  
  /* Set the port params */
  err = H264VENC_SetPortDefinition(h264VencClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

	err = H264ENC_SetAllParams(h264VencClient);
	if(OMX_ErrorNone != err)
	{
  	OMTB_OmxReturnStatus(err);
	}  

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to idle state */
  err = OMX_SendCommand(h264VencClient->handle,
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
  err = H264VENC_AllocateResources(h264VencClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;
  
  /* send command to component to go to executing state */
  err = OMX_SendCommand(h264VencClient->handle,
                        OMX_CommandStateSet,
                        OMX_StateExecuting,
                        NULL);
  
  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }
  
  /* waiting for component to go to executing state */
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem, TIMM_OSAL_SUSPEND);

  /* Update the func create count */
  H264VENC_funcCreateCount++;
  PRINT(PRINT_MUST, "%s: H264 FUNC Create Count: %u\n", __func__,
                    (unsigned int) H264VENC_funcCreateCount);

  /* Use the initial I/O buffers and make empty/fill this buffer calls */
  err = H264VENC_UseInitialResources(h264VencClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  /* Update the data streaming thread arguments */
  strmThreadArg[h264VencClient->instanceNo].h264VencClient = h264VencClient;
#ifdef OMTB_FF_DRIVER_DISPLAY
  strmThreadArg[h264VencClient->instanceNo].dssClient  = dssClient;
#endif
  
  
  /* Create input data read thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VencClient->inPortParams.inDataStrmThrdId,
                    (void *)H264VENC_InputDataRead,
                    0,
                    &strmThreadArg[h264VencClient->instanceNo],
                    OMTB_H264_VENC_TSK_STACK_SIZE,
                    OMTB_H264_VENC_GROUP_PRIORITY,
                    (signed char*)"H264_VENC_DATA_IN_TSK");
    
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video encode input data read thread\n");
  }

  /* Create output data write thread */
  threadRes = TIMM_OSAL_CreateTask(
                    (void *)&h264VencClient->outPortParams.outDataStrmThrdId,
                    (void *)H264VENC_OutputDataWrite,
                    0,
                    &strmThreadArg[h264VencClient->instanceNo],
                    OMTB_H264_VENC_TSK_STACK_SIZE,
                    OMTB_H264_VENC_GROUP_PRIORITY,
                    (signed char*)"H264_VENC_DATA_OUT_TSK");
  
  if(TIMM_OSAL_ERR_NONE != threadRes)
  {
    PRINT(PRINT_ERR_MSG,
          "Unable to start H264 video encode output data write thread\n");
  }
  

  /* Wait for EOS */
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eofSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "The execution of the video encoding\
                           process is terminated\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* state change of component from executing to idle */
  err = OMX_SendCommand(h264VencClient->handle,
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
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem, TIMM_OSAL_SUSPEND);

  PRINT(DEFAULT_MESSAGES, "State transitioned to Idle\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* sending command to component to go to loaded state */
  err = OMX_SendCommand(h264VencClient->handle,
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
  h264VencClient->outPortParams.flagEos = OMX_FALSE;
  h264VencClient->inPortParams.flagInputEos = OMX_FALSE;

  /* Function to reelase the resources allocated by component */
  err = H264VENC_FreeResources(h264VencClient);

  if(OMX_ErrorNone != err)
  {
    OMTB_OmxReturnStatus(err);
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "State transitioned to loaded\n");

  /* API entry time calculation macro */
  OMTB_OMX_API_ENTRY_TIME_CALC;

  /* Delete the component instance */
  err = OMX_FreeHandle(h264VencClient->handle);

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
    h264VencClient->handle = NULL;
    PRINT(PRINT_MUST,
          "\r      \n\nOMTB-Deleted h264Venc Instance #: %d\n\n", 
          (int) h264VencClient->instanceNo);
    omtbClass[OMTB_H264_VENC_CLASS].handle[h264VencClient->instanceNo] = NULL;
  }

  PRINT(PRINT_SIMPLE_MESSAGE, "video encoder freed\n");

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
  H264VENC_FreeMemory(h264VencClient);

  /* Update the func delete count */
  H264VENC_funcDeleteCount++;
  PRINT(PRINT_MUST, "%s: H264 FUNC Delete Count: %u\n", __func__,
                    (unsigned int) H264VENC_funcDeleteCount);

  /* reset the flag used to determine whether Func/API thread is created */
  h264VencClient->flagThrCreated = OMX_FALSE;
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(
    omtbClass[OMTB_H264_VENC_CLASS].execThreadId[h264VencClient->instanceNo]);
#else
   TIMM_OSAL_ExitTask(
      omtbClass[OMTB_H264_VENC_CLASS].execThreadId[h264VencClient->instanceNo],
      NULL);
#endif
}


/******************************************************************************\
 *      H264VENC_FreeResources Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            h264VencClient: Pointer to OMTB_H264_VENC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VENC_FreeResources(OMTB_H264_VENC_PRIVATE *h264VencClient)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  /* Ask component to Free input port buffers */
  
  for(i = 0; i < h264VencClient->inPortParams.pInPortDef->nBufferCountActual ;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video enc I/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",\
          (unsigned int) h264VencClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n", \
          (unsigned int) h264VencClient->inPortParams.pInPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",\
          (unsigned int) h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == h264VencClient->inPortParams.flagUseInBuffer)
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST //Brijesh
	  /*Free the TilerBuffer*/
	  MemMgr_Free(h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer);   
		
#else //Brijesh 
    
      TIMM_OSAL_CMEM_Free (
              h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]->pBuffer, 
              h264VencClient->inPortParams.pInPortDef->nBufferSize);
#endif //Brijesh
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;
  
    /* freeing buffers of component input ports */
    err = OMX_FreeBuffer( h264VencClient->handle,
                        h264VencClient->inPortParams.pInPortDef->nPortIndex,
                        h264VencClient->inPortParams.comp.h264Venc.pInBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
    
  } // for(i = 0; i < h264VencClient->inPortParams.pInPortDef
  
  /* Ask component to Free output port buffers */  
  for(i = 0; i < h264VencClient->outPortParams.pOutPortDef->nBufferCountActual;
      i++)
  {
    PRINT(PRINT_DEBUG_MESSAGE, "Free Video enc O/P ports\n");
    
    PRINT(PRINT_DEBUG_MESSAGE, "Value of Handle is %x\n",
          (unsigned int) h264VencClient->handle);
    PRINT(PRINT_DEBUG_MESSAGE, "Port Index is %x\n",
          (unsigned int) h264VencClient->outPortParams.pOutPortDef->nPortIndex);
    PRINT(PRINT_DEBUG_MESSAGE, "Buff addr is  %x\n",
          (unsigned int) h264VencClient->outPortParams.comp.h264Venc.pOutBuff);
  
    /* Release the memory allocated for buffer in case of OMX_UseBuffer */
    if(OMX_TRUE == h264VencClient->outPortParams.flagUseOutBuffer)
    {
#ifdef TILER //OMX_H264E_LINUX_TILERTEST
	  /*Free the TilerBuffer*/
	  MemMgr_Free(h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i]->pBuffer);
#else //Brijesh              
    
      TIMM_OSAL_CMEM_Free (
              h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i]->pBuffer, 
              h264VencClient->outPortParams.pOutPortDef->nBufferSize);
#endif //Brijesh
    }
    
    /* API entry time calculation macro */
    OMTB_OMX_API_ENTRY_TIME_CALC;

    /* freeing buffers of component output ports */
    err = OMX_FreeBuffer( h264VencClient->handle,
                      h264VencClient->outPortParams.pOutPortDef->nPortIndex,
                      h264VencClient->outPortParams.comp.h264Venc.pOutBuff[i]);
  
    /* API exit time calculation macro */
    OMTB_OMX_API_EXIT_TIME_CALC("OMX_FreeBuffer");
  
    if(OMX_ErrorNone != err)
    {
      OMTB_OmxReturnStatus(err);
      return err;
    }
  
  } // for(i = 0; i < h264VencClient->outPortParams.pOutPortDef ...
  
  /*wait for event handler to report completion of loaded command */
  TIMM_OSAL_SemaphoreObtain(h264VencClient->eventSem, TIMM_OSAL_SUSPEND);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

  return err;
}


/******************************************************************************\
 *      H264VENC_FreeMemory Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            h264VencClient: Pointer to OMTB_H264_VENC_PRIVATE struct
 *
 *  @return   None
 *
 */

void H264VENC_FreeMemory(OMTB_H264_VENC_PRIVATE *h264VencClient)
{
  int instanceNo = h264VencClient->instanceNo;

  /* validate the function arguments */
  ASSERT(NULL != h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Entering <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);
  
  /* Release callback structure memory */
  if(NULL != h264VencClient->cb)
    TIMM_OSAL_Free(h264VencClient->cb);
  
  /* Release input port memory */
  if(NULL != h264VencClient->inPortParams.pInPortDef)
    TIMM_OSAL_Free(h264VencClient->inPortParams.pInPortDef);
  
  /* Release output port memory */
  if(NULL != h264VencClient->outPortParams.pOutPortDef)
    TIMM_OSAL_Free(h264VencClient->outPortParams.pOutPortDef);
  
  /* Release component event handling semaphore memory */
  if(NULL != h264VencClient->eventSem)
    TIMM_OSAL_SemaphoreDelete(h264VencClient->eventSem);
  
  /* Release EOF handling semaphore memory */
  if(NULL != h264VencClient->eofSem)
    TIMM_OSAL_SemaphoreDelete(h264VencClient->eofSem);
  
  /* Release input data read thread control semaphore memory */
  if(NULL != h264VencClient->inPortParams.inputDataReady)
    TIMM_OSAL_SemaphoreDelete(h264VencClient->inPortParams.inputDataReady);
  
  /* Release output data write thread control semaphore memory */
  if(NULL != h264VencClient->outPortParams.outputDataReady)
    TIMM_OSAL_SemaphoreDelete(h264VencClient->outPortParams.outputDataReady);

  /* Release input data read thread exit semaphore memory */
  if(NULL != h264VencClient->inPortParams.inputDataReadThrExit)
    TIMM_OSAL_SemaphoreDelete(
              h264VencClient->inPortParams.inputDataReadThrExit);
  
  /* Release output data write thread exit semaphore memory */
  if(NULL != h264VencClient->outPortParams.outputDataWriteThrExit)
    TIMM_OSAL_SemaphoreDelete(
              h264VencClient->outPortParams.outputDataWriteThrExit);
  
  if(OMTB_FILE == h264VencClient->outPortParams.dataOutPutMode)
  {
    /* closing the output file */
    if(NULL != h264VencClient->outPortParams.fout)
      fclose(h264VencClient->outPortParams.fout);
  }
  else if(OMTB_DRIVER == h264VencClient->outPortParams.dataOutPutMode)
  {
    /* Driver interface is TBD. So currently using File mode for driver also */
    /* closing the output file */
    if(NULL != h264VencClient->outPortParams.fout)
      fclose(h264VencClient->outPortParams.fout);
  }
  
  /* closing the input file */
  if(NULL != h264VencClient->inPortParams.fin)
    fclose(h264VencClient->inPortParams.fin);
  
  /* Release component specific memory */
  if(NULL != h264VencClient->pH264)
    TIMM_OSAL_Free(h264VencClient->pH264);
  
  /* Release app pvt data private structure */
  if(NULL != h264VencClient)
    TIMM_OSAL_Free(h264VencClient);
  
  PRINT(PRINT_FUNCTION_NAME, "Leaving <Function , instanc #> : <%s , %d>\n",
                              __func__, (int)instanceNo);
}


/******************************************************************************\
 *      H264VENC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the h264 video encoder component
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

OMX_ERRORTYPE H264VENC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;

  /* thread argument validation */
  ASSERT(NULL != hComponent);
  ASSERT(NULL != pAppData);
  
  h264VencClient = pAppData;

  PRINT(PRINT_FUNCTION_NAME, "Entering <Callback , instanc #> : <%s , %d>\n",
                              __func__, (int)h264VencClient->instanceNo);

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
          TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
        }
        /* port enable or disable command related event */
        else if ( OMX_CommandPortEnable   == Data1  || 
                  OMX_CommandPortDisable  == Data1  )
        {
          PRINT(PRINT_SIMPLE_MESSAGE,
                "In %s: OMX_CommandPortEnable/OMX_CommandPortDisable\n",
                __func__);

          /* Post the component event sem */
          TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
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
          //TIMM_OSAL_SemaphoreRelease(h264VencClient->eofSem);
        }
      }
      break;
    
    case OMX_EventError:  /* Event error */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventErrorBr\n", __func__);
		
        PRINT(PRINT_DEBUG_MESSAGE, "\nError1 is %d \n",(OMX_ERRORTYPE)Data1);
        PRINT(PRINT_DEBUG_MESSAGE, "\nError1 is %x \n",(OMX_ERRORTYPE)Data1);		

		PRINT(PRINT_DEBUG_MESSAGE, "\nError2 is %d \n",(OMX_ERRORTYPE)Data2);		
        PRINT(PRINT_DEBUG_MESSAGE, "\nError2 is %x \n",(OMX_ERRORTYPE)Data2);				
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
      }
      break;
    
    case OMX_EventMark: /* Event mark buffer */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventMark\n", __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
      }
      break;
    
    case OMX_EventResourcesAcquired:  /* Event resources acquired */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventResourcesAcquired\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
      }
      break;
    
    case OMX_EventComponentResumed: /* Event component resumed */
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: OMX_EventComponentResumed\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
      }
      break;
    
    case OMX_EventDynamicResourcesAvailable:// Event dynamic resources available
      {
        PRINT(PRINT_SIMPLE_MESSAGE, 
              "In %s: OMX_EventDynamicResourcesAvailable\n",
              __func__);
        
        /* Post the component event sem */
        TIMM_OSAL_SemaphoreRelease(h264VencClient->eventSem);
      }
      break;
    
    case OMX_EventMax:   // Event not implemented
      {
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param1 is %i\n",
              __func__, (int)Data1);
        PRINT(PRINT_SIMPLE_MESSAGE, "In %s: Param2 is %i\n",
              __func__, (int)Data2);
        err = OMX_ErrorNotImplemented;
      }
      break;

    default:  // Event Bad Parameter
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
                              __func__, (int)h264VencClient->instanceNo);
  
  return err;
}


/******************************************************************************\
 *      H264VENC_InputDataRead Thread Definition
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

void H264VENC_InputDataRead(void *threadsArg)
{
  unsigned int actualSize = 0,dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
  unsigned int numRemainingIn = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->h264VencClient);
    
  h264VencClient = strmDataThreadArg->h264VencClient;

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == h264VencClient->inPortParams.flagInputEos)
  {
  
    /* waiting for empty input buffer via callback */
    TIMM_OSAL_SemaphoreObtain(h264VencClient->inPortParams.inputDataReady, 
                              TIMM_OSAL_SUSPEND);

    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (h264VencClient->inPortParams.ipBufPipe,
                                        (void*) &numRemainingIn);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Empty This buffer:%d\n",\
          (int) numRemainingIn);

    while(numRemainingIn)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering EmptyThisBuffer Loop\n");
    
      /* Read empty buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe (h264VencClient->inPortParams.ipBufPipe,
                              &pBufferIn,
                              sizeof(pBufferIn),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );

      /* Fill the data in the empty buffer */
      dataRead = H264VENC_FillData(
                            h264VencClient,
                            pBufferIn);

      /* Exit the loop if no data available */
      if(0 >= dataRead)
      {
          PRINT(PRINT_ERR_MSG, "No dataRead\n");
          h264VencClient->inPortParams.flagInputEos = OMX_TRUE;
          break;
      }
      else
      {
        /* increase the frame counter if some data is available */
        h264VencClient->inPortParams.comp.h264Venc.frameCount++;
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
      err = OMX_GetState(h264VencClient->handle, &pState);

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
             to component and will be read again in H264VENC_UseInitialResources
             on Func/API mode thread creation */
          if(0 == h264VencClient->inPortParams.comp.h264Venc.frameCount)
            h264VencClient->inPortParams.comp.h264Venc.frameCount = 0;
          else
            h264VencClient->inPortParams.comp.h264Venc.frameCount--;
          
          fseek(h264VencClient->inPortParams.fin, -dataRead, SEEK_CUR);

          continue;
        }
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Pass the input buffer to the component */
      err = OMX_EmptyThisBuffer(h264VencClient->handle, pBufferIn);
    
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
      h264VencClient->inPortParams.flagInputEos = OMX_TRUE;
      h264VencClient->outPortParams.flagEos = OMX_TRUE;
    }
  
  }
  
  PRINT(DEFAULT_MESSAGES, "The execution of the video encoding\
                           input data read is terminated\n");

  /* post input buffer read thread exit */
  TIMM_OSAL_SemaphoreRelease(h264VencClient->inPortParams.inputDataReadThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(h264VencClient->inPortParams.inDataStrmThrdId);
#else
  TIMM_OSAL_ExitTask(h264VencClient->inPortParams.inDataStrmThrdId, NULL);
#endif
}


/******************************************************************************\
 *      H264VENC_OutputDataWrite Thread Definition
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

void H264VENC_OutputDataWrite(void *threadsArg)
{
  unsigned int actualSize = 0,framesEncoded = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMTB_H264_VENC_PRIVATE* h264VencClient = NULL;
  OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
  unsigned int numRemainingOut = 0;
  OMTB_DATA_THREAD_ARG *strmDataThreadArg = (OMTB_DATA_THREAD_ARG *)threadsArg;
#ifdef OMTB_FF_DRIVER_DISPLAY
  OmtbDispInfo *dssClient = NULL;
#endif
  OMX_STATETYPE pState = OMX_StateInvalid;
  OMTB_OMX_API_TIMING_VAR_DECL;

  /* thread argument validation */
  ASSERT(NULL != threadsArg);
  ASSERT(NULL != strmDataThreadArg->h264VencClient);
#ifdef OMTB_FF_DRIVER_DISPLAY
  ASSERT(NULL != strmDataThreadArg->dssClient);
  dssClient  = strmDataThreadArg->dssClient;
#endif
  h264VencClient = strmDataThreadArg->h264VencClient;
  

  PRINT(PRINT_MUST, "Entering <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);
  
  while(OMX_ErrorNone == err && 
        OMX_FALSE     == h264VencClient->outPortParams.flagEos)
  {
    /* waiting for filled output buffer via callback */
    TIMM_OSAL_SemaphoreObtain(h264VencClient->outPortParams.outputDataReady, 
                              TIMM_OSAL_SUSPEND);
    
    /* Read the number of buffers available in pipe */
    TIMM_OSAL_GetPipeReadyMessageCount (h264VencClient->outPortParams.opBufPipe,
                                        (void*) &numRemainingOut);
    
    PRINT(PRINT_DEBUG_MESSAGE, "Num remaining in Fill This buffer: %d\n",\
          (int) numRemainingOut);
    
    while(numRemainingOut)
    {
      PRINT(PRINT_DEBUG_MESSAGE,"Entering FillThisBuffer Loop\n");
    
      /* Read filled buffer pointer from the pipe */
      TIMM_OSAL_ReadFromPipe( h264VencClient->outPortParams.opBufPipe,
                              &pBufferOut,
                              sizeof(pBufferOut),
                              (void*) &actualSize,
                              TIMM_OSAL_SUSPEND );
      
      if(OMTB_FILE == h264VencClient->outPortParams.dataOutPutMode)
      {
        /* write data to output file */ //Brijesh TODO: buffer offset is TODO for OMTB
        fwrite( (pBufferOut->pBuffer + pBufferOut->nOffset), //Brijesh added new was earlier = pBufferOut->pBuffer,
                sizeof(char),
                pBufferOut->nFilledLen,
                h264VencClient->outPortParams.fout);
        
        PRINT(PRINT_SIMPLE_MESSAGE, "Frame:%d encoded\n",++framesEncoded );
        fflush (h264VencClient->outPortParams.fout);
      }
      else if(OMTB_DRIVER == h264VencClient->outPortParams.dataOutPutMode)
      {
        /* Get empty buffer from the driver */
        fwrite( pBufferOut->pBuffer,
                sizeof(char),
                pBufferOut->nFilledLen,
                h264VencClient->outPortParams.fout);
        
        PRINT(PRINT_SIMPLE_MESSAGE, "Frame:%d encoded\n",++framesEncoded );
        fflush (h264VencClient->outPortParams.fout);
        PRINT(PRINT_MUST,"Writing to file as driver interface not supported\n");
      }

      /* reset the buffer filled length */
      pBufferOut->nFilledLen = 0;

      /* Exit the loop if EOS is reached */
      if(OMX_BUFFERFLAG_EOS == pBufferOut->nFlags)
      {
      
        PRINT(PRINT_MUST, "EOS reached ...\n");
        h264VencClient->outPortParams.flagEos = OMX_TRUE;
        break;
      }

      /* API entry time calculation macro */
      OMTB_OMX_API_ENTRY_TIME_CALC;

      /* Get the current state of the component */
      err = OMX_GetState(h264VencClient->handle, &pState);

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
      err = OMX_FillThisBuffer(h264VencClient->handle, pBufferOut);
      
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
  
  PRINT(DEFAULT_MESSAGES, "The execution of the video encoding\
                           output data write is terminated\n");

  /* Post the EOS reached semaphore */
  TIMM_OSAL_SemaphoreRelease(h264VencClient->eofSem);

  /* waiting for output buffer write thread exit */
  TIMM_OSAL_SemaphoreRelease(
            h264VencClient->outPortParams.outputDataWriteThrExit);
  
  PRINT(PRINT_MUST, "Leaving <Thread , instance #> : <%s , %d>\n",
                     __func__, (int)h264VencClient->instanceNo);

  /* exit the thread */
#ifdef WTSD_TEMP
  TIMM_OSAL_DeleteTask(h264VencClient->outPortParams.outDataStrmThrdId);
#else
  TIMM_OSAL_ExitTask(h264VencClient->outPortParams.outDataStrmThrdId, NULL);
#endif
}


/******************************************************************************\
 *      H264VENC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the h264 video encoder component
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

OMX_ERRORTYPE H264VENC_EmptyBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_H264_VENC_PRIVATE* pAppData = ptrAppData;
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
  //  OMTB_OMX_API_EXIT_TIME_CALC("H264VENC_EmptyBufferDone");
  
  return err;
}


/******************************************************************************\
 *      H264VENC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the h264 video encoder component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE H264VENC_FillBufferDone(
  OMX_HANDLETYPE hComponent,
  OMX_PTR ptrAppData,
  OMX_BUFFERHEADERTYPE* pBuffer)
{

  OMTB_H264_VENC_PRIVATE* pAppData = ptrAppData;
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
  //  OMTB_OMX_API_EXIT_TIME_CALC("H264VENC_FillBufferDone");
  
  return err;
}

//TODO: temporary to sort the compilation error Brijesh
#ifdef TILER
OMX_ERRORTYPE OMXH264_Util_Memcpy_1Dto2D_ENC(
	OMX_PTR pDst2D, 
	OMX_PTR pSrc1D, 
	OMX_U32 nSize1D, 
	OMX_U32 nHeight2D, 
	OMX_U32 nWidth2D, 
	OMX_U32 nStride2D)

{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U32 retval = TIMM_OSAL_ERR_UNKNOWN;
	OMX_U8 *pInBuffer;
	OMX_U8 *pOutBuffer;
	OMX_U32 nSizeLeft,i;

	nSizeLeft = nSize1D;
	pInBuffer = (OMX_U8 *)pSrc1D;
	pOutBuffer = (OMX_U8 *)pDst2D;
	//The lower limit is copied. If nSize1D < H*W then 1Dsize is copied else H*W is copied
	for(i = 0; i < nHeight2D; i++)
	{
	    if(nSizeLeft >= nWidth2D)
	    {
			retval=TIMM_OSAL_Memcpy(pOutBuffer, pInBuffer, nWidth2D);
			//Brijesh : GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
	    }
	    else
	    {
			retval=TIMM_OSAL_Memcpy(pOutBuffer, pInBuffer, nSizeLeft);
			//Brijesh : GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
	        break;
	    }
	    nSizeLeft -= nWidth2D;
	    pInBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pInBuffer + nWidth2D);
	    pOutBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pOutBuffer + nStride2D);
	}

//EXIT:
//	Brijesh : 	TIMM_OSAL_Exiting(eError);
	return eError;

}

OMX_ERRORTYPE H264ENC_SetAllParams(OMTB_H264_VENC_PRIVATE* h264Client) //Brijesh
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_HANDLETYPE pHandle = NULL;

	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE tBitStreamFormat;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE tInputImageFormat;
	OMX_VIDEO_PARAM_TRANSFORM_BLOCKSIZETYPE tTransformBlockSize;
//	OMX_VIDEO_PARAM_AVCSLICEFMO tAVCFMO;
	
	//OMX_U32 i;


	pHandle = h264Client->handle;

	/*Profile Level settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tProfileLevel,OMX_VIDEO_PARAM_PROFILELEVELTYPE);
	tProfileLevel.nPortIndex=H264_APP_OUTPUTPORT;
	
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	tProfileLevel.eProfile= OMX_VIDEO_AVCProfileHigh;
  	tProfileLevel.eLevel= OMX_VIDEO_AVCLevel41;
	
	eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	
	/*BitStream Format settings*/	
	OMX_TEST_INIT_STRUCT_PTR(&tBitStreamFormat,OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE);
	tBitStreamFormat.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoBitStreamFormatSelect, &tBitStreamFormat);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	tBitStreamFormat.eStreamFormat=0;//pAppData->H264_TestCaseParams->BitStreamFormat;
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoBitStreamFormatSelect, &tBitStreamFormat);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	
	/*Encoder Preset settings*/	
	OMX_TEST_INIT_STRUCT_PTR(&tEncoderPreset,OMX_VIDEO_PARAM_ENCODER_PRESETTYPE);
	tEncoderPreset.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	tEncoderPreset.eEncodingModePreset=2;//pAppData->H264_TestCaseParams->EncodingPreset;	
	tEncoderPreset.eRateControlPreset=4;//pAppData->H264_TestCaseParams->RateCntrlPreset;
	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	/*Image Type settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tInputImageFormat,OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE);
	tInputImageFormat.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoFrameDataContentSettings, &tInputImageFormat);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	tInputImageFormat.eContentType=0;//pAppData->H264_TestCaseParams->InputContentType;
	if(tInputImageFormat.eContentType!=OMX_Video_Progressive){
		tInputImageFormat.eInterlaceCodingType=0;//pAppData->H264_TestCaseParams->InterlaceCodingType; 
	}	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoFrameDataContentSettings, &tInputImageFormat);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	/*Transform BlockSize settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tTransformBlockSize,OMX_VIDEO_PARAM_TRANSFORM_BLOCKSIZETYPE);
	tTransformBlockSize.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoTransformBlockSize, &tTransformBlockSize);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	
	tTransformBlockSize.eTransformBlocksize=1;//pAppData->H264_TestCaseParams->TransformBlockSize;
	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoTransformBlockSize, &tTransformBlockSize);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	
/*

	if((pAppData->H264_TestCaseParams->bFMO)&&((pAppData->H264_TestCaseParams->nBitEnableAdvanced>>1)==0)){
		*Basic FMO  settings*
		OMX_TEST_INIT_STRUCT_PTR(&tAVCFMO,OMX_VIDEO_PARAM_AVCSLICEFMO);
		tAVCFMO.nPortIndex=H264_APP_OUTPUTPORT;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoSliceFMO, &tAVCFMO);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	


		tAVCFMO.nNumSliceGroups =  pAppData->H264_TestCaseParams->nNumSliceGroups;
		tAVCFMO.nSliceGroupMapType = pAppData->H264_TestCaseParams->nSliceGroupMapType;
		tAVCFMO.eSliceMode = pAppData->H264_TestCaseParams->eSliceMode;
			
		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoSliceFMO, &tAVCFMO);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	

	}	
*/

/*Set the Advanced Setting if any
	eError =  H264ENC_SetAdvancedParams(pAppData,test_index);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	
*/

	/*Settings from Initial DynamicParams*/
	eError =H264ENC_SetParamsFromInitialDynamicParams(h264Client);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	return eError;
	
}


OMX_ERRORTYPE H264ENC_SetParamsFromInitialDynamicParams(OMTB_H264_VENC_PRIVATE* pAppData)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
    	OMX_HANDLETYPE pHandle = NULL;
    	//OMX_U32 LFrameRate,LBitRate,LIntraPeriod;
    	//OMX_BOOL bFramerate=OMX_FALSE,bBitrate=OMX_FALSE,bIntraperiod=OMX_FALSE;
    	//OMX_U32 nRemoveList[9];
    	OMX_U8 i;//,RemoveIndex=0;
/*
	OMX_VIDEO_CONFIG_MESEARCHRANGETYPE tMESearchrange;
	OMX_CONFIG_INTRAREFRESHVOPTYPE tIntraRefresh;
	OMX_CONFIG_INTRAREFRESHVOPTYPE tForceFrame;
	OMX_VIDEO_CONFIG_QPSETTINGSTYPE tQPSettings;
	OMX_VIDEO_CONFIG_SLICECODINGTYPE tSliceCodingparams;
	OMX_VIDEO_CONFIG_PIXELINFOTYPE tPixelInfo;
*/
	OMX_VIDEO_PARAM_AVCTYPE tAVCParams;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_VIDEO_PARAM_PORTFORMATTYPE tVideoParams;
	OMX_PORT_PARAM_TYPE tPortParams;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

//	TIMM_OSAL_Entering();
	
	OMX_TEST_INIT_STRUCT_PTR(&tPortDef,OMX_PARAM_PORTDEFINITIONTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tPortParams,OMX_PORT_PARAM_TYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tVideoParams,OMX_VIDEO_PARAM_PORTFORMATTYPE);	
	OMX_TEST_INIT_STRUCT_PTR(&tVidEncBitRate,OMX_VIDEO_PARAM_BITRATETYPE);	
	OMX_TEST_INIT_STRUCT_PTR(&tAVCParams,OMX_VIDEO_PARAM_AVCTYPE);
	
	
	pHandle = pAppData->handle;

	
	/*Get the Number of Ports*/
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoInit, &tPortParams);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	for(i=0; i < tPortParams.nPorts ; i++){
		tPortDef.nPortIndex = i;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	


		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoPortFormat, &tVideoParams);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	


		if(tPortDef.eDir == OMX_DirInput){
			
			/*set the actual number of buffers required*/
			tPortDef.nBufferCountActual=1;//pAppData->H264_TestCaseParams->maxInterFrameInterval;
			/*set the video format settings*/
			tPortDef.format.video.nFrameWidth = 176;//pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nStride = 176;//pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nFrameHeight = 144;//pAppData->H264_TestCaseParams->height;
			tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;//pAppData->H264_TestCaseParams->inputChromaFormat;
			
			/*settings for OMX_IndexParamVideoPortFormat*/
			tVideoParams.eColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;//pAppData->H264_TestCaseParams->inputChromaFormat;
			
		}
		if(tPortDef.eDir == OMX_DirOutput){
			/*settings for OMX_IndexParamPortDefinition*/
			tPortDef.format.video.nFrameWidth = 176;//pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nStride = 176;//pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nFrameHeight = 144;//pAppData->H264_TestCaseParams->height;			
			tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;			
			/*
			if(bFramerate){
				tPortDef.format.video.xFramerate =LFrameRate;
				tVideoParams.xFramerate=LFrameRate;
			}
			if(bBitrate){
				tPortDef.format.video.nBitrate=LBitRate;
			}
			*/
			
			/*settings for OMX_IndexParamVideoPortFormat*/
			tVideoParams.eCompressionFormat = OMX_VIDEO_CodingAVC;			
		}

		eError = OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	


		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoPortFormat, &tVideoParams);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	


		/*AVC Settings*/	
		tAVCParams.nPortIndex=OMX_DirOutput;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoAvc, &tAVCParams);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	

		tAVCParams.eProfile = OMX_VIDEO_AVCProfileHigh;//pAppData->H264_TestCaseParams->profile;
		tAVCParams.eLevel= OMX_VIDEO_AVCLevel41;//pAppData->H264_TestCaseParams->level;
		/*
		if(bIntraperiod==OMX_TRUE){
			tAVCParams.nPFrames =LIntraPeriod;
		}
		*/
		
		tAVCParams.nBFrames = 0;//pAppData->H264_TestCaseParams->maxInterFrameInterval-1;
		tAVCParams.bEnableFMO= 0;//pAppData->H264_TestCaseParams->bFMO;
		tAVCParams.bEntropyCodingCABAC=0;//pAppData->H264_TestCaseParams->bCABAC;
		tAVCParams.bconstIpred=0;//pAppData->H264_TestCaseParams->bConstIntraPred;
		tAVCParams.eLoopFilterMode=0;//pAppData->H264_TestCaseParams->bLoopFilter;
		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoAvc, &tAVCParams);
		if(OMX_ErrorNone != eError)
			{
					OMTB_OmxReturnStatus(eError);
					return eError;
			}	

	}

	tVidEncBitRate.nPortIndex=OMX_DirOutput;
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoBitrate, &tVidEncBitRate);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	

	tVidEncBitRate.eControlRate =0;//pAppData->H264_TestCaseParams->RateControlAlg;
	/*
	if(bBitrate){
		tVidEncBitRate.nTargetBitrate=LBitRate;
	}
	*/
	eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoBitrate, &tVidEncBitRate);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	


	
	/*Update the DynamicSettingsArray
	eError = H264E_Update_DynamicSettingsArray(RemoveIndex,nRemoveList);
	if(OMX_ErrorNone != eError)
		{
				OMTB_OmxReturnStatus(eError);
				return eError;
		}	
*/
	
	return eError;
	
}

#endif

/*----------------------------- end of file ----------------------------------*/

