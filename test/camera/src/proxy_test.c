#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <syslog.h>


#include <OMX_Core.h>
#include <OMX_Component.h>
#include "timm_osal_interfaces.h"
#include "timm_osal_trace.h"
#include <OMX_Image.h>
//#include "../../../../omx/omx_il_1_x/omx_iss_cam/inc/omx_iss_cam_def.h"
#include "camera_test.h"
#include <OMX_TI_IVCommon.h>
#include <OMX_TI_Index.h>

#include "mmplatform.h"
#include <RcmClient.h>
#include <HeapBuf.h>
#include <SharedRegion.h>

#define BUF_HEAP
#define SRCHANGES
extern HeapBuf_Handle heapHandle;
#define MAX_NO_BUFFERS 32
#define DEFAULT_BUFF_CNT 4
#define PAGE_SIZE 4096

#define OUTPUT_NUMPORTS 6 //change number of ports here

#define NUM_DOMAINS 0x6
#define OMX_NOPORT 0xfffffffe

#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)   \
    memset((_s_), 0x0, sizeof(_name_));         \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision = 0x0;          \
    (_s_)->nVersion.s.nStep = 0x0

int streaming = 0, numbuf=0;
int qbuf_calls = 0,count =0;


int TotalCapFrame = 0;
OMX_ERRORTYPE eError = OMX_ErrorNone;
OMX_HANDLETYPE hComp = NULL;
OMX_CALLBACKTYPE oCallbacks;
SampleCompTestCtxt *pContext;
SampleCompTestCtxt oAppData;
OMX_PARAM_PORTDEFINITIONTYPE portCheck;


/* OMX functions*/
/*========================================================*/
/* @ fn OMX_TEST_ErrorToString :: ERROR  to  STRING   */
/*========================================================*/
OMX_STRING OMX_TEST_ErrorToString (OMX_ERRORTYPE eError)
{
    OMX_STRING errorString;

        switch (eError) {
        case OMX_ErrorNone:
                errorString = "ErrorNone";
                break;
        case OMX_ErrorInsufficientResources:
                errorString = "ErrorInsufficientResources";
                break;
        case OMX_ErrorUndefined:
                errorString = "ErrorUndefined";
                break;
        case OMX_ErrorInvalidComponentName:
                errorString = "ErrorInvalidComponentName";
                break;
        case OMX_ErrorComponentNotFound:
                errorString = "ErrorComponentNotFound";
                break;
        case OMX_ErrorInvalidComponent:
                errorString = "ErrorInvalidComponent";
                break;
        case OMX_ErrorBadParameter:
                errorString = "ErrorBadParameter";
                break;
        case OMX_ErrorNotImplemented:
                errorString = "ErrorNotImplemented";
                break;
        case OMX_ErrorUnderflow:
                errorString = "ErrorUnderflow";
                break;
        case OMX_ErrorOverflow:
                errorString = "ErrorOverflow";
                break;
        case OMX_ErrorHardware:
                errorString = "ErrorHardware";
                break;
        case OMX_ErrorInvalidState:
                errorString = "ErrorInvalidState";
                break;
        case OMX_ErrorStreamCorrupt:
                errorString = "ErrorStreamCorrupt";
                break;
        case OMX_ErrorPortsNotCompatible:
                errorString = "ErrorPortsNotCompatible";
                break;
        case OMX_ErrorResourcesLost:
                errorString = "ErrorResourcesLost";
                break;
        case OMX_ErrorNoMore:
                errorString = "ErrorNoMore";
                break;
        case OMX_ErrorVersionMismatch:
                errorString = "ErrorVersionMismatch";
                break;
        case OMX_ErrorNotReady:
                errorString = "ErrorNotReady";
                break;
        case OMX_ErrorTimeout:
                errorString = "ErrorTimeout";
                break;
        case OMX_ErrorSameState:
                errorString = "ErrorSameState";
                break;
        case OMX_ErrorResourcesPreempted:
                errorString = "ErrorResourcesPreempted";
                break;
        case OMX_ErrorPortUnresponsiveDuringAllocation:
                errorString = "ErrorPortUnresponsiveDuringAllocation";
                break;
        case OMX_ErrorPortUnresponsiveDuringDeallocation:
                errorString = "ErrorPortUnresponsiveDuringDeallocation";
                break;
        case OMX_ErrorPortUnresponsiveDuringStop:
                errorString = "ErrorPortUnresponsiveDuringStop";
                break;
        case OMX_ErrorIncorrectStateTransition:
                errorString = "ErrorIncorrectStateTransition";
                break;
        case OMX_ErrorIncorrectStateOperation:
                errorString = "ErrorIncorrectStateOperation";
                break;
        case OMX_ErrorUnsupportedSetting:
                errorString = "ErrorUnsupportedSetting";
                break;
        case OMX_ErrorUnsupportedIndex:
                errorString = "ErrorUnsupportedIndex";
                break;
        case OMX_ErrorBadPortIndex:
                errorString = "ErrorBadPortIndex";
                break;
        case OMX_ErrorPortUnpopulated:
                errorString = "ErrorPortUnpopulated";
                break;
        case OMX_ErrorComponentSuspended:
                errorString = "ErrorComponentSuspended";
                break;
        case OMX_ErrorDynamicResourcesUnavailable:
                errorString = "ErrorDynamicResourcesUnavailable";
                break;
        case OMX_ErrorMbErrorsInFrame:
                errorString = "ErrorMbErrorsInFrame";
                break;
        case OMX_ErrorFormatNotDetected:
                errorString = "ErrorFormatNotDetected";
                break;
        case OMX_ErrorContentPipeOpenFailed:
                errorString = "ErrorContentPipeOpenFailed";
                break;
        case OMX_ErrorContentPipeCreationFailed:
                errorString = "ErrorContentPipeCreationFailed";
                break;
        case OMX_ErrorSeperateTablesUsed:
                errorString = "ErrorSeperateTablesUsed";
                break;
        case OMX_ErrorTunnelingUnsupported:
                errorString = "ErrorTunnelingUnsupported";
                break;
        default:
                errorString = "<unknown>";
                break;
        }
        return errorString;
}

/*========================================================*/
/* @ fn OMX_TEST_StateToString :: STATE  to  STRING   */
/*========================================================*/
OMX_STRING OMX_TEST_StateToString (OMX_STATETYPE eState)
{
       OMX_STRING StateString;
       switch (eState) {
        case OMX_StateInvalid:
                StateString = "Invalid";
                break;
        case OMX_StateLoaded:
                StateString = "Loaded";
                break;
        case OMX_StateIdle:
                StateString = "Idle";
                break;
        case OMX_StateExecuting:
                StateString = "Executing";
                break;
        case OMX_StatePause:
                StateString = "Pause";
                break;
        case OMX_StateWaitForResources:
                StateString = "WaitForResources ";
                break;
        default:
                StateString = "<unknown>";
                break;
       }
       return StateString;
}

/* Application callback Functions */
/*========================================================*/
/* @ fn SampleTest_EventHandler :: Application callback   */
/*========================================================*/
OMX_ERRORTYPE SampleTest_EventHandler(OMX_IN OMX_HANDLETYPE hComponent,
                                          OMX_IN OMX_PTR pAppData,
                                          OMX_IN OMX_EVENTTYPE eEvent,
                                          OMX_IN OMX_U32 nData1,
                                          OMX_IN OMX_U32 nData2,
                                          OMX_IN OMX_PTR pEventData)
{
       SampleCompTestCtxt *pContext;

	if(pAppData == NULL)
		return OMX_ErrorNone;

	pContext = (SampleCompTestCtxt *)pAppData;
	printf("D in Event Handler event = %d\n", eEvent);
	switch (eEvent) {
        case OMX_EventCmdComplete:
                if (OMX_CommandStateSet == nData1) {
			printf (" Component Transitioned to %s state \n",
			OMX_TEST_StateToString((OMX_STATETYPE)nData2));
			pContext->eState = (OMX_STATETYPE) nData2;
			printf("\n pContext->eState%d\n",pContext->eState);
			TIMM_OSAL_SemaphoreRelease (pContext->hStateSetEvent);
		} else if (OMX_CommandFlush == nData1) {
			/* Nothing to do over here */
		} else if (OMX_CommandPortDisable == nData1) {
			/* Nothing to do over here */
		} else if (OMX_CommandPortEnable == nData1) {
			/* Nothing to do over here */
		} else if (OMX_CommandMarkBuffer == nData1) {
			/* Nothing to do over here */
		}
	break;

	case OMX_EventError:
              break;

	case OMX_EventMark:
              break;

	case OMX_EventPortSettingsChanged:
              break;

	case OMX_EventBufferFlag:
              break;

	case OMX_EventResourcesAcquired:
              break;

	case OMX_EventComponentResumed:
              break;

	case OMX_EventDynamicResourcesAvailable:
              break;

	case OMX_EventPortFormatDetected:
              break;

	default:
              break;
	}

	goto OMX_TEST_BAIL;

	OMX_TEST_BAIL:
              return OMX_ErrorNone;
}


/*========================================================*/
/* @ fn SampleTest_EmptyBufferDone :: Application callback    */
/*========================================================*/
OMX_ERRORTYPE SampleTest_EmptyBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_PTR pAppData,
                                   OMX_IN OMX_BUFFERHEADERTYPE* pBuffer)
{
        SampleCompTestCtxt *pContext;
        
        if(pAppData == NULL)
                return OMX_ErrorNone;
        
        pContext = (SampleCompTestCtxt *)pAppData;
        printf("\nDummy Function\n");
        goto OMX_TEST_BAIL;
        
        OMX_TEST_BAIL:
                return OMX_ErrorNone;
}

static int omx_q_buffers(int index, int PortNum)
{
	struct port_param *sPortParam;
	sPortParam = &(pContext->sPortParam[PortNum]);
                eError = OMX_FillThisBuffer(pContext->hComp, 
                                        sPortParam->bufferheader[index]);
                
                OMX_TEST_BAIL_IF_ERROR(eError);
        return 0;
        
OMX_TEST_BAIL:
        if (eError != OMX_ErrorNone) {
                printf("\n D ERROR FROM SEND Q_BUF PARAMS\n");
        }
        return eError;
}
/*========================================================*/
/* @ fn SampleTest_FillBufferDone ::   Application callback  */
/*========================================================*/
OMX_ERRORTYPE SampleTest_FillBufferDone(OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_IN OMX_PTR pAppData,
                                   OMX_IN OMX_BUFFERHEADERTYPE* pBuffHeader)
{
	struct port_param *pPortParam;
        OMX_ERRORTYPE eError = OMX_ErrorNone;

	if(TotalCapFrame == 4)
	{
		TIMM_OSAL_SemaphoreRelease (pContext->hExitSem);
		return eError;
	}
        
	pPortParam = &(pContext->sPortParam[pBuffHeader->nOutputPortIndex]);
        if (pBuffHeader->nOutputPortIndex == OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW) {
		printf("D Preview port capture frame Done number = %d\n", pPortParam->nCapFrame);
#if 0
		if (pPortParam->nCapFrame == 1) {
                    OMX_CONFIG_BOOLEANTYPE bOMX;
                    OMX_TEST_INIT_STRUCT(bOMX, OMX_CONFIG_BOOLEANTYPE);
                    bOMX.bEnabled = OMX_TRUE;
                    eError = OMX_SetConfig(pContext->hComp, OMX_IndexConfigCapturing, &bOMX);
			OMX_TEST_BAIL_IF_ERROR (eError);
                }
#endif
		pPortParam->nCapFrame++;
		TotalCapFrame++;
		omx_q_buffers(0, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW);
        } else {
		if (pBuffHeader->nOutputPortIndex == OMX_CAMERA_PORT_VIDEO_OUT_VIDEO) {
			pPortParam->nCapFrame++;
			printf("D VIDEO port capture frame Done number = %d\n", pPortParam->nCapFrame++);
			if(pPortParam->nCapFrame == 1)
			{
				OMX_CONFIG_BOOLEANTYPE bOMX;
				OMX_TEST_INIT_STRUCT(bOMX, OMX_CONFIG_BOOLEANTYPE);
				bOMX.bEnabled = OMX_FALSE;
				eError = OMX_SetConfig(pContext->hComp, OMX_IndexConfigCapturing, &bOMX);
				OMX_TEST_BAIL_IF_ERROR (eError);
			}
		}
		TotalCapFrame++;
		pPortParam->nCapFrame++;
		omx_q_buffers(0, OMX_CAMERA_PORT_VIDEO_OUT_VIDEO);
	}
        printf("D FBD port = %d ", (int)pBuffHeader->nOutputPortIndex );
        
        printf("\n ===FBD DONE ===\n");
        return OMX_ErrorNone;
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone){
                printf("\n D Error in FillBufferDone\n");
                return eError;
        }
}

/*========================================================*/
/* @ fn SampleTest_AllocateBuffers ::   Allocates the Resources on the available ports  */
/*========================================================*/
OMX_ERRORTYPE SampleTest_AllocateBuffers( OMX_PARAM_PORTDEFINITIONTYPE *pPortDef, OMX_U8 *pTmpBuffer)
{
        OMX_ERRORTYPE eError = OMX_ErrorNone;
        OMX_BUFFERHEADERTYPE *pBufferHdr;
        OMX_U32 i;
        uint buffersize;
	struct port_param *sPort;
        
        printf("\nD number of buffers to aloocated =0x%x \n",
				(uint)pPortDef->nBufferCountActual);
        printf("D SampleTest_AllocateBuffer width = %d height = %d\n",
                                (int)pPortDef->format.video.nFrameWidth,
                                (int)pPortDef->format.video.nFrameHeight);
        
        /* Allocate the buffers now */
        buffersize = pPortDef->format.video.nFrameWidth 
                        * pPortDef->format.video.nFrameHeight * 2 + PAGE_SIZE;
        
	/* struct to hold data particular to Port */
	sPort = &(pContext->sPortParam[pPortDef->nPortIndex]);

        /* Loop to allocate buffers */
        for (i = 0; i < pPortDef->nBufferCountActual; i++) {        
        /* working with Shared region as of now so allocate buffers
        * using HeadBuf_alloc
        */
#ifdef BUF_HEAP
        /* change the buffer size to be allocated here Ashutosh */
        pTmpBuffer = HeapBuf_alloc(heapHandle, buffersize, 0);
        printf("\npBuffer %d address at A9 side = %x\n",i,(uint)pTmpBuffer);
        
        if (pTmpBuffer== TIMM_OSAL_NULL) {
                printf("OMX_ErrorInsufficientResources\n");
                return -1;
        }
	sPort->hostbufaddr[i] = (OMX_U32)pTmpBuffer;
        pTmpBuffer = (OMX_U8 *)SharedRegion_getSRPtr(pTmpBuffer, 2);
        printf("\npBuffer Address for Ducati side  = %x\n",(uint)pTmpBuffer);
#endif
        eError = OMX_UseBuffer(pContext->hComp, &pBufferHdr, 
                        pPortDef->nPortIndex, 0, buffersize,pTmpBuffer);
        OMX_TEST_BAIL_IF_ERROR (eError);
        
        printf("\n ---- ALLIGNMENT PART ---- \n");
        
	pBufferHdr->pAppPrivate = pContext;
	pBufferHdr->nSize = sizeof(OMX_BUFFERHEADERTYPE);
	pBufferHdr->nVersion.s.nVersionMajor = 1 ;
	pBufferHdr->nVersion.s.nVersionMinor = 1 ;
	pBufferHdr->nVersion.s.nRevision = 0 ;
	pBufferHdr->nVersion.s.nStep =  0;

	printf("D buffer address = %x \n", (unsigned int)sPort->hostbufaddr[i]);
                
	strcpy((void *)sPort->hostbufaddr[i],"CAMERA DRIVER! YAY");
	printf("D the contents of the buffers are %s \n", 
					(char *)sPort->hostbufaddr[i]);
        sPort->bufferheader[i]= pBufferHdr;
	sPort->nCapFrame  = 0;
        } //end of for loop for buffer count times
        return 0;
        
        OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone){
                printf("\n D ERROR FROM SAMPLE_TEST_ALLOCATEBUFFERS\n");
                printf("D retruning from allocate buffer \n");
                return eError;
        }
	return eError;
}

/*========================================================*/
/* @ fn SampleTest_DeInitBuffers ::   Destroy the resources  */
/*========================================================*/
OMX_ERRORTYPE SampleTest_DeInitBuffers(SampleCompTestCtxt *pContext)
{
        OMX_ERRORTYPE eError = OMX_ErrorNone;
        OMX_BUFFERHEADERTYPE *pBufferHdr;
        OMX_U32 j;
        
        /* This function will allocate only 2 output buffers for port index 0 
                        and will return the buffer header  */
        
        /* TO DO FIXME deinit preview buffers also */
        for (j = 0; j < MAX_NO_BUFFERS; j++) {
		pBufferHdr = pContext->sPortParam[OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW].bufferheader[j];
		printf("D deinit buffer header = %x \n", (unsigned int )pBufferHdr);
                if (pBufferHdr){
                        eError = OMX_FreeBuffer (pContext->hComp, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW, pBufferHdr);
                        OMX_TEST_BAIL_IF_ERROR (eError);
			pBufferHdr = NULL;
                }
		pBufferHdr = pContext->sPortParam[OMX_CAMERA_PORT_VIDEO_OUT_VIDEO].bufferheader[j];
		printf("D deinit buffer header Video= %x \n", (unsigned int )pBufferHdr);
                if (pBufferHdr){
                        eError = OMX_FreeBuffer (pContext->hComp, OMX_CAMERA_PORT_VIDEO_OUT_VIDEO, pBufferHdr);
                        OMX_TEST_BAIL_IF_ERROR (eError);
			pBufferHdr = NULL;
                }
        }
        return 0;
        
        OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone){
                printf("\n D ERROR FROM DEINIT BUFFERS CALL\n");
                return eError;
        }
	return eError;
}

/*========================================================*/
/* @ fn SampleTest_TransitionWait ::   Waits for the transition to be completed ,
 *  incase of loaded to idle Allocates the Resources and while idle to loaded
 *  destroys the resources */
/*========================================================*/
OMX_ERRORTYPE SampleTest_TransitionWait(OMX_STATETYPE eToState)
{
        OMX_ERRORTYPE eError = OMX_ErrorNone;
        OMX_PARAM_PORTDEFINITIONTYPE tPortDef, tPortDefPreview;
        
        /* send command to change the state from Loaded to Idle */
        printf("\n IN TransitionWait send command to transition component to %d\n",eToState );
        eError = OMX_SendCommand (pContext->hComp, OMX_CommandStateSet,
                                                        eToState, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);

        /* Command is sent to the Component but it will not change the state 
        * to Idle untill we call UseThisBuffer or AllocateBuffer for 
        * nBufferCountActual times.
        */ 
        /* Verify that the component is still in Loaded state */
        eError = OMX_GetState (pContext->hComp, &pContext->eState);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        if ((eToState == OMX_StateIdle) && 
        (pContext->eState == OMX_StateLoaded)) {
		OMX_U8 *pTmpBuffer;
                /* first handle the video port */
                OMX_TEST_INIT_STRUCT (tPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
                tPortDef.nPortIndex =  pContext->nVideoPortIndex;  
                eError = OMX_GetParameter(pContext->hComp,
				OMX_IndexParamPortDefinition, (OMX_PTR)&tPortDef);
                OMX_TEST_BAIL_IF_ERROR(eError);
                printf("D getparam called from SampleTest_TransitionWait \n");
                
                /* call GetParameter for preview port */
                OMX_TEST_INIT_STRUCT (tPortDefPreview, 
                                                OMX_PARAM_PORTDEFINITIONTYPE);
                tPortDefPreview.nPortIndex =  pContext->nPrevPortIndex;  
                eError = OMX_GetParameter(pContext->hComp,
				OMX_IndexParamPortDefinition, (OMX_PTR)&tPortDefPreview);
                OMX_TEST_BAIL_IF_ERROR(eError);
                /* now allocate the desired number of buffers for video port*/
                eError = SampleTest_AllocateBuffers (&tPortDef, pTmpBuffer);
                OMX_TEST_BAIL_IF_ERROR(eError);               
                
                /* now allocate the desired number of buffers for preview port*/
                eError = SampleTest_AllocateBuffers (&tPortDefPreview, pTmpBuffer);
                OMX_TEST_BAIL_IF_ERROR(eError);
        
        } else if ((eToState == OMX_StateLoaded) && 
                                        (pContext->eState == OMX_StateIdle)) {
                eError = SampleTest_DeInitBuffers (pContext);
                OMX_TEST_BAIL_IF_ERROR(eError);
        }

        printf("D ------- obtaining Semaphore \n");
        TIMM_OSAL_SemaphoreObtain (pContext->hStateSetEvent, TIMM_OSAL_SUSPEND);
        printf("D Semaphore got released \n");
        /* by this time the component should have come to Idle state */
        
        if(pContext->eState != eToState)
                OMX_TEST_SET_ERROR_BAIL (OMX_ErrorUndefined,\
                                                "InComplete Transition\n");
        
        printf(" D returning from SampleTest_TransitionWait \n");

        OMX_TEST_BAIL:
        if (eError != OMX_ErrorNone) {        
                printf("\n D ERROR FROM SAMPLETEST_TRANSITIONWAIT\n");
        }
        return eError;
}

/* daemon part to interact with v4l2 driver */



static int send_streamoff_param()
{
        /* Transition back to Idle state  */
        eError = SampleTest_TransitionWait (OMX_StateIdle);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        printf("\n CALLED IDLE -> LOADED \n");
        /* Transition back to Loaded state */
        eError = SampleTest_TransitionWait(OMX_StateLoaded);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        return 0;
        
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone) {
                printf("\n D ERROR FROM SEND STREAMOFF PARAMS\n");
        }
        return eError;


}



static int SetFormat( int width, int height, OMX_COLOR_FORMATTYPE pix_fmt)
{
	struct port_param *sVidPortParam; 
        pContext->nPorts=OUTPUT_NUMPORTS;
        
        printf(" D Calling GetParameter \n");         

        OMX_TEST_INIT_STRUCT_PTR (&portCheck, OMX_PARAM_PORTDEFINITIONTYPE);
        portCheck.nPortIndex = pContext->nPrevPortIndex;
        printf("\nD Preview portindex = %d \n",(int)portCheck.nPortIndex);
        eError = OMX_GetParameter (pContext->hComp, OMX_IndexParamPortDefinition,
                                                &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        portCheck.format.video.nFrameWidth = width;
        portCheck.format.video.nFrameHeight = height;
        portCheck.format.video.eColorFormat = pix_fmt;
	portCheck.format.video.nStride = width * 1;
        /* FIXME the BPP is hardcoded here but it should not be less*/
        portCheck.nBufferSize = width * height * 2;
        
        /* fill some default buffer count as of now.  */
        portCheck.nBufferCountActual = DEFAULT_BUFF_CNT; 
        eError = OMX_SetParameter(pContext->hComp, 
                                OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        /* check if parameters are set correctly by calling GetParameter() */
        eError = OMX_GetParameter(pContext->hComp, 
                                OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        printf("\n *** PRV Width = %ld", portCheck.format.video.nFrameWidth);
        printf("\n *** PRV Height = %ld", portCheck.format.video.nFrameHeight);

        printf("\n *** PRV IMG FMT = %x", portCheck.format.video.eColorFormat);
        printf("\n ** PRV portCheck.nBufferSize = %ld\n",portCheck.nBufferSize);
        printf("\n ** PRV portCheck.nBufferCountMin = %ld\n",
                                                portCheck.nBufferCountMin);
        printf("\n ** PRV portCheck.nBufferCountActual = %ld\n",
                                                portCheck.nBufferCountActual);
        printf("\n ** PRV portCheck.format.video.nStride = %ld\n",
                                                portCheck.format.video.nStride);
        
        
        //for port OMX_CAMERA_PORT_VIDEO_OUT_VIDEO
        
        OMX_TEST_INIT_STRUCT_PTR (&portCheck, OMX_PARAM_PORTDEFINITIONTYPE);
        portCheck.nPortIndex = pContext->nVideoPortIndex;
        printf("\n VIDEO PORTINDEX = %d \n",(int)portCheck.nPortIndex);
        eError = OMX_GetParameter (pContext->hComp, 
                                OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
	sVidPortParam = &(pContext->sPortParam[pContext->nVideoPortIndex]);
        
        portCheck.format.video.nFrameWidth = sVidPortParam->nCapWidth = width;
        portCheck.format.video.nFrameHeight = sVidPortParam->nCapHeight = height;
        portCheck.format.video.eColorFormat = pix_fmt;
        /* FIXME the BPP is hardcoded here */
        portCheck.nBufferSize = width * height * 2;
	portCheck.format.video.nStride = width * 1;
        
        /* fill some default buffer count as of now.  */
        portCheck.nBufferCountActual = DEFAULT_BUFF_CNT; 
        eError = OMX_SetParameter(pContext->hComp, 
                                OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        /* check if parameters are set correctly by calling GetParameter() */
        eError = OMX_GetParameter(pContext->hComp,
                                OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);
        printf("\n *** VID Width = %ld", portCheck.format.video.nFrameWidth);
        printf("\n *** VID Height = %ld", portCheck.format.video.nFrameHeight);

        printf("\n *** VID IMG FMT = %x", portCheck.format.video.eColorFormat);
        printf("\n ** VID portCheck.nBufferSize = %ld\n",portCheck.nBufferSize);
        printf("\n ** VID portCheck.nBufferCountMin = %ld\n",
                                                portCheck.nBufferCountMin);
        printf("\n ** VID portCheck.nBufferCountActual = %ld\n",
                                                portCheck.nBufferCountActual);
        printf("\n ** VID portCheck.format.video.nStride = %ld\n",
                                                portCheck.format.video.nStride);
       
	/* Set Noise Filter mode to OFF */
	OMX_TEST_INIT_STRUCT_PTR(&(sVidPortParam->tVNFMode), OMX_PARAM_VIDEONOISEFILTERTYPE);
	sVidPortParam->tVNFMode.eMode = OMX_VideoNoiseFilterModeOff;
	eError = OMX_SetParameter(pContext->hComp, OMX_IndexParamVideoNoiseFilter, &(sVidPortParam->tVNFMode));
        OMX_TEST_BAIL_IF_ERROR(eError);
    
	/* set YUV range to Restricted (ITURBT601) */
	OMX_TEST_INIT_STRUCT_PTR(&(sVidPortParam->tYUVRange), OMX_PARAM_VIDEOYUVRANGETYPE);
	sVidPortParam->tYUVRange.eYUVRange = OMX_ITURBT601   ; 
	eError = OMX_SetParameter(pContext->hComp, OMX_IndexParamVideoCaptureYUVRange, &(sVidPortParam->tYUVRange));
        OMX_TEST_BAIL_IF_ERROR(eError);
	
	/* Set Video Stabilization Mode to OFF */
	OMX_TEST_INIT_STRUCT_PTR(&(sVidPortParam->tVidStabParam), OMX_CONFIG_BOOLEANTYPE);
	OMX_TEST_INIT_STRUCT_PTR(&(sVidPortParam->tVidStabConfig), OMX_CONFIG_FRAMESTABTYPE);
	sVidPortParam->tVidStabParam.bEnabled = OMX_FALSE; sVidPortParam->tVidStabConfig.bStab = OMX_FALSE;	
	eError = OMX_SetParameter(pContext->hComp, OMX_IndexParamFrameStabilisation, &(sVidPortParam->tVidStabParam));
        OMX_TEST_BAIL_IF_ERROR(eError);
	eError = OMX_SetConfig(pContext->hComp, OMX_IndexConfigCommonFrameStabilisation, &(sVidPortParam->tVidStabConfig));
        OMX_TEST_BAIL_IF_ERROR(eError);
        /* the OMX component has been set with desired settings. Send the 
        * response to V4l2 driver now
        */
        
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone) 
                printf("\n D ERROR FROM SEND S_FMT PARAMS\n");
        return eError;
}


static int process_close()
{
        /* Free the OMX handle and call Deinit */
        printf("D Calling OMX_FreeHandle \n");
        eError = OMX_FreeHandle (hComp);
        printf("\n Done with FREEHANDLE with error = %d\n", eError);
        OMX_TEST_BAIL_IF_ERROR(eError);                                                                 

        eError = OMX_Deinit();
        printf("\n Done with DEINIT with error = %d\n", eError);
        OMX_TEST_BAIL_IF_ERROR(eError);
        return 0;
        
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone)
                printf("\n D ERROR FROM PROCESS_CLOSE\n");
        return eError;

}

int main (int argc, char *argv[])
{
	
	/* read the parameter passed to the program */


        /* to load the images on the ducati side through CCS this call is
        * essential
        */
        printf("\nCalling platform init\n");
        mmplatform_init(1);
        printf("\nWait until RCM Server is created on other side. Press any key after that\n");
        getchar();
        
        pContext = &oAppData;
        memset(pContext, 0x0, sizeof(SampleCompTestCtxt));
        
	/* Initialize the callback handles */
        oCallbacks.EventHandler    = SampleTest_EventHandler;
        oCallbacks.EmptyBufferDone = SampleTest_EmptyBufferDone;
        oCallbacks.FillBufferDone  = SampleTest_FillBufferDone;

	/* video and pre port indexes */
	pContext->nVideoPortIndex = OMX_CAMERA_PORT_VIDEO_OUT_VIDEO;
	pContext->nPrevPortIndex = OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW;

	/* Initialize the Semaphores 1. for events and other for FillBufferDone */
        TIMM_OSAL_SemaphoreCreate(&pContext->hStateSetEvent, 0);
        TIMM_OSAL_SemaphoreCreate(&pContext->hExitSem, 0);
        
        printf("D call OMX_init \n");
        /* Initialize the OMX component */
        eError = OMX_Init();
        OMX_TEST_BAIL_IF_ERROR(eError);

        printf("D call OMX_getHandle \n");
        /* Get the handle of OMX camera component */

        eError = OMX_GetHandle (&hComp, (OMX_STRING)"OMX.TI.DUCATI1.VIDEO.CAMERA",
                                pContext, &oCallbacks);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        /* store the handle in global structure */
        pContext->hComp = hComp;
        
        /* disable all ports and then enable preview and video out port 
        * enabling two ports are necessary right now for OMX Camera component
        * to work. 
        */
        printf("D Disabling all the ports \n");
        eError = OMX_SendCommand(pContext->hComp, OMX_CommandPortDisable,
                                                        OMX_ALL, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
	/* Enable PREVIEW and VIDEO PORT */
        eError = OMX_SendCommand(pContext->hComp, OMX_CommandPortEnable, 
                                OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);

        eError = OMX_SendCommand(pContext->hComp, OMX_CommandPortEnable,
                                OMX_CAMERA_PORT_VIDEO_OUT_VIDEO, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        printf("D Appropriate ports are enabled successfully \n");
        
	/* FIXME right now hardcoding the width and heignt of the preview port.
	 * Preview port only works with NV12 format. Hardcoding that also
	 * Taken the values from omx test case */
	SetFormat(176, 144, OMX_COLOR_FormatYUV420SemiPlanar);

	/* change state to idle. in between allocate the buffers and 
	 * submit them to port
	 */
        eError = SampleTest_TransitionWait(OMX_StateIdle);
        OMX_TEST_BAIL_IF_ERROR(eError);
        printf("\nD IDLE TO EXECUTING ** \n");

	/* change state Executing */
        eError = OMX_SendCommand(pContext->hComp, OMX_CommandStateSet, 
					OMX_StateExecuting, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);
        
        /*wait till the transition get completed*/
        TIMM_OSAL_SemaphoreObtain (pContext->hStateSetEvent, TIMM_OSAL_SUSPEND);
        printf("D state changed to Executing \n");
        
	/* call FillThisBuffer for Video port for once */
	//omx_q_buffers(0,OMX_CAMERA_PORT_VIDEO_OUT_VIDEO );

	omx_q_buffers(0, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW);

	TIMM_OSAL_SemaphoreObtain (pContext->hExitSem, TIMM_OSAL_SUSPEND);
//TIMM_OSAL_SemaphoreObtain (pContext->hStateSetEvent, TIMM_OSAL_SUSPEND);//PK
	send_streamoff_param();
	process_close();

OMX_TEST_BAIL:
        if (eError != OMX_ErrorNone) {
                printf("\n ERROR from THREAD1_FUNC n");
                return eError;
        }
	return eError;
}

