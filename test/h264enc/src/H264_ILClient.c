/*
*   @file  H264D_ILClient.c
*   This file contains the structures and macros that are used in the Application which tests the OpenMAX component
*
*  @path \WTSD_DucatiMMSW\omx\khronos1.1\omx_h264_dec\test
*
*  @rev 1.0
*/

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/**----- system and platform files ----------------------------**/



/**-------program files ----------------------------------------**/
#include "H264_ILClient.h"



#if 0
/*Enable the corresponding flags*/
/*HeapBuf & Shared Region Defines*/
#define OMX_H264E_SRCHANGES 
#define OMX_H264E_BUF_HEAP
#define OMX_H264E_NONTILERTEST
#endif

#if 1
#define OMX_H264E_TILERTEST
#define OMX_H264E_LINUX_TILERTEST
#endif




#ifdef OMX_H264E_BUF_HEAP
#include <unistd.h>
#include <mmplatform.h>
#include <RcmClient.h>
#include <HeapBuf.h>
#include <SharedRegion.h>
#endif

#ifdef OMX_H264E_LINUX_TILERTEST
/*Tiler APIs*/
#include <memmgr/memmgr.h>
#include <mmplatform.h>

#endif


#define H264_INPUT_READY	1
#define H264_OUTPUT_READY	2
#define H264_ERROR_EVENT	4
#define H264_EOS_EVENT 8
#define H264_STATETRANSITION_COMPLETE 16
#define H264_APP_INPUTPORT 0
#define H264_APP_OUTPUTPORT 1

#define NUMTIMESTOPAUSE 1

#ifdef OMX_H264E_BUF_HEAP
extern HeapBuf_Handle heapHandle;
#endif


#ifdef OMX_H264E_DUCATIAPPM3_TILERTEST
#define STRIDE_8BIT (16 * 1024)
#define STRIDE_16BIT (32 * 1024)
#endif

#ifdef OMX_H264E_LINUX_TILERTEST
#define STRIDE_8BIT (4 * 1024)
#define STRIDE_16BIT (4 * 1024)
#endif

//Not on Linux
//static TIMM_OSAL_TRACEGRP TraceGrp;

static TIMM_OSAL_PTR pSem_Events = NULL;

extern H264E_TestCaseParams H264_TestCaseTable[5];
extern H264E_TestCaseParamsAdvanced H264_TestCaseAdvTable[1];
extern H264E_TestCaseParamsDynamic H264_TestCaseDynTable[1];


static TIMM_OSAL_PTR H264VE_Events;
static TIMM_OSAL_PTR H264VE_CmdEvent;


static OMX_U32 nFramesRead=0; /*Frames read from the input file*/
static OMX_U32 TempCount=0;/* loop count to test Pause->Exectuing*/

static OMX_U32 OutputFilesize=0;
static OMX_U32 H264ClientStopFrameNum;


/*Global Settings*/
static OMX_U8 DynamicSettingsCount=0;
static OMX_U8 DynamicSettingsArray[9];
static OMX_U8 DynFrameCountArray_Index[9];
static OMX_U8 PauseFrameNum[NUMTIMESTOPAUSE]={5};	


/*-------------------------function prototypes ------------------------------*/

/*Resource allocation & Free Functions*/
static OMX_ERRORTYPE H264ENC_FreeResources(H264E_ILClient* pAppData);
static OMX_ERRORTYPE H264ENC_AllocateResources(H264E_ILClient* pAppData);

/*Fill Data into the Buffer from the File for the ETB calls*/
OMX_ERRORTYPE H264ENC_FillData(H264E_ILClient* pAppData,OMX_BUFFERHEADERTYPE *pBuf);

/*Static Configurations*/
static OMX_ERRORTYPE H264ENC_SetAllParams(H264E_ILClient* pAppData,OMX_U32 test_index);
OMX_ERRORTYPE H264ENC_SetAdvancedParams(H264E_ILClient* pAppData,OMX_U32 test_index);
OMX_ERRORTYPE H264ENC_SetParamsFromInitialDynamicParams(H264E_ILClient* pAppData,OMX_U32 test_index);

/*Runtime Configurations*/
OMX_ERRORTYPE OMXH264Enc_ConfigureRunTimeSettings(H264E_ILClient * pApplicationData,OMX_H264E_DynamicConfigs* H264EConfigStructures,OMX_U32 test_index,OMX_U32 FrameNumber);
OMX_ERRORTYPE H264E_Populate_BitFieldSettings(OMX_U32 BitFileds,OMX_U8* Count,OMX_U8* Array);
OMX_ERRORTYPE H264E_Update_DynamicSettingsArray(OMX_U8 RemCount,OMX_U32* RemoveArray);


/*Wait for state transition complete*/
static OMX_ERRORTYPE H264ENC_WaitForState(H264E_ILClient* pAppData);

/*Call Back Functions*/
OMX_ERRORTYPE H264ENC_EventHandler(OMX_HANDLETYPE hComponent,OMX_PTR ptrAppData,OMX_EVENTTYPE eEvent,
                           OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);

OMX_ERRORTYPE H264ENC_FillBufferDone (OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer);

OMX_ERRORTYPE H264ENC_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer);

/*Error Handling*/
void OMXH264Enc_Client_ErrorHandling(H264E_ILClient* pApplicationData,OMX_U32 nErrorIndex,OMX_ERRORTYPE eGenError);
OMX_STRING H264_GetErrorString(OMX_ERRORTYPE error);

/*Main Entry Function*/
//void OMXH264Enc_TestEntry(Int32 paramSize, void *pParam);

/*Complete Functionality Implementation*/
OMX_ERRORTYPE OMXH264Enc_CompleteFunctionality(H264E_ILClient* pApplicationData,OMX_U32 test_index);

/*In Executing Loop Functionality*/
OMX_ERRORTYPE OMXH264Enc_InExecuting(H264E_ILClient* pApplicationData,OMX_U32 test_index);

/*Dynamic Port Configuration*/
OMX_ERRORTYPE OMXH264Enc_DynamicPortConfiguration(H264E_ILClient * pApplicationData,OMX_U32 nPortIndex);


/*Tiler Buffer <-> 1D*/
OMX_ERRORTYPE OMXH264_Util_Memcpy_1Dto2D(OMX_PTR pDst2D, OMX_PTR pSrc1D, OMX_U32 nSize1D, OMX_U32 nHeight2D, OMX_U32 nWidth2D, OMX_U32 nStride2D);
OMX_ERRORTYPE OMXH264_Util_Memcpy_2Dto1D(OMX_PTR pDst1D, OMX_PTR pSrc2D, OMX_U32 nSize1D, OMX_U32 nHeight2D, OMX_U32 nWidth2D, OMX_U32 nStride2D);



static OMX_ERRORTYPE H264ENC_FreeResources(H264E_ILClient* pAppData)
{
	OMX_U32 retval = TIMM_OSAL_ERR_UNKNOWN;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_PORT_PARAM_TYPE tPortParams;
	OMX_U32 i,j;
	
	GOTO_EXIT_IF(!pAppData,OMX_ErrorBadParameter);

	TIMM_OSAL_Entering();
	
	pHandle = pAppData->pHandle;
	/*Initialize the structure*/
	OMX_TEST_INIT_STRUCT_PTR(&tPortDef,OMX_PARAM_PORTDEFINITIONTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tPortParams,OMX_PORT_PARAM_TYPE);
		
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoInit, &tPortParams);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	for(i=0; i < tPortParams.nPorts ; i++){
		
		tPortDef.nPortIndex = i;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		if(i==0){
			for (j = 0; j < tPortDef.nBufferCountActual; j++){
				if(!pAppData->H264_TestCaseParams->bInAllocatebuffer)
            			{
#ifdef OMX_H264E_BUF_HEAP
				HeapBuf_free(heapHandle, pAppData->pInBuff[j]->pBuffer, pAppData->pInBuff[j]->nAllocLen);
#elif defined (OMX_H264E_LINUX_TILERTEST)
             	/*Free the TilerBuffer*/
			        MemMgr_Free(pAppData->pInBuff[j]->pBuffer);   

#else        
		               TIMM_OSAL_Free(pAppData->pInBuff[j]->pBuffer);
#endif                 
            			}
				eError = OMX_FreeBuffer(pAppData->pHandle, tPortDef.nPortIndex, pAppData->pInBuff[j]);
				GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			}
		}else if(i==1){
			for (j = 0; j < tPortDef.nBufferCountActual; j++){
				if(!pAppData->H264_TestCaseParams->bInAllocatebuffer)
				{
#ifdef OMX_H264E_BUF_HEAP
				HeapBuf_free(heapHandle, pAppData->pOutBuff[j]->pBuffer, pAppData->pOutBuff[j]->nAllocLen);
#elif defined (OMX_H264E_LINUX_TILERTEST)
             	/*Free the TilerBuffer*/
				MemMgr_Free(pAppData->pOutBuff[j]->pBuffer);
#else                       
				TIMM_OSAL_Free(pAppData->pOutBuff[j]->pBuffer);
#endif               
				}

				eError = OMX_FreeBuffer(pAppData->pHandle, tPortDef.nPortIndex, pAppData->pOutBuff[j]);
				GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			}
		}	
	}

	if(pAppData->IpBuf_Pipe){
		retval = TIMM_OSAL_DeletePipe(pAppData->IpBuf_Pipe);
		GOTO_EXIT_IF((retval != 0),OMX_ErrorNotReady);
	}

	if(pAppData->OpBuf_Pipe){
		retval = TIMM_OSAL_DeletePipe(pAppData->OpBuf_Pipe);
		GOTO_EXIT_IF((retval != 0),OMX_ErrorNotReady);
	}
	if(pAppData->pInBuff)
		TIMM_OSAL_Free(pAppData->pInBuff);

	if(pAppData->pOutBuff)
		TIMM_OSAL_Free(pAppData->pOutBuff);


EXIT:
		TIMM_OSAL_Exiting(eError);
		return eError;
}


static OMX_ERRORTYPE H264ENC_AllocateResources(H264E_ILClient* pAppData)
{
	OMX_U32 retval = TIMM_OSAL_ERR_UNKNOWN;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	OMX_PORT_PARAM_TYPE tPortParams;
	OMX_U8 *pTmpBuffer; /*Used with Use Buffer calls*/	
	OMX_U32 i,j;
	
#ifdef OMX_H264E_LINUX_TILERTEST
	MemAllocBlock *MemReqDescTiler;
	OMX_PTR TilerAddr=NULL;
#endif

	TIMM_OSAL_Entering();
	GOTO_EXIT_IF(!pAppData,OMX_ErrorBadParameter);
	
	pHandle = pAppData->pHandle;
#ifdef OMX_H264E_LINUX_TILERTEST
	MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	GOTO_EXIT_IF((MemReqDescTiler== TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);	
#endif

	/*Initialize the structure*/
	OMX_TEST_INIT_STRUCT_PTR(&tPortDef,OMX_PARAM_PORTDEFINITIONTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tPortParams,OMX_PORT_PARAM_TYPE);
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoInit, &tPortParams);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	for(i=0; i < tPortParams.nPorts ; i++){
	tPortDef.nPortIndex = i;
	eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		if(tPortDef.nPortIndex==H264_APP_INPUTPORT){			
			pAppData->pInBuff = TIMM_OSAL_Malloc((sizeof(OMX_BUFFERHEADERTYPE*) * tPortDef.nBufferCountActual), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
			GOTO_EXIT_IF((pAppData->pInBuff == TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);
			/* Create a pipes for Input Buffers.. used to queue data from the callback. */
			TIMM_OSAL_Trace("\nCreating i/p pipe\n");
    			retval = TIMM_OSAL_CreatePipe(&(pAppData->IpBuf_Pipe),sizeof(OMX_BUFFERHEADERTYPE*) * tPortDef.nBufferCountActual,
    									sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);	
			
			/*allocate/Use buffer calls for input*/	
			for (j = 0; j < tPortDef.nBufferCountActual; j++)
			{
				if(pAppData->H264_TestCaseParams->bInAllocatebuffer){
					/*Min Buffer requirements depends on the Amount of Stride.....here No padding so Width==Stride */
					eError = OMX_AllocateBuffer(pAppData->pHandle, &(pAppData->pInBuff[j]), tPortDef.nPortIndex, pAppData, ((pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height*3/2)));
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);	

					retval = TIMM_OSAL_WriteToPipe(pAppData->IpBuf_Pipe, &(pAppData->pInBuff[j]), sizeof(pAppData->pInBuff[j]), TIMM_OSAL_SUSPEND);
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);
				}else{
					/*USE Buffer calls*/
#ifdef OMX_H264E_BUF_HEAP
					pTmpBuffer = HeapBuf_alloc(heapHandle, (pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height*3/2),0);

#elif defined (OMX_H264E_LINUX_TILERTEST)
					MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
					MemReqDescTiler[0].dim.area.width=pAppData->H264_TestCaseParams->width;/*width*/
					MemReqDescTiler[0].dim.area.height=pAppData->H264_TestCaseParams->height;/*height*/
					MemReqDescTiler[0].stride=STRIDE_8BIT;
					
					MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
					MemReqDescTiler[1].dim.area.width=pAppData->H264_TestCaseParams->width/2;/*width*/
					MemReqDescTiler[1].dim.area.height=pAppData->H264_TestCaseParams->height/2;/*height*/
					MemReqDescTiler[1].stride=STRIDE_16BIT;
					
					//MemReqDescTiler.reserved
					/*call to tiler Alloc*/
					TIMM_OSAL_Trace("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
					TIMM_OSAL_Trace("\nTiler buffer allocated is %x\n",TilerAddr);
                    pTmpBuffer = (OMX_U8 *)TilerAddr;
						
#else                      
					pTmpBuffer= TIMM_OSAL_Malloc(((pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height*3/2)), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif		
                   			 GOTO_EXIT_IF((pTmpBuffer== TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);
#ifdef OMX_H264E_SRCHANGES        
					TIMM_OSAL_Trace("\npBuffer before UB = %x\n",pTmpBuffer);
					pTmpBuffer = (char *)SharedRegion_getSRPtr(pTmpBuffer, 2);
					TIMM_OSAL_Trace("\npBuffer SR before UB = %x\n",pTmpBuffer);
#endif
					GOTO_EXIT_IF((pTmpBuffer== TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);
					eError = OMX_UseBuffer(pAppData->pHandle, &(pAppData->pInBuff[j]), tPortDef.nPortIndex, pAppData, ((pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height*3/2)),pTmpBuffer);
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#ifdef OMX_H264E_SRCHANGES
					TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",pAppData->pInBuff[j]->pBuffer);
					pAppData->pInBuff[j]->pBuffer = SharedRegion_getPtr(pAppData->pInBuff[j]->pBuffer);
					TIMM_OSAL_Trace("\npBuffer after UB = %x\n",pAppData->pInBuff[j]->pBuffer);	
#endif 
					TIMM_OSAL_Trace("\nWriting to i/p pipe\n");	
					
					retval = TIMM_OSAL_WriteToPipe(pAppData->IpBuf_Pipe, &(pAppData->pInBuff[j]), sizeof(pAppData->pInBuff[j]), TIMM_OSAL_SUSPEND);
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);
					
				}				
				
			}
		
            TIMM_OSAL_Trace("\nSetting i/p ready event\n");
			retval = TIMM_OSAL_EventSet (H264VE_Events, H264_INPUT_READY, TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
            TIMM_OSAL_Trace("\ni/p ready event set\n");
		}else if(tPortDef.nPortIndex==H264_APP_OUTPUTPORT){
			
			pAppData->pOutBuff = TIMM_OSAL_Malloc((sizeof(OMX_BUFFERHEADERTYPE*) * tPortDef.nBufferCountActual), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
			GOTO_EXIT_IF((pAppData->pOutBuff == TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);
			/* Create a pipes for Output Buffers.. used to queue data from the callback. */	
			TIMM_OSAL_Trace("\nCreating o/p pipe\n");
			retval = TIMM_OSAL_CreatePipe(&(pAppData->OpBuf_Pipe),sizeof(OMX_BUFFERHEADERTYPE*) * tPortDef.nBufferCountActual,
													sizeof(OMX_BUFFERHEADERTYPE*), OMX_TRUE );
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);
			/*allocate buffers for output*/
			for (j= 0; j < tPortDef.nBufferCountActual; j++)
			{
				if(pAppData->H264_TestCaseParams->bOutAllocatebuffer){
					/*Min Buffer requirements depends on the Amount of Stride.....here No padding so Width==Stride */
					eError = OMX_AllocateBuffer(pAppData->pHandle, &(pAppData->pOutBuff[j]), tPortDef.nPortIndex, pAppData, (pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height));
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);		

					retval=TIMM_OSAL_WriteToPipe(pAppData->OpBuf_Pipe, &(pAppData->pOutBuff[j]), sizeof(pAppData->pOutBuff[j]), TIMM_OSAL_SUSPEND);
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);		
				}else{
					/*USE Buffer calls*/
#ifdef OMX_H264E_BUF_HEAP
        				pTmpBuffer = HeapBuf_alloc(heapHandle, (pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height),0);
   
#elif defined (OMX_H264E_LINUX_TILERTEST)
					MemReqDescTiler[0].pixelFormat=PIXEL_FMT_PAGE;
					MemReqDescTiler[0].dim.len=pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height;
					MemReqDescTiler[0].stride = 0;
                    //MemReqDescTiler.stride
					//MemReqDescTiler.reserved
					/*call to tiler Alloc*/
					TIMM_OSAL_Trace("\nBefore tiler alloc for the Codec Internal buffer %d\n");
					TilerAddr=MemMgr_Alloc(MemReqDescTiler,1);
					TIMM_OSAL_Trace("\nTiler buffer allocated is %x\n",TilerAddr);
					pTmpBuffer=(OMX_U8*)TilerAddr;
#else                     
					pTmpBuffer= TIMM_OSAL_Malloc(((pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height)), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
#endif                    
					GOTO_EXIT_IF((pTmpBuffer== TIMM_OSAL_NULL),OMX_ErrorInsufficientResources);
#ifdef OMX_H264E_SRCHANGES        
					TIMM_OSAL_Trace("\npBuffer before UB = %x\n",pTmpBuffer);
					pTmpBuffer = (char *)SharedRegion_getSRPtr(pTmpBuffer, 2);
					TIMM_OSAL_Trace("\npBuffer SR before UB = %x\n",pTmpBuffer);
#endif   
					TIMM_OSAL_Trace("\ncall to use buffer\n");	
					eError = OMX_UseBuffer(pAppData->pHandle, &(pAppData->pOutBuff[j]), tPortDef.nPortIndex, pAppData, ((pAppData->H264_TestCaseParams->width*pAppData->H264_TestCaseParams->height)),pTmpBuffer);
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#ifdef OMX_H264E_SRCHANGES
					TIMM_OSAL_Trace("\npBuffer SR after UB = %x\n",pAppData->pOutBuff[j]->pBuffer);
					pAppData->pOutBuff[j]->pBuffer = SharedRegion_getPtr(pAppData->pOutBuff[j]->pBuffer);
					TIMM_OSAL_Trace("\npBuffer after UB = %x\n",pAppData->pOutBuff[j]->pBuffer); 
#endif 

					TIMM_OSAL_Trace("\nWriting to o/p pipe\n");	
					retval = TIMM_OSAL_WriteToPipe(pAppData->OpBuf_Pipe, &(pAppData->pOutBuff[j]), sizeof(pAppData->pOutBuff[j]), TIMM_OSAL_SUSPEND);
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorInsufficientResources);
				}	
				
			}
		
            TIMM_OSAL_Trace("\nSetting o/p ready event\n");
			retval = TIMM_OSAL_EventSet (H264VE_Events, H264_OUTPUT_READY, TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
            TIMM_OSAL_Trace("\no/p ready event set\n");
		}else{
			TIMM_OSAL_Trace("Port index Assigned is Neither Input Nor Output");
			eError=OMX_ErrorBadPortIndex;			
			GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		}
	}
#ifdef OMX_H264E_LINUX_TILERTEST 
    	if(MemReqDescTiler)
		TIMM_OSAL_Free(MemReqDescTiler);
    
#endif	 	
EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;

}


OMX_STRING H264_GetErrorString(OMX_ERRORTYPE error)
{
	OMX_STRING errorString;
	TIMM_OSAL_Entering();

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


OMX_ERRORTYPE H264ENC_FillData(H264E_ILClient* pAppData,OMX_BUFFERHEADERTYPE *pBuf)
{
	OMX_U32 nRead = 0;
	OMX_U32 framesizetoread = 0;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
#ifdef OMX_H264E_TILERTEST	
    	OMX_U8 *pTmpBuffer = NULL;
	OMX_U32 size1D = 0;
#endif
#ifdef  OMX_H264E_LINUX_TILERTEST
	OMX_U32 Addoffset=0;
#endif

	TIMM_OSAL_Entering();
	
	framesizetoread = pAppData->H264_TestCaseParams->width * pAppData->H264_TestCaseParams->height * 3  / 2;

	/*Tiler Interface code*/
#ifdef OMX_H264E_TILERTEST
	 pTmpBuffer = (OMX_U8 *)TIMM_OSAL_Malloc(framesizetoread,OMX_FALSE,0,0);
       if(pTmpBuffer == NULL){
	   	eError=OMX_ErrorInsufficientResources;
	   	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
       }      
	 size1D=framesizetoread;
	 if((!feof(pAppData->fIn))&&((nRead = fread(pTmpBuffer,sizeof(OMX_U8),framesizetoread, pAppData->fIn))!=0)&&(H264ClientStopFrameNum>nFramesRead)){
		nFramesRead++; 
		TIMM_OSAL_Trace( "number of bytes read from %d frame=%d",nFramesRead,nRead);
		eError= OMXH264_Util_Memcpy_1Dto2D(pBuf->pBuffer, pTmpBuffer, size1D, pAppData->H264_TestCaseParams->height, pAppData->H264_TestCaseParams->width, STRIDE_8BIT); 
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		if(framesizetoread < (pAppData->H264_TestCaseParams->height * pAppData->H264_TestCaseParams->width))
		   pTmpBuffer += size1D;
		else{
		   pTmpBuffer += (pAppData->H264_TestCaseParams->height * pAppData->H264_TestCaseParams->width);
		   size1D-=(pAppData->H264_TestCaseParams->height * pAppData->H264_TestCaseParams->width);
		}
		#ifdef OMX_H264E_DUCATIAPPM3_TILERTEST
		eError= OMXH264_Util_Memcpy_1Dto2D(((OMX_TI_PLATFORMPRIVATE *)pBuf->pPlatformPrivate)->pAuxBuf1, pTmpBuffer, size1D, 
		           								(pAppData->H264_TestCaseParams->height/2), pAppData->H264_TestCaseParams->width, STRIDE_16BIT); 
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		#endif
		#ifdef OMX_H264E_LINUX_TILERTEST
		Addoffset=(pAppData->H264_TestCaseParams->height * STRIDE_8BIT);
		eError= OMXH264_Util_Memcpy_1Dto2D((pBuf->pBuffer+Addoffset), pTmpBuffer, size1D, 
		           								(pAppData->H264_TestCaseParams->height/2), pAppData->H264_TestCaseParams->width, STRIDE_16BIT); 
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		#endif
		
	}
	else{	
			/*Incase of Full Record update the StopFrame nUmber to limit the ETB & FTB calls*/
			H264ClientStopFrameNum=nFramesRead;
			pBuf->nFlags = OMX_BUFFERFLAG_EOS;
			TIMM_OSAL_Trace("end of file reached and EOS flag has been set");
	}
    
#endif

#ifdef OMX_H264E_NONTILERTEST
  /**********************Original Code*************************************************/	
	if((!feof(pAppData->fIn))&&((nRead = fread(pBuf->pBuffer,sizeof(OMX_U8),framesizetoread, pAppData->fIn))!=0)&&(H264ClientStopFrameNum>nFramesRead)){
			nFramesRead++; 
			TIMM_OSAL_Trace( "number of bytes read from %d frame=%d",nFramesRead,nRead);
		
	}
	else{	
			/*Incase of Full Record update the StopFrame nUmber to limit the ETB & FTB calls*/
			H264ClientStopFrameNum=nFramesRead;
			pBuf->nFlags = OMX_BUFFERFLAG_EOS;
			TIMM_OSAL_Trace("end of file reached and EOS flag has been set");
	}
	/**********************Original Code  ENDS*************************************************/
#endif
	pBuf->nFilledLen  = nRead;
	pBuf->nOffset=0; /*offset is Zero*/
EXIT:
TIMM_OSAL_Exiting(eError);
return eError;
	
}

OMX_ERRORTYPE H264ENC_SetAdvancedParams(H264E_ILClient* pAppData,OMX_U32 test_index){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
    	OMX_HANDLETYPE pHandle = NULL;
    	OMX_U8 AdvancedSettingsCount=0;
    	OMX_U8 AdancedSettingsArray[6];
    	OMX_U8 i;
	OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE tNALUSettings;	
	OMX_VIDEO_PARAM_MEBLOCKSIZETYPE tMEBlockSize;
	OMX_VIDEO_PARAM_INTRAREFRESHTYPE tIntrarefresh;
	OMX_VIDEO_PARAM_VUIINFOTYPE tVUISettings;
	OMX_VIDEO_PARAM_INTRAPREDTYPE tIntraPred;
	OMX_VIDEO_PARAM_AVCADVANCEDFMOTYPE tAdvancedFMO;
	OMX_VIDEO_PARAM_DATASYNCMODETYPE tVidDataMode;	

	TIMM_OSAL_Entering();
	
    	if(!pAppData){
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}
	pHandle = pAppData->pHandle;

	/*Get the BitFiledSettings*/
    	eError= H264E_Populate_BitFieldSettings(pAppData->H264_TestCaseParams->nBitEnableAdvanced,&AdvancedSettingsCount,AdancedSettingsArray);
    	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	for(i=0;i<AdvancedSettingsCount;i++){
		switch(AdancedSettingsArray[i]){
			case 0:				
				if(pAppData->H264_TestCaseParams->BitStreamFormat==OMX_Video_BitStreamFormatNAL){					
					OMX_TEST_INIT_STRUCT_PTR(&tNALUSettings,OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE);
					/*Set the Port*/
					tNALUSettings.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoNALUsettings, &tNALUSettings);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tNALUSettings.nStartofSequence= H264_TestCaseAdvTable[test_index].NALU.nStartofSequence;     
				  	tNALUSettings.nEndofSequence= H264_TestCaseAdvTable[test_index].NALU.nEndofSequence;
					tNALUSettings.nIDR=H264_TestCaseAdvTable[test_index].NALU.nIDR;
					tNALUSettings.nIntraPicture=H264_TestCaseAdvTable[test_index].NALU.nIntraPicture;
					tNALUSettings.nNonIntraPicture=H264_TestCaseAdvTable[test_index].NALU.nNonIntraPicture;
					
					eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoNALUsettings, &tNALUSettings);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
    				}else{
    					TIMM_OSAL_Error( "Set the Bit Stream format to NALU to take these settings into effect");	
    				}
				
			break;
			case 1:
				/*set param to OMX_TI_VIDEO_PARAM_FMO_ADVANCEDSETTINGS*/
				if(pAppData->H264_TestCaseParams->bFMO==OMX_TRUE){					
					OMX_TEST_INIT_STRUCT_PTR(&tAdvancedFMO,OMX_VIDEO_PARAM_AVCADVANCEDFMOTYPE);
					/*Set the Port*/
					tAdvancedFMO.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoAdvancedFMO, &tAdvancedFMO);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tAdvancedFMO.nNumSliceGroups= H264_TestCaseAdvTable[test_index].FMO.nNumSliceGroups;
					tAdvancedFMO.nSliceGroupMapType= H264_TestCaseAdvTable[test_index].FMO.nSliceGroupMapType;   
					tAdvancedFMO.nSliceGroupChangeRate= H264_TestCaseAdvTable[test_index].FMO.nSliceGroupChangeRate;   
					tAdvancedFMO.nSliceGroupChangeCycle= H264_TestCaseAdvTable[test_index].FMO.nSliceGroupChangeCycle; 
					for(i=0;i<H264ENC_MAXNUMSLCGPS;i++){
						tAdvancedFMO.nSliceGroupParams[i]= H264_TestCaseAdvTable[test_index].FMO.nSliceGroupParams[i];  
					}										
					eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoAdvancedFMO, &tAdvancedFMO);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
				}else{
						TIMM_OSAL_Error( "Set the FMO flag to TRUE to take these settings into effect");	
				}
			break;
			case 2:
				/*set param to OMX_TI_VIDEO_PARAM_ME_BLOCKSIZE*/
				OMX_TEST_INIT_STRUCT_PTR(&tMEBlockSize,OMX_VIDEO_PARAM_MEBLOCKSIZETYPE);
				/*Set the Port*/
				tMEBlockSize.nPortIndex=H264_APP_OUTPUTPORT;
				eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoMEBlockSize, &tMEBlockSize);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				
				tMEBlockSize.eMinBlockSizeP= H264_TestCaseAdvTable[test_index].MEBlock.eMinBlockSizeP;     
			  	tMEBlockSize.eMinBlockSizeB= H264_TestCaseAdvTable[test_index].MEBlock.eMinBlockSizeB;			
				
				eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoMEBlockSize, &tMEBlockSize);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
			break;
			case 3:
				/*set param to OMX_VIDEO_PARAM_INTRAREFRESHTYPE*/
				OMX_TEST_INIT_STRUCT_PTR(&tIntrarefresh,OMX_VIDEO_PARAM_INTRAREFRESHTYPE);
				/*Set the Port*/
				tIntrarefresh.nPortIndex=H264_APP_OUTPUTPORT;
				eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoIntraRefresh, &tIntrarefresh);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				
				tIntrarefresh.eRefreshMode= H264_TestCaseAdvTable[test_index].IntraRefresh.eRefreshMode;
				if(tIntrarefresh.eRefreshMode==OMX_VIDEO_IntraRefreshBoth){
					tIntrarefresh.nAirRef=H264_TestCaseAdvTable[test_index].IntraRefresh.nRate;
				}
				else if(tIntrarefresh.eRefreshMode==OMX_VIDEO_IntraRefreshCyclic){
					tIntrarefresh.nCirMBs=H264_TestCaseAdvTable[test_index].IntraRefresh.nRate;
				}else if(tIntrarefresh.eRefreshMode==OMX_VIDEO_IntraRefreshAdaptive){
					tIntrarefresh.nAirMBs=H264_TestCaseAdvTable[test_index].IntraRefresh.nRate;					
				}else{
					TIMM_OSAL_Error( "Unsupported settings");	
				}
			  	eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoIntraRefresh, &tIntrarefresh);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
			break;
			case 4:
				/*set param to OMX_VIDEO_PARAM_VUIINFOTYPE*/
				OMX_TEST_INIT_STRUCT_PTR(&tVUISettings,OMX_VIDEO_PARAM_VUIINFOTYPE);
				/*Set the Port*/
				tVUISettings.nPortIndex=H264_APP_OUTPUTPORT;
				eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoVUIsettings, &tVUISettings);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				
				tVUISettings.bAspectRatioPresent= H264_TestCaseAdvTable[test_index].VUI.bAspectRatioPresent;     
			  	tVUISettings.bFullRange= H264_TestCaseAdvTable[test_index].VUI.bFullRange;
				tVUISettings.ePixelAspectRatio= H264_TestCaseAdvTable[test_index].VUI.eAspectRatio;
				
				eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoVUIsettings, &tVUISettings);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
			break;
			case 5:
				/*set param to OMX_TI_VIDEO_PARAM_INTRAPRED*/
				OMX_TEST_INIT_STRUCT_PTR(&tIntraPred,OMX_VIDEO_PARAM_INTRAPREDTYPE);
				/*Set the Port*/
				tIntraPred.nPortIndex=H264_APP_OUTPUTPORT;
				eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoIntraPredictionSettings, &tIntraPred);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				
				tIntraPred.nLumaIntra16x16Enable= H264_TestCaseAdvTable[test_index].IntraPred.nLumaIntra16x16Enable;     
			  	tIntraPred.nLumaIntra8x8Enable= H264_TestCaseAdvTable[test_index].IntraPred.nLumaIntra8x8Enable;
				tIntraPred.nLumaIntra4x4Enable= H264_TestCaseAdvTable[test_index].IntraPred.nLumaIntra4x4Enable;
				tIntraPred.nLumaIntra8x8Enable= H264_TestCaseAdvTable[test_index].IntraPred.nChromaIntra8x8Enable;
				tIntraPred.eChromaComponentEnable= H264_TestCaseAdvTable[test_index].IntraPred.eChromaComponentEnable;
				
				eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoIntraPredictionSettings, &tIntraPred);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError); 
			break;
			case 6:
				/*set param to OMX_VIDEO_PARAM_DATASYNCMODETYPE*/
				/*Initialize the structure*/
				OMX_TEST_INIT_STRUCT_PTR(&tVidDataMode,OMX_VIDEO_PARAM_DATASYNCMODETYPE);
				/*DATA SYNC MODE Settings*/	
				/*Set the Port*/
				tIntraPred.nPortIndex=H264_APP_INPUTPORT;
				eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDataSyncMode, &tVidDataMode);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				tVidDataMode.eDataMode = H264_TestCaseAdvTable[test_index].DataSync.inputDataMode;
				tVidDataMode.nNumDataUnits = H264_TestCaseAdvTable[test_index].DataSync.numInputDataUnits;
				eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDataSyncMode, &tVidDataMode);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				/*Set the Port*/
				tIntraPred.nPortIndex=H264_APP_OUTPUTPORT;
				eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDataSyncMode, &tVidDataMode);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
				tVidDataMode.eDataMode = H264_TestCaseAdvTable[test_index].DataSync.outputDataMode;
				tVidDataMode.nNumDataUnits =H264_TestCaseAdvTable[test_index].DataSync.numOutputDataUnits;
				eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoDataSyncMode, &tVidDataMode);
				GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
			break;
			default:
				TIMM_OSAL_Error( "Invalid inputs");	
		}
	}

 EXIT:
 	TIMM_OSAL_Exiting(eError);
    	return eError;   	
}

OMX_ERRORTYPE H264E_Populate_BitFieldSettings(OMX_U32 BitFileds,OMX_U8* Count,OMX_U8* Array){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U8 i,j,LCount=0;

	TIMM_OSAL_Entering();
	
	for(i=0,j=0;i<6;i++){
		if(BitFileds>>i){
			Array[j++]=i;
			LCount++;			
		}		
	}
	*Count=LCount;
	TIMM_OSAL_Exiting(eError);
	return eError;
}
OMX_ERRORTYPE H264E_Update_DynamicSettingsArray(OMX_U8 RemCount,OMX_U32* RemoveArray){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U8 i,j,LCount=0,LRemcount=0;
	OMX_U8 LocalDynArray[9],LocalDynArray_Index[9];

	TIMM_OSAL_Entering();
	
	for(i=0;i<DynamicSettingsCount;i++){
		for(j=LRemcount;j<RemCount;j++){
			if(RemoveArray[j]!=DynamicSettingsArray[i]){
				LocalDynArray[LCount]=DynamicSettingsArray[i];
				LocalDynArray_Index[LCount]=DynFrameCountArray_Index[i];
				LCount++;
				LRemcount++;
			}
		}
	}
	/*update the DynamicSettingsArray after removing*/
	DynamicSettingsCount=LCount;
	for(i=0;i<DynamicSettingsCount;i++){
		DynamicSettingsArray[i]=LocalDynArray[LCount];
		DynFrameCountArray_Index[i]=LocalDynArray_Index[LCount];
	}
	TIMM_OSAL_Exiting(eError);
	return eError;
}


OMX_ERRORTYPE H264ENC_SetParamsFromInitialDynamicParams(H264E_ILClient* pAppData,OMX_U32 test_index){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
    	OMX_HANDLETYPE pHandle = NULL;
    	OMX_U32 LFrameRate,LBitRate,LIntraPeriod;
    	OMX_BOOL bFramerate=OMX_FALSE,bBitrate=OMX_FALSE,bIntraperiod=OMX_FALSE;
    	OMX_U32 nRemoveList[9];
    	OMX_U8 i,RemoveIndex=0;

	OMX_VIDEO_CONFIG_MESEARCHRANGETYPE tMESearchrange;
	OMX_CONFIG_INTRAREFRESHVOPTYPE tIntraRefresh;
	OMX_CONFIG_INTRAREFRESHVOPTYPE tForceFrame;
	OMX_VIDEO_CONFIG_QPSETTINGSTYPE tQPSettings;
	OMX_VIDEO_CONFIG_SLICECODINGTYPE tSliceCodingparams;
	OMX_VIDEO_CONFIG_PIXELINFOTYPE tPixelInfo;

	OMX_VIDEO_PARAM_AVCTYPE tAVCParams;
	OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
	OMX_VIDEO_PARAM_PORTFORMATTYPE tVideoParams;
	OMX_PORT_PARAM_TYPE tPortParams;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

	TIMM_OSAL_Entering();
	
	OMX_TEST_INIT_STRUCT_PTR(&tPortDef,OMX_PARAM_PORTDEFINITIONTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tPortParams,OMX_PORT_PARAM_TYPE);
	OMX_TEST_INIT_STRUCT_PTR(&tVideoParams,OMX_VIDEO_PARAM_PORTFORMATTYPE);	
	OMX_TEST_INIT_STRUCT_PTR(&tVidEncBitRate,OMX_VIDEO_PARAM_BITRATETYPE);	
	OMX_TEST_INIT_STRUCT_PTR(&tAVCParams,OMX_VIDEO_PARAM_AVCTYPE);
	
	
    	if(!pAppData){
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}
	pHandle = pAppData->pHandle;

	eError= H264E_Populate_BitFieldSettings(pAppData->H264_TestCaseParams->nBitEnableDynamic,&DynamicSettingsCount,DynamicSettingsArray);
    	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	
    	for(i=0;i<DynamicSettingsCount;i++){
		switch(DynamicSettingsArray[i]){
			case 0:
				if(H264_TestCaseDynTable[test_index].DynFrameRate.nFrameNumber[DynFrameCountArray_Index[i]]==0){
					bFramerate=OMX_TRUE;
					LFrameRate=H264_TestCaseDynTable[test_index].DynFrameRate.nFramerate[DynFrameCountArray_Index[i]];
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynFrameRate.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
								
			break;
			case 1:
				if(H264_TestCaseDynTable[test_index].DynBitRate.nFrameNumber[DynFrameCountArray_Index[i]]==0){
					bBitrate=OMX_TRUE;
					LBitRate=H264_TestCaseDynTable[test_index].DynBitRate.nBitrate[DynFrameCountArray_Index[i]];
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynBitRate.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 2:
				/*set config to OMX_TI_VIDEO_CONFIG_ME_SEARCHRANGE*/
				
				if(H264_TestCaseDynTable[test_index].DynMESearchRange.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					OMX_TEST_INIT_STRUCT_PTR(&tMESearchrange,OMX_VIDEO_CONFIG_MESEARCHRANGETYPE);				
					eError = OMX_GetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigVideoMESearchRange, &tMESearchrange);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tMESearchrange.eMVAccuracy=H264_TestCaseDynTable[test_index].DynMESearchRange.nMVAccuracy[DynFrameCountArray_Index[i]];
					tMESearchrange.nHorSearchRangeP=H264_TestCaseDynTable[test_index].DynMESearchRange.nHorSearchRangeP[DynFrameCountArray_Index[i]];
					tMESearchrange.nVerSearchRangeP=H264_TestCaseDynTable[test_index].DynMESearchRange.nVerSearchRangeP[DynFrameCountArray_Index[i]];
					tMESearchrange.nHorSearchRangeB=H264_TestCaseDynTable[test_index].DynMESearchRange.nHorSearchRangeB[DynFrameCountArray_Index[i]];
					tMESearchrange.nVerSearchRangeB=H264_TestCaseDynTable[test_index].DynMESearchRange.nVerSearchRangeB[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigVideoMESearchRange, &tMESearchrange);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynMESearchRange.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 3:
				/*set config to OMX_CONFIG_INTRAREFRESHVOPTYPE*/
				if(H264_TestCaseDynTable[test_index].DynForceFrame.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					OMX_TEST_INIT_STRUCT_PTR(&tForceFrame,OMX_CONFIG_INTRAREFRESHVOPTYPE);				
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tForceFrame);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tForceFrame.IntraRefreshVOP=H264_TestCaseDynTable[test_index].DynForceFrame.ForceIFrame[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tForceFrame);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynForceFrame.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 4:
				/*set config to OMX_TI_VIDEO_CONFIG_QPSETTINGS*/
				if(H264_TestCaseDynTable[test_index].DynQpSettings.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					OMX_TEST_INIT_STRUCT_PTR(&tQPSettings,OMX_VIDEO_CONFIG_QPSETTINGSTYPE);				
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tQPSettings);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tQPSettings.nQpI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpI[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMaxI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxI[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMinI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinI[DynFrameCountArray_Index[i]];
					tQPSettings.nQpP=H264_TestCaseDynTable[test_index].DynQpSettings.nQpP[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMaxI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxP[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMinI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinP[DynFrameCountArray_Index[i]];
					tQPSettings.nQpOffsetB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpOffsetB[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMaxB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxB[DynFrameCountArray_Index[i]];
					tQPSettings.nQpMinB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinB[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &tQPSettings);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynQpSettings.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 5:
				/*set config to OMX_VIDEO_CONFIG_AVCINTRAPERIOD*/				
				if(H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nFrameNumber[DynFrameCountArray_Index[i]]==0){					
					bIntraperiod=OMX_TRUE;
					LIntraPeriod=H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nIntraFrameInterval[DynFrameCountArray_Index[i]];					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 6:
				/*set config to OMX_TI_VIDEO_CONFIG_SLICECODING*/
				if(H264_TestCaseDynTable[test_index].DynSliceSettings.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					OMX_TEST_INIT_STRUCT_PTR(&tSliceCodingparams,OMX_VIDEO_CONFIG_SLICECODINGTYPE);				
					eError = OMX_GetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigSliceSettings, &tSliceCodingparams);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tSliceCodingparams.eSliceMode=H264_TestCaseDynTable[test_index].DynSliceSettings.eSliceMode[DynFrameCountArray_Index[i]];
					tSliceCodingparams.nSlicesize=H264_TestCaseDynTable[test_index].DynSliceSettings.nSlicesize[DynFrameCountArray_Index[i]];
					
					eError = OMX_SetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigSliceSettings, &tSliceCodingparams);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynSliceSettings.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
				
			break;
			case 7:
				/*set config to OMX_TI_VIDEO_CONFIG_PIXELINFO*/
				if(H264_TestCaseDynTable[test_index].DynPixelInfo.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					OMX_TEST_INIT_STRUCT_PTR(&tPixelInfo,OMX_VIDEO_CONFIG_PIXELINFOTYPE);				
					eError = OMX_GetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigVideoPixelInfo, &tPixelInfo);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					tPixelInfo.nWidth=H264_TestCaseDynTable[test_index].DynPixelInfo.nWidth[DynFrameCountArray_Index[i]];
					tPixelInfo.nHeight=H264_TestCaseDynTable[test_index].DynPixelInfo.nHeight[DynFrameCountArray_Index[i]];
					
					eError = OMX_SetConfig(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexConfigVideoPixelInfo, &tPixelInfo);
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynPixelInfo.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;			
			default:
				TIMM_OSAL_Error( "Invalid inputs");	
		}
	}

	/*Get the Number of Ports*/
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoInit, &tPortParams);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	for(i=0; i < tPortParams.nPorts ; i++){
		tPortDef.nPortIndex = i;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoPortFormat, &tVideoParams);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

		if(tPortDef.eDir == OMX_DirInput){
			
			/*set the actual number of buffers required*/
			tPortDef.nBufferCountActual=pAppData->H264_TestCaseParams->maxInterFrameInterval;
			/*set the video format settings*/
			tPortDef.format.video.nFrameWidth = pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nStride = pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nFrameHeight = pAppData->H264_TestCaseParams->height;
			tPortDef.format.video.eColorFormat = pAppData->H264_TestCaseParams->inputChromaFormat;
			
			/*settings for OMX_IndexParamVideoPortFormat*/
			tVideoParams.eColorFormat = pAppData->H264_TestCaseParams->inputChromaFormat;
			
		}
		if(tPortDef.eDir == OMX_DirOutput){
			/*settings for OMX_IndexParamPortDefinition*/
			tPortDef.format.video.nFrameWidth = pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nStride = pAppData->H264_TestCaseParams->width;
			tPortDef.format.video.nFrameHeight = pAppData->H264_TestCaseParams->height;			
			tPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;			
			if(bFramerate){
				tPortDef.format.video.xFramerate =LFrameRate;
				tVideoParams.xFramerate=LFrameRate;
			}
			if(bBitrate){
				tPortDef.format.video.nBitrate=LBitRate;
			}
			/*settings for OMX_IndexParamVideoPortFormat*/
			tVideoParams.eCompressionFormat = OMX_VIDEO_CodingAVC;			
		}

		eError = OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoPortFormat, &tVideoParams);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

		/*AVC Settings*/	
		tAVCParams.nPortIndex=OMX_DirOutput;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoAvc, &tAVCParams);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
		tAVCParams.eProfile = pAppData->H264_TestCaseParams->profile;
		tAVCParams.eLevel= pAppData->H264_TestCaseParams->level;
		if(bIntraperiod==OMX_TRUE){
			tAVCParams.nPFrames =LIntraPeriod;
		}
		tAVCParams.nBFrames = pAppData->H264_TestCaseParams->maxInterFrameInterval-1;
		tAVCParams.bEnableFMO= pAppData->H264_TestCaseParams->bFMO;
		tAVCParams.bEntropyCodingCABAC=pAppData->H264_TestCaseParams->bCABAC;
		tAVCParams.bconstIpred=pAppData->H264_TestCaseParams->bConstIntraPred;
		tAVCParams.eLoopFilterMode=pAppData->H264_TestCaseParams->bLoopFilter;
		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoAvc, &tAVCParams);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);    	
	}

	tVidEncBitRate.nPortIndex=OMX_DirOutput;
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoBitrate, &tVidEncBitRate);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	tVidEncBitRate.eControlRate =pAppData->H264_TestCaseParams->RateControlAlg;
	if(bBitrate){
		tVidEncBitRate.nTargetBitrate=LBitRate;
	}
	eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoBitrate, &tVidEncBitRate);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	
	/*Update the DynamicSettingsArray*/
	eError = H264E_Update_DynamicSettingsArray(RemoveIndex,nRemoveList);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	
EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
	
}



static OMX_ERRORTYPE H264ENC_SetAllParams(H264E_ILClient* pAppData,OMX_U32 test_index)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_HANDLETYPE pHandle = NULL;

	OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
	OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE tBitStreamFormat;
	OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
	OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE tInputImageFormat;
	OMX_VIDEO_PARAM_TRANSFORM_BLOCKSIZETYPE tTransformBlockSize;
	OMX_VIDEO_PARAM_AVCSLICEFMO tAVCFMO;
	
	OMX_U32 i;

	TIMM_OSAL_Entering();

	if(!pAppData){
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}
	pHandle = pAppData->pHandle;

	/*Profile Level settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tProfileLevel,OMX_VIDEO_PARAM_PROFILELEVELTYPE);
	tProfileLevel.nPortIndex=H264_APP_OUTPUTPORT;
	
	eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	tProfileLevel.eProfile= pAppData->H264_TestCaseParams->profile;     
  	tProfileLevel.eLevel= pAppData->H264_TestCaseParams->level;
	
	eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoProfileLevelCurrent, &tProfileLevel);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	
	/*BitStream Format settings*/	
	OMX_TEST_INIT_STRUCT_PTR(&tBitStreamFormat,OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE);
	tBitStreamFormat.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoBitStreamFormatSelect, &tBitStreamFormat);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	tBitStreamFormat.eStreamFormat=pAppData->H264_TestCaseParams->BitStreamFormat;
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoBitStreamFormatSelect, &tBitStreamFormat);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	
	/*Encoder Preset settings*/	
	OMX_TEST_INIT_STRUCT_PTR(&tEncoderPreset,OMX_VIDEO_PARAM_ENCODER_PRESETTYPE);
	tEncoderPreset.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	tEncoderPreset.eEncodingModePreset=pAppData->H264_TestCaseParams->EncodingPreset;	
	tEncoderPreset.eRateControlPreset=pAppData->H264_TestCaseParams->RateCntrlPreset;
	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoEncoderPreset, &tEncoderPreset);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	/*Image Type settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tInputImageFormat,OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE);
	tInputImageFormat.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoFrameDataContentSettings, &tInputImageFormat);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	tInputImageFormat.eContentType=pAppData->H264_TestCaseParams->InputContentType;
	if(tInputImageFormat.eContentType!=OMX_Video_Progressive){
		tInputImageFormat.eInterlaceCodingType=pAppData->H264_TestCaseParams->InterlaceCodingType; 
	}	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoFrameDataContentSettings, &tInputImageFormat);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	/*Transform BlockSize settings*/
	OMX_TEST_INIT_STRUCT_PTR(&tTransformBlockSize,OMX_VIDEO_PARAM_TRANSFORM_BLOCKSIZETYPE);
	tTransformBlockSize.nPortIndex=H264_APP_OUTPUTPORT;
	eError = OMX_GetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoTransformBlockSize, &tTransformBlockSize);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	
	tTransformBlockSize.eTransformBlocksize=pAppData->H264_TestCaseParams->TransformBlockSize;
	
	eError = OMX_SetParameter(pHandle, (OMX_INDEXTYPE)OMX_TI_IndexParamVideoTransformBlockSize, &tTransformBlockSize);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	if((pAppData->H264_TestCaseParams->bFMO)&&((pAppData->H264_TestCaseParams->nBitEnableAdvanced>>1)==0)){
		/*Basic FMO  settings*/	
		OMX_TEST_INIT_STRUCT_PTR(&tAVCFMO,OMX_VIDEO_PARAM_AVCSLICEFMO);
		tAVCFMO.nPortIndex=H264_APP_OUTPUTPORT;
		eError = OMX_GetParameter(pHandle, OMX_IndexParamVideoSliceFMO, &tAVCFMO);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

		tAVCFMO.nNumSliceGroups =  pAppData->H264_TestCaseParams->nNumSliceGroups;
		tAVCFMO.nSliceGroupMapType = pAppData->H264_TestCaseParams->nSliceGroupMapType;
		tAVCFMO.eSliceMode = pAppData->H264_TestCaseParams->eSliceMode;
			
		eError = OMX_SetParameter(pHandle, OMX_IndexParamVideoSliceFMO, &tAVCFMO);
		GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
	}	
	
	/*Set the Advanced Setting if any*/
	eError =  H264ENC_SetAdvancedParams(pAppData,test_index);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	/*Settings from Initial DynamicParams*/
	eError =H264ENC_SetParamsFromInitialDynamicParams(pAppData,test_index);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);	

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

	/* This method will wait for the component to get to the desired state. */
static OMX_ERRORTYPE H264ENC_WaitForState(H264E_ILClient* pAppData)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents, numRemaining;

	TIMM_OSAL_Entering();
	
	/* Wait for an event */
	uRequestedEvents = (H264_STATETRANSITION_COMPLETE|H264_ERROR_EVENT);
	retval = TIMM_OSAL_EventRetrieve (H264VE_CmdEvent, uRequestedEvents,
			TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorInsufficientResources);
	if (pRetrievedEvents & H264_ERROR_EVENT) {
		eError=pAppData->eAppError;
	}else{
		/*transition Complete*/
		eError=OMX_ErrorNone;
	}	

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

OMX_ERRORTYPE H264ENC_EventHandler(OMX_HANDLETYPE hComponent,OMX_PTR ptrAppData,OMX_EVENTTYPE eEvent,
                           OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;

	TIMM_OSAL_Entering();

	if(!ptrAppData){
	    eError = OMX_ErrorBadParameter;
	    goto EXIT;
	}

	switch (eEvent)
	{
		case OMX_EventCmdComplete:
			retval = TIMM_OSAL_EventSet (H264VE_CmdEvent, H264_STATETRANSITION_COMPLETE, TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
			break;

		case OMX_EventError:
			TIMM_OSAL_Error( "received from the component in EventHandler=%d",(OMX_ERRORTYPE)nData1);
			retval = TIMM_OSAL_EventSet (H264VE_Events, H264_ERROR_EVENT, TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
			retval = TIMM_OSAL_EventSet (H264VE_CmdEvent, H264_ERROR_EVENT, TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
			break;

		case OMX_EventBufferFlag:
			if (nData2 == OMX_BUFFERFLAG_EOS){
				retval = TIMM_OSAL_EventSet (H264VE_Events, H264_EOS_EVENT, TIMM_OSAL_EVENT_OR);
				GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
			}				
			TIMM_OSAL_Trace( "Event: EOS has reached the client");
			break;

		case OMX_EventMark:
		case OMX_EventPortSettingsChanged:
		case OMX_EventResourcesAcquired:
		case OMX_EventComponentResumed:
		case OMX_EventDynamicResourcesAvailable:
		case OMX_EventPortFormatDetected:
		case OMX_EventMax:
	        eError = OMX_ErrorNotImplemented;
			break;
		default:
			eError = OMX_ErrorBadParameter;
			break;


	} // end of switch

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

OMX_ERRORTYPE H264ENC_FillBufferDone (OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer)
{
	H264E_ILClient* pAppData = ptrAppData;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	TIMM_OSAL_Entering();
	
#ifdef OMX_H264E_SRCHANGES
	TIMM_OSAL_Trace("\npBuffer SR after FBD = %x\n",pBuffer->pBuffer);
	pBuffer->pBuffer = SharedRegion_getPtr(pBuffer->pBuffer);
	TIMM_OSAL_Trace("\npBuffer after FBD = %x\n",pBuffer->pBuffer);
#endif      


	TIMM_OSAL_Trace( "Write the buffer to the Output pipe");	
   	retval = TIMM_OSAL_WriteToPipe(pAppData->OpBuf_Pipe, &pBuffer, sizeof(pBuffer), TIMM_OSAL_SUSPEND);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
	TIMM_OSAL_Trace( "Set the Output Ready event");	
	retval = TIMM_OSAL_EventSet (H264VE_Events, H264_OUTPUT_READY, TIMM_OSAL_EVENT_OR);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);

	
EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

OMX_ERRORTYPE H264ENC_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE* pBuffer)
{
	H264E_ILClient* pAppData = ptrAppData;
    	OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	TIMM_OSAL_Entering();
	
#ifdef OMX_H264E_SRCHANGES
	TIMM_OSAL_Trace("\npBuffer SR after EBD = %x\n",pBuffer->pBuffer);
	pBuffer->pBuffer = SharedRegion_getPtr(pBuffer->pBuffer);
	TIMM_OSAL_Trace("\npBuffer after EBD = %x\n",pBuffer->pBuffer);
#endif 
	TIMM_OSAL_Trace( "Write the buffer to the Input pipe");	
	retval = TIMM_OSAL_WriteToPipe(pAppData->IpBuf_Pipe, &pBuffer, sizeof(pBuffer),  TIMM_OSAL_SUSPEND);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
	TIMM_OSAL_Trace( "Set the Input Ready event");	
	retval = TIMM_OSAL_EventSet (H264VE_Events, H264_INPUT_READY, TIMM_OSAL_EVENT_OR);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);

	
EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}

void main()
{
	H264E_ILClient pAppData;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U32 test_index = 0;
    
    OMX_U32 setup;
#ifdef OMX_H264E_LINUX_TILERTEST
/*Only domx setup will be done by platform init since basic ipc setup has been 
done by syslink daemon*/
    setup = 2;
#else
/*Will do basic ipc setup, call proc load and aetup domx as well*/
    setup = 1;
#endif 
//Need to call mmplatform_init on linux
    mmplatform_init(setup);    


    printf("\nWait until RCM Server is created on other side. Press any key after that\n");
    getchar();
	
	/*Setting the Trace Group */
//Trace Grp not on Linux for now    
#if 0 
	TraceGrp=TIMM_OSAL_GetTraceGrp();
	TIMM_OSAL_SetTraceGrp((TraceGrp|(1<<3)));/*to VideoEncoder*/
    
	TIMM_OSAL_Trace( "tracegroup=%x",TraceGrp);
#endif
	eError = OMX_Init();	
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	for(test_index=0;test_index<1;test_index++)
	{
		//printf("\nRunning Testcasenumber=%d\n",test_index);
		TIMM_OSAL_Trace( "Running Testcasenumber=%d",test_index);
		
		pAppData.H264_TestCaseParams = &H264_TestCaseTable[test_index];

		/*Check for the Test case ID*/
		switch(pAppData.H264_TestCaseParams->TestCaseId){
			case 0:
				/*call to Total functionality*/
				TIMM_OSAL_Trace( "Running TestCaseId=%d",pAppData.H264_TestCaseParams->TestCaseId);
				eError=OMXH264Enc_CompleteFunctionality(&pAppData,test_index);
			break;
			case 1:
				/*call to gethandle & Freehandle: Simple Test*/
			break;
			default:
				TIMM_OSAL_Trace( "Not Implemented");
				//printf("\nNot Implemented\n");
						
		}
		OutputFilesize=0;
		nFramesRead=0;
		//TIMM_OSAL_EndMemDebug();
	EXIT:
	TIMM_OSAL_Trace("Reached the end of the programming");
	
	}/*End of test index loop*/
	
	printf("\nCalling platform deinit()\n");
    mmplatform_deinit();
    printf("\nPlatform deinitialized\n");	
}

OMX_ERRORTYPE OMXH264Enc_CompleteFunctionality(H264E_ILClient* pApplicationData,OMX_U32 test_index){
	H264E_ILClient *pAppData;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_CALLBACKTYPE appCallbacks;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;

	TIMM_OSAL_Entering();
	/*Get the AppData*/
	pAppData=pApplicationData;
	
	/*initialize the call back functions before the get handle function*/
	appCallbacks.EventHandler 		= H264ENC_EventHandler;
	appCallbacks.EmptyBufferDone 	= H264ENC_EmptyBufferDone;
	appCallbacks.FillBufferDone 		= H264ENC_FillBufferDone;
	
	/*create event*/
	retval = TIMM_OSAL_EventCreate (&H264VE_Events);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorInsufficientResources);
	retval = TIMM_OSAL_EventCreate (&H264VE_CmdEvent);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorInsufficientResources);
	

	/* Load the H264ENCoder Component */
    	eError = OMX_GetHandle(&pHandle,(OMX_STRING)"OMX.TI.DUCATI1.VIDEO.H264E",pAppData, &appCallbacks);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_Trace( "Got the Component Handle =%x",pHandle);
	
	pAppData->pHandle = pHandle;
	/* Open the input file for data to be rendered.  */
	pAppData->fIn = fopen(pAppData->H264_TestCaseParams->InFile, "rb");
	GOTO_EXIT_IF((pAppData->fIn == NULL),OMX_ErrorUnsupportedSetting);
	/* Open the output file */
	pAppData->fOut = fopen(pAppData->H264_TestCaseParams->OutFile, "wb");
	GOTO_EXIT_IF((pAppData->fOut== NULL),OMX_ErrorUnsupportedSetting);

	/*Set the Component Static Params*/
	TIMM_OSAL_Trace( "Set the Component Initial Parameters (in Loaded state)");
	eError = H264ENC_SetAllParams(pAppData, test_index);
	if(eError != OMX_ErrorNone){
		OMXH264Enc_Client_ErrorHandling(pAppData,1,eError);
		goto EXIT;
	}
	
       TIMM_OSAL_Trace( "call to goto Loaded -> Idle state");
	/* OMX_SendCommand expecting OMX_StateIdle */
	eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
	if(eError != OMX_ErrorNone){
		OMXH264Enc_Client_ErrorHandling(pAppData,1,eError);
		goto EXIT;
	}

	TIMM_OSAL_Trace( "Allocate the resources for the state trasition to Idle to complete");
	eError = H264ENC_AllocateResources(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	/* Wait for initialization to complete.. Wait for Idle state of component  */
	eError = H264ENC_WaitForState(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	
	TIMM_OSAL_Trace( "call to goto Idle -> executing state");
	eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateExecuting, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	eError = H264ENC_WaitForState(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	eError = OMX_GetState(pHandle,&(pAppData->eState));
	TIMM_OSAL_Trace( "Trasition to executing state is completed");

	/*Executing Loop*/
	eError =OMXH264Enc_InExecuting(pAppData,test_index);	
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	/*Command to goto Idle*/
	TIMM_OSAL_Trace( "call to goto executing ->Idle state");
	eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateIdle, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	   eError = H264ENC_WaitForState(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_Trace( "call to goto Idle -> Loaded state");
	eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateLoaded, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_Trace( "Free up the Component Resources");
	eError = H264ENC_FreeResources(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	eError = H264ENC_WaitForState(pAppData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	/*close the files after processing has been done*/
	TIMM_OSAL_Trace( "Close the Input & Output file");
	if(pAppData->fIn)
		fclose(pAppData->fIn);
	if(pAppData->fOut)
		fclose(pAppData->fOut);
	
	/* UnLoad the Encoder Component */
	TIMM_OSAL_Trace( "call to FreeHandle()");
	eError = OMX_FreeHandle(pHandle);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	

	
	EXIT:
		/* De-Initialize OMX Core */
		TIMM_OSAL_Trace( "call OMX Deinit");		
		eError = OMX_Deinit();
		
		/*Delete the Events*/
		TIMM_OSAL_EventDelete(H264VE_Events);
		TIMM_OSAL_EventDelete(H264VE_CmdEvent);
		
		if(eError != OMX_ErrorNone){
			TIMM_OSAL_Error( "%s",H264_GetErrorString(eError));
		}		
		
		TIMM_OSAL_Exiting(eError);

}


OMX_ERRORTYPE OMXH264Enc_InExecuting(H264E_ILClient* pApplicationData,OMX_U32 test_index){

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	H264E_ILClient *pAppData;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_BUFFERHEADERTYPE* pBufferIn = NULL;
	OMX_BUFFERHEADERTYPE* pBufferOut = NULL;
	OMX_U32 actualSize,fwritesize,fflushstatus;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;	
	TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents, numRemaining;
	OMX_U8 FrameNumToProcess=1,PauseFrameNumIndex=0,nCountFTB=1;
	OMX_H264E_DynamicConfigs OMXConfigStructures;
	OMX_BOOL bStop=OMX_FALSE;
	

	TIMM_OSAL_Entering();
	/*Initialize the ConfigStructures*/
	
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tFrameRate),OMX_CONFIG_FRAMERATETYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tBitRate),OMX_VIDEO_CONFIG_BITRATETYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tMESearchrange),OMX_VIDEO_CONFIG_MESEARCHRANGETYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tForceframe),OMX_CONFIG_INTRAREFRESHVOPTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tQPSettings),OMX_VIDEO_CONFIG_QPSETTINGSTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tAVCIntraPeriod),OMX_VIDEO_CONFIG_AVCINTRAPERIOD);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tSliceSettings),OMX_VIDEO_CONFIG_SLICECODINGTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(OMXConfigStructures.tPixelInfo),OMX_VIDEO_CONFIG_PIXELINFOTYPE);


	if(!pApplicationData){
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}
	/*Get the AppData*/
	pAppData=pApplicationData;
	/*Populate the Handle*/
	pHandle = pAppData->pHandle;
	
     	/*Check: partial or full record & in case of partial record check for stop frame number */
	if(pAppData->H264_TestCaseParams->TestType==PARTIAL_RECORD){
		H264ClientStopFrameNum=pAppData->H264_TestCaseParams->StopFrameNum;
		TIMM_OSAL_Trace( "StopFrame in Partial record case=%d",H264ClientStopFrameNum);	
	}else if(pAppData->H264_TestCaseParams->TestType==FULL_RECORD){
		/*go ahead with processing until it reaches EOS..update the "LFTBStopFrameNum" with sufficiently big value: used in case of FULL RECORD*/
		H264ClientStopFrameNum=1000;		
	}else{
		eError = OMX_ErrorBadParameter;
		TIMM_OSAL_Error( "Invalid parameter:TestType ");	
		goto EXIT;
	}
	while ((eError == OMX_ErrorNone) && ((pAppData->eState == OMX_StateExecuting) || (pAppData->eState == OMX_StatePause)) ) {
		
		/* Wait for an event (input/output/error) */
        TIMM_OSAL_Trace("\nWait for an event (input/output/error)\n");
		uRequestedEvents = (H264_INPUT_READY | H264_OUTPUT_READY | H264_ERROR_EVENT|H264_EOS_EVENT);
		retval = TIMM_OSAL_EventRetrieve (H264VE_Events, uRequestedEvents,
				TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents, TIMM_OSAL_SUSPEND);
        TIMM_OSAL_Trace("\nEvent recd. = %d\n", pRetrievedEvents);                
		GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorInsufficientResources);
		
		/*Check for Error Event & Exit upon receiving the Error*/
		GOTO_EXIT_IF((pRetrievedEvents & H264_ERROR_EVENT), pAppData->eAppError);

		/*Check for OUTPUT READY Event*/
		numRemaining = 0;
		if (pRetrievedEvents & H264_OUTPUT_READY) {
			TIMM_OSAL_Trace( "in Output Ready Event");	
			retval = TIMM_OSAL_GetPipeReadyMessageCount (pAppData->OpBuf_Pipe, &numRemaining);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
			TIMM_OSAL_Trace( "Output Msg count=%d",numRemaining);
			while (numRemaining) {
				/*read from the pipe*/
				TIMM_OSAL_Trace( "Read Buffer from the Output Pipe to give the Next FTB call");	
				retval = TIMM_OSAL_ReadFromPipe(pAppData->OpBuf_Pipe, &pBufferOut, sizeof(pBufferOut), &actualSize, TIMM_OSAL_SUSPEND );
				GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorNotReady);
				if(pBufferOut->nFilledLen){
					TIMM_OSAL_Trace( "Write the data into output file before it is given for the next FTB call");	
					fwritesize=fwrite((pBufferOut->pBuffer+(pBufferOut->nOffset)), sizeof(OMX_U8), pBufferOut->nFilledLen, pAppData->fOut);
					fflushstatus=fflush(pAppData->fOut);
					if(fflushstatus!=0)
						TIMM_OSAL_Error( "While fflush operation");					
					OutputFilesize+=pBufferOut->nFilledLen;
				}
				TIMM_OSAL_Trace( "filledlen=%d  fwritesize=%d, OutputFilesize=%d",pBufferOut->nFilledLen,fwritesize,OutputFilesize);	
				TIMM_OSAL_Trace( "update the nFilledLen & nOffset");	
				pBufferOut->nFilledLen = 0;
				pBufferOut->nOffset=0;
				TIMM_OSAL_Trace( "Before the FTB call");	
				if(H264ClientStopFrameNum!=nCountFTB){	
#ifdef OMX_H264E_SRCHANGES
					pBufferOut->pBuffer = (char *)SharedRegion_getSRPtr(pBufferOut->pBuffer, 2);	
#endif

					eError = OMX_FillThisBuffer(pHandle, pBufferOut);				
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#ifdef OMX_H264E_SRCHANGES        
					pBufferOut->pBuffer = SharedRegion_getPtr(pBufferOut->pBuffer); 	   
#endif

					/*increase the FTB count*/
					nCountFTB++;
				}else
					goto EXIT;
				
				retval = TIMM_OSAL_GetPipeReadyMessageCount (pAppData->OpBuf_Pipe, &numRemaining);
				GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
				TIMM_OSAL_Trace( "Output Msg count=%d",numRemaining);
				
			} 
		}

		/*Check for EOS Event*/
		GOTO_EXIT_IF((pRetrievedEvents & H264_EOS_EVENT), OMX_ErrorNone);
		
		/*Check for Input Ready Event*/
		numRemaining = 0;
		if ((pRetrievedEvents & H264_INPUT_READY)) {
			/*Check wheteher the EOS is already sent or Not*/
			if(H264ClientStopFrameNum==FrameNumToProcess){
					bStop=OMX_TRUE;
			}			
			/*continue with the ETB calls only if bstop is false*/
			if(!bStop){
				TIMM_OSAL_Trace( "In Input Ready event");			
				retval = TIMM_OSAL_GetPipeReadyMessageCount (pAppData->IpBuf_Pipe, &numRemaining);
				GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
				TIMM_OSAL_Trace( "Input Msg count=%d",numRemaining);
				while((numRemaining)){
					/*read from the pipe*/
					TIMM_OSAL_Trace( "Read Buffer from the Input Pipe to give the Next ETB call");
					retval = TIMM_OSAL_ReadFromPipe (pAppData->IpBuf_Pipe, &pBufferIn,
							sizeof(pBufferIn), &actualSize, TIMM_OSAL_SUSPEND );
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorNotReady);
					TIMM_OSAL_Trace( "Read Data from the Input File");		
					eError=H264ENC_FillData(pAppData,pBufferIn);
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
					TIMM_OSAL_Trace( "Call to ETB");
#ifdef OMX_H264E_SRCHANGES
					TIMM_OSAL_Trace("\npBuffer before ETB = %x\n",pBufferIn->pBuffer);
					pBufferIn->pBuffer = (char *)SharedRegion_getSRPtr(pBufferIn->pBuffer, 2);
					TIMM_OSAL_Trace("\npBuffer SR before ETB = %x\n",pBufferIn->pBuffer);		
#endif

					eError = OMX_EmptyThisBuffer(pHandle, pBufferIn);
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#ifdef OMX_H264E_SRCHANGES
					TIMM_OSAL_Trace("\npBuffer SR after ETB = %x\n",pBufferIn->pBuffer);
					pBufferIn->pBuffer = SharedRegion_getPtr(pBufferIn->pBuffer);
					TIMM_OSAL_Trace("\npBuffer after ETB = %x\n",pBufferIn->pBuffer);
#endif 


					/*Send command to Pause*/
					if((PauseFrameNum[PauseFrameNumIndex]==FrameNumToProcess)&&(PauseFrameNumIndex<NUMTIMESTOPAUSE)){
						TIMM_OSAL_Trace( "command to go Exectuing -> Pause state");		
						eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StatePause, NULL);
						GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

						eError = H264ENC_WaitForState(pHandle);
						GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
						PauseFrameNumIndex++;
						TIMM_OSAL_Trace( "Component is in Paused state");		
					}
					
					/*Call to Dynamic Settings*/
					TIMM_OSAL_Trace( "Configure the RunTime Settings");
					eError = OMXH264Enc_ConfigureRunTimeSettings(pAppData,&OMXConfigStructures, test_index,FrameNumToProcess);
					GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
					
					/*Increase the frame count that has been sent for processing*/
					FrameNumToProcess++;				

					retval = TIMM_OSAL_GetPipeReadyMessageCount (pAppData->IpBuf_Pipe, &numRemaining);
					GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE), OMX_ErrorBadParameter);
					TIMM_OSAL_Trace( "Input Msg count=%d",numRemaining);
				}
			}
		}

		/*instead of sleep......to send the executing State command*/
		while((pAppData->eState==OMX_StatePause)&&(TempCount<10) ){
			TempCount++;
		}		
		if(TempCount==10){
			TIMM_OSAL_Trace( "command to go Pause -> Executing state");	
			eError = OMX_SendCommand(pHandle,OMX_CommandStateSet, OMX_StateExecuting, NULL);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			eError = H264ENC_WaitForState(pHandle);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			TempCount=0;
			
		}

		/*get the Component State*/
		eError = OMX_GetState(pHandle, &(pAppData->eState));
		TIMM_OSAL_Trace( "Component Current State=%d",pAppData->eState);
		
	}
EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
	
}

OMX_ERRORTYPE OMXH264Enc_ConfigureRunTimeSettings(H264E_ILClient * pApplicationData,OMX_H264E_DynamicConfigs* H264EConfigStructures,OMX_U32 test_index,OMX_U32 FrameNumber){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_HANDLETYPE pHandle = NULL;
	OMX_U32 nRemoveList[9];
    	OMX_U8 i,RemoveIndex=0;


	TIMM_OSAL_Entering();
		
    	if(!pApplicationData){
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}
	pHandle = pApplicationData->pHandle;			

	for(i=0;i<DynamicSettingsCount;i++){
		switch(DynamicSettingsArray[i]){
			case 0:
				/*set config to OMX_CONFIG_FRAMERATETYPE */
				if(H264_TestCaseDynTable[test_index].DynFrameRate.nFrameNumber[DynFrameCountArray_Index[i]]==FrameNumber){
					/*Set Port Index*/
					H264EConfigStructures->tFrameRate.nPortIndex=H264_APP_OUTPUTPORT;					
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoFramerate, &(H264EConfigStructures->tFrameRate));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					H264EConfigStructures->tFrameRate.xEncodeFramerate=H264_TestCaseDynTable[test_index].DynFrameRate.nFramerate[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoFramerate, &(H264EConfigStructures->tFrameRate));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynFrameRate.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 1:
				/*set config to OMX_VIDEO_CONFIG_BITRATETYPE*/
				if(H264_TestCaseDynTable[test_index].DynBitRate.nFrameNumber[DynFrameCountArray_Index[i]]==FrameNumber){
					/*Set Port Index*/
					H264EConfigStructures->tBitRate.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoBitrate, &(H264EConfigStructures->tBitRate));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					H264EConfigStructures->tBitRate.nEncodeBitrate=H264_TestCaseDynTable[test_index].DynBitRate.nBitrate[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoBitrate, &(H264EConfigStructures->tBitRate));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynFrameRate.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 2:
				/*set config to OMX_TI_VIDEO_CONFIG_ME_SEARCHRANGE*/
				if(H264_TestCaseDynTable[test_index].DynMESearchRange.nFrameNumber[DynFrameCountArray_Index[i]]==FrameNumber){
					/*Set Port Index*/
					H264EConfigStructures->tMESearchrange.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_TI_IndexConfigVideoMESearchRange, &(H264EConfigStructures->tBitRate));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					H264EConfigStructures->tMESearchrange.eMVAccuracy=H264_TestCaseDynTable[test_index].DynMESearchRange.nMVAccuracy[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tMESearchrange.nHorSearchRangeP=H264_TestCaseDynTable[test_index].DynMESearchRange.nHorSearchRangeP[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tMESearchrange.nVerSearchRangeP=H264_TestCaseDynTable[test_index].DynMESearchRange.nVerSearchRangeP[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tMESearchrange.nHorSearchRangeB=H264_TestCaseDynTable[test_index].DynMESearchRange.nHorSearchRangeB[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tMESearchrange.nVerSearchRangeB=H264_TestCaseDynTable[test_index].DynMESearchRange.nVerSearchRangeB[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_TI_IndexConfigVideoMESearchRange, &(H264EConfigStructures->tMESearchrange));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynMESearchRange.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 3:
				/*set config to OMX_CONFIG_INTRAREFRESHVOPTYPE*/
				if(H264_TestCaseDynTable[test_index].DynForceFrame.nFrameNumber[DynFrameCountArray_Index[i]]==0){					
					/*Set Port Index*/
					H264EConfigStructures->tForceframe.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &(H264EConfigStructures->tForceframe));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					H264EConfigStructures->tForceframe.IntraRefreshVOP=H264_TestCaseDynTable[test_index].DynForceFrame.ForceIFrame[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &(H264EConfigStructures->tForceframe));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynForceFrame.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 4:
				/*set config to OMX_TI_VIDEO_CONFIG_QPSETTINGS*/
				if(H264_TestCaseDynTable[test_index].DynQpSettings.nFrameNumber[DynFrameCountArray_Index[i]]==0){					
					/*Set Port Index*/
					H264EConfigStructures->tQPSettings.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &(H264EConfigStructures->tQPSettings));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					H264EConfigStructures->tQPSettings.nQpI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpI[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMaxI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxI[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMinI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinI[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpP=H264_TestCaseDynTable[test_index].DynQpSettings.nQpP[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMaxI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxP[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMinI=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinP[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpOffsetB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpOffsetB[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMaxB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMaxB[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tQPSettings.nQpMinB=H264_TestCaseDynTable[test_index].DynQpSettings.nQpMinB[DynFrameCountArray_Index[i]];

					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoIntraVOPRefresh, &(H264EConfigStructures->tQPSettings));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynQpSettings.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			case 5:
				/*set config to OMX_VIDEO_CONFIG_AVCINTRAPERIOD*/
				if(H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nFrameNumber[DynFrameCountArray_Index[i]]==0){					
					/*Set Port Index*/
					H264EConfigStructures->tAVCIntraPeriod.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_IndexConfigVideoAVCIntraPeriod, &(H264EConfigStructures->tAVCIntraPeriod));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					H264EConfigStructures->tAVCIntraPeriod.nIDRPeriod=H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nIntraFrameInterval[DynFrameCountArray_Index[i]];
					
					eError = OMX_SetConfig(pHandle, OMX_IndexConfigVideoAVCIntraPeriod, &(H264EConfigStructures->tAVCIntraPeriod));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynIntraFrmaeInterval.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}				
			break;
			case 6:
				/*set config to OMX_TI_VIDEO_CONFIG_SLICECODING*/
				if(H264_TestCaseDynTable[test_index].DynSliceSettings.nFrameNumber[DynFrameCountArray_Index[i]]==0){					
					/*Set Port Index*/
					H264EConfigStructures->tSliceSettings.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_TI_IndexConfigSliceSettings, &(H264EConfigStructures->tSliceSettings));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					H264EConfigStructures->tSliceSettings.eSliceMode=H264_TestCaseDynTable[test_index].DynSliceSettings.eSliceMode[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tSliceSettings.nSlicesize=H264_TestCaseDynTable[test_index].DynSliceSettings.nSlicesize[DynFrameCountArray_Index[i]];
					
					eError = OMX_SetConfig(pHandle, OMX_TI_IndexConfigSliceSettings, &(H264EConfigStructures->tSliceSettings));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynSliceSettings.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
				
			break;
			case 7:
				/*set config to OMX_TI_VIDEO_CONFIG_PIXELINFO*/
				if(H264_TestCaseDynTable[test_index].DynPixelInfo.nFrameNumber[DynFrameCountArray_Index[i]]==0){					

					/*Set Port Index*/
					H264EConfigStructures->tPixelInfo.nPortIndex=H264_APP_OUTPUTPORT;
					eError = OMX_GetConfig(pHandle, OMX_TI_IndexConfigVideoPixelInfo, &(H264EConfigStructures->tPixelInfo));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

					H264EConfigStructures->tPixelInfo.nWidth=H264_TestCaseDynTable[test_index].DynPixelInfo.nWidth[DynFrameCountArray_Index[i]];
					H264EConfigStructures->tPixelInfo.nHeight=H264_TestCaseDynTable[test_index].DynPixelInfo.nHeight[DynFrameCountArray_Index[i]];
					
					eError = OMX_SetConfig(pHandle, OMX_TI_IndexConfigVideoPixelInfo, &(H264EConfigStructures->tPixelInfo));
					GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);
					
					DynFrameCountArray_Index[i]++;
					/*check to remove from the DynamicSettingsArray list*/
					if(H264_TestCaseDynTable[test_index].DynPixelInfo.nFrameNumber[DynFrameCountArray_Index[i]]==-1){
						nRemoveList[RemoveIndex++]=DynamicSettingsArray[i];
					}
				}
			break;
			default:		
            break;
		}
	}
	
	/*Update the DynamicSettingsArray*/
		eError= H264E_Update_DynamicSettingsArray(RemoveIndex,nRemoveList);

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;
}


/*
Copies 1D buffer to 2D buffer. All heights, widths etc. should be in bytes.
The function copies the lower no. of bytes i.e. if nSize1D < (nHeight2D * nWidth2D)
then nSize1D bytes is copied else (nHeight2D * nWidth2D) bytes is copied.
This function does not return any leftover no. of bytes, the calling function
needs to take care of leftover bytes on its own
*/
OMX_ERRORTYPE OMXH264_Util_Memcpy_1Dto2D(OMX_PTR pDst2D, OMX_PTR pSrc1D, OMX_U32 nSize1D, OMX_U32 nHeight2D, OMX_U32 nWidth2D, OMX_U32 nStride2D)
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
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
	    }
	    else
	    {
			retval=TIMM_OSAL_Memcpy(pOutBuffer, pInBuffer, nSizeLeft);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
	        break;
	    }
	    nSizeLeft -= nWidth2D;
	    pInBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pInBuffer + nWidth2D);
	    pOutBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pOutBuffer + nStride2D);
	}

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;

}


/*
Copies 2D buffer to 1D buffer. All heights, widths etc. should be in bytes.
The function copies the lower no. of bytes i.e. if nSize1D < (nHeight2D * nWidth2D)
then nSize1D bytes is copied else (nHeight2D * nWidth2D) bytes is copied.
This function does not return any leftover no. of bytes, the calling function
needs to take care of leftover bytes on its own
*/
OMX_ERRORTYPE OMXH264_Util_Memcpy_2Dto1D(OMX_PTR pDst1D, OMX_PTR pSrc2D, OMX_U32 nSize1D, OMX_U32 nHeight2D, OMX_U32 nWidth2D, OMX_U32 nStride2D)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_U32 retval = TIMM_OSAL_ERR_UNKNOWN;
	OMX_U8 *pInBuffer;
	OMX_U8 *pOutBuffer;
	OMX_U32 nSizeLeft,i;

	nSizeLeft = nSize1D;
	pInBuffer = (OMX_U8 *)pSrc2D;
	pOutBuffer = (OMX_U8 *)pDst1D;
    //The lower limit is copied. If nSize1D < H*W then 1Dsize is copied else H*W is copied
    for(i = 0; i < nHeight2D; i++)
    {
        if(nSizeLeft >= nWidth2D)
        {
            retval=TIMM_OSAL_Memcpy(pOutBuffer, pInBuffer, nWidth2D);
		GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
        }
        else
        {
            retval=TIMM_OSAL_Memcpy(pOutBuffer, pInBuffer, nSizeLeft);
		GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),OMX_ErrorUndefined);
            break;
        }
        nSizeLeft -= nWidth2D;
        pInBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pInBuffer + nStride2D);
        pOutBuffer = (OMX_U8 *)((TIMM_OSAL_U32)pOutBuffer + nWidth2D);
    }

EXIT:
TIMM_OSAL_Exiting(eError);
return eError;

}


OMX_ERRORTYPE OMXH264Enc_DynamicPortConfiguration(H264E_ILClient * pApplicationData,OMX_U32 nPortIndex){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	H264E_ILClient *pAppData;
	
	TIMM_OSAL_Entering();
	
	/*validate the Input Prameters*/
	if(pApplicationData==NULL){
		eError=OMX_ErrorBadParameter;
		goto EXIT;		
	}
	pAppData=pApplicationData;

	/*validate the Port Index*/
	if(nPortIndex>H264_APP_OUTPUTPORT){
		eError=OMX_ErrorBadPortIndex;
		goto EXIT;		
	}
	
	/*call OMX_Sendcommand with cmd as Port Disable */
	eError = OMX_SendCommand(pAppData->pHandle,OMX_CommandPortDisable,nPortIndex, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

	/*Configure the port with new Settings: tbd*/

	/*call OMX_Sendcommand with cmd as Port Enable*/
	eError = OMX_SendCommand(pAppData->pHandle,OMX_CommandPortEnable, nPortIndex, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone),eError);

EXIT:
	TIMM_OSAL_Exiting(eError);
	return eError;

}
void OMXH264Enc_Client_ErrorHandling(H264E_ILClient * pApplicationData,OMX_U32 nErrorIndex,OMX_ERRORTYPE eGenError){
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	H264E_ILClient *pAppData;
	
	TIMM_OSAL_Entering();
	
	/*validate the Input Prameters*/
	if(pApplicationData==NULL){
		eError=OMX_ErrorBadParameter;
		goto EXIT;		
	}
	pAppData=pApplicationData;
	switch(nErrorIndex){
		case 1:
			/*Error During Set Params or Error During state Transition Loaded->Idle*/
			eError = OMX_FreeHandle(pAppData->pHandle);
			TIMM_OSAL_Error("%s  During FreeHandle",H264_GetErrorString(eGenError));						
		break;
		case 2:
			/*TODO: Error During Allocate Buffers*/
		break;
		case 3:
			/*TODO: state Invalid*/
		break;
		case 4:
			/*TODO: Error During state Transition Idle->Executing*/
		break;
		case 5:
			/*TODO: Error During state Transition Executing->Pause*/
		break;
		case 6:
			/*TODO: Error During state Transition Pause->Executing*/
		break;
		case 7:
			/*TODO: Error During state Transition Executing->idle*/
		break;
		case 8:
			/*TODO: Error During state Transition Pause->Idle*/
		break;
		case 9:
			/*TODO: Error During state Transition Idle->Loaded*/
		break;
		case 10:
			/*TODO: Error while in Executing*/
		break;
		case 11:
			/*TODO: Error during FreeBuffer*/
		break;
	}

EXIT:
TIMM_OSAL_Exiting(eError);

}
	



        
	



	

       
        










