/* ======================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ======================================================================= */
/**
* @file omx_jpegd.c
*
* This File contains testcases for the omx jpeg Decoder.
*
* @path
*
* @rev  0.1
*/
/* ----------------------------------------------------------------------------
*!
*! Revision History
*! =======================================================================
*!  Initial Version
* ========================================================================*/

/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

/*-------program files ----------------------------------------*/
#include <timm_osal_interfaces.h>
#include <timm_osal_trace.h>
#include <timm_osal_error.h>
#include <timm_osal_memory.h>

#include <omx_types.h>
#include <omx_core.h>
#include <omx_image.h>
#include <omx_index.h>
#include <omx_ti_index.h>
#include <omx_ivcommon.h>

#include "omx_jpegd_util.h"
#include "omx_jpegd_test.h"
#include "omx_jpegd.h"

#define OUTPUT_FILE "JPEGD_0001_320x240_420.jpg"
#define REF_OUTPUT_FILE "Ref_JPEGD_0001_320x240_420.yuv"


#define OMX_LINUX_TILERTEST

#ifdef OMX_LINUX_TILERTEST
/*Tiler APIs*/
#include <memmgr/memmgr.h>
#include <memmgr/tiler.h>
#endif

FILE *op_file,*ref_op_file;
   int ch1,ch2;
    int pass;
    int while_pass=0,loc_diff=0;



/* PAK
extern void BigEndianRead32Bits(Uint32 *pVal, Uint8* ptr);
extern void BigEndianRead16Bits(Uint16 *pVal, Uint8* ptr);*/
/*extern TIMM_OSAL_U32 TI_GetThumbnailInformation(Uint8 *pBuffer,
                                          Uint32 *pNextIFDOffset,
                                          EXIF_THUMBNAIL_INFO *pThumbnail,
                                          Uint32 ulLittleEndian);*/
OMX_ERRORTYPE JPEGD_releaseOnSemaphore(OMX_U16 unSemType);
OMX_Status JPEGD_ParseTestCases (uint32_t uMsg, void *pParam, uint32_t paramSize);

/*
OMX_ERRORTYPE TI_JpegOcpCore_ParseJpegHeader( OMX_PTR hTObj, OMX_U32 nInputBitstreamSize);
OMX_ERRORTYPE TI_GetExifInformation(OMX_EXIF_INFO_SUPPORTED *pExif,
                                                            OMX_U8 *pExifBuffer);
*/
/* Semaphore type indicators */

#define SEMTYPE_DUMMY  0
#define SEMTYPE_EVENT  1
#define SEMTYPE_ETB    2
#define SEMTYPE_FTB    3

#define MAX_4BIT 15
/*
#define THC_Trace(ARGS,...)  TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARGS,##__VA_ARGS__)
#define THC_Error(ARGS,...)   TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARGS,##__VA_ARGS__)
#define THC_Entering   TIMM_OSAL_EnteringExt
#define THC_Exiting(ARG) TIMM_OSAL_ExitingExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARG)*/
#define JPEGD_Trace(ARGS,...)  TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARGS,##__VA_ARGS__)
#define JPEGD_Error(ARGS,...)   TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARGS,##__VA_ARGS__)
#define JPEGD_Entering   TIMM_OSAL_EnteringExt
#define JPEGD_Exiting(ARG) TIMM_OSAL_ExitingExt(TIMM_OSAL_TRACEGRP_OMXIMGDEC,ARG)


/* Jpeg Decoder input and output file names along with path */
OMX_U8 omx_jpegd_input_file_path_and_name[MAX_STRING_SIZE];
OMX_U8 omx_jpegd_output_file_path_and_name[MAX_STRING_SIZE];

OMX_STRING strJPEGD = "OMX.TI.DUCATI1.IMAGE.JPEGD";

/* Test context info */
JpegDecoderTestObject TObjD ;

//#define OMX_JPEGD_INPUT_PATH  "..\\..\\..\\..\\..\\alg\\jpeg_dec\\test\\test_vectors\\input\\"
//#define OMX_JPEGD_OUTPUT_PATH "..\\..\\..\\..\\..\\alg\\jpeg_dec\\test\\test_vectors\\output\\"
#define OMX_JPEGD_INPUT_PATH "/camera_bin/"
#define OMX_JPEGD_OUTPUT_PATH "/camera_bin/"

/* ===================================================================== */
/**
* @fn JPEGD_TEST_CalculateBufferSize   Calculates the buffer size for the JPEG Decoder testcases
*
* @param[in] width      Message code.
* @param[in] height     pointer to Function table entry
* @param[in] format    size of the Function table entry structure
*
* @return
*       Size of the buffer
*
*/
/* ===================================================================== */

OMX_S32 JPEGD_TEST_CalculateBufferSize ( OMX_IN OMX_S32 width,
                                         OMX_IN OMX_S32 height,
                                         OMX_IN OMX_COLOR_FORMATTYPE format)
{
    OMX_S32 size;

    if(format == OMX_COLOR_FormatYUV420PackedSemiPlanar)
    {
        if((width & 0x0F)!=0)
    	     width = width + 16 - (width & 0x0F);
        if((height & 0x0F)!=0)
            height = height + 16 - (height & 0x0F);
        size =  ((width * height) + ((width*height)/2));

    }
    else if(format == OMX_COLOR_FormatCbYCrY)
    {
        if((width & 0x0F)!=0)
    	     width = width + 16 - (width & 0x0F);
        if((height & 0x7)!=0)
            height = height + 8 - (height & 0x7);
        size =  ((width * height)*2.0);

    }
    else if (format == OMX_COLOR_Format16bitRGB565)
    {
        size = ((width * height)*3);
    }
    else if (format == OMX_COLOR_Format32bitARGB8888)
    {
        size = ((width * height)*4);
    }
    else
        size = 0;

    return(size);
}



/* ==================================================================== */
/**
* @fn JPEGD_TEST_UCP_CalculateSliceBufferSize  Executes the JPEG Decoder testcases
*
* @param[in] width      Message code.
* @param[in] height     pointer to Function table entry
* @param[in] format    size of the Function table entry structure
*
* @return
*
*  @see  <headerfile>.h
*/
/* ==================================================================== */

OMX_S32 JPEGD_TEST_UCP_CalculateSliceBufferSize(
                                                       OMX_IN OMX_S32 width,
                                                       OMX_IN OMX_S32 height,
                                                       OMX_IN OMX_S32 SliceHeight,
                                                       OMX_IN OMX_COLOR_FORMATTYPE format)
{

    OMX_S32 size;

    if(format == OMX_COLOR_FormatYUV420PackedSemiPlanar)
    	{
    	   if((width & 0x0F)!=0)
    	      width = width + 16 - (width & 0x0F);
		   if((height & 0x0F)!=0)
		      height = height + 16 - (height & 0x0F);
		   if(SliceHeight==0)
              size =  ((width * height) + ((width*height)/2));
		   else
		   	 size = width * SliceHeight * 1.5;

    	}
    else if(format == OMX_COLOR_FormatYCbYCr)
    	{
    	    if((width & 0x0F)!=0)
    	      width = width + 16 - (width & 0x0F);
	    if((height & 0x7)!=0)
		      height = height + 8 - (height & 0x7);
	    if(SliceHeight==0)
                size =  width * height * 2;
	    else
		  size = width * SliceHeight * 2;

    	}
    else if(format == OMX_COLOR_Format16bitRGB565)
    	{
    	  if((width & 0x7)!=0)
    	      width = width + 8 - (width & 0x7);
	  if((height & 0x7)!=0)
		      height = height + 8 - (height & 0x7);
	  if(SliceHeight==0)
             size = width * height;
	  else
	      size = width * SliceHeight * 2;

    	}
    else if(format == OMX_COLOR_Format32bitARGB8888)
    	{
    	  if((width & 0x7)!=0)
    	      width = width + 8 - (width & 0x7);
	  if((height & 0x7)!=0)
		      height = height + 8 - (height & 0x7);
	  if(SliceHeight==0)
             size = width * height;
	  else
	      size = width * SliceHeight * 4;

    	}
    else
        size = 0;


    return(size);
}



/* Application callback Functions */

/* ==================================================================== */
/**
* @fn OMX_JPEGD_TEST_EventHandler  Handles various events like command complete etc
*
* @param[in] hComponent   Handle of the component to be accessed.
* @param[in] pAppData     pointer to Function table entry
* @param[in] eEvent       Event the component notifies to the application
* @param[in] nData1       OMX_ERRORTYPE or OMX_COMMANDTYPE for error or command event
* @param[in] nData2       OMX_STATETYPE for state changes
* @param[in] pEventData   Pointer to additional event specific data
*
* @return
*     OMX_RET_PASS if the test passed
*     OMX_RET_FAIL if the test fails, or possibly other special conditions.
*
*/
/* ==================================================================== */

OMX_ERRORTYPE OMX_JPEGD_TEST_EventHandler ( OMX_IN OMX_HANDLETYPE hComponent,
                                            OMX_IN OMX_PTR pAppData,
                                            OMX_IN OMX_EVENTTYPE eEvent,
                                            OMX_IN OMX_U32 nData1,
                                            OMX_IN OMX_U32 nData2,
                                            OMX_IN OMX_PTR pEventData)
{

    OMX_ERRORTYPE eError = (OMX_ERRORTYPE)nData1;

    /* Error event is reported through callback */

    if(eEvent == OMX_EventError)
      {
	      eError = JPEGD_releaseOnSemaphore(SEMTYPE_EVENT);
             JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
			         "Error while releasing Event semaphore");
      }


    /* CommandComplete event is reported through callback */

    else if(eEvent == OMX_EventCmdComplete)
    {
        if(nData1 == OMX_CommandStateSet)
        {
            if(nData2 == OMX_StateIdle)
            {
                eError = JPEGD_releaseOnSemaphore(SEMTYPE_EVENT);
	            JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  	                "Error while releasing Event semaphore");
            }
 	        else if(nData2 == OMX_StateExecuting)
            {
                eError = JPEGD_releaseOnSemaphore(SEMTYPE_EVENT);
	    	    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  	                "Error while releasing Event semaphore");
 	        }

            /* Till now not supported */
 	        else if(nData2 == OMX_StatePause)
            {
                eError = JPEGD_releaseOnSemaphore(SEMTYPE_EVENT);
		        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  	                 "Error while releasing Event semaphore");
 	        }
	        else if(nData2 == OMX_StateLoaded)
            {
                eError = JPEGD_releaseOnSemaphore(SEMTYPE_EVENT);
		        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  	                   "Error while releasing Event semaphore");
            }
        }
	    else if(nData1 == OMX_CommandFlush)
		{

	    }
    }

EXIT:
    return eError;
}



/* =================================================================== */
/**
* @fn OMX_JPEGD_TEST_EmptyBufferDone   Returns emptied buffers from I/p port to application
*
* @param[in] hComponent       Handle of the component to be accessed.
* @param[in] pAppData         pointer to Function table entry
* @param[in] pBufferHeader    size of the Function table entry structure
*
*
* @return
*     OMX_RET_PASS if the test passed
*     OMX_RET_FAIL if the test fails, or possibly other special conditions.
*
*/
/* =================================================================== */

OMX_ERRORTYPE OMX_JPEGD_TEST_EmptyBufferDone ( OMX_IN OMX_HANDLETYPE hComponent,
                                               OMX_IN OMX_PTR pAppData,
                                               OMX_IN OMX_BUFFERHEADERTYPE* pBufferHeader)
{

    OMX_ERRORTYPE eError = OMX_ErrorUndefined;

    JPEGD_Trace("OMX_JPEGD_TEST_EmptyBufferDone");

    if(pBufferHeader->nAllocLen == 0)
        JPEGD_Trace("[ZERO length]");

    if(pBufferHeader->nFlags & OMX_BUFFERFLAG_EOS){
        pBufferHeader->nFlags &= ~OMX_BUFFERFLAG_EOS;
        JPEGD_Trace("[EOS detected]");
    }

    /* Release EmptyThisBuffer semaphore */
    eError = JPEGD_releaseOnSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error while releasing EmptyThisBuffer semaphore");

    eError = OMX_ErrorNone;
EXIT:
    return eError;

}

/* ==================================================================== */
/**
* @fn OMX_JPEGD_TEST_FillBufferDone  Returns filled buffers from O/P port to the application for emptying
*
* @param[out] hComponent     Handle of the component to be accessed.
* @param[out] pAppData       pointer to Function table entry
* @param[out] pBufferHeader  size of the Function table entry structure
*
* @return
*     OMX_RET_PASS if the test passed
*     OMX_RET_FAIL if the test fails,or possibly other special conditions.
*
*/
/* ===================================================================== */

OMX_ERRORTYPE OMX_JPEGD_TEST_FillBufferDone ( OMX_OUT OMX_HANDLETYPE hComponent,
                                              OMX_OUT OMX_PTR pAppData,
                                              OMX_OUT OMX_BUFFERHEADERTYPE* pBufferHeader)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    JpegDecoderTestObject  *hTObj = &(TObjD);

    JPEGD_Trace("OMX_JPEGD_TEST_FillBufferDone");


    /* Update the output buffer */
    hTObj->pOutBufHeader->nOutputPortIndex     = pBufferHeader->nOutputPortIndex;
    hTObj->pOutBufHeader->pBuffer              = pBufferHeader->pBuffer;
    hTObj->pOutBufHeader->nFilledLen           = pBufferHeader->nFilledLen;
    hTObj->pOutBufHeader->nOffset              = pBufferHeader->nOffset;
    hTObj->pOutBufHeader->nAllocLen            = pBufferHeader->nAllocLen;
    hTObj->pOutBufHeader->nFlags               = pBufferHeader->nFlags;
    hTObj->pOutBufHeader->pAppPrivate          = pBufferHeader->pAppPrivate;

    if(hTObj->pOutBufHeader->nOutputPortIndex == OMX_JPEGD_TEST_OUTPUT_PORT)
     {
        JPEGD_Trace("output");
     }
    else
        JPEGD_Trace("Unknown port");


    if(hTObj->pOutBufHeader->nAllocLen == 0)
        JPEGD_Trace("[ZERO length]");

    if(hTObj->pOutBufHeader->nFlags & OMX_BUFFERFLAG_EOS)
     {
        hTObj->pOutBufHeader->nFlags &= ~OMX_BUFFERFLAG_EOS;
        JPEGD_Trace("[EOS detected]");
     }

    /* Release FillThisBuffer semaphore */
    eError = JPEGD_releaseOnSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error while releasing FillThisBuffer semaphore");

	eError = OMX_ErrorNone;
EXIT:
    return eError;


}


/* ===================================================================== */
/*
 * @fn JPEGD_pendOnSemaphore()
 * Pends on a semaphore
 * unSemType => 1 : Event Semaphore 2: EmptyThisBuffer semaphore 3: FillThisBuffer semaphore
 */
/* ===================================================================== */

OMX_ERRORTYPE JPEGD_pendOnSemaphore (OMX_U16 unSemType)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    JpegDecoderTestObject  *hTObj = &(TObjD);


    if(unSemType == SEMTYPE_DUMMY)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreObtain(hTObj->pOmxJpegdecSem_dummy,
			                                      TIMM_OSAL_SUSPEND);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
			                       OMX_ErrorInsufficientResources,"");
    }
    else if(unSemType == SEMTYPE_EVENT)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreObtain(hTObj->pOmxJpegdecSem_events,
			                                       TIMM_OSAL_SUSPEND);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
			                   OMX_ErrorInsufficientResources,"");
    }
    else if(unSemType == SEMTYPE_ETB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreObtain(hTObj->pOmxJpegdecSem_ETB,
			                                        TIMM_OSAL_SUSPEND);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
			                             OMX_ErrorInsufficientResources,"");
    }
    else if(unSemType == SEMTYPE_FTB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreObtain(hTObj->pOmxJpegdecSem_FTB,
			                                        TIMM_OSAL_SUSPEND);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
			                           OMX_ErrorInsufficientResources,"");
    }

	eError = OMX_ErrorNone;
EXIT:
    return eError;
}



/* =================================================================== */
/*
 * @fn JPEGD_releaseOnSemaphore()
 * Pends on a semaphore
 * unSemType => 1 : Event Semaphore 2: EmptyThisBuffer semaphore 3: FillThisBuffer semaphore
 */
/* =================================================================== */

OMX_ERRORTYPE JPEGD_releaseOnSemaphore(OMX_U16 unSemType)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    JpegDecoderTestObject  *hTObj = &(TObjD);

    if(unSemType == SEMTYPE_DUMMY)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreRelease(hTObj->pOmxJpegdecSem_dummy);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
                                      "Error while relasing dummy semaphore");
    }
    else if(unSemType == SEMTYPE_EVENT)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreRelease(hTObj->pOmxJpegdecSem_events);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
                                      "Error while relasing Event semaphore");
    }
    else if(unSemType == SEMTYPE_ETB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreRelease(hTObj->pOmxJpegdecSem_ETB);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
                                      "Error while relasing EmptyThisBuffer semaphore");
    }
    else if(unSemType == SEMTYPE_FTB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreRelease(hTObj->pOmxJpegdecSem_FTB);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
                                      "Error while relasing FillThisBuffer semaphore");
    }

	eError = OMX_ErrorNone;
EXIT:
    return eError;

}

/* ==================================================================== */
/*
 * @fn JPEGD_CreateSemaphore()
 * Pends on a semaphore
 * unSemType => 1 : Event Semaphore, 2: EmptyThisBuffer semaphore, 3: FillThisBuffer semaphore
 */
/* ==================================================================== */

OMX_ERRORTYPE JPEGD_CreateSemaphore (OMX_U16 unSemType)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    JpegDecoderTestObject  *hTObj = &(TObjD);


    if(unSemType == SEMTYPE_DUMMY)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreCreate(&hTObj->pOmxJpegdecSem_dummy,0);
        JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating dummy semaphore");
    }
    else if(unSemType == SEMTYPE_EVENT)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreCreate(&hTObj->pOmxJpegdecSem_events,0);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating event semaphore");
    }
    else if(unSemType == SEMTYPE_ETB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreCreate(&hTObj->pOmxJpegdecSem_ETB,0);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating EmptyThisBuffer semaphore");
    }
    else if(unSemType == SEMTYPE_FTB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreCreate(&hTObj->pOmxJpegdecSem_FTB,0);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating FillThisBuffer semaphore");
    }

	eError = OMX_ErrorNone;
EXIT:
    return eError;

}

/* ===================================================================== */
/*
 * @fn JPEGD_DeleteSemaphore()
 * Pends on a semaphore
 * unSemType => 0 : Dummy Semaphore 1 : Event Semaphore 2: EmptyThisBuffer semaphore  3:FillThisBuffer semaphore
 */
/* ===================================================================== */

OMX_ERRORTYPE JPEGD_DeleteSemaphore (OMX_U16 unSemType)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    JpegDecoderTestObject  *hTObj = &(TObjD);


    if(unSemType == SEMTYPE_DUMMY)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreDelete(hTObj->pOmxJpegdecSem_dummy);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating dummy semaphore");
    }
    else if(unSemType == SEMTYPE_EVENT)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreDelete(hTObj->pOmxJpegdecSem_events);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating event semaphore");
    }
    else if(unSemType == SEMTYPE_ETB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreDelete(hTObj->pOmxJpegdecSem_ETB);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating EmptyThisBuffer semaphore");
    }
    else if(unSemType == SEMTYPE_FTB)
    {
        bReturnStatus = TIMM_OSAL_SemaphoreDelete(hTObj->pOmxJpegdecSem_FTB);
	JPEGD_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		                      "Error creating FillThisBuffer semaphore");
    }

	eError = OMX_ErrorNone;
EXIT:
    return eError;

}



/* ===================================================================== */
/**
* @fn JPEGD_ParseTestCases     Parse the parameters passed to the JPEG Decoder
*
* @param[in] uMsg         Message code.
* @param[in] pParam       pointer to Function table entry containing test case parameters
* @param[in] paramSize    size of the Function table entry structure
*
* @return
*     OMX_STATUS_OK if the testcase parameters are parsed correctly,
*     OMX_ERROR_INVALID_PARAM if there are wrong parameters.
*
*/
/* ===================================================================== */
OMX_Status JPEGD_ParseTestCases(uint32_t uMsg, void *pParam, uint32_t paramSize)

{
    OMX_Status status = OMX_STATUS_OK;
    OMX_TEST_CASE_ENTRY *testcaseEntry;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    char testData[MAX_STRING_SIZE];
    char *subStr;
    char *token[30] = {};
    OMX_S32 count = 0;
    JpegDecoderTestObject  *hTObj = &TObjD;
    testcaseEntry = (OMX_TEST_CASE_ENTRY *)pParam;

    /* Point to test case array */
    if(uMsg != OMX_MSG_EXECUTE)
    {
        status = OMX_ERROR_INVALID_PARAM;
        goto EXIT;
    }

    /* Parse test case parameters */
    testcaseEntry->pDynUserData = testcaseEntry->pUserData;
    strcpy(testData,testcaseEntry->pDynUserData);
    subStr = strtok(testData," ");

    while(subStr != NULL)
    {
        token[count] = subStr;
        count++;
        subStr = strtok(NULL," ");
    }

    /* Parse input file name along with path*/
    strcpy((char *)omx_jpegd_input_file_path_and_name, OMX_JPEGD_INPUT_PATH);
    strcat((char *)omx_jpegd_input_file_path_and_name, token[0]);
    JPEGD_Trace("omx_jpegd_input_file_path_and_name: %s", omx_jpegd_input_file_path_and_name);

    /* Parse output file name along with path*/
    strcpy((char *)omx_jpegd_output_file_path_and_name, OMX_JPEGD_OUTPUT_PATH);
    strcat((char *)omx_jpegd_output_file_path_and_name, token[1]);
    JPEGD_Trace("omx_jpegd_output_file_path_and_name: %s", omx_jpegd_output_file_path_and_name);

    /* Parse input image width value */
    hTObj->tTestcaseParam.unOpImageWidth = atoi(token[2]);
    JPEGD_Trace("Input Width: %d", hTObj->tTestcaseParam.unOpImageWidth);

    /* Parse input image height value */
    hTObj->tTestcaseParam.unOpImageHeight = atoi(token[3]);
    JPEGD_Trace("Input Height: %d", hTObj->tTestcaseParam.unOpImageHeight);

    /* Parse slice height value */
    hTObj->tTestcaseParam.unSliceHeight = atoi(token[4]);
    JPEGD_Trace("Slice Height: %d", hTObj->tTestcaseParam.unSliceHeight);


    /* Parse input image color format */
    if(!strcmp((char*)token[5], "OMX_COLOR_FormatCbYCrY"))
    {
        hTObj->tTestcaseParam.eInColorFormat = OMX_COLOR_FormatCbYCrY;
        JPEGD_Trace("Input Image color format is OMX_COLOR_FormatCbYCrY");
    }
    else if(!strcmp((char*)token[5], "OMX_COLOR_FormatYUV420PackedSemiPlanar"))
    {
	 hTObj->tTestcaseParam.eInColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;
	 JPEGD_Trace("Input Image color format is OMX_COLOR_FormatYUV420PackedSemiPlanar");
    }
    else if(!strcmp((char*)token[5], "OMX_COLOR_FormatYUV444Interleaved"))
    {
        hTObj->tTestcaseParam.eInColorFormat = OMX_COLOR_FormatYUV444Interleaved;
        JPEGD_Trace("Input Image color format is OMX_COLOR_FormatYUV444Interleaved");
    }	
    else
    {
        JPEGD_ASSERT(0,OMX_ErrorBadParameter,"Unsupported color format parameter\n");
    }

    /* Parse output image color format */
    if(!strcmp((char*)token[6], "OMX_COLOR_FormatCbYCrY"))
    {
        hTObj->tTestcaseParam.eOutColorFormat = OMX_COLOR_FormatCbYCrY;
        JPEGD_Trace("Output Image color format is OMX_COLOR_FormatCbYCrY");
    }
    else if(!strcmp((char*)token[6], "OMX_COLOR_FormatYUV420PackedSemiPlanar"))
    {
        hTObj->tTestcaseParam.eOutColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;
        JPEGD_Trace("Output Image color format is OMX_COLOR_FormatYUV420PackedSemiPlanar");
    }
    else if(!strcmp((char*)token[6], "OMX_COLOR_Format16bitRGB565"))
    {
        hTObj->tTestcaseParam.eOutColorFormat = OMX_COLOR_Format16bitRGB565;
        JPEGD_Trace("Output Image color format is OMX_COLOR_Format16bitRGB565");
    }
    else if(!strcmp((char*)token[6], "OMX_COLOR_Format32bitARGB8888"))
    {
        hTObj->tTestcaseParam.eOutColorFormat = OMX_COLOR_Format32bitARGB8888;
        JPEGD_Trace("Output Image color format is OMX_COLOR_Format32bitARGB8888");
    }
    else
    {
        JPEGD_ASSERT(0, OMX_ErrorBadParameter, "Unsupported color format parameter\n");
    }

     /* Parse input uncompressed image mode value  */
    if(!strcmp((char*)token[7],"OMX_JPEG_UncompressedModeFrame"))
    {
        hTObj->tTestcaseParam.eOutputImageMode = OMX_JPEG_UncompressedModeFrame;
        JPEGD_Trace("Uncompressed image mode: OMX_JPEG_UncompressedModeFrame");
    }
    else if(!strcmp((char*)token[7],"OMX_JPEG_UncompressedModeSlice")) 
    {
	 hTObj->tTestcaseParam.eOutputImageMode = OMX_JPEG_UncompressedModeSlice;
	 JPEGD_Trace("Uncompressed image mode: OMX_JPEG_UncompressedModeSlice");
    }
    else
    {
        JPEGD_ASSERT(0,OMX_ErrorBadParameter,"Unsupported color format parameter\n");
    }


    /* Parse X Origin value */
    hTObj->pSubRegionDecode.nXOrg = atoi (token[8]);
    JPEGD_Trace("X Origin: %d", hTObj->pSubRegionDecode.nXOrg);

    /* Parse X Origin value */
    hTObj->pSubRegionDecode.nYOrg = atoi (token[9]);
    JPEGD_Trace("Y Origin: %d", hTObj->pSubRegionDecode.nYOrg);


    /* Parse X Length value */
    hTObj->pSubRegionDecode.nXLength = atoi (token[10]);
    JPEGD_Trace("X Length: %d", hTObj->pSubRegionDecode.nXLength);

    /* Parse Y Length value */
    hTObj->pSubRegionDecode.nYLength = atoi (token[11]);
    JPEGD_Trace("Y Length: %d", hTObj->pSubRegionDecode.nYLength);


    /* Read bIsApplnMemAllocatorforInBuff flag */
    if(!strcmp((char*)token[12],"OMX_TRUE"))
    {
        hTObj->tTestcaseParam.bIsApplnMemAllocatorforInBuff = OMX_TRUE;
        JPEGD_Trace("bIsApplnMemAllocatorforInBuff = OMX_TRUE");
    }
    else if(!strcmp((char*)token[12],"OMX_FALSE"))
    {
        hTObj->tTestcaseParam.bIsApplnMemAllocatorforInBuff = OMX_FALSE;
        JPEGD_Trace(" bIsApplnMemAllocatorforInBuff = OMX_FALSE");
    }
    else
    {
        JPEGD_ASSERT(0,OMX_ErrorBadParameter,"Invalid test case parameter value\n");
    }

    /* Read bIsApplnMemAllocatorforOutBuff flag */
    if(!strcmp((char*)token[13],"OMX_TRUE"))
    {
        hTObj->tTestcaseParam.bIsApplnMemAllocatorforOutBuff = OMX_TRUE;
        JPEGD_Trace("bIsApplnMemAllocatorforOutBuff = OMX_TRUE");
    }
    else if(!strcmp((char*)token[13],"OMX_FALSE"))
    {
        hTObj->tTestcaseParam.bIsApplnMemAllocatorforOutBuff = OMX_FALSE;
        JPEGD_Trace("bIsApplnMemAllocatorforOutBuff = OMX_FALSE");
    }
    else
    {
        JPEGD_ASSERT(0,OMX_ErrorBadParameter,"Invalid test case parameter value\n");
    }

    eError = OMX_ErrorNone;

EXIT:
    if(!eError ==  OMX_ErrorNone)
    {
        status = OMX_ERROR_INVALID_PARAM;
    }
    return status;
}


/* ==================================================================== */
/**
  
}; * @fn JPEGD_TestFrameMode    Executes the JPEG Decoder testcases For Frame mode
*
* @param[in] uMsg         Message code.
* @param[in] pParam       pointer to Function table entry
* @param[in] paramSize    size of the Function table entry structure
*
* @return
*     OMX_RET_PASS if the test passed,
*     OMX_RET_FAIL if the test fails,or possibly other special conditions.
*
*/
/* ==================================================================== */


OMX_TestStatus JPEGD_TestFrameMode (uint32_t uMsg, void *pParam, uint32_t paramSize)
{
    OMX_Status status = OMX_STATUS_OK;
    OMX_TestStatus testResult = OMX_RET_PASS;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_CALLBACKTYPE tJpegDecoderCb;
    OMX_U32 nframeSize,nInputBitstreamSize;
    OMX_U32 *p_in, *p_out;
    OMX_U32 unOpImageWidth,unOpImageHeight,unSliceHeight;
    OMX_COLOR_FORMATTYPE eInColorFormat,eOutColorFormat;
    OMX_JPEG_UNCOMPRESSEDMODETYPE eOutputImageMode;
    OMX_BOOL bIsApplnMemAllocatorforInBuff, bIsApplnMemAllocatorforOutBuff;
    FILE *ipfile,*opfile;
    OMX_S32  nreadSize;
    OMX_COMPONENTTYPE *component;
    OMX_U32 actualSize = 0;
    JpegDecoderTestObject *hTObj= &TObjD;
    int totalRead = 0;
    char *currPtr = NULL;
#ifdef OMX_LINUX_TILERTEST
	MemAllocBlock *MemReqDescTiler;
	OMX_PTR TilerAddrIn=NULL;
	OMX_PTR TilerAddrOut=NULL;
#endif


    JPEGD_Trace("*** Test Case : %s ***", __FUNCTION__);
	printf("\n P JPEGD_TestFrameMode ");

    status = JPEGD_ParseTestCases(uMsg,pParam,paramSize);

    if(status != OMX_STATUS_OK)
    {
        testResult = OMX_RET_FAIL;
	 printf("\n P FAIL ");
	    goto EXIT;
    }

    unOpImageWidth = hTObj->tTestcaseParam.unOpImageWidth;
    unOpImageHeight = hTObj->tTestcaseParam.unOpImageHeight;
    unSliceHeight = hTObj->tTestcaseParam.unSliceHeight;
    eInColorFormat = hTObj->tTestcaseParam.eInColorFormat;
    eOutColorFormat = hTObj->tTestcaseParam.eOutColorFormat;
    eOutputImageMode = hTObj->tTestcaseParam.eOutputImageMode;
    bIsApplnMemAllocatorforInBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforInBuff;
    bIsApplnMemAllocatorforOutBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforOutBuff;

    /* Input file size */
    ipfile = fopen((const char *)omx_jpegd_input_file_path_and_name, "rb");
    if(ipfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s", omx_jpegd_input_file_path_and_name);
	 printf("\n P Error in opening input file ");
        goto EXIT;
    }
    else
    {
        fseek(ipfile, 0, SEEK_END);
        nInputBitstreamSize = ftell(ipfile); //-bitstream_start;
        fseek(ipfile, 0, SEEK_SET);
        fclose(ipfile);
    }

    nframeSize = JPEGD_TEST_CalculateBufferSize(unOpImageWidth, unOpImageHeight, eOutColorFormat);

    /**************************************************************************************/

    /* Create a dummy semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating dummy semaphore");
    JPEGD_Trace("Dummy semaphore created");
 printf("\n P Dummy Semaphore Created ");

    /* Create an Event semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating Event semaphore");
    JPEGD_Trace("Event semaphore created");
 printf("\n P Event Semaphore Created ");

    /* Create an EmptyThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore created");
 printf("\n P EmplyThisBuffer semaphore created ");

    /* Create a FillThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore created");
 printf("\n P FillThisBuffer Semaphore Created ");


    /***************************************************************************************/

    /* Set call back functions */
    tJpegDecoderCb.EmptyBufferDone = OMX_JPEGD_TEST_EmptyBufferDone;
    tJpegDecoderCb.EventHandler = OMX_JPEGD_TEST_EventHandler;
    tJpegDecoderCb.FillBufferDone = OMX_JPEGD_TEST_FillBufferDone;

    /* Load the OMX_JPG_DEC Component */
    hTObj->hOMXJPEGD = NULL;
    eError= OMX_GetHandle(&hTObj->hOMXJPEGD, strJPEGD, hTObj,
		                          &tJpegDecoderCb);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error while obtaining component handle");
    JPEGD_Trace("OMX_GetHandle successful");
 printf("\n P OMX_GetHandle successful ");

    /* Handle of the component to be accessed */
    component = (OMX_COMPONENTTYPE *)hTObj->hOMXJPEGD;

    /*Initialize size and version information for all structures*/
    JPEGD_STRUCT_INIT(hTObj->tJpegdecPortInit,OMX_PORT_PARAM_TYPE);
    JPEGD_STRUCT_INIT(hTObj->tInputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);
    JPEGD_STRUCT_INIT(hTObj->tOutputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);
    JPEGD_STRUCT_INIT(hTObj->tUncompressedMode, OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE);
    JPEGD_STRUCT_INIT(hTObj->pSubRegionDecode, OMX_IMAGE_PARAM_DECODE_SUBREGION);

    /***************************************************************************************/

    /*  Test Set Parameters OMX_PORT_PARAM_TYPE */
    hTObj->tJpegdecPortInit.nPorts           = 2;
    hTObj->tJpegdecPortInit.nStartPortNumber = 0;
    eError = OMX_SetParameter (component, OMX_IndexParamImageInit,&hTObj->tJpegdecPortInit);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		          "Error in Set Parameters OMX_PORT_PARAM_TYPE ");
    JPEGD_Trace("OMX_SetParameter image param done");
 printf("\n P OMX_SetParameter image param done ");

    /* Test Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for input port */
    hTObj->tInputPortDefnType.nPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->tInputPortDefnType.eDir = OMX_DirInput;
    hTObj->tInputPortDefnType.nBufferCountActual = 1;
    hTObj->tInputPortDefnType.nBufferCountMin    = 1;
    hTObj->tInputPortDefnType.nBufferSize        = nInputBitstreamSize;
    hTObj->tInputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tInputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tInputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tInputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tInputPortDefnType.format.image.nFrameWidth   = nInputBitstreamSize;
    hTObj->tInputPortDefnType.format.image.nFrameHeight  = 1;
    hTObj->tInputPortDefnType.format.image.nStride       = 1;
    hTObj->tInputPortDefnType.format.image.nSliceHeight  = 1;
    hTObj->tInputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tInputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    hTObj->tInputPortDefnType.format.image.eColorFormat = eInColorFormat;
    hTObj->tInputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tInputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tInputPortDefnType.nBufferAlignment = 0;

    eError = OMX_SetParameter(component, OMX_IndexParamPortDefinition,
		                         &hTObj->tInputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for input port");
    JPEGD_Trace("OMX_SetParameter input port defn done successfully");
 printf("\n P OMX_SetParameter input port defn done successfully");

    /* Test Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for output port */
    hTObj->tOutputPortDefnType.nPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;
    hTObj->tOutputPortDefnType.eDir = OMX_DirOutput;
    hTObj->tOutputPortDefnType.nBufferCountActual = 1;
    hTObj->tOutputPortDefnType.nBufferCountMin    = 1;
    hTObj->tOutputPortDefnType.nBufferSize = nframeSize;
    hTObj->tOutputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tOutputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tOutputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tOutputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tOutputPortDefnType.format.image.nFrameWidth   = unOpImageWidth;
    hTObj->tOutputPortDefnType.format.image.nFrameHeight  = unOpImageHeight;
    hTObj->tOutputPortDefnType.format.image.nStride = unOpImageWidth;
    hTObj->tOutputPortDefnType.format.image.nSliceHeight = unSliceHeight;
    hTObj->tOutputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tOutputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
    hTObj->tOutputPortDefnType.format.image.eColorFormat = eOutColorFormat;
    hTObj->tOutputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tOutputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tOutputPortDefnType.nBufferAlignment = 64;

    eError = OMX_SetParameter (hTObj->hOMXJPEGD, OMX_IndexParamPortDefinition,
		               &hTObj->tOutputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for output port");
    JPEGD_Trace("OMX_SetParameter output port defn done successfully");
 printf("\n P OMX_SetParameter output port defn done successfully ");

    /* Test Set Parameteres OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE */
    hTObj->tUncompressedMode.nPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->tUncompressedMode.eUncompressedImageMode = eOutputImageMode;

    eError = OMX_SetParameter (hTObj->hOMXJPEGD,
		                  (OMX_INDEXTYPE)OMX_TI_IndexParamJPEGUncompressedMode,
		                            &hTObj->tUncompressedMode);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		                     "Error in Set Parameters OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE");
    JPEGD_Trace("OMX_SetParameter input image uncompressed mode done successfully");
printf("\n OMX_SetParameter input image uncompressed mode done successfully");

    /* Set Parameteres OMX_IMAGE_PARAM_DECODE_SUBREGION   */
    eError = OMX_SetParameter(component,
                                (OMX_INDEXTYPE)OMX_TI_IndexParamDecodeSubregion,
                                &hTObj->pSubRegionDecode);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
       "Error in Set Parameters OMX_IMAGE_PARAM_DECODE_SUBREGION for sub region decode");
    JPEGD_Trace("OMX_SetParameter sub region decode done successfully");
 printf("\n P OMX_SetParameter sub region decode done successfully ");


    /* Create data pipe for input port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
	                        OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE, 1);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorContentPipeCreationFailed,
	                        "Error while creating input pipe for JPEGD test object" );

    /* Create data pipe for output port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
	                        OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE, 1);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorContentPipeCreationFailed ,
		                    "Error while creating output pipe for JPEGD test object");


    /**************************************************************************************/

    /* Move to idle state */
    eError = OMX_SendCommand(component, OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error while issueing Idle command");

#ifdef OMX_LINUX_TILERTEST
printf("\n TILER BUFFERS \n");
	MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	if (!MemReqDescTiler)
		printf("\nD can't allocate memory for Tiler block allocation \n");
#endif

	if(bIsApplnMemAllocatorforInBuff==OMX_TRUE)
	{
#ifdef OMX_LINUX_TILERTEST
					MemReqDescTiler[0].pixelFormat = TILFMT_PAGE;
					MemReqDescTiler[0].dim.len=  nInputBitstreamSize;
					MemReqDescTiler[0].stride=0;

					printf("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddrIn=MemMgr_Alloc(MemReqDescTiler,1);
					printf("\nTiler buffer allocated is %x\n",TilerAddrIn);
                    p_in = (OMX_U32 *)TilerAddrIn;

#else
printf("\n +++WHY HERE ???? +++\n");
        /* Request the component to allocate input buffer and buffer header */
        p_in =(OMX_U32 *) TIMM_OSAL_MallocExtn(nInputBitstreamSize, TIMM_OSAL_TRUE, 0,
						 TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        JPEGD_ASSERT( NULL != p_in, eError,
            "Error while allocating input buffer by OMX component");

#endif
        eError = OMX_UseBuffer(component, &(hTObj->pInBufHeader), OMX_JPEGD_TEST_INPUT_PORT,
	     NULL, nInputBitstreamSize, (OMX_U8 *)p_in);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
	     "Error while allocating input buffer by OMX client");
	}
    else
    {
        eError = OMX_AllocateBuffer(component ,&hTObj->pInBufHeader,
	       OMX_JPEGD_TEST_INPUT_PORT,NULL,nInputBitstreamSize);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating input buffer by OMX component");
    }


	if(bIsApplnMemAllocatorforOutBuff==OMX_TRUE)
	{
#ifdef OMX_LINUX_TILERTEST
					MemReqDescTiler[0].pixelFormat=TILFMT_PAGE;
					MemReqDescTiler[0].dim.len=  nframeSize;
					MemReqDescTiler[0].stride=0;

					printf("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddrOut=MemMgr_Alloc(MemReqDescTiler,1);
					printf("\nTiler buffer allocated is %x\n",TilerAddrOut);
                    p_out = (OMX_U32 *)TilerAddrOut;

#else
        /* Request the component to allocate output buffer and buffer header */
        p_out = (OMX_U32 *)TIMM_OSAL_MallocExtn(nframeSize, TIMM_OSAL_TRUE, 64,
                                                                           TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        JPEGD_ASSERT( NULL != p_out, eError,
            "Error while allocating output buffer by OMX component");
#endif
        eError = OMX_UseBuffer(component, &(hTObj->pOutBufHeader), OMX_JPEGD_TEST_OUTPUT_PORT,
            NULL, nframeSize, (OMX_U8 *)p_out);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
            "Error while allocating output buffer by OMX client");

	}
    else
    {
        eError = OMX_AllocateBuffer(component, &hTObj->pOutBufHeader,
                     OMX_JPEGD_TEST_OUTPUT_PORT, NULL, nframeSize);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
            "Error while allocating output buffer by OMX component");
    }
    hTObj->pOutBufHeader->nAllocLen = nframeSize;

    /*************************************************************************************/

    /*Move to Executing state*/
    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateExecuting,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    ipfile = fopen((const char *)omx_jpegd_input_file_path_and_name, "rb");
    if(ipfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s", omx_jpegd_input_file_path_and_name);
        goto EXIT;
    }

    TIMM_OSAL_Memset (hTObj->pInBufHeader->pBuffer, 0, nInputBitstreamSize);

    currPtr = (char *) hTObj->pInBufHeader->pBuffer;
    do
    {
        nreadSize = -1;
        nreadSize = fread (currPtr, 1, nInputBitstreamSize,ipfile);
        if (feof(ipfile))
        {
            break;
        }
        totalRead += nreadSize;
        currPtr += nreadSize;
    } while (totalRead <nInputBitstreamSize);
    fclose(ipfile);


    /* Write the input buffer info to the input data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
            &hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );

    /* Write the output buffer info to the output data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
            &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );


    /********************************************************************************************/

    /* Send input and output buffers to the component for processing */
    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
	             &hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader), &actualSize, 0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources ,"");

    hTObj->pInBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;
    hTObj->pInBufHeader->nInputPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->pInBufHeader->nAllocLen = nreadSize;
    hTObj->pInBufHeader->nFilledLen= hTObj->pInBufHeader->nAllocLen;
    hTObj->pInBufHeader->nOffset = 0;

    eError = component->EmptyThisBuffer(component,hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
                    &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),&actualSize,0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );

    hTObj->pOutBufHeader->nOutputPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;

    TIMM_OSAL_Memset (hTObj->pOutBufHeader->pBuffer, 0, hTObj->pOutBufHeader->nAllocLen);
    eError = component->FillThisBuffer(component, hTObj->pOutBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /* Wait till output  buffer is processed */
    eError = JPEGD_pendOnSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /***************************************************************************************/

    opfile = fopen((const char *)omx_jpegd_output_file_path_and_name,"wb");
    if (opfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s", omx_jpegd_output_file_path_and_name);
 printf("\n P Error in opening output file ");
        goto EXIT;
    }

    fwrite(hTObj->pOutBufHeader->pBuffer,1,hTObj->pOutBufHeader->nFilledLen,opfile);
    fclose (opfile);






//------------------------- comparing results start
#if 0 
//not needed now

eError = OMX_ErrorNone;
        printf("\nTest case has ended, now comparing output and reference files\n");
                op_file = fopen (OUTPUT_FILE, "rb");
            if (NULL == op_file) {
            printf ("\n Error in opening generated output file!!!");
            eError = OMX_ErrorInsufficientResources;
            }

            ref_op_file = fopen (REF_OUTPUT_FILE, "rb");
            if(NULL == ref_op_file ) {
           printf ("\n Error in opening REF OUTPUT FILE");
           eError = OMX_ErrorInsufficientResources;
            }
                pass = 1;
                while(1)
                {
                        if(eError != OMX_ErrorNone)
                        {
                                pass = 0;
                                break;
                        }
                        ch1 = fgetc(op_file);
                        ch2 = fgetc(ref_op_file);
                        loc_diff++;
                        if(ch1 == EOF || ch2 == EOF)
                        {
                                break;
                        }
                        if(ch1 != ch2)
                        {
                                pass = 0;
                                printf("\nFILES NOT SAME \n");
                                printf("\n ################### LOCATION OF DIFFERENCE: %d ################### \n", loc_diff);
                                break;
                        }
                }
printf("\n DONE WITH CHECK PASS = %d",pass);
                fclose (op_file);
                fclose (ref_op_file);
                if(pass == 1)
            printf (" \n ----- SUCCESS: Test Case has Passed\n");
                else
                {
                        printf("\n FAILURE: Test case has failed. (EOF not reached)\n");
                        eError = OMX_ErrorUndefined;
                }

#endif
//------------------------- comparing results end


        if (hTObj->pInBufHeader){
#ifdef OMX_LINUX_TILERTEST
                MemMgr_Free(hTObj->pInBufHeader->pBuffer);
                printf("\nBACK FROM INPUT BUFFER mmgr_free call \n");
#endif

    eError = OMX_FreeBuffer(component ,OMX_JPEGD_TEST_INPUT_PORT,
                            hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
                "Error while Dellocating input buffer by OMX component");
}
    /* Request the component to free up output buffers and buffer headers */


        if (hTObj->pOutBufHeader){
#ifdef OMX_LINUX_TILERTEST 
                MemMgr_Free(hTObj->pOutBufHeader->pBuffer);
#endif

    eError = OMX_FreeBuffer(component, OMX_JPEGD_TEST_OUTPUT_PORT,
                            hTObj->pOutBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
                "Error while Dellocating output buffer by OMX component");
}


    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    /* Delete input port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe(hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT] );
    JPEGD_REQUIRE(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		        "Error while deleting input pipe of test object" );

    /* Create output port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT] );
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,
		        "Error while deleting output pipe of test object" );

    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateLoaded,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    /* Delete a dummy semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating dummy semaphore");
    JPEGD_Trace("Dummy semaphore deleted");
	printf("\n Dummy semaphore deleted");

    /* Delete an Event semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating Event semaphore");
    JPEGD_Trace("Event semaphore deleted");
printf("\n Event semaphore deleted");

    /* Delete an EmptyThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in creating EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore deleted");
printf("\n EmptyThisBuffer semaphore deleted");

     /* Delete a FillThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in creating FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore deleted");
printf("\nFillThisBuffer semaphore deleted");


    /* Free input and output buffer pointers */
/*
    if(bIsApplnMemAllocatorforInBuff==OMX_TRUE)
    {
        TIMM_OSAL_Free(p_in);
    }
    if(bIsApplnMemAllocatorforOutBuff==OMX_TRUE)
    {
        TIMM_OSAL_Free(p_out);
    }
*/
    /*************************************************************************************/

    /* Calling OMX_Core OMX_FreeHandle function to unload a component */
    eError = OMX_FreeHandle (component);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		           "Error while unloading JPEGD component");
    JPEGD_Trace("OMX Test: OMX_FreeHandle() successful");
printf("\n OMX Test: OMX_FreeHandle() successful");
    eError = OMX_ErrorNone;

EXIT:
    if(eError != OMX_ErrorNone)
        testResult = OMX_RET_FAIL;


    return testResult;
}


/* ==================================================================== */
/**
* @fn JPEGD_TestSliceMode    Executes the JPEG Decoder testcases For Slice mode
*
* @param[in] uMsg         Message code.
* @param[in] pParam       pointer to Function table entry
* @param[in] paramSize    size of the Function table entry structure
*
* @return
*     OMX_RET_PASS if the test passed,
*     OMX_RET_FAIL if the test fails,or possibly other special conditions.
*
*/
/* ==================================================================== */


OMX_TestStatus JPEGD_TestSliceMode (uint32_t uMsg, void *pParam, uint32_t paramSize)
{
    OMX_Status status = OMX_STATUS_OK;
    OMX_TestStatus testResult = OMX_RET_PASS;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_CALLBACKTYPE tJpegDecoderCb;
    OMX_S32 counter;
    OMX_U32 nframeSize,nframeSizeslice,nInputBitstreamSize;
    OMX_U32 *p_in, *p_out;
    OMX_U32 unOpImageWidth,unOpImageHeight,unSliceHeight;
    OMX_COLOR_FORMATTYPE eInColorFormat,eOutColorFormat;
    OMX_JPEG_UNCOMPRESSEDMODETYPE eOutputImageMode;
    OMX_BOOL bIsApplnMemAllocatorforInBuff, bIsApplnMemAllocatorforOutBuff;
    FILE *ipfile,*opfile;
    char fout_name[100], slice_ext[20];;
    OMX_S32  nreadSize;
    OMX_COMPONENTTYPE *component;
    OMX_U32 actualSize = 0, slice_idx;
    JpegDecoderTestObject *hTObj= &TObjD;
    int totalRead = 0;
    char *currPtr = NULL;
    OMX_U32 nOutputBytes;  //, unSliceDimension;

#ifdef OMX_LINUX_TILERTEST
	MemAllocBlock *MemReqDescTiler;
	OMX_PTR TilerAddrIn=NULL;
	OMX_PTR TilerAddrOut=NULL;
#endif

    JPEGD_Trace("*** Test Case : %s ***", __FUNCTION__);


    status = JPEGD_ParseTestCases(uMsg,pParam,paramSize);

    if(status != OMX_STATUS_OK)
    {
        testResult = OMX_RET_FAIL;
	    goto EXIT;
    }

    unOpImageWidth = hTObj->tTestcaseParam.unOpImageWidth;
    unOpImageHeight = hTObj->tTestcaseParam.unOpImageHeight;
    unSliceHeight = hTObj->tTestcaseParam.unSliceHeight;
    eInColorFormat = hTObj->tTestcaseParam.eInColorFormat;
    eOutColorFormat = hTObj->tTestcaseParam.eOutColorFormat;
    eOutputImageMode = hTObj->tTestcaseParam.eOutputImageMode;
    bIsApplnMemAllocatorforInBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforInBuff;
    bIsApplnMemAllocatorforOutBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforOutBuff;

    /* Input file size */
    ipfile = fopen ((const char *)omx_jpegd_input_file_path_and_name,"rb");
    if(ipfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s", omx_jpegd_input_file_path_and_name);
        goto EXIT;
    }
    else
    {
        fseek(ipfile,0,SEEK_END);
        nInputBitstreamSize = ftell(ipfile);//-bitstream_start;
        fseek(ipfile, 0, SEEK_SET);
        fclose(ipfile);
    }

    nframeSize = JPEGD_TEST_CalculateBufferSize(unOpImageWidth, unOpImageHeight, eOutColorFormat);
    nframeSizeslice = JPEGD_TEST_UCP_CalculateSliceBufferSize(unOpImageWidth,unOpImageHeight,
		 	                                 unSliceHeight , eOutColorFormat);

    /**************************************************************************************/

    /* Create a dummy semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating dummy semaphore");
    JPEGD_Trace("Dummy semaphore created");

    /* Create an Event semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating Event semaphore");
    JPEGD_Trace("Event semaphore created");

    /* Create an EmptyThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore created");

    /* Create a FillThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore created");


    /***************************************************************************************/

    /* Set call back functions */
    tJpegDecoderCb.EmptyBufferDone = OMX_JPEGD_TEST_EmptyBufferDone;
    tJpegDecoderCb.EventHandler = OMX_JPEGD_TEST_EventHandler;
    tJpegDecoderCb.FillBufferDone = OMX_JPEGD_TEST_FillBufferDone;

    /* Load the OMX_JPG_DEC Component */
    hTObj->hOMXJPEGD = NULL;
    eError= OMX_GetHandle(&hTObj->hOMXJPEGD, strJPEGD, hTObj,
		                          &tJpegDecoderCb);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error while obtaining component handle");
    JPEGD_Trace("OMX_GetHandle successful");

    /* Handle of the component to be accessed */
    component = (OMX_COMPONENTTYPE *)hTObj->hOMXJPEGD;

    /*Initialize size and version information for all structures*/
    JPEGD_STRUCT_INIT(hTObj->tJpegdecPortInit,OMX_PORT_PARAM_TYPE);
    JPEGD_STRUCT_INIT(hTObj->tInputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);
    JPEGD_STRUCT_INIT(hTObj->tOutputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);
    JPEGD_STRUCT_INIT(hTObj->tUncompressedMode, OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE);
    JPEGD_STRUCT_INIT(hTObj->pSubRegionDecode, OMX_IMAGE_PARAM_DECODE_SUBREGION);

    /***************************************************************************************/

    /*  Test Set Parameters OMX_PORT_PARAM_TYPE */
    hTObj->tJpegdecPortInit.nPorts           = 2;
    hTObj->tJpegdecPortInit.nStartPortNumber = 0;
    eError = OMX_SetParameter (component, OMX_IndexParamImageInit,&hTObj->tJpegdecPortInit);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		          "Error in Set Parameters OMX_PORT_PARAM_TYPE ");
    JPEGD_Trace("OMX_SetParameter image param done");

    /* Test Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for input port */
    hTObj->tInputPortDefnType.nPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->tInputPortDefnType.eDir = OMX_DirInput;
    hTObj->tInputPortDefnType.nBufferCountActual = 1;
    hTObj->tInputPortDefnType.nBufferCountMin    = 1;
    hTObj->tInputPortDefnType.nBufferSize        = nInputBitstreamSize;
    hTObj->tInputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tInputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tInputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tInputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tInputPortDefnType.format.image.nFrameWidth   = nInputBitstreamSize;
    hTObj->tInputPortDefnType.format.image.nFrameHeight  = 1;
    hTObj->tInputPortDefnType.format.image.nStride       = 1;
    hTObj->tInputPortDefnType.format.image.nSliceHeight  = 1;
    hTObj->tInputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tInputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    hTObj->tInputPortDefnType.format.image.eColorFormat = eInColorFormat;
    hTObj->tInputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tInputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tInputPortDefnType.nBufferAlignment = 0;

    eError = OMX_SetParameter(component, OMX_IndexParamPortDefinition,
		                         &hTObj->tInputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for input port");
    JPEGD_Trace("OMX_SetParameter input port defn done successfully");


    /* Test Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for output port */
    hTObj->tOutputPortDefnType.nPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;
    hTObj->tOutputPortDefnType.eDir = OMX_DirOutput;
    hTObj->tOutputPortDefnType.nBufferCountActual = 1;
    hTObj->tOutputPortDefnType.nBufferCountMin    = 1;
    hTObj->tOutputPortDefnType.nBufferSize = nframeSizeslice;
    hTObj->tOutputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tOutputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tOutputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tOutputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tOutputPortDefnType.format.image.nFrameWidth   = unOpImageWidth;
    hTObj->tOutputPortDefnType.format.image.nFrameHeight  = unSliceHeight;
    hTObj->tOutputPortDefnType.format.image.nStride = unOpImageWidth;
    hTObj->tOutputPortDefnType.format.image.nSliceHeight = unSliceHeight;
    hTObj->tOutputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tOutputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
    hTObj->tOutputPortDefnType.format.image.eColorFormat = eOutColorFormat;
    hTObj->tOutputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tOutputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tOutputPortDefnType.nBufferAlignment = 64;

    eError = OMX_SetParameter (hTObj->hOMXJPEGD, OMX_IndexParamPortDefinition,
		               &hTObj->tOutputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		  "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for output port");
    JPEGD_Trace("OMX_SetParameter output port defn done successfully");


    /* Test Set Parameteres OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE */
	hTObj->tUncompressedMode.nPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
	hTObj->tUncompressedMode.eUncompressedImageMode 	 = eOutputImageMode;

	eError = OMX_SetParameter (hTObj->hOMXJPEGD,
		                  (OMX_INDEXTYPE)OMX_TI_IndexParamJPEGUncompressedMode,
		                            &hTObj->tUncompressedMode);
	JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		                     "Error in Set Parameters OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE");
	JPEGD_Trace("OMX_SetParameter input image uncompressed mode done successfully");
 
    /* Set Parameteres OMX_IMAGE_PARAM_DECODE_SUBREGION   */
    eError = OMX_SetParameter(component, 
                                (OMX_INDEXTYPE)OMX_TI_IndexParamDecodeSubregion,
                                &hTObj->pSubRegionDecode);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
       "Error in Set Parameters OMX_IMAGE_PARAM_DECODE_SUBREGION for sub region decode");
    JPEGD_Trace("OMX_SetParameter sub region decode done successfully");


    /*if(hTObj->tOutputPortDefnType.format.image.nSliceHeight!=0)
    {
        unSliceDimension = unOpImageWidth * unSliceHeight * 1.5;
    }
    else
    {
         unSliceDimension = unOpImageWidth *unOpImageHeight * 1.5;
    } */

    /* Create data pipe for input port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
	                        OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE, 1);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorContentPipeCreationFailed,
	                        "Error while creating input pipe for JPEGD test object" );

    /* Create data pipe for output port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
	                        OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE, 1);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorContentPipeCreationFailed ,
		                    "Error while creating output pipe for JPEGD test object");


    /**************************************************************************************/

    /* Move to idle state */
    eError = OMX_SendCommand(component, OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error while issueing Idle command");

#ifdef OMX_LINUX_TILERTEST
printf("\n TILER BUFFERS \n");
	MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	if (!MemReqDescTiler)
		printf("\nD can't allocate memory for Tiler block allocation \n");
#endif



    if(bIsApplnMemAllocatorforInBuff==OMX_TRUE)
    {
#ifdef OMX_LINUX_TILERTEST
					MemReqDescTiler[0].pixelFormat = TILFMT_PAGE;
					MemReqDescTiler[0].dim.len=  nInputBitstreamSize;
					MemReqDescTiler[0].stride=0;

					printf("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddrIn=MemMgr_Alloc(MemReqDescTiler,1);
					printf("\nTiler buffer allocated is %x\n",TilerAddrIn);
                    p_in = (OMX_U32 *)TilerAddrIn;

#else
        /* Request the component to allocate input buffer and buffer header */
        p_in =(OMX_U32 *) TIMM_OSAL_MallocExtn(nInputBitstreamSize, TIMM_OSAL_TRUE, 0, 
                                                                         TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        JPEGD_ASSERT( NULL != p_in, eError,
               "Error while allocating input buffer by OMX component");
#endif

        eError = OMX_UseBuffer(component, &(hTObj->pInBufHeader), OMX_JPEGD_TEST_INPUT_PORT, 
		NULL, nInputBitstreamSize, (OMX_U8 *)p_in);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating input buffer by OMX client");
    }
    else
    {
        eError = OMX_AllocateBuffer(component ,&hTObj->pInBufHeader,
	       OMX_JPEGD_TEST_INPUT_PORT,NULL,nInputBitstreamSize);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating input buffer by OMX component");
    }

    if(bIsApplnMemAllocatorforOutBuff==OMX_TRUE)
    {
#ifdef OMX_LINUX_TILERTEST
					MemReqDescTiler[0].pixelFormat=TILFMT_PAGE;
					MemReqDescTiler[0].dim.len=  nframeSize;
					MemReqDescTiler[0].stride=0;

					printf("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddrOut=MemMgr_Alloc(MemReqDescTiler,1);
					printf("\nTiler buffer allocated is %x\n",TilerAddrOut);
                    p_out = (OMX_U32 *)TilerAddrOut;

#else

        /* Request the component to allocate output buffer and buffer header */
        p_out = (OMX_U32 *)TIMM_OSAL_MallocExtn(nframeSizeslice, TIMM_OSAL_TRUE, 64, 
                                                                           TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        JPEGD_ASSERT( NULL != p_out, eError,
            "Error while allocating output buffer by OMX component");
#endif
        eError = OMX_UseBuffer(component, &(hTObj->pOutBufHeader), OMX_JPEGD_TEST_OUTPUT_PORT,
            NULL, nframeSizeslice, (OMX_U8 *)p_out);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
            "Error while allocating output buffer by OMX client");
    }
    else
    {
        eError = OMX_AllocateBuffer(component ,&hTObj->pInBufHeader,
	       OMX_JPEGD_TEST_OUTPUT_PORT,NULL,nframeSize);
        JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating output buffer by OMX component");
    }

    hTObj->pOutBufHeader->nAllocLen = nframeSizeslice;

    /*************************************************************************************/
    
    /*Move to Executing state*/
    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateExecuting,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    ipfile = fopen((const char *)omx_jpegd_input_file_path_and_name, "rb");
    if(ipfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s", omx_jpegd_input_file_path_and_name);
        goto EXIT;
    }

    TIMM_OSAL_Memset (hTObj->pInBufHeader->pBuffer, 0, nInputBitstreamSize);

    currPtr = (char *) hTObj->pInBufHeader->pBuffer;
    do
    {
        nreadSize = -1;
        nreadSize = fread (currPtr, 1, nInputBitstreamSize,ipfile);
        if (feof(ipfile))
        {
            break;
        }
        totalRead += nreadSize;
        currPtr += nreadSize;
    } while (totalRead <nInputBitstreamSize);
    fclose(ipfile);


    /* Write the input buffer info to the input data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
            &hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );

    /* Write the output buffer info to the output data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
            &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );


    /********************************************************************************************/

    /* Send input and output buffers to the component for processing */
    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
	             &hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader), &actualSize, 0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources ,"");

    hTObj->pInBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;
    hTObj->pInBufHeader->nInputPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->pInBufHeader->nAllocLen = nreadSize;
    hTObj->pInBufHeader->nFilledLen= hTObj->pInBufHeader->nAllocLen;
    hTObj->pInBufHeader->nOffset = 0;

    eError = component->EmptyThisBuffer(component,hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
                    &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),&actualSize,0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );

    hTObj->pOutBufHeader->nOutputPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;
    TIMM_OSAL_Memset (hTObj->pOutBufHeader->pBuffer, 0, hTObj->pOutBufHeader->nAllocLen);

    counter = 0;
    nOutputBytes = 0;

#if 0
    /* Clean already existing output file */
    opfile = fopen((const char *)omx_jpegd_output_file_path_and_name,"w+b");
    if (opfile == NULL) 
    {
        JPEGD_Trace("Error opening the output file %s\r", omx_jpegd_output_file_path_and_name);
        goto EXIT;
    }
    fclose(opfile);
#endif

    slice_idx = 0;
    while(nOutputBytes <nframeSize) // here loop over total buffer size
    {
        counter++;
        JPEGD_Trace("Slice Number: %d\r", counter);
        strcpy(fout_name, (char *)omx_jpegd_output_file_path_and_name);
        if (nframeSizeslice > 0)
        {
            slice_idx++;
            eError = component->FillThisBuffer(component, hTObj->pOutBufHeader);
            JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

            /* Wait till output  buffer is processed */
            eError = JPEGD_pendOnSemaphore(SEMTYPE_FTB);
            JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");
            nOutputBytes += nframeSizeslice;
            JPEGD_Trace("Semaphores and no of bytes compleated %d\r", nOutputBytes);

            sprintf(slice_ext, "_%d", slice_idx);
            strcat(fout_name, slice_ext);

           opfile = fopen(fout_name, "wb");
           if (opfile == NULL) 
            {
                JPEGD_Trace("Error opening the output file %s\r", fout_name);
                goto EXIT;
            }

            fwrite(hTObj->pOutBufHeader->pBuffer, 1, hTObj->pOutBufHeader->nFilledLen,opfile);
            fclose(opfile);
        }
    }

    /***************************************************************************************/
	
    /* Request the component to free up input  buffers and buffer headers */
        if (hTObj->pInBufHeader){
#ifdef OMX_LINUX_TILERTEST
                MemMgr_Free(hTObj->pInBufHeader->pBuffer);
                printf("\nBACK FROM INPUT BUFFER mmgr_free call \n");
#endif

    eError = OMX_FreeBuffer(component ,OMX_JPEGD_TEST_INPUT_PORT,
                                     hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		            "Error while Dellocating input buffer by OMX component");
	}
    /* Request the component to free up output buffers and buffer headers */
        if (hTObj->pOutBufHeader){
#ifdef OMX_LINUX_TILERTEST
                MemMgr_Free(hTObj->pOutBufHeader->pBuffer);
#endif

    eError = OMX_FreeBuffer(component, OMX_JPEGD_TEST_OUTPUT_PORT,
                              hTObj->pOutBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		        "Error while Dellocating output buffer by OMX component");
	}
    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /* Delete input port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe(hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT] );
    JPEGD_REQUIRE(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
		        "Error while deleting input pipe of test object" );

    /* Create output port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT] );
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,
		        "Error while deleting output pipe of test object" );

    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateLoaded,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /* Delete a dummy semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating dummy semaphore");
    JPEGD_Trace("Dummy semaphore deleted");

    /* Delete an Event semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating Event semaphore");
    JPEGD_Trace("Event semaphore deleted");

    /* Delete an EmptyThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore deleted");

     /* Delete a FillThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore deleted");

    /* Free input and output buffer pointers */
/*    if(bIsApplnMemAllocatorforInBuff==OMX_TRUE && p_in != NULL)
    {
        TIMM_OSAL_Free(p_in);
    }
    if(bIsApplnMemAllocatorforOutBuff==OMX_TRUE && p_out != NULL)
    {
        TIMM_OSAL_Free(p_out);
    }*/

    /*************************************************************************************/

    /* Calling OMX_Core OMX_FreeHandle function to unload a component */
    eError = OMX_FreeHandle (component);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error while unloading JPEGD component");
    JPEGD_Trace ("OMX Test: OMX_FreeHandle() successful");
    eError = OMX_ErrorNone;

EXIT:
    if(eError != OMX_ErrorNone)
        testResult = OMX_RET_FAIL;


    return testResult;

}


///////////////////////////////s t a r t
#if 0

/* ==================================================================== */
/**
* @fn JPEGD_TestEntry_Exif Executes the JPEG Decoder testcases For Frame mode
*
* @param[in] uMsg            Message code.
* @param[in] pParam        pointer to Function table entry
* @param[in] paramSize    size of the Function table entry structure
*
* @return
*     OMX_RET_PASS if the test passed, OMX_RET_FAIL if the test fails,
*     or possibly other special conditions.
*
*/
/* ==================================================================== */


OMX_TestStatus JPEGD_TestEntry_Exif(uint32_t uMsg, void *pParam, uint32_t paramSize)
{
    OMX_Status status = OMX_STATUS_OK;
    OMX_TestStatus testResult = OMX_RET_PASS;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_CALLBACKTYPE tJpegDecoderCb;
    OMX_U32 nframeSize, nInputBitstreamSize, nBufferSize = 0;
    OMX_U32 unOpImageWidth, unOpImageHeight, unSliceHeight;
    OMX_COLOR_FORMATTYPE eInColorFormat, eOutColorFormat;
    //OMX_BOOL bIsApplnMemAllocatorforInBuff, bIsApplnMemAllocatorforOutBuff;
    FILE *ipfile, *opfile;
    OMX_S32 nreadSize;
    OMX_COMPONENTTYPE *component;
    OMX_U32 actualSize = 0;
    JpegDecoderTestObject  *hTObj = &TObjD;
    OMX_EXIF_INFO_SUPPORTED *pExifInfoSupport;
    OMX_EXIF_THUMBNAIL_INFO *pThumbnailInfo;


    JPEGD_Entering();

    JPEGD_Trace("*** Test Case : %s ***", __FUNCTION__);

    status = JPEGD_ParseTestCases(uMsg,pParam,paramSize);
    if(status != OMX_STATUS_OK)
	{
        testResult = OMX_RET_FAIL;
	    goto EXIT;
    }

    unOpImageWidth = hTObj->tTestcaseParam.unOpImageWidth;
    unOpImageHeight = hTObj->tTestcaseParam.unOpImageHeight;
    unSliceHeight = hTObj->tTestcaseParam.unSliceHeight;
    eInColorFormat = hTObj->tTestcaseParam.eInColorFormat;
    eOutColorFormat = hTObj->tTestcaseParam.eOutColorFormat;
    //bIsApplnMemAllocatorforInBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforInBuff;
    //bIsApplnMemAllocatorforOutBuff = hTObj->tTestcaseParam.bIsApplnMemAllocatorforOutBuff;

	/* Input file size */

    nInputBitstreamSize = sizeof("D:/test_vectorsJPG_DEC_5_320x240.jpg");
    nframeSize = JPEGD_TEST_CalculateBufferSize(unOpImageWidth, unOpImageHeight, eOutColorFormat);

    /**************************************************************************************/
    
    /* Create a dummy semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating dummy semaphore");
    JPEGD_Trace("Event semaphore created");

    /* Create an Event semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error in creating Event semaphore");
    JPEGD_Trace("Event semaphore created \n");

    /* Create an EmptyThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error in creating EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore created \n");

    /* Create a FillThisBuffer semaphore */
    eError = JPEGD_CreateSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error in creating FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore created \n");

    /***************************************************************************************/

    /* Set call back functions */
    tJpegDecoderCb.EmptyBufferDone = OMX_JPEGD_TEST_EmptyBufferDone;
    tJpegDecoderCb.EventHandler    = OMX_JPEGD_TEST_EventHandler;
    tJpegDecoderCb.FillBufferDone  = OMX_JPEGD_TEST_FillBufferDone;

    /* Load the OMX_JPG_DEC Component */
    hTObj->hOMXJPEGD = NULL;
    eError= OMX_GetHandle(&hTObj->hOMXJPEGD, strJPEGD, hTObj,
		                          &tJpegDecoderCb);
    JPEGD_ASSERT(eError == OMX_ErrorNone , eError,
		"Error while obtaining component handle");
    JPEGD_Trace("OMX_GetHandle sucessfully \n");

    /* Handle of the component to be accessed */
    component = (OMX_COMPONENTTYPE *)hTObj->hOMXJPEGD;

    /*Initialize size and version information for all structures*/
    JPEGD_STRUCT_INIT(hTObj->tJpegdecPortInit,OMX_PORT_PARAM_TYPE);
    JPEGD_STRUCT_INIT(hTObj->tInputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);
    JPEGD_STRUCT_INIT(hTObj->tOutputPortDefnType,OMX_PARAM_PORTDEFINITIONTYPE);

    /***************************************************************************************/
    /*  Test Set Parameteres OMX_PORT_PARAM_TYPE */
    hTObj->tJpegdecPortInit.nPorts                     = 2;
    hTObj->tJpegdecPortInit.nStartPortNumber    = 0;
    eError = OMX_SetParameter (component, OMX_IndexParamImageInit, &hTObj->tJpegdecPortInit);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error in Set Parameters OMX_PORT_PARAM_TYPE ");
    JPEGD_Trace("OMX_SetParameter image param done \n");


    /* Test Set Parameteres (OMX_PARAM_PORTDEFINITIONTYPE for input port */
    hTObj->tInputPortDefnType.nPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->tInputPortDefnType.eDir = OMX_DirInput;
    hTObj->tInputPortDefnType.nBufferCountActual = 1;
    hTObj->tInputPortDefnType.nBufferCountMin = 1;
    hTObj->tInputPortDefnType.nBufferSize = nBufferSize;
    hTObj->tInputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tInputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tInputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tInputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tInputPortDefnType.format.image.nFrameWidth = unOpImageWidth;
    hTObj->tInputPortDefnType.format.image.nFrameHeight = unOpImageHeight;
    hTObj->tInputPortDefnType.format.image.nStride = 0;
    hTObj->tInputPortDefnType.format.image.nSliceHeight = unSliceHeight;
    hTObj->tInputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tInputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingJPEG;
    hTObj->tInputPortDefnType.format.image.eColorFormat = eInColorFormat;
    hTObj->tInputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tInputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tInputPortDefnType.nBufferAlignment = 0;

    eError = OMX_SetParameter (component, OMX_IndexParamPortDefinition,
		                         &hTObj->tInputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		   "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for input port");
    JPEGD_Trace("OMX_SetParameter input port defn done successfully\n");


    /* Test Set Parameteres OMX_PARAM_PORTDEFINITIONTYPE for output port */
    hTObj->tOutputPortDefnType.nPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;
    hTObj->tOutputPortDefnType.eDir = OMX_DirOutput;
    hTObj->tOutputPortDefnType.nBufferCountActual = 1;
    hTObj->tOutputPortDefnType.nBufferCountMin = 1;
    hTObj->tOutputPortDefnType.nBufferSize = (OMX_U32)(unOpImageWidth * unOpImageHeight
		             *((eOutColorFormat == OMX_COLOR_FormatYUV420PackedSemiPlanar)?1.5:2)/3);
    hTObj->tOutputPortDefnType.bEnabled = OMX_TRUE;
    hTObj->tInputPortDefnType.bPopulated = OMX_FALSE;
    hTObj->tOutputPortDefnType.eDomain = OMX_PortDomainImage;
    hTObj->tOutputPortDefnType.format.image.cMIMEType = "OMXJPEGD";
    hTObj->tOutputPortDefnType.format.image.pNativeRender = 0 ;
    hTObj->tOutputPortDefnType.format.image.nFrameWidth = unOpImageWidth;
    hTObj->tOutputPortDefnType.format.image.nFrameHeight = unOpImageHeight;

    hTObj->tOutputPortDefnType.format.image.nStride = 0;
    hTObj->tOutputPortDefnType.format.image.nSliceHeight = unSliceHeight;
    hTObj->tOutputPortDefnType.format.image.bFlagErrorConcealment = OMX_FALSE;
    hTObj->tOutputPortDefnType.format.image.eCompressionFormat = OMX_IMAGE_CodingUnused;
    hTObj->tOutputPortDefnType.format.image.eColorFormat = eOutColorFormat;
    hTObj->tOutputPortDefnType.format.image.pNativeWindow = 0x0;
    hTObj->tOutputPortDefnType.bBuffersContiguous = OMX_FALSE;
    hTObj->tOutputPortDefnType.nBufferAlignment = 64;

    eError = OMX_SetParameter (hTObj->hOMXJPEGD, OMX_IndexParamPortDefinition,
		                        &hTObj->tOutputPortDefnType);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		 "Error in Set Parameters OMX_PARAM_PORTDEFINITIONTYPE for output port");
    JPEGD_Trace("OMX_SetParameter output port defn done successfully\n");


    /* Create data pipe for input port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
		                         OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE, 0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,
		OMX_ErrorContentPipeCreationFailed,
		"Error while creating input pipe for JPEGD test object" );

    /* Create data pipe for output port */
    bReturnStatus = TIMM_OSAL_CreatePipe (&hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
		OMX_JPEGD_TEST_PIPE_SIZE, OMX_JPEGD_TEST_PIPE_MSG_SIZE,  0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,
		OMX_ErrorContentPipeCreationFailed ,
		"Error while creating output pipe for JPEGD test object");

    /*************************************************************************************/  

    /*Move to idle state*/
    eError = OMX_SendCommand(component, OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"Error while issueing Idle command");

    /* Request the component to allocate input buffer and buffer header */
    eError = OMX_AllocateBuffer(component ,&hTObj->pInBufHeader,
    OMX_JPEGD_TEST_INPUT_PORT,NULL,nInputBitstreamSize);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating input buffer by OMX component");

    /* Request the component to allocate output buffer and buffer header */
    eError = OMX_AllocateBuffer(component, &hTObj->pOutBufHeader,
    OMX_JPEGD_TEST_OUTPUT_PORT, NULL, nframeSize);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while allocating output buffer by OMX component");
    /* Request the component to allocate buffer header  OMX_EXIF_INFO_SUPPORTED*/
    pThumbnailInfo = (OMX_EXIF_THUMBNAIL_INFO *) 
		               TIMM_OSAL_MallocExtn(sizeof(OMX_EXIF_THUMBNAIL_INFO),
		               TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    pExifInfoSupport = (OMX_EXIF_INFO_SUPPORTED *) TIMM_OSAL_MallocExtn(sizeof(OMX_EXIF_INFO_SUPPORTED),
		                 TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    hTObj->pExifInfoSupport = pExifInfoSupport;
    hTObj->pExifInfoSupport->pThumbnailInfo = pThumbnailInfo;

    /*************************************************************************************/
    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateExecuting,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    ipfile = fopen ((const char *)omx_jpegd_input_file_path_and_name,"r");

    if(ipfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s\n", omx_jpegd_input_file_path_and_name);
	    goto EXIT;
    }
    else
    {
        nreadSize = fread (hTObj->pInBufHeader->pBuffer, 1, nInputBitstreamSize,ipfile);
    }

    /*Call for EXIF header read*/
    eError = TI_JpegOcpCore_ParseJpegHeader(hTObj, nInputBitstreamSize);

    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in creating Event semaphore");
    fclose(ipfile);
    // JPEGD_Trace("Error opening the file %d\r", hTObj->pExifInfoSupport->Orientation);

    /* Write the input buffer info to the input data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
					&hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );


    /* Write the output buffer info to the output data pipe */
    bReturnStatus = TIMM_OSAL_WriteToPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
                    &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),TIMM_OSAL_SUSPEND);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );


    /********************************************************************************************/

    /* Send input and output buffers to the component for processing */
    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT],
			&hTObj->pInBufHeader, sizeof (hTObj->pInBufHeader), &actualSize, 0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources ,"");

    hTObj->pInBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;
    hTObj->pInBufHeader->nInputPortIndex = OMX_JPEGD_TEST_INPUT_PORT;
    hTObj->pInBufHeader->nAllocLen = nreadSize;
    hTObj->pInBufHeader->nFilledLen= hTObj->pInBufHeader->nAllocLen;
    hTObj->pInBufHeader->nOffset = 0;

    eError = component->EmptyThisBuffer(component,hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    bReturnStatus = TIMM_OSAL_ReadFromPipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT],
                    &hTObj->pOutBufHeader, sizeof (hTObj->pOutBufHeader),&actualSize,0);
    JPEGD_REQUIRE( bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorInsufficientResources,"" );

    hTObj->pOutBufHeader->nOutputPortIndex = OMX_JPEGD_TEST_OUTPUT_PORT;
    hTObj->pOutBufHeader->nFilledLen= 0;
    hTObj->pOutBufHeader->nAllocLen = nreadSize;
    hTObj->pInBufHeader->nOffset = 0;

    eError = component->FillThisBuffer(component, hTObj->pOutBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

     /* Wait till output  buffer is processed */
    eError = JPEGD_pendOnSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /***************************************************************************************/

    opfile = fopen ((const char *)omx_jpegd_output_file_path_and_name,"w");
    if (opfile == NULL)
    {
        JPEGD_Trace("Error opening the file %s\n", omx_jpegd_output_file_path_and_name);
        goto EXIT;
    }
    fwrite (hTObj->pOutBufHeader->pBuffer,1,hTObj->pOutBufHeader->nFilledLen,opfile);
    fclose (opfile);

    /* Request the component to free up input  buffers and buffer headers */
    eError = OMX_FreeBuffer(component ,OMX_JPEGD_TEST_INPUT_PORT,
                            hTObj->pInBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while Dellocating input buffer by OMX component");

    /* Request the component to free up output buffers and buffer headers */
    eError = OMX_FreeBuffer(component, OMX_JPEGD_TEST_OUTPUT_PORT,
                            hTObj->pOutBufHeader);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,
		"Error while Dellocating output buffer by OMX component");

    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateIdle,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    eError = JPEGD_pendOnSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "");

    /* Delete input port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe(hTObj->dataPipes[OMX_JPEGD_TEST_INPUT_PORT] );
    JPEGD_REQUIRE(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
	    "Error while deleting input pipe of test object" );

    /* Create output port data pipe */
    bReturnStatus = TIMM_OSAL_DeletePipe (hTObj->dataPipes[OMX_JPEGD_TEST_OUTPUT_PORT] );
    JPEGD_REQUIRE(bReturnStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources,
	    "Error while deleting output pipe of test object" );

    eError = OMX_SendCommand(component,OMX_CommandStateSet, OMX_StateLoaded,NULL);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError,"");

    /* Delete a dummy semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_DUMMY);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in deleting dummy semaphore");
    JPEGD_Trace("Event semaphore deleted");

    /* Delete an Event semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_EVENT);
    JPEGD_ASSERT(eError == OMX_ErrorNone, eError, "Error in deleting Event semaphore");
    JPEGD_Trace("Event semaphore deleted");

    /* Delete an EmptyThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_ETB);
    JPEGD_ASSERT( eError == OMX_ErrorNone, eError,
		   "Error in deleting EmptyThisBuffer semaphore");
    JPEGD_Trace("EmptyThisBuffer semaphore deleted");

    /* Delete a FillThisBuffer semaphore */
    eError = JPEGD_DeleteSemaphore(SEMTYPE_FTB);
    JPEGD_ASSERT( eError == OMX_ErrorNone, eError,
		      "Error in deleting FillThisBuffer semaphore");
    JPEGD_Trace("FillThisBuffer semaphore deleted");

    /*************************************************************************************/

    /* Calling OMX_Core OMX_FreeHandle function to unload a component */
    eError = OMX_FreeHandle (component);
    JPEGD_ASSERT( eError == OMX_ErrorNone, eError,
		        "Error while unloading JPEGD component");
    JPEGD_Trace ("OMX Test: OMX_FreeHandle() successful");
    eError = OMX_ErrorNone;

EXIT:
    if(eError != OMX_ErrorNone)
    {
        testResult = OMX_RET_FAIL;
    }
    JPEGD_Exiting(testResult);
    return testResult;
}


/* ======================================================================= */
/**
*  @fn TI_JpegOcpCore_ParseJpegHeader() takes a input pointer to pointer
*  to decoder component private data structure, parses the bitstream and
*  extracts the decode parameters and returns JPEGOCP_SUCCESS or Error Type.
*
*  @return                JPEGOCP_SUCCESS or Error Type
*/
/* ======================================================================= */

OMX_ERRORTYPE TI_JpegOcpCore_ParseJpegHeader( OMX_PTR hTObj, OMX_U32 nInputBitstreamSize)
{

    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    JpegDecoderTestObject  *hTObj1 = (JpegDecoderTestObject *)hTObj;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_U8* pBuffer, *pBufferTemp;
    OMX_U32 i;
    OMX_U16 usJpegMarker, ulSkipFactor;
    OMX_U32 ulSOFHeaderDone = 0, nSOSMarkers = 0;
    OMX_U8 bDataPrecision;
    //OMX_U8 bYSamplingFactor, bCbSamplingFactor, bCrSamplingFactor, bHuffmanTableSelector;
    OMX_U32 ulImageWidth, ulImageHeight; //, nColorComponents, nScanHeaderComponents;

     pBuffer = hTObj1->pInBufHeader->pBuffer;

    /* Check the Parameter list received */

    if(nInputBitstreamSize == 0)
    {
        goto EXIT;
    }

    /* Start parsing through the bitstream header starting from 0xFFD8 */
    for(i=0; i<nInputBitstreamSize;)
    {
        usJpegMarker = *pBuffer++;
        if(usJpegMarker!=0xFF)
        {
            i += 1;
            break;
        }
        usJpegMarker <<= 8;
        usJpegMarker |= *pBuffer++;

        if(usJpegMarker==0xFF00 || usJpegMarker==0xFFFF)
        {
            i += 2;
            break;
        }

        switch(usJpegMarker)
        {
            case 0xFFD8:
                i += 2;
                break;
            case 0xFFE1:
            {
                OMX_U32 ulExifSize=0;
                OMX_U8 *pExifBuffer;
                OMX_EXIF_INFO_SUPPORTED *pExifData;

                pBufferTemp = pBuffer;
                ulSkipFactor = *pBuffer++;
                ulSkipFactor <<= 8;
                ulSkipFactor |= *pBuffer++;

                pBuffer -= 2;
                pBufferTemp = pBuffer;
                ulExifSize = *pBuffer++;
                ulExifSize <<= 8;
                ulExifSize |= *pBuffer++;
                JPEG_DEC_GOTO_EXIT_IF((ulExifSize > 64*1024),
                          OMX_ErrorBadParameter);

                /* Return back to the application if EXIF structure is
                * not allocated, but continue with the decoding process */

                if(hTObj1->pExifInfoSupport != NULL)
                {
                    hTObj1->pExifInfoSupport->ulExifSize = ulExifSize;

                    pExifBuffer = (OMX_U8 *)TIMM_OSAL_MallocExtn(ulExifSize *
                                   sizeof(OMX_U8), TIMM_OSAL_TRUE, 0, 
								   TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    JPEG_DEC_GOTO_EXIT_IF((pExifBuffer==NULL),OMX_ErrorBadParameter);

                    TIMM_OSAL_Memcpy(pExifBuffer,(OMX_U8 *)(pBufferTemp-2),ulExifSize);

                    pExifData = hTObj1->pExifInfoSupport;

                    /* Get EXIF information into the EXIF structure pointer variable pExif */

                        bReturnStatus = TI_GetExifInformation(pExifData, pExifBuffer);
                        JPEG_DEC_GOTO_EXIT_IF((bReturnStatus!=TIMM_OSAL_ERR_NONE),
							                   OMX_ErrorBadParameter);

                        TIMM_OSAL_Free(pExifBuffer);
                    }

                    /* Jump to next marker in the JPEG Header */
                    i += ulSkipFactor;
                    pBuffer = pBufferTemp + ulSkipFactor;
                }
            break;

            case 0xFFC0:/* Start of Frame Header */
                pBufferTemp = pBuffer;
                /*
                 * Get the Length of the marker and various other fields of
                 * SOF header
                 */
                ulSkipFactor = *pBuffer++;
                ulSkipFactor <<= 8;
                ulSkipFactor |= *pBuffer++;
                ulSOFHeaderDone = 1;
                bDataPrecision = *pBuffer++;
                JPEG_DEC_GOTO_EXIT_IF((bDataPrecision != 0x8),
                                         OMX_ErrorBadParameter);

                /*
                 * Width And Height of the Image to be decoded
                 */
                ulImageHeight = *pBuffer++;
                ulImageHeight <<= 8;
                ulImageHeight |= *pBuffer++;

                ulImageWidth = *pBuffer++;
                ulImageWidth <<= 8;
                ulImageWidth |= *pBuffer++;

                /*
                 * Number of color components in the JPEG Image to be decoded
                 * and their Sampling information
                 */
                //nColorComponents = *pBuffer++;
                ++pBuffer;
                //bYSamplingFactor = *pBuffer++;
                //bCbSamplingFactor = *(pBuffer+2);
                //bCrSamplingFactor = *(pBuffer+5);

                /* Jump to next marker in the JPEG Header */
                i += ulSkipFactor;
                pBuffer = pBufferTemp + ulSkipFactor;
            break;
            case 0xFFDA:/* Start of Scan Header */
                JPEG_DEC_GOTO_EXIT_IF((ulSOFHeaderDone == 0),
                                OMX_ErrorBadParameter);

                JPEG_DEC_GOTO_EXIT_IF((nSOSMarkers != 0),
                                OMX_ErrorBadParameter);

                pBufferTemp = pBuffer;
                /*
                 * Get the Length of the marker and various other fields of
                 * SOF header
                 */
                ulSkipFactor = *pBuffer++;
                ulSkipFactor <<= 8;
                ulSkipFactor |= *pBuffer++;
                /*pBuffer += 2;*/
                /*
                 * Number of Scan components in the JPEG Image to be decoded
                 */
                //nScanHeaderComponents = *pBuffer++;

                pBuffer++;
                //bHuffmanTableSelector = *pBuffer++;
                /*
                 * Get the Huffman table (AC/DC) used and store appropriately
                 */

                pBuffer++;

                /* Jump to next marker in the JPEG Header */

				i += ulSkipFactor;
                pBuffer = pBufferTemp + ulSkipFactor;
                nSOSMarkers += 1;
            break;

            case 0xFFC1:
            case 0xFFC2:
            case 0xFFC3:
            case 0xFFC5:
            case 0xFFC6:
            case 0xFFC7:
            case 0xFFC8:
            case 0xFFC9:
            case 0xFFCA:
            case 0xFFCB:
            case 0xFFCD:
            case 0xFFCE:
            case 0xFFCF:
                JPEG_DEC_GOTO_EXIT_IF(OMX_TRUE,
                                OMX_ErrorBadParameter);

            default:

                /* Skip the header which are not relevant ones */

                ulSkipFactor = *(pBuffer);
                ulSkipFactor <<= 8;
                ulSkipFactor |= *(pBuffer+1);
                i += ulSkipFactor;
                pBuffer += ulSkipFactor;
        }

    }
     eError = OMX_ErrorNone;
EXIT:
    return eError;
}

/* ======================================================================= */
/**
*  @fn TI_GetExifInformation() takes two input pointers parses the EXIF
*  part of the bitstream and extracts the EXIF information into the
*  OMX_EXIF_INFO_SUPPORTED data pointer.
*  Returns JPEGOCP_SUCCESS or Error Type.
*
*  @param pExif         A pointer EXIF data structure OMX_EXIF_INFO_SUPPORTED
*  @param pExifBuffer   A pointer to a EXIF Buffer
*
*  @pre                 This is executed only when bitstream has a 'FF E1'
*                       tag somewhere after 'FF D8'.
*
*  @post                None
*  @return              JPEGOCP_SUCCESS or Error Type
*/
/* ======================================================================= */
OMX_ERRORTYPE TI_GetExifInformation(OMX_EXIF_INFO_SUPPORTED *pExif,
                                                            OMX_U8 *pExifBuffer)
{
    OMX_U8 *pBuffer,*pTagHeader,*pValOffset;
    OMX_EXIF_INFO_SUPPORTED *pExif_temp;
    Uint32 SkipFactor, TagCount, TagValOffset, NextIFDOffset, NextOffset;
    OMX_U32 j=0;
    //OMX_U32 LastValOffset;
    Uint16 nTagValues, Tag, TagType;
    OMX_EXIF_THUMBNAIL_INFO *pThumbnail;
    OMX_ERRORTYPE eError = OMX_ErrorNone; // = OMX_ErrorUndefined;
    //TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_U32 ulLittleEndian = 0;
    //OMX_U32 BYTES_REQUIREMENT[11] = {
    //0, 1, 1, 2, 4, 8, 0, 1, 0, 4, 8 };

    pThumbnail = pExif->pThumbnailInfo;

    JPEG_DEC_GOTO_EXIT_IF(((pExif==NULL) || (pExifBuffer==NULL) || (pThumbnail==NULL )),
                                                                OMX_ErrorBadParameter);

    pExif_temp = pExif;
    pBuffer = pExifBuffer;
    if( !(*(pBuffer+4)=='E' || *(pBuffer+4)=='e') &&
        !(*(pBuffer+5)=='X' || *(pBuffer+5)=='x') &&
        !(*(pBuffer+6)=='I' || *(pBuffer+6)=='i') &&
        !(*(pBuffer+7)=='F' || *(pBuffer+7)=='f'))
    {
        JPEGD_Trace("Exif Format not correct. Returning..\n");
        return OMX_ErrorBadParameter;
    }
    pTagHeader = pBuffer + 0xA;
    if((*pTagHeader == 0x49) && (*(pTagHeader+1) == 0x49))
    {
        ulLittleEndian = 1;
    }

    pBuffer += 0xE;

    if(ulLittleEndian == 1)
    {
        SkipFactor = (OMX_U32)(*pBuffer);
    }
    else
    {
        BigEndianRead32Bits(&SkipFactor,pBuffer);
    }

    pBuffer = pTagHeader + SkipFactor;/* 0th IFD starts at this location */

    //LastValOffset = 0;
    NextIFDOffset = 0;
    NextOffset = 0;
    TagValOffset = 0;

    do
    {
        pBuffer += NextOffset;
        NextOffset = 0;

        if(ulLittleEndian == 1)
        {
            nTagValues = (OMX_U16)((*pBuffer) + (*(pBuffer+1)<<8));
        }
        else
        {
            BigEndianRead16Bits(&nTagValues,pBuffer);
        }
        pBuffer += sizeof(OMX_U16);

        for(j=0;j<nTagValues;j++)
        {
            if(ulLittleEndian == 1)
            {
                Tag = (OMX_U16)((*pBuffer) + (*(pBuffer+1)<<8));
                pBuffer += sizeof(OMX_U16);

                TagType = (OMX_U16)((*pBuffer) + (*(pBuffer+1)<<8));
                pBuffer += sizeof(OMX_U16);

                TagCount = (OMX_U32)((*pBuffer) + (*(pBuffer+1)<<8) +
                                      (*(pBuffer+2)<<16) + (*(pBuffer+3)<<24));
                pBuffer += sizeof(OMX_U32);

                TagValOffset = (OMX_U32)((*pBuffer) + (*(pBuffer+1)<<8) +
                                      (*(pBuffer+2)<<16) + (*(pBuffer+3)<<24));
                pBuffer += sizeof(OMX_U32);

            }
            else
            {
                BigEndianRead16Bits(&Tag,pBuffer);
                pBuffer += sizeof(OMX_U16);

                BigEndianRead16Bits(&TagType,pBuffer);
                pBuffer += sizeof(OMX_U16);

                BigEndianRead32Bits(&TagCount,pBuffer);
                pBuffer += sizeof(OMX_U32);

                BigEndianRead32Bits(&TagValOffset,pBuffer);
                pBuffer += sizeof(OMX_U32);
            }

            switch (Tag)
            {
                case 0x013B:/*Artist*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pArtist = (char *)TIMM_OSAL_MallocExtn(TagCount,
						         TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pArtist,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x0112:/*Orientation*/

                    JPEG_DEC_GOTO_EXIT_IF((!(TagType == (OMX_U16)TAG_TYPE_LONG ||
                                           TagType == (OMX_U16)TAG_TYPE_SHORT)),
                                     OMX_ErrorBadParameter);
                    pExif_temp->Orientation = (Exif_Orientation)TagValOffset;
                break;
                case 0x0131:/*Software*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pSoftware = (char *)TIMM_OSAL_MallocExtn(TagCount,
						         TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pSoftware,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x8298:/*Copyright*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pCopyright = (char *)TIMM_OSAL_MallocExtn(TagCount,
						       TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pCopyright,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x9003:/*Create Time*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pCreationDateTime =
                                                 (char *)TIMM_OSAL_MallocExtn(TagCount,
									TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pCreationDateTime,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x0132:/*Last Change Date Time*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pLastChangeDateTime =
                                                  (char *)TIMM_OSAL_MallocExtn(TagCount,
									TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pLastChangeDateTime,pValOffset,
                                                                     TagCount);

                    pValOffset = 0;
                break;
                case 0x010E:/*ImageDescription*/

                     JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pImageDescription =
                                                 (char *)TIMM_OSAL_MallocExtn(TagCount,
									TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pImageDescription,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x010F:/*Make*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pMake = (char *)TIMM_OSAL_MallocExtn(TagCount,
						         TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pMake,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0x0110:/*Model*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_ASCII),
                                     OMX_ErrorBadParameter);

                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pModel = (char *)TIMM_OSAL_MallocExtn(TagCount,
						        TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, NULL);
                    TIMM_OSAL_Memcpy(pExif_temp->pModel,pValOffset,TagCount);

                    pValOffset = 0;
                break;
                case 0xA002:/*imagewidth*/

                    JPEG_DEC_GOTO_EXIT_IF((!(TagType == (OMX_U16)TAG_TYPE_LONG ||
                                           TagType == (OMX_U16)TAG_TYPE_SHORT)),
                                     OMX_ErrorBadParameter);
                    pExif_temp->ulImageWidth = TagValOffset;
                break;
                case 0xA003:/*ImageHeight*/

                    JPEG_DEC_GOTO_EXIT_IF((!(TagType == (OMX_U16)TAG_TYPE_LONG ||
                                           TagType == (OMX_U16)TAG_TYPE_SHORT)),
                                     OMX_ErrorBadParameter);
                    pExif_temp->ulImageHeight = TagValOffset;
                break;
                case 0x8769:/*Next Offset of the 0th IFD*/

                    JPEG_DEC_GOTO_EXIT_IF((TagType != (OMX_U16)TAG_TYPE_LONG),
                                     OMX_ErrorBadParameter);
                    NextOffset = TagValOffset;
                break;
                default:

                break;
            }
/*            if((TagType == TAG_TYPE_ASCII) || (TagType == TAG_TYPE_RATIONAL)
                                            || (TagType == TAG_TYPE_SRATIONAL))
            {
                LastValOffset = TagValOffset + (TagCount *
                                           BYTES_REQUIREMENT[(OMX_U32)TagType]);
            } 
*/
        }
        if(NextIFDOffset==0)
        {
            if(ulLittleEndian == 1)
            {
                NextIFDOffset = (OMX_U32)((*pBuffer) + (*(pBuffer+1)<<8) +
                                (*(pBuffer+2)<<16) + (*(pBuffer+3)<<24));
            }
            else
            {
                BigEndianRead32Bits(&NextIFDOffset,pBuffer);
            }
        }

        if(NextOffset==0)
        {
            if(ulLittleEndian == 1)
            {
                NextOffset = (OMX_U32)((*pBuffer) + (*(pBuffer+1)<<8) +
                             (*(pBuffer+2)<<16) + (*(pBuffer+3)<<24));
            }
            else
            {
                BigEndianRead32Bits(&NextOffset,pBuffer);
            }
            pBuffer += sizeof(OMX_U32);
        }

        if((NextOffset>(pExif->ulExifSize-10)) ||
                  (NextIFDOffset >(pExif->ulExifSize-10)))
        {
            NextIFDOffset =0;
            NextOffset=0;
        }

        pBuffer = pTagHeader;/* Reset */

    } while(NextOffset!=NULL);

    if(NextIFDOffset!=NULL)
    {
        //bReturnStatus = TI_GetThumbnailInformation(pTagHeader, &NextIFDOffset,
                                       //pThumbnail, ulLittleEndian);
        //JPEG_DEC_GOTO_EXIT_IF((bReturnStatus != TIMM_OSAL_ERR_NONE), bReturnStatus);
        //JPEG_DEC_GOTO_EXIT_IF(bReturnStatus != TIMM_OSAL_ERR_NONE,
                                        //OMX_ErrorBadParameter);
    }
    //bReturnStatus=TIMM_OSAL_ERR_NONE;
    eError = OMX_ErrorNone;

EXIT:
    //return bReturnStatus;
    return eError;
}

#endif






///// e n d
