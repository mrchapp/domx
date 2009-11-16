/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright 2009 Texas Instruments Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found 
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/*!  
 *****************************************************************************
 * \file
 *    H264_ILClient.c                                                         
 *
 * \brief  
 *  This file contains IL Client implementation specific to OMX H264 Decoder
 *    
 * \version 1.0
 *
 *****************************************************************************
 */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
#include <string.h>
//temp
#include <unistd.h>
//#include <ti/sysbios/knl/Task.h>
//#include <ih264vdec.h>
//#include <ti/sdo/codecs/h264dec3/h264vdec.h>
//#include <ti/sdo/codecs/h264dec3/h264vdec_ti.h>

#include <H264_Decoder_ILClient.h>
#include <timm_osal_events.h>
#include <timm_osal_pipes.h>
#include <timm_osal_semaphores.h>
#include <timm_osal_error.h>
#include <timm_osal_task.h>
#include <timm_osal_memory.h>
//#include <WTSD_DucatiMMSW/test/thc/thc.h>

#if 0
//AD - only for linux testing
#define OMX_H264D_SRCHANGES 
#define OMX_H264D_BUF_HEAP
#define OMX_H264D_NONTILERTEST
#endif
#if 1
#define OMX_H264D_LINUX_TILERTEST
#define OMX_H264D_USEBUFFERENABLED
#undef OMX_H264D_ALLOCBUFFERENABLED
#endif
#ifdef OMX_H264D_BUF_HEAP
#include <unistd.h>
#include <mmplatform.h>


#include <RcmClient.h>
#include <HeapBuf.h>
#include <SharedRegion.h>
#endif

#ifdef OMX_H264D_LINUX_TILERTEST
#include <memmgr.h>
#include "mmplatform.h"

#endif
#ifdef OMX_H264D_BUF_HEAP
extern HeapBuf_Handle heapHandle;
#endif
#ifdef OMX_H264D_LINUX_TILERTEST
#define STRIDE_8BIT (4 * 1024)
#define STRIDE_16BIT (4 * 1024)
#endif
/****************************************************************
 * DEFINES
 ****************************************************************/

/** Event definition to indicate input buffer consumed */
#define H264_DECODER_INPUT_READY 1

/** Event definition to indicate output buffer consumed */
#define H264_DECODER_OUTPUT_READY   2

/** Event definition to indicate error in processing */
#define H264_DECODER_ERROR_EVENT 4

/** Event definition to indicate End of stream */
#define H264_DECODER_END_OF_STREAM 8

#define H264_STATETRANSITION_COMPLETE 16
#define     PADX    32
#define     PADY    24

/****************************************************************
 * GLOBALS
 ****************************************************************/

static TIMM_OSAL_PTR pSem_Events = NULL;
static TIMM_OSAL_PTR pSem_InputPort = NULL;
static TIMM_OSAL_PTR pSem_OutputPort = NULL;
static TIMM_OSAL_PTR pInputPtr = NULL;
static TIMM_OSAL_PTR myEvent;
static TIMM_OSAL_PTR H264VD_CmdEvent;

static TIMM_OSAL_U32 num_out_buffers;
extern H264_Decoder_TestCaseParamsType H264_Decoder_TestCaseParams[];



/* ==========================================================================
*
*@func   outputDisplayFrame()
*
*@brief  Sub function to write output data to a file
*
*@param  outArgs                  output arguments
*@param  xoff                     horizontal offset
*@param  yoff                     vertical offset
*
*@ret    none
*
* ============================================================================
*/
#ifdef H264D_WRITEOUTPUT
static unsigned short outputDisplayFrame(IH264VDEC_OutArgs *outArgs, unsigned short xoff, unsigned short yoff,
                   unsigned short ref_width, unsigned short ref_height,
                   unsigned short width, unsigned short height, unsigned short pic_struct, unsigned short hrd,
                   unsigned int fieldBuf, FILE *fout, unsigned int complianceMode)
{
    IVIDEO2_BufDesc *displayBufs = (IVIDEO2_BufDesc *)outArgs->viddec3OutArgs.displayBufs.pBufDesc;

  /*--------------------------------------------------------------------------------
   Here the actual frame data (w/out padding), is extracted and dumped.
   Also UV is de-interleaved.   
  --------------------------------------------------------------------------------*/
  char *lumaAddr   = (char *)displayBufs->planeDesc[0].buf;
  char *chromaAddr = (char *)displayBufs->planeDesc[1].buf;
  unsigned short retval = 0;
  char *CbBuf, *CrBuf, *YBuf;
  unsigned int pic_size, i, j;

  pic_size = width * height;

  lumaAddr   = (char *)((unsigned int)displayBufs->planeDesc[0].buf +  (yoff * ref_width) + xoff);
  chromaAddr = (char *)((unsigned int)displayBufs->planeDesc[1].buf +  ((yoff>>1) * ref_width) + xoff);

  YBuf = (char *)fieldBuf;
  for(i=0 ; i<height ; i++)
  {
    memcpy(YBuf, lumaAddr, width);
    YBuf    += width;
    lumaAddr += ref_width;
  }

  CbBuf = (char *)(fieldBuf + pic_size);
  CrBuf = (char *)(fieldBuf + pic_size + (pic_size>>2));
  for(i=0 ; i<(height>>1) ; i++)
  {
    for(j=0 ; j<(width>>1) ; j++)
    {
      CbBuf[j] = chromaAddr[(j*2)];
      CrBuf[j] = chromaAddr[(j*2)+1];
    }
    CbBuf    += (width>>1);
    CrBuf    += (width>>1);
    chromaAddr += ref_width;
  }

  if(!complianceMode)
    fwrite((void *)fieldBuf, sizeof(unsigned char), ((pic_size*3)>>1), fout);


  return (retval);
}
#endif
/* ==========================================================================
*
*@func   outputDisplayField()
*
*@brief  Sub  Function to interleave 2 fields.
* In case of interlaced or PICAFF streams, the frame is stored as
* 2 separate field. This function is used to interleaved the 2 fields
* and form the frame.The interleaving is done in fieldBuf and is then 
* written out
*
*@param  outArgs                  output arguments
*@param  xoff                     horizontal offset
*@param  yoff                     vertical offset
*
*@ret    none
*
* ============================================================================*/
#ifdef H264D_WRITEOUTPUT
static unsigned short outputDisplayField(IH264VDEC_OutArgs *outArgs, unsigned short xoff, unsigned short yoff,
                   unsigned short ref_width, unsigned short ref_height,
                   unsigned short width, unsigned short height, unsigned short pic_struct, unsigned short hrd,
                   unsigned int fieldBuf, unsigned short bottom_field_flag, FILE *fout, unsigned int complianceMode)
{
    IVIDEO2_BufDesc *displayBufs = (IVIDEO2_BufDesc *)&outArgs->viddec3OutArgs.displayBufs.bufDesc[0];

  /*--------------------------------------------------------------------------------
   Here the actual frame data (w/out padding), is extracted and dumped.
   Also UV is de-interleaved.
   Top and Bottom fields are interleaved and written   
  --------------------------------------------------------------------------------*/
  char *lumaAddr1, *lumaAddr2, *chromaAddr1, *chromaAddr2;
  unsigned short retval = 0;
  char *CbBuf, *CrBuf, *YBuf;
  unsigned int pic_size, i, j;
  
    unsigned int fieldSizeY;
    unsigned int fieldSizeCbCr;

    fieldSizeY = (ref_width*ref_height)>>1;
    fieldSizeCbCr = fieldSizeY >> 1; 

  pic_size = width * height;

  lumaAddr1   = (char *)((unsigned int)displayBufs->planeDesc[0].buf +  (yoff * ref_width) + xoff);
  chromaAddr1 = (char *)((unsigned int)displayBufs->planeDesc[1].buf +  ((yoff>>1) * ref_width) + xoff);

  lumaAddr2   = lumaAddr1   + fieldSizeY;
  chromaAddr2 = chromaAddr1 + fieldSizeCbCr;

  YBuf = (char *)fieldBuf;
  for(i=0 ; i<(height>>1) ; i++)
    {
    memcpy(YBuf, lumaAddr1, width);
    YBuf      += width;
    lumaAddr1 += ref_width;
    memcpy(YBuf, lumaAddr2, width);
    YBuf      += width;
    lumaAddr2 += ref_width;
    }

  CbBuf = (char *)(fieldBuf + pic_size);
  CrBuf = (char *)(fieldBuf + pic_size + (pic_size>>2));
  for(i=0 ; i<(height>>2) ; i++)
  {
    for(j=0 ; j<(width>>1) ; j++)
    {
      CbBuf[j] = chromaAddr1[(j*2)];
      CrBuf[j] = chromaAddr1[(j*2)+1];
    }
    CbBuf    += (width>>1);
    CrBuf    += (width>>1);
    chromaAddr1 += ref_width;

    for(j=0 ; j<(width>>1) ; j++)
    {
      CbBuf[j] = chromaAddr2[(j*2)];
      CrBuf[j] = chromaAddr2[(j*2)+1];
    }
    CbBuf    += (width>>1);
    CrBuf    += (width>>1);
    chromaAddr2 += ref_width;
  }
  if(!complianceMode)
    fwrite((void *)fieldBuf, sizeof(unsigned char), ((pic_size*3)>>1), fout);

  return (retval);
}
#endif
/* ==========================================================================
*
*@func   TestApp_WriteOutputData()
*
*@brief  Application function to write output data to a file
*
*@param  fOutFile           output file ptr
*
*@param  outArgs            OutArgs from process
*
*@param  fieldBuf            Scratch buffer for interleaving the data
*
*@ret    none
*
* ============================================================================
*/
#ifdef H264D_WRITEOUTPUT
static XDAS_Void TestApp_WriteOutputData
(
  FILE            *fOutFile,
  IH264VDEC_OutArgs   *outArgs,
  unsigned int              fieldBuf,
  unsigned int              complianceMode
)
{
    unsigned int width, height, paddedheight, hrd;
    unsigned int xOffset, yOffset;

  /*-------------------------------------------------------------------------*/
  /*  Pointer to Display buffer structure                                    */
  /*-------------------------------------------------------------------------*/
    IVIDEO2_BufDesc *displayBufs = (IVIDEO2_BufDesc *)&outArgs->viddec3OutArgs.displayBufs.bufDesc[0];

    paddedheight = displayBufs->imageRegion.bottomRight.y + displayBufs->imageRegion.topLeft.y;

	height = displayBufs->activeFrameRegion.bottomRight.y - displayBufs->activeFrameRegion.topLeft.y;
	width = displayBufs->activeFrameRegion.bottomRight.x - displayBufs->activeFrameRegion.topLeft.x;

	hrd = (displayBufs->repeatFrame >= 0)? 1 : 0;

	xOffset = displayBufs->activeFrameRegion.topLeft.x;
	yOffset = displayBufs->activeFrameRegion.topLeft.y;

    /*-------------------------------------------------------------
     contentType indicates if the frame was stored as field or not.
     contentType :0 , frame storage
     otherwise, it is field storage
    ---------------------------------------------------------------*/    
	if(displayBufs->contentType == IVIDEO_PROGRESSIVE)
    {
		//Progressive frame
		outputDisplayFrame(outArgs,
				    (unsigned short)xOffset,
				    (unsigned short)yOffset,
					displayBufs->imagePitch,
					paddedheight,
					(unsigned short) width,
					(unsigned short) height,
					displayBufs->repeatFrame,
					hrd, fieldBuf, fOutFile,complianceMode);
  
    }
    else
    {
        /*---------------------------------------------------------------
         The frame is stored as 2 fields. So top and bottom fields have to
          be interleaved. The interleaving is done in fieldBuf and is then
          written out
        -----------------------------------------------------------------*/
        /* Double the paddedheight and height dimensions for two fields */
        paddedheight <<= 1;
        height  <<= 1;
        /*---------------------------------------------------------------
         Write Top and Bottom field
         -----------------------------------------------------------------*/
         outputDisplayField(outArgs,
                 (unsigned short) xOffset,
                 (unsigned short) yOffset,
				 (unsigned short) displayBufs->imagePitch,
                 paddedheight,
                 (unsigned short) width,
                 (unsigned short) height,
				 displayBufs->repeatFrame, hrd, fieldBuf, 0, fOutFile, complianceMode);

      }
}
#endif
/* ========================================================================== */
/**
* H264DEC_AllocateResources() : Allocates the resources required for H264 
* Decoder. 
*
*
* @param pAppData   : Pointer to the application data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_AllocateResources(H264_Client* pAppData)
{
    OMX_U32 retval;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    pAppData->pCb = (OMX_CALLBACKTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_CALLBACKTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if (!pAppData->pCb) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pAppData->pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if (!pAppData->pInPortDef) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pAppData->pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if (!pAppData->pOutPortDef) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC)
    {
       pAppData->pH264  = (OMX_VIDEO_PARAM_AVCTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_VIDEO_PARAM_AVCTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
        if (!pAppData->pH264) {
            eError = OMX_ErrorInsufficientResources;
            goto EXIT;
        }
    }
    else
    {
        printf("Invalid compression format value.\n");
        eError = OMX_ErrorUnsupportedSetting;
        goto EXIT;
    }

    /* Create a pipes for Input and Output Buffers.. used to queue data from the callback. */
    retval = TIMM_OSAL_CreatePipe(&(pAppData->IpBuf_Pipe),sizeof(OMX_BUFFERHEADERTYPE*) * NUM_OF_IN_BUFFERS,
                                        sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
    if (retval != 0)
    {
        printf("Error: TIMM_OSAL_CreatePipe failed to open\n");
        eError = OMX_ErrorContentPipeCreationFailed;
        goto EXIT;
    }

    printf("\nEnter the number of output buffers to allocate ");
    scanf("%d", &num_out_buffers);
    retval = TIMM_OSAL_CreatePipe(&(pAppData->OpBuf_Pipe),sizeof(OMX_BUFFERHEADERTYPE*) * num_out_buffers, sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
    if (retval != 0)
    {
        printf("Error: TIMM_OSAL_CreatePipe failed to open\n");
        eError = OMX_ErrorContentPipeCreationFailed;
        goto EXIT;
    }

EXIT:
        return eError;

}

/* ========================================================================== */
/**
* H264DEC_FreeResources() : Free the resources allocated for H264 
* Decoder. 
*
*
* @param pAppData   : Pointer to the application data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static void H264DEC_FreeResources(H264_Client* pAppData)
{
    if(pAppData->pCb)
           TIMM_OSAL_Free(pAppData->pCb);
    printf("\npAppData->pCb deleted\n");

    if(pAppData->pInPortDef)
           TIMM_OSAL_Free(pAppData->pInPortDef);
    printf("\npAppData->pInPortDef deleted\n");           

    if(pAppData->pOutPortDef)
           TIMM_OSAL_Free(pAppData->pOutPortDef);
    printf("\npAppData->pOutPortDef deleted\n");            

    if(pAppData->pH264)
           TIMM_OSAL_Free(pAppData->pH264);
    printf("\npAppData->pH264 deleted\n"); 

    if(pAppData->IpBuf_Pipe)
        TIMM_OSAL_DeletePipe(pAppData->IpBuf_Pipe);
    printf("\npAppData->IpBuf_Pipe deleted\n"); 

    if(pAppData->OpBuf_Pipe)
        TIMM_OSAL_DeletePipe(pAppData->OpBuf_Pipe);
    printf("\npAppData->OpBuf_Pipe deleted\n"); 

    return;

}

/* ========================================================================== */
/**
* H264_GetDecoderErrorString() : Function to map the OMX error enum to string
*
* @param error   : OMX Error type
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */
static OMX_STRING H264_GetDecoderErrorString(OMX_ERRORTYPE error)
{
    OMX_STRING errorString;

    switch(error)
    {
        case OMX_ErrorNone:
            errorString = "OMX_ErrorNone";
            break;
        case OMX_ErrorInsufficientResources:
            errorString = "OMX_ErrorInsufficientResources";
            break;
        case OMX_ErrorUndefined:
            errorString = "OMX_ErrorUndefined";
            break;
        case OMX_ErrorInvalidComponentName:
            errorString = "OMX_ErrorInvalidComponentName";
            break;
        case OMX_ErrorComponentNotFound:
            errorString = "OMX_ErrorComponentNotFound";
            break;
        case OMX_ErrorInvalidComponent:
            errorString = "OMX_ErrorInvalidComponent";
            break;
        case OMX_ErrorBadParameter:
            errorString = "OMX_ErrorBadParameter";
            break;
        case OMX_ErrorNotImplemented:
            errorString = "OMX_ErrorNotImplemented";
            break;
        case OMX_ErrorUnderflow:
            errorString = "OMX_ErrorUnderflow";
            break;
        case OMX_ErrorOverflow:
            errorString = "OMX_ErrorOverflow";
            break;
        case OMX_ErrorHardware:
            errorString = "OMX_ErrorHardware";
            break;
        case OMX_ErrorInvalidState:
            errorString = "OMX_ErrorInvalidState";
            break;
        case OMX_ErrorStreamCorrupt:
            errorString = "OMX_ErrorStreamCorrupt";
            break;
        case OMX_ErrorPortsNotCompatible:
            errorString = "OMX_ErrorPortsNotCompatible";
            break;
        case OMX_ErrorResourcesLost:
            errorString = "OMX_ErrorResourcesLost";
            break;
        case OMX_ErrorNoMore:
            errorString = "OMX_ErrorNoMore";
            break;
        case OMX_ErrorVersionMismatch:
            errorString = "OMX_ErrorVersionMismatch";
            break;
        case OMX_ErrorNotReady:
            errorString = "OMX_ErrorNotReady";
            break;
        case OMX_ErrorTimeout:
            errorString = "OMX_ErrorTimeout";
            break;
        default:
            errorString = "<unknown>";
    }
    return errorString;
}

/* ========================================================================== */
/**
* H264DEC_FillData() : Function to fill the input buffer with data.
* This function currently reads the entire file into one single memory chunk.
* May require modification to support bigger file sizes.
*
*
* @param pAppData   : Pointer to the application data
* @param pBuf       : Pointer to the input buffer
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_U32 H264DEC_FillData(H264_Client* pAppData,OMX_BUFFERHEADERTYPE *pBuf)
{
    OMX_U32 nRead = 0;
    OMX_U32 framesizetoread = 0;

    if(!feof(pAppData->fIn))
    {
        fseek (pAppData->fIn, 0, SEEK_END);
        framesizetoread = ftell (pAppData->fIn);
        fseek (pAppData->fIn, 0, SEEK_SET);
        nRead = fread(pBuf->pBuffer,sizeof(OMX_U8),framesizetoread, pAppData->fIn);
        if(nRead & 0x01)
        {
            pBuf->pBuffer[nRead++] = 0x00;
        }
        pBuf->nFilledLen = nRead;
        pBuf->nAllocLen = nRead;
        pBuf->nFlags = OMX_BUFFERFLAG_EOS;
        pInputPtr = pBuf->pBuffer;
        
     #ifdef H264D_DEBUG
        printf("\n%d bytes read into the input buffer\n", (int)framesizetoread);
        printf("\nOMX Input buff = 0x%x\n",pBuf->pBuffer);
     #endif
    }
    else
    {
        nRead = 0;
        pBuf->pBuffer = NULL;
    }

    return nRead;
}

/* ========================================================================== */
/**
* H264DEC_SetParamPortDefinition() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the H264 Decoder
* Component
*
* @param pAppData   : Pointer to the application data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_SetParamPortDefinition(H264_Client* pAppData)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_HANDLETYPE pHandle = pAppData->pHandle;
    OMX_PORT_PARAM_TYPE portInit;

    if(!pHandle){
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    OMX_H264VD_TEST_INIT_STRUCT_PTR(&portInit, OMX_PORT_PARAM_TYPE);

    portInit.nPorts = 2;
    portInit.nStartPortNumber = 0;
    eError = OMX_SetParameter(pAppData->pHandle, OMX_IndexParamVideoInit, &portInit);
    if(eError != OMX_ErrorNone)
    {
        printf("OMX_SetParameter Resulted an Error %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

    OMX_H264VD_TEST_INIT_STRUCT_PTR(pAppData->pInPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

    pAppData->pInPortDef->nPortIndex = 0x0;    // Need to put input port index macro from spurthi's interface file
    pAppData->pInPortDef->eDir = OMX_DirInput;
    pAppData->pInPortDef->nBufferCountActual = NUM_OF_IN_BUFFERS;
    pAppData->pInPortDef->nBufferCountMin = 1;

    fseek (pAppData->fIn, 0, SEEK_END);
    pAppData->pInPortDef->nBufferSize = ftell (pAppData->fIn) + sizeof(OMX_U32);
    fseek (pAppData->fIn, 0, SEEK_SET);
    
    //pAppData->pInPortDef->nBufferSize = (pAppData->nWidth * pAppData->nHeight); // approx
    pAppData->pInPortDef->bEnabled = OMX_TRUE;
    pAppData->pInPortDef->bPopulated = OMX_FALSE;
    pAppData->pInPortDef->eDomain = OMX_PortDomainVideo;
    pAppData->pInPortDef->bBuffersContiguous = OMX_FALSE;
    pAppData->pInPortDef->nBufferAlignment = 0x0; // Mention memory alignment if any


    /* OMX_VIDEO_PORTDEFINITION values for input port */
    pAppData->pInPortDef->format.video.cMIMEType = "H264";//"YUV420";
    pAppData->pInPortDef->format.video.pNativeRender = NULL;
    pAppData->pInPortDef->format.video.nFrameWidth = pAppData->nWidth; // It is not necessary to mention width and hight according to comment written in OMX_Video.h
    pAppData->pInPortDef->format.video.nFrameHeight = pAppData->nHeight;
    pAppData->pInPortDef->format.video.nStride = -1; // h264Deocoder doesn't need this info
    pAppData->pInPortDef->format.video.nSliceHeight = 0; // h264Deocoder doesn't need this info
    pAppData->pInPortDef->format.video.nBitrate = 10485760;  // pAppData->nBitRate; // Confirm this
    pAppData->pInPortDef->format.video.xFramerate = 30; // h264Deocoder doesn't need this info
    pAppData->pInPortDef->format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
    pAppData->pInPortDef->format.video.bFlagErrorConcealment = OMX_FALSE;
    pAppData->pInPortDef->format.video.eColorFormat = pAppData->ColorFormat;

    eError = OMX_SetParameter (pAppData->pHandle, OMX_IndexParamPortDefinition, pAppData->pInPortDef);
    if (eError != OMX_ErrorNone) {
        printf("OMX_SetParameter Resulted an Error %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
    OMX_H264VD_TEST_INIT_STRUCT_PTR(pAppData->pOutPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

    pAppData->pOutPortDef->nPortIndex = 0x1;
    pAppData->pOutPortDef->eDir = OMX_DirOutput;
    pAppData->pOutPortDef->nBufferCountActual = num_out_buffers;
    pAppData->pOutPortDef->nBufferCountMin = 1;
    pAppData->pOutPortDef->nBufferSize = (65280 + 48000);//(pAppData->nWidth * pAppData->nHeight) * 5;//3 / 2; // Different if padding is included
    pAppData->pOutPortDef->bEnabled = OMX_TRUE;
    pAppData->pOutPortDef->bPopulated = OMX_FALSE;
    pAppData->pOutPortDef->eDomain = OMX_PortDomainVideo;
    pAppData->pOutPortDef->bBuffersContiguous = OMX_FALSE;
    pAppData->pOutPortDef->nBufferAlignment = 0x0; // Mention memory alignment if any

    /* OMX_VIDEO_PORTDEFINITION values for output port */
    pAppData->pOutPortDef->format.video.cMIMEType = "H264";//"YUV420";
    pAppData->pOutPortDef->format.video.pNativeRender = NULL;
    pAppData->pOutPortDef->format.video.nFrameWidth = ((pAppData->nWidth + (2*PADX) + 127) & 0xFFFFFF80);// 256; //pAppData->nWidth; //256
    pAppData->pOutPortDef->format.video.nFrameHeight = ((pAppData->nHeight + (4*PADY))); //pAppData->nHeight; //240
    pAppData->pOutPortDef->format.video.nStride = 0;
    pAppData->pOutPortDef->format.video.nSliceHeight = 0;
    pAppData->pOutPortDef->format.video.nBitrate = 10485760;
    pAppData->pOutPortDef->format.video.xFramerate = 30;
    pAppData->pOutPortDef->format.video.bFlagErrorConcealment = OMX_FALSE;
    pAppData->pOutPortDef->format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
    pAppData->pOutPortDef->format.video.eColorFormat = pAppData->ColorFormat;

    eError = OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, pAppData->pOutPortDef);
    if (eError != OMX_ErrorNone) {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

EXIT:
    return eError;
}

/* ========================================================================== */
/**
* H264DEC_WaitForState() : This method will wait for the component to get 
* to the correct state.
*
* @param pHandle        : Handle to the component
* @param DesiredState   : Desired 
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_WaitForState(OMX_HANDLETYPE* pHandle,
                                OMX_STATETYPE DesiredState)
{
     OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents, numRemaining;

	TIMM_OSAL_Entering();
	
	/* Wait for an event */
	uRequestedEvents = (H264_STATETRANSITION_COMPLETE|H264_DECODER_ERROR_EVENT);
	retval = TIMM_OSAL_EventRetrieve (H264VD_CmdEvent, uRequestedEvents,
			TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);
	
	if (TIMM_OSAL_ERR_NONE != retval) {
          printf("\nError in EventRetrieve !\n");
          eError = OMX_ErrorInsufficientResources;
          goto EXIT;
        }
	
	if (pRetrievedEvents & H264_DECODER_ERROR_EVENT) {
		eError=OMX_ErrorUndefined; //TODO: Needs to be changed
	}else{
		eError=OMX_ErrorNone;
        }

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

/* ========================================================================== */
/**
* H264DEC_ChangePortSettings() : This method will 
*
* @param pHandle        : Handle to the component
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_ChangePortSettings(H264_Client* pAppData)
{

    TIMM_OSAL_ERRORTYPE retval;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 i;

    eError = OMX_SendCommand(pAppData->pHandle, OMX_CommandPortDisable, pAppData->pOutPortDef->nPortIndex, NULL);
    if(eError != OMX_ErrorNone) {
        printf("Error from SendCommand OMX_CommandPortDisable \n");
        goto EXIT;
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++){
        eError = OMX_FreeBuffer(pAppData->pHandle, pAppData->pOutPortDef->nPortIndex, pAppData->pOutBuff[i]);
        if(eError != OMX_ErrorNone) {
            printf("Error in OMX_FreeBuffer");
            goto EXIT;
        }
    }

    eError = OMX_GetParameter(pAppData->pHandle,OMX_IndexParamPortDefinition,pAppData->pOutPortDef);
    if(eError != OMX_ErrorNone) {
        printf("Error in OMX_GetParameter");
        goto EXIT;
    }

    eError = OMX_SendCommand(pAppData->pHandle,OMX_CommandPortEnable,pAppData->pOutPortDef->nPortIndex, NULL);
    if(eError != OMX_ErrorNone) {
        printf("Error in OMX_SendCommand:OMX_CommandPortEnable");
        goto EXIT;
    }

    retval = TIMM_OSAL_ClearPipe(pAppData->OpBuf_Pipe);
    if (retval != TIMM_OSAL_ERR_NONE) {
        printf("Error in clearing Output Pipe!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
    {
        eError = OMX_AllocateBuffer(pAppData->pHandle, &pAppData->pOutBuff[i], pAppData->pOutPortDef->nPortIndex, pAppData, pAppData->pOutPortDef->nBufferSize);
        if(eError != OMX_ErrorNone) {
            printf("Error in Allocating buffers");
            goto EXIT;
        }

        retval = TIMM_OSAL_WriteToPipe(pAppData->OpBuf_Pipe, &pAppData->pOutBuff[i], sizeof(pAppData->pOutBuff[i]), TIMM_OSAL_SUSPEND);
        if (retval != TIMM_OSAL_ERR_NONE) {
            printf("Error in writing to out pipe!\n");
            eError = OMX_ErrorNotReady;
            return eError;
        }
    }

EXIT:
    return eError;

}

/* ========================================================================== */
/**
* H264DEC_EventHandler() : This method is the event handler implementation to 
* handle events from the OMX H264 Derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : 
* @param eEvent            :
* @param nData1            :
* @param nData2            :
* @param pEventData        :
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_EventHandler(OMX_HANDLETYPE hComponent,OMX_PTR ptrAppData,OMX_EVENTTYPE eEvent,
                           OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
    H264_Client* pAppData = ptrAppData;
/*    OMX_STATETYPE state;*/
    TIMM_OSAL_ERRORTYPE retval;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

//    eError = pComponent->GetState(hComponent, &state);

    switch (eEvent)
    {
        case OMX_EventCmdComplete:
            TIMM_OSAL_SemaphoreRelease (pSem_Events);
			retval = TIMM_OSAL_EventSet (H264VD_CmdEvent, H264_STATETRANSITION_COMPLETE, TIMM_OSAL_EVENT_OR);
			if (retval != TIMM_OSAL_ERR_NONE) {
                printf("\nError in setting the event!\n");
                eError = OMX_ErrorNotReady;
                return eError;
            }
            break;
        case OMX_EventError:
            TIMM_OSAL_SemaphoreRelease (pSem_Events);
			retval = TIMM_OSAL_EventSet (H264VD_CmdEvent, H264_DECODER_ERROR_EVENT, TIMM_OSAL_EVENT_OR);
			if (retval != TIMM_OSAL_ERR_NONE) {
                printf("\nError in setting the event!\n");
                eError = OMX_ErrorNotReady;
                return eError;
            }
            break;
        case OMX_EventMark:

            break;
        case OMX_EventPortSettingsChanged:

            /* In case of change in output buffer sizes re-allocate the buffers*/
            eError = H264DEC_ChangePortSettings(pAppData);

            break;
        case OMX_EventBufferFlag:
                    retval = TIMM_OSAL_EventSet (myEvent, H264_DECODER_END_OF_STREAM, TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE) {
        printf("\nError in setting the event!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }
            /* EOS here nData1-> port....  nData2->OMX_BUFFERFLAG_EOS*/
            break;
        case OMX_EventResourcesAcquired:

            break;
        case OMX_EventComponentResumed:

            break;
        case OMX_EventDynamicResourcesAvailable:

            break;
        case OMX_EventPortFormatDetected:

            break;
        case OMX_EventMax:

            break;

    } // end of switch

    return eError;
}

/* ========================================================================== */
/**
* H264DEC_FillBufferDone() : This method handles the fill buffer done event
* got from the derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : Pointer to the app data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_FillBufferDone (OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer)
{
    H264_Client* pAppData = ptrAppData;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    TIMM_OSAL_ERRORTYPE retval;

    {
#ifdef SRCHANGES
printf("\npBuffer SR after FBD = %x\n",pBuffer->pBuffer);
pBuffer->pBuffer = SharedRegion_getPtr(pBuffer->pBuffer);
printf("\npBuffer after FBD = %x\n",pBuffer->pBuffer);
#endif    
    
    retval = TIMM_OSAL_WriteToPipe(pAppData->OpBuf_Pipe, &pBuffer, sizeof(pBuffer), TIMM_OSAL_SUSPEND); //timeout - TIMM_OSAL_SUSPEND ??
    if (retval != TIMM_OSAL_ERR_NONE) {
        printf("Error writing to Output buffer Pipe!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }

    retval = TIMM_OSAL_EventSet (myEvent, H264_DECODER_OUTPUT_READY, TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE) {
        printf("Error in setting the o/p event!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }
    }

    return eError;
}

/* ========================================================================== */
/**
* H264DEC_FillBufferDone() : This method handles the Empty buffer done event
* got from the derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : Pointer to the app data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE H264DEC_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer)
{
    H264_Client* pAppData = ptrAppData;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    TIMM_OSAL_ERRORTYPE retval;
#ifdef SRCHANGES
printf("\npBuffer SR after EBD = %x\n",pBuffer->pBuffer);
pBuffer->pBuffer = SharedRegion_getPtr(pBuffer->pBuffer);
printf("\npBuffer after EBD = %x\n",pBuffer->pBuffer);
#endif
    retval = TIMM_OSAL_WriteToPipe(pAppData->IpBuf_Pipe, &pBuffer, sizeof(pBuffer),  TIMM_OSAL_SUSPEND);
    if (retval != TIMM_OSAL_ERR_NONE) {
        printf("Error writing to Input buffer i/p Pipe!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }

    retval = TIMM_OSAL_EventSet (myEvent, H264_DECODER_INPUT_READY, TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE) {
        printf("Error in setting the event!\n");
        eError = OMX_ErrorNotReady;
        return eError;
    }

    return eError;
}

/* ========================================================================== */
/**
* H264_Decoder_main() : This method is the function that needs to be invoked
* from any test framework. This will execute all the tests that are defined in 
* testcase list. This will mimic the functioning of IL Client and test the OMX
* H264 Decoder implementation
*
* @param param1        : Currently used 
* @param param2        : Currently used
*
*  @return      
*  None
*/
/* ========================================================================== */
void main()
{
    H264_Client *pAppData = TIMM_OSAL_NULL;
    OMX_HANDLETYPE pHandle;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
    OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
    TIMM_OSAL_ERRORTYPE   tTIMMSemStatus;
    OMX_U32 i;
    OMX_U32 actualSize;
    OMX_U32 nRead;
    OMX_U32 test_index;
    OMX_CALLBACKTYPE appCallbacks;

    TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;
    OMX_U32 ii = 0;
    OMX_U32 mem_count_start = 0;
    OMX_U32 mem_size_start = 0;
    OMX_U32 mem_count_end = 0;
    OMX_U32 mem_size_end = 0;
    OMX_U32 frames_decoded;
    int input;
    int main_input;
    //Memory_Stats stats;
    char configFile[] = "Decoder_input.cfg";
    FILE *fconfigFile;
    char line[256];
    char file_input[256];
    int file_parameter;
	OMX_U32 setup;
    
    //AD - only for linux testing
    OMX_U8 *pTmpBuffer;
    
//#ifdef OMX_H264D_LINUX_TILERTEST
	MemAllocBlock *MemReqDescTiler;
	OMX_PTR TilerAddr=NULL;
#ifdef OMX_H264D_LINUX_TILERTEST
		setup = 2;
#else
		setup = 1;
#endif 
		mmplatform_init(setup);   
    printf("\nWait until RCM Server is created on other side. Press any key after that\n");
    getchar();
    //AD - ends

    appCallbacks.EventHandler         = H264DEC_EventHandler;
    appCallbacks.EmptyBufferDone     = H264DEC_EmptyBufferDone;
    appCallbacks.FillBufferDone     = H264DEC_FillBufferDone;

    printf("\n Choose 0/1: ");
    printf("\n 0: Normal test vectors ");
    printf("\n 1: Additional test vectors ");
    scanf("%d", &main_input);
    

    if(main_input == 0)
    {
       for(test_index = 0; test_index < 7;test_index++)
       {
          printf("\n Decode test vector %d: %s", (int)test_index, H264_Decoder_TestCaseParams[test_index].InFile);
       }
       //printf("\n Enter the test case number to execute (0-6, 100 to execute all testcases) : ");
       printf("\n Enter the test case number to execute (0-6 : ");

       #ifdef SCANF_WORKAROUND
           /* Already scanned earlier, just replace the value */
           input = param1;
       #else
           scanf("%d", &input);
       #endif
    }
    else
    {
       /* Check for the input file and open the input file */
       printf("\n Trying to open file Decoder_input.cfg ");
       fconfigFile = fopen(configFile,"r"); 

       if (!fconfigFile)
       {
         printf("%s - File not found. Place the file in the location of the base_image",configFile);
         return;
       }

       /* HACK - TO ALTER THE FLOW OF EXECUTION BELOW */
       input = 100;
    }


   for(ii = 0; ii < 7; ii++)
   {

      if(input != 100)
      {
         if(input < 0 || input > 7)
         {
            printf("\n The input is not in range. Enter a valid value\n");
	    return;
         }
         else
         {
            test_index = input;
         }
      }
      else
      {
         test_index = ii;
      }

      frames_decoded = 0;

      printf("\n");
      printf("\n-----------------------------------------------");
      printf("\n H264 Decoder Test case begin");

      if(main_input == 0)
      {
         printf("\n Decode test vector %s", H264_Decoder_TestCaseParams[test_index].InFile);
      }

      //mem_count_start = TIMM_OSAL_GetMemCounter();
      //mem_size_start = TIMM_OSAL_GetMemUsage();

      printf("\n Value from GetMemCounter = %d", mem_count_start);
      printf("\n Value from GetMemUsage = %d", mem_size_start);

      //Memory_getStats(NULL, &stats);
      //printf("\n Total size = %d", stats.totalSize); 
      //printf("\n Total free size = %d", stats.totalFreeSize); 
      //printf("\n Largest Free size = %d", stats.largestFreeSize); 
      printf("\n");

      tTIMMSemStatus = TIMM_OSAL_EventCreate (&myEvent);
      if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus) {
          printf("Error in creating event!\n");
          eError = OMX_ErrorInsufficientResources;
          goto EXIT;
      }
	  tTIMMSemStatus = TIMM_OSAL_EventCreate (&H264VD_CmdEvent);
	if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus) {
          printf("Error in creating event!\n");
          eError = OMX_ErrorInsufficientResources;
          goto EXIT;
      }
      pAppData = (H264_Client*)TIMM_OSAL_Malloc(sizeof(H264_Client), TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
      if (!pAppData) {
          printf("Error allocating pAppData!\n");
          eError = OMX_ErrorInsufficientResources;
          goto EXIT;
      }
      memset(pAppData, 0x00, sizeof(H264_Client));

      /* Open the file of data to be rendered.  */
      if(main_input == 0)
      {
         pAppData->fIn = fopen(H264_Decoder_TestCaseParams[test_index].InFile, "rb");
      }
      else
      {
          /* Get the file name from the file */
         if(fgets(line,254,fconfigFile))
         {
            sscanf(line,"%s",file_input);
	    printf("\n Input file = %s", file_input);
            pAppData->fIn = fopen(file_input, "rb");
         }
         else
         {
            printf("\n Could not get the input file. Check the file syntax");
            return;
         }
      }

      if(pAppData->fIn == NULL)
      {
          printf("Error: failed to open the file <%s> for reading", H264_Decoder_TestCaseParams[test_index].InFile);
          goto EXIT;
      }
  
     #ifdef H264D_WRITEOUTPUT
      if(main_input == 0)
      {
         pAppData->fOut = fopen(H264_Decoder_TestCaseParams[test_index].OutFile, "wb");
      }
      else
      {
	 /* Get the file name from the file */
         if(fgets(line,254,fconfigFile))
         {
            sscanf(line,"%s",file_input);
	    printf("\n Output file = %s", file_input);
            pAppData->fOut = fopen(file_input, "wb");
         }
         else
         {
            printf("\n Could not get the output file. Check the file syntax");
            return;
         }
      }

      if(pAppData->fOut == NULL)
      {
          printf("Error: failed to open the file <%s> for writing", H264_Decoder_TestCaseParams[test_index].OutFile);
          goto EXIT;
      }
     #endif
  
      pAppData->eCompressionFormat = OMX_VIDEO_CodingAVC;
      if(main_input == 0)
      {
         pAppData->nWidth = H264_Decoder_TestCaseParams[test_index].width;
         pAppData->nHeight = H264_Decoder_TestCaseParams[test_index].height;
         pAppData->ColorFormat = H264_Decoder_TestCaseParams[test_index].ColorFormat;
      }
      else
      {
         /* Get the Width */
         if(fgets(line,254,fconfigFile))
         {
            sscanf(line,"%d",&file_parameter);
	    printf("\n Width = %d", file_parameter);
            pAppData->nWidth = file_parameter;
         }
         else
         {
            printf("\n Could not get the width. Check the file syntax");
            return;
         }	      

         /* Get the Height */
         if(fgets(line,254,fconfigFile))
         {
            sscanf(line,"%d",&file_parameter);
	    printf("\n Height = %d", file_parameter);
            pAppData->nHeight = file_parameter;
         }
         else
         {
            printf("\n Could not get the Height. Check the file syntax");
            return;
         }	      

         /* Get the ColorFormat */
         if(fgets(line,254,fconfigFile))
         {
            sscanf(line,"%d",&file_parameter);
	    printf("\n ColorFormat = %d\n", file_parameter);
            pAppData->ColorFormat = file_parameter;
         }
         else
         {
            printf("\n Could not get the ColorFormat. Check the file syntax");
            return;
         }	      
      }

  
      eError = H264DEC_AllocateResources(pAppData); // Allocate memory for the structure fields present in the pAppData(H264_Client)
      if (eError != OMX_ErrorNone) {
          printf("Error allocating resources in main!\n");
          eError = OMX_ErrorInsufficientResources;
          goto EXIT;
      }


    pAppData->eState = OMX_StateInvalid;
    *pAppData->pCb = appCallbacks;

    tTIMMSemStatus = TIMM_OSAL_SemaphoreCreate (&pSem_InputPort,0);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Create failed!");
        goto EXIT;
    }
    tTIMMSemStatus = TIMM_OSAL_SemaphoreCreate (&pSem_OutputPort,0);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Create failed!");
        goto EXIT;
    }
    tTIMMSemStatus = TIMM_OSAL_SemaphoreCreate (&pSem_Events,0);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Create failed!");
        goto EXIT;
    }


    eError = OMX_Init();    // Is this must ???
    /* Currently this function is dummy. No need to check return */
#if 0
    if(eError != OMX_ErrorNone) {
        printf("Error returned by OMX_Init() : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }
#endif

    /* Load the H264Decoder Component */
    eError = OMX_GetHandle(&pHandle,(OMX_STRING)"OMX.TI.DUCATI1.VIDEO.H264D"/*StrH264Decoder*/,pAppData, pAppData->pCb);
    if( (eError != OMX_ErrorNone) || (pHandle == NULL) ) {
        printf ("Error in Get Handle function : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    pAppData->pHandle = pHandle;
    pAppData->pComponent = (OMX_COMPONENTTYPE *)pHandle;


    H264DEC_SetParamPortDefinition(pAppData);

    /* OMX_SendCommand expecting OMX_StateIdle */
    eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(eError != OMX_ErrorNone) {
     printf("Error in SendCommand()-OMX_StateIdle State set : %s \n", H264_GetDecoderErrorString(eError));
     goto EXIT;
    }
    printf("\nCame back from send command without error\n");

#ifdef OMX_H264D_LINUX_TILERTEST
		MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
		  if( MemReqDescTiler== TIMM_OSAL_NULL ) {
		    printf ("Error during memory allocation : OMX_ErrorInsufficientResources \n");
		    goto EXIT;
		}
#endif
/*use buffer calls*/
#ifdef OMX_H264D_USEBUFFERENABLED
	for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
    printf("\nAllocating buffer no. %d\n",i);
#ifdef OMX_H264D_BUF_HEAP
        pTmpBuffer = HeapBuf_alloc(heapHandle, pAppData->pInPortDef->nBufferSize, 0);
#elif defined (OMX_H264D_LINUX_TILERTEST)
		MemReqDescTiler[0].pixelFormat=PIXEL_FMT_PAGE;
		MemReqDescTiler[0].dim.len=pAppData->pInPortDef->nBufferSize;
		MemReqDescTiler[0].stride = 0;
		TIMM_OSAL_Trace("\nBefore tiler alloc for the Codec Internal buffer %d\n");
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,1);
		TIMM_OSAL_Trace("\nTiler buffer allocated is %x\n",TilerAddr);
		pTmpBuffer=(OMX_U8*)TilerAddr;
#else        
		pTmpBuffer= TIMM_OSAL_Malloc(((pAppData->pInPortDef->nBufferSize)), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif        
		if(pTmpBuffer== TIMM_OSAL_NULL) {
			printf("OMX_ErrorInsufficientResources\n");
			goto EXIT;
		}
#ifdef OMX_H264D_SRCHANGES        
			TIMM_OSAL_Trace("\npBuffer before UB = %x\n",pTmpBuffer);
pTmpBuffer = (char *)SharedRegion_getSRPtr(pTmpBuffer, 2);
			TIMM_OSAL_Trace("\npBuffer SR before UB = %x\n",pTmpBuffer);
			 if(pTmpBuffer== TIMM_OSAL_NULL) {
				printf("OMX_ErrorInsufficientResources\n");
				goto EXIT;
			}
#endif
			TIMM_OSAL_Trace("\ncall to use buffer\n");	
		eError = OMX_UseBuffer(pHandle, &(pAppData->pInBuff[i]), pAppData->pInPortDef->nPortIndex, pAppData, (pAppData->pInPortDef->nBufferSize),pTmpBuffer);			
			if(eError != OMX_ErrorNone)
			{
			    printf("Error %s:    after Usebuffer\n", H264_GetDecoderErrorString(eError));
			    goto EXIT;
			}
#ifdef OMX_H264D_SRCHANGES
			TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",pAppData->pInBuff[i]->pBuffer);
pAppData->pInBuff[i]->pBuffer = SharedRegion_getPtr(pAppData->pInBuff[i]->pBuffer);
			TIMM_OSAL_Trace("\npBuffer after UB = %x\n",pAppData->pInBuff[i]->pBuffer); 
#endif
    }
	for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
    printf("\nAllocating buffer no. %d\n",i);
#ifdef OMX_H264D_BUF_HEAP	
        pTmpBuffer = HeapBuf_alloc(heapHandle, pAppData->pOutPortDef->nBufferSize, 0);
#elif defined (OMX_H264D_LINUX_TILERTEST)
		MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
		MemReqDescTiler[0].dim.area.width=256;/*width*/
		MemReqDescTiler[0].dim.area.height=240;/*height*/
		MemReqDescTiler[0].stride=STRIDE_8BIT;
		MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
		MemReqDescTiler[1].dim.area.width=128;/*width*/
		MemReqDescTiler[1].dim.area.height=120;/*height*/
		MemReqDescTiler[1].stride=STRIDE_16BIT;
		TIMM_OSAL_Trace("\nBefore tiler alloc for the Codec Internal buffer %d\n");
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
		TIMM_OSAL_Trace("\nTiler buffer allocated is %x\n",TilerAddr);
		pTmpBuffer = (OMX_U8 *)TilerAddr;
#else
        pTmpBuffer= TIMM_OSAL_Malloc((pAppData->pOutPortDef->nBufferSize), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif		
        if(pTmpBuffer== TIMM_OSAL_NULL) {
			printf("OMX_ErrorInsufficientResources\n");
			goto EXIT;
		}
#ifdef OMX_H264D_SRCHANGES        
			TIMM_OSAL_Trace("\npBuffer before UB = %x\n",pTmpBuffer);
pTmpBuffer = (char *)SharedRegion_getSRPtr(pTmpBuffer, 2);
			TIMM_OSAL_Trace("\npBuffer SR before UB = %x\n",pTmpBuffer);
#endif        
			if(pTmpBuffer== TIMM_OSAL_NULL) {
				printf("OMX_ErrorInsufficientResources\n");
				goto EXIT;
			}
		eError = OMX_UseBuffer(pHandle, &(pAppData->pOutBuff[i]), pAppData->pOutPortDef->nPortIndex, pAppData, (pAppData->pOutPortDef->nBufferSize),pTmpBuffer);						
			if(eError != OMX_ErrorNone)
			{
			    printf("Error %s:    after Usebuffer\n", H264_GetDecoderErrorString(eError));
			    goto EXIT;
			}
#ifdef OMX_H264D_SRCHANGES
			TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",pAppData->pInBuff[j]->pBuffer);
pAppData->pOutBuff[i]->pBuffer = SharedRegion_getPtr(pAppData->pOutBuff[i]->pBuffer);
			TIMM_OSAL_Trace("\npBuffer after UB = %x\n",pAppData->pInBuff[j]->pBuffer);	
#endif	
    }

#endif
#ifdef OMX_H264D_ALLOCBUFFERENABLED
	for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
	        eError = OMX_AllocateBuffer(pHandle, /*&pBufferIn*/&pAppData->pInBuff[i], pAppData->pInPortDef->nPortIndex, pAppData, pAppData->pInPortDef->nBufferSize);
	   }
	  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
	        eError = OMX_AllocateBuffer(pHandle, /*&pBufferOut*/&pAppData->pOutBuff[i], pAppData->pOutPortDef->nPortIndex, pAppData, pAppData->pOutPortDef->nBufferSize);
	  }
	
#endif

    /* Wait for initialization to complete.. Wait for Idle stete of component  */
    eError = H264DEC_WaitForState(pHandle, OMX_StateIdle);
    if(eError != OMX_ErrorNone) {
        printf("Error %s:    WaitForState has timed out \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if(eError != OMX_ErrorNone)
    {
        printf ("Error from SendCommand-Executing State set :%s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    eError = H264DEC_WaitForState(pHandle, OMX_StateExecuting);
    if(eError != OMX_ErrorNone)
    {
        printf("Error %s:    WaitForState has timed out \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }
//#if 0
    for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
        nRead = H264DEC_FillData (pAppData, pAppData->pInBuff[i]);
        if(nRead <= 0) {
            break;
        }
#ifdef SRCHANGES
printf("\npBuffer before ETB = %x\n",pAppData->pInBuff[i]->pBuffer);
pAppData->pInBuff[i]->pBuffer = (char *)SharedRegion_getSRPtr(pAppData->pInBuff[i]->pBuffer, 2);
printf("\npBuffer SR before ETB = %x\n",pAppData->pInBuff[i]->pBuffer);        
#endif
        eError = pAppData->pComponent->EmptyThisBuffer(pHandle, pAppData->pInBuff[i]);
#ifdef SRCHANGES
printf("\npBuffer SR after ETB = %x\n",pAppData->pInBuff[i]->pBuffer);
pAppData->pInBuff[i]->pBuffer = SharedRegion_getPtr(pAppData->pInBuff[i]->pBuffer);
printf("\npBuffer after ETB = %x\n",pAppData->pInBuff[i]->pBuffer);
#endif        
        if(eError != OMX_ErrorNone) {
            printf ("Error from Empty this buffer : %s \n", H264_GetDecoderErrorString(eError));
            goto EXIT;
        }
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
#ifdef OMX_H264D_SRCHANGES
pAppData->pOutBuff[i]->pBuffer = (char *)SharedRegion_getSRPtr(pAppData->pOutBuff[i]->pBuffer, 2);    
#endif
        eError = pAppData->pComponent->FillThisBuffer(pHandle, pAppData->pOutBuff[i]);
#ifdef OMX_H264D_SRCHANGES        
pAppData->pOutBuff[i]->pBuffer = SharedRegion_getPtr(pAppData->pOutBuff[i]->pBuffer);        
#endif
        if(eError != OMX_ErrorNone) {
            printf ("Error from Fill this buffer : %s \n", H264_GetDecoderErrorString(eError));
            goto EXIT;
        }
    }
printf("\nDone with ETB/FTB, calling get state\n");
    eError = OMX_GetState(pHandle, &pAppData->eState);
printf("\nReturned from GetState, state = %d\n", pAppData->eState);
    /* Initialize the number of encoded frames to zero */
    pAppData->nEncodedFrms = 0;

    while ((eError == OMX_ErrorNone) && (pAppData->eState != OMX_StateIdle)) {

        TIMM_OSAL_U32 numRemaining = 0;

        printf("\nWait for an event (input/output/error)\n");
        uRequestedEvents = (H264_DECODER_INPUT_READY | H264_DECODER_OUTPUT_READY | H264_DECODER_ERROR_EVENT | H264_DECODER_END_OF_STREAM);
        tTIMMSemStatus = TIMM_OSAL_EventRetrieve (myEvent, uRequestedEvents,
                TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);
        if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus) {
            printf("Error in creating event!\n");
            eError = OMX_ErrorUndefined;
            goto EXIT;
        }
printf("\nRecd event\n");
    if(pRetrievedEvents & H264_DECODER_END_OF_STREAM)
    {
           //pBufferIn->pBuffer = pInputPtr;
       printf("\n End of stream received");
       printf("\n");
       break;
    }
    

        if (pRetrievedEvents & H264_DECODER_INPUT_READY) {
        printf("\nInput ready recd\n");

            do {
                /*read from the pipe*/
                TIMM_OSAL_ReadFromPipe (pAppData->IpBuf_Pipe, &pBufferIn,
                        sizeof(pBufferIn), &actualSize, TIMM_OSAL_SUSPEND );
                   if(pBufferIn == NULL)
                   {
                   printf("\nNull recd from pipe\n");
                   }
                   else
                   {
                   printf("\nHdr recd from pipe = 0x%x\n",pBufferIn);
                   }

            #if 0
                nRead = H264DEC_FillData(pAppData,pBufferIn);
                if(nRead <= 0) {
                    break;
                }
            #endif
printf("\nComapring\n");
        if(pBufferIn->nAllocLen > pBufferIn->nFilledLen)
        {
        printf("\nalloc len > filled len\n");
        pBufferIn->nOffset = pBufferIn->nFilledLen;
           //pBufferIn->pBuffer = (OMX_U8 *)((OMX_U32)pBufferIn->pBuffer + pBufferIn->nFilledLen);
                   pBufferIn->nFilledLen  = pBufferIn->nAllocLen;
        }
        else
        {
        printf("\nalloc len = filled len\n");
           pBufferIn->nFilledLen = pBufferIn->nAllocLen; 
           pBufferIn->nFlags = OMX_BUFFERFLAG_EOS;
        }

                pBufferIn->nTickCount  = 0;
                //pBufferOut->nTickCount = 0;

     #ifdef H264D_DEBUG
        printf("\n Frame %d Decoded ", ++frames_decoded);
        printf("\n");
     #endif

    if(pBufferIn->pBuffer == NULL)
{
printf("\nBuffer is NULL\n");    
}
#ifdef OMX_H264D_SRCHANGES
printf("\npBuffer before ETB = %x\n",pBufferIn->pBuffer);
pBufferIn->pBuffer = (char *)SharedRegion_getSRPtr(pBufferIn->pBuffer, 2);
printf("\npBuffer SR before ETB = %x\n",pBufferIn->pBuffer);        
#endif
                eError = pAppData->pComponent->EmptyThisBuffer(pHandle, pBufferIn);
#ifdef OMX_H264D_SRCHANGES
printf("\npBuffer SR after ETB = %x\n",pBufferIn->pBuffer);
pBufferIn->pBuffer = SharedRegion_getPtr(pBufferIn->pBuffer);
printf("\npBuffer after ETB = %x\n",pBufferIn->pBuffer);
#endif                  
                if(eError != OMX_ErrorNone) {
                    printf ("Error from Empty this buffer : %s \n", H264_GetDecoderErrorString(eError));
                    goto EXIT;
                }
                TIMM_OSAL_GetPipeReadyMessageCount (pAppData->IpBuf_Pipe, &numRemaining);
            } while (numRemaining);
        }

        if (pRetrievedEvents & H264_DECODER_OUTPUT_READY) {
            do {
		long fieldBuf;
                /*read from the pipe*/
                TIMM_OSAL_ReadFromPipe(pAppData->OpBuf_Pipe, &pBufferOut, sizeof(pBufferOut), &actualSize, TIMM_OSAL_SUSPEND );

                fieldBuf = (long) TIMM_OSAL_Malloc((((720 + (2*PADX)) * (576 + (4*PADY)) * 3) >> 1), TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
		 
		

            #ifdef H264D_WRITEOUTPUT
                TestApp_WriteOutputData
                (
                  pAppData->fOut,
                  (IH264VDEC_OutArgs*)pBufferOut->pInputPortPrivate,
		  fieldBuf, 0
                );
                fflush(pAppData->fOut);
            #endif
                pBufferOut->nFilledLen = 0;
#ifdef OMX_H264D_SRCHANGES
pBufferOut->pBuffer = (char *)SharedRegion_getSRPtr(pBufferOut->pBuffer, 2);    
#endif
                eError = pAppData->pComponent->FillThisBuffer(pHandle, pBufferOut);
#ifdef OMX_H264D_SRCHANGES        
pBufferOut->pBuffer = SharedRegion_getPtr(pBufferOut->pBuffer);        
#endif                
                if(eError != OMX_ErrorNone) {
                    printf ("Error from Fill this buffer : %s \n", H264_GetDecoderErrorString(eError));
                    goto EXIT;
                }

                pAppData->nEncodedFrms++;

		TIMM_OSAL_Free((TIMM_OSAL_PTR)fieldBuf);

                TIMM_OSAL_GetPipeReadyMessageCount (pAppData->OpBuf_Pipe, &numRemaining);
            } while (numRemaining);
        }

        if (pRetrievedEvents & H264_DECODER_ERROR_EVENT) {
            eError = OMX_ErrorUndefined;
        }

        eError = OMX_GetState(pHandle, &pAppData->eState);
    }
//#endif
    eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateIdle, NULL);
    if(eError != OMX_ErrorNone) {
        printf ("Error from SendCommand-Idle State set : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    eError = H264DEC_WaitForState(pHandle, OMX_StateIdle);
    if(eError != OMX_ErrorNone) {
        printf("Error %s:    WaitForState has timed out \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }


    eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateLoaded, NULL);
    if(eError != OMX_ErrorNone) {
        printf ("Error from SendCommand-Loaded State set : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }

    for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++){
#ifdef OMX_H264D_USEBUFFERENABLED
	#ifdef OMX_H264E_BUF_HEAP
       HeapBuf_free(heapHandle, pAppData->pInBuff[i]->pBuffer, pAppData->pInBuff[i]->nAllocLen);
	#elif defined (OMX_H264D_LINUX_TILERTEST)
		MemMgr_Free(pAppData->pInBuff[i]->pBuffer);   
#else
         TIMM_OSAL_Free(pAppData->pInBuff[i]->pBuffer);
#endif    
#endif
        eError = OMX_FreeBuffer(pHandle, pAppData->pInPortDef->nPortIndex,pAppData->pInBuff[i]);
        if(eError != OMX_ErrorNone) {
            printf("Error in OMX_FreeBuffer : %s \n", H264_GetDecoderErrorString(eError));
            goto EXIT;
        }
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++){
#ifdef OMX_H264D_USEBUFFERENABLED
	#ifdef OMX_H264E_BUF_HEAP
       HeapBuf_free(heapHandle, pAppData->pOutBuff[i]->pBuffer, pAppData->pOutBuff[i]->nAllocLen);
	#elif defined (OMX_H264D_LINUX_TILERTEST)
		MemMgr_Free(pAppData->pOutBuff[i]->pBuffer);   
#else
         TIMM_OSAL_Free(pAppData->pOutBuff[i]->pBuffer);
#endif        
#endif
        eError = OMX_FreeBuffer(pHandle, pAppData->pOutPortDef->nPortIndex, pAppData->pOutBuff[i]);
        if(eError != OMX_ErrorNone) {
            printf("Error in OMX_FreeBuffer : %s \n", H264_GetDecoderErrorString(eError));
            goto EXIT;
        }
    }
    
//Added by AD    
    eError = H264DEC_WaitForState(pHandle, OMX_StateLoaded);
    if(eError != OMX_ErrorNone) {
        printf("Error %s:    WaitForState has timed out \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }


    /* De-Initialize OMX Core */
    eError = OMX_Deinit();
    /* Currently this function is dummy. No need to check return */
#if 0
    if (eError != OMX_ErrorNone) {
        printf("Failed to de-init OMX Core! : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }
#endif

    TIMM_OSAL_SemaphoreObtain (pSem_Events, TIMM_OSAL_SUSPEND);

    /* UnLoad the Decoder Component */
    eError = OMX_FreeHandle(pHandle);
    if( (eError != OMX_ErrorNone)) {
        printf ("Error in Free Handle function : %s \n", H264_GetDecoderErrorString(eError));
        goto EXIT;
    }
#ifdef OMX_H264D_LINUX_TILERTEST 
	if(MemReqDescTiler)
	TIMM_OSAL_Free(MemReqDescTiler);
#endif

    tTIMMSemStatus = TIMM_OSAL_SemaphoreDelete(pSem_InputPort);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Delete failed!");
        goto EXIT;
    }
    else
    {
        printf("\nInput sem deleted\n");
    }
    tTIMMSemStatus = TIMM_OSAL_SemaphoreDelete(pSem_OutputPort);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Delete failed!");
        goto EXIT;
    }
    else
    {
        printf("\nOutput sem deleted\n");
    }
    tTIMMSemStatus = TIMM_OSAL_SemaphoreDelete(pSem_Events);
    if(tTIMMSemStatus != TIMM_OSAL_ERR_NONE) {
        printf("Semaphore Delete failed!");
        goto EXIT;
    }
    else
    {
        printf("\nEvent sem deleted\n");
    }

EXIT:

    if(pAppData)
    {
       if(pAppData->fIn)
           fclose(pAppData->fIn);
       printf("\nInput file closed\n");           

       #ifdef H264D_WRITEOUTPUT
          if(pAppData->fOut)
              fclose(pAppData->fOut);
       #endif

       H264DEC_FreeResources(pAppData);

       TIMM_OSAL_Free(pAppData);
       printf("\nAppdata freed\n");
    }

    tTIMMSemStatus = TIMM_OSAL_EventDelete(myEvent);
    if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus) {
        printf("Error in creating event!\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    else
    {
        printf("\nMy event deleted\n");
    }
	tTIMMSemStatus =TIMM_OSAL_EventDelete(H264VD_CmdEvent);
	if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus) {
        printf("Error in creating event!\n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    else
    {
        printf("\nCmd event deleted\n");
    }

    printf("\n");
    printf("\n H264 Decoder Test End");

    //mem_count_end = TIMM_OSAL_GetMemCounter();
    //mem_size_end = TIMM_OSAL_GetMemUsage();
    //printf("\n Value from GetMemCounter = %d", mem_count_end);
    //printf("\n Value from GetMemUsage = %d", mem_size_end);
/*
    if(mem_count_start != mem_count_end)
    {
            printf("\n Memory leak detected. Bytes lost = %d", (mem_size_end - mem_size_start));
    }
*/    
    printf("\n-----------------------------------------------");
    printf("\n");
    
      if(input != 100)
          break;

      if(main_input == 1 && !feof(fconfigFile))
      {
         ii = 0;
      }
      else
      {
         continue;
      }

    }
    printf("\nCalling platform deinit()\n");
    mmplatform_deinit();
    printf("\nPlatform deinitialized\n");
}
