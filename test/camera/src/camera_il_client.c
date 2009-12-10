/* standard linux includes */
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
//#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

/*omx related include files */
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <timm_osal_interfaces.h>
#include <timm_osal_trace.h>
#include <OMX_Image.h>
//#include "../../../../omx/omx_il_1_x/omx_iss_cam/inc/omx_iss_cam_def.h"
#include <OMX_TI_IVCommon.h>
#include <OMX_TI_Index.h>

/* bridge related include files */
#include <mmplatform.h>
#include <RcmClient.h>

/* shared region related include files */
#ifdef BUF_HEAP
#include <HeapBuf.h>
#include <SharedRegion.h>
#endif


/* camera test case include files */
#include "camera_test.h"

//#define VIDEO_DEVICE1 "/dev/v4l/video1"
#define VIDEO_DEVICE1 "/dev/video1"

#define OMX_TILERTEST
#define DSS_ENABLE

#if 0
#define BUF_HEAP
#define SRCHANGES
extern HeapBuf_Handle heapHandle;
#endif

#define ROUND_UP32(x)   (((x) + 31) & ~31)
#define ROUND_UP16(x)   (((x) + 15) & ~15)

#define E(STR) "OMX CAM TEST ERROR: %s:%s --> " STR, __FILE__, __FUNCTION__

#define OMXCAM_USE_BUFFER

#define CAM_EVENT_CMD_STATE_CHANGE      (1 << 0)
#define CAM_EVENT_MAIN_LOOP_EXIT        (1 << 1)

#define CAM_FRAME_TO_MAKE_STILL_CAPTURE     6
#define CAM_ZOOM_IN_START_ON_FRAME          (CAM_FRAME_TO_MAKE_STILL_CAPTURE + 1) // Count from 0
#define CAM_ZOOM_IN_FRAMES                  2
#define CAM_ZOOM_IN_STEP                    0x10000

#define CAM_FRAMES_TO_CAPTURE           \
    (                                   \
          CAM_ZOOM_IN_START_ON_FRAME    \
        + CAM_ZOOM_IN_FRAMES            \
        + 1                             \
    )



#define GOTO_EXIT_IF(_CONDITION,_ERROR) {                                       \
    if ((_CONDITION)) {                                                         \
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_OMXCAM, "Error :: %s : %s : %d :: ", __FILE__, __FUNCTION__, __LINE__);  \
        TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM, "Exiting because: %s \n", #_CONDITION);                          \
        eError = (_ERROR);                                                      \
        goto EXIT;                                                              \
    }                                                                           \
}


/*Tiler APIs*/
#ifdef TILER_BUFFERS
#include <memmgr.h>
#include <mmplatform.h>
#define STRIDE_8BIT (4 * 1024)
#define STRIDE_16BIT (4 * 1024)
#endif

int zoom_prv;
int TotalCapFrame = 0;
OMX_ERRORTYPE eError = OMX_ErrorNone;
OMX_HANDLETYPE hComp = NULL;
OMX_CALLBACKTYPE oCallbacks;
SampleCompTestCtxt *pContext;
SampleCompTestCtxt oAppData;

/* command line arguments */
unsigned int cmdwidth, cmdheight;
char *cmdformat;
OMX_COLOR_FORMATTYPE omx_pixformat;
//int test_camera_preview(void);
int test_camera_preview( int width, int height, char *image_format);
int fun_case2_case3(void);
int test_camera_zoom(void);
int vid1_fd=0;
OMX_U32 v4l2pixformat;
int streamon=0;

/*--------data declarations -----------------------------------*/
typedef struct {
    OMX_HANDLETYPE pHandle;
    OMX_U32 nCapruredFrames;
    OMX_U32 nPreviewPortIndex;
    OMX_BUFFERHEADERTYPE **ppPreviewBuffers;
    OMX_U32 nPreviewBuffers;
    OMX_U32 nCapturePortIndex;
    OMX_BUFFERHEADERTYPE **ppCaptureBuffers;
    OMX_U32 nCaptureBuffers;
    FILE *pPrvOutputFile;
    FILE *pCapOutputFile;
    OMX_U32 nPrvWidth;
    OMX_U32 nPrvHeight;
    OMX_U32 nPrvStride;
    OMX_U32 nCapWidth;
    OMX_U32 nCapHeight;
    OMX_U32 nCapStride;
    OMX_COLOR_FORMATTYPE ePrvFormat;
    OMX_COLOR_FORMATTYPE eCapColorFormat;
    OMX_IMAGE_CODINGTYPE eCapCompressionFormat;
    OMX_U32 nPrvBytesPerPixel;
    OMX_U32 nCapBytesPerPixel;
    OMX_MIRRORTYPE   eCaptureMirror;
    OMX_S32          nCaptureRot;
} test_OMX_CAM_AppData_t;

struct {
	void *start;
	size_t length;
} *buffers;

static TIMM_OSAL_PTR CameraEvents = NULL;



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

OMX_U32 getv4l2pixformat(const char *image_fmt)
{

	if (!strcmp (image_fmt, "YUYV"))
		return V4L2_PIX_FMT_YUYV;
	else if (!strcmp (image_fmt, "UYVY"))
		return V4L2_PIX_FMT_UYVY;
	else if (!strcmp (image_fmt, "RGB565"))
		return V4L2_PIX_FMT_RGB565;
	else if (!strcmp (image_fmt, "RGB565X"))
		return V4L2_PIX_FMT_RGB565X;
	else if (!strcmp (image_fmt, "RGB24"))
		return V4L2_PIX_FMT_RGB24;
	else if (!strcmp (image_fmt, "RGB32"))
		return V4L2_PIX_FMT_RGB32;
	else if (!strcmp (image_fmt, "NV12"))
		return V4L2_PIX_FMT_NV12;
	else {
		printf("fmt has to be NV12, UYVY, RGB565, RGB32, "
			"RGB24, UYVY or RGB565X!\n");
		return -1;
	}
}

OMX_COLOR_FORMATTYPE getomxformat(const char *format)
{
	printf("D format = %s\n", format);


        if (!strcmp (format, "YUYV")){
                printf(" pixel format = %s\n", "YUYV");
			return OMX_COLOR_FormatYCbYCr;
	}
        else if (!strcmp (format, "UYVY")){
                printf(" pixel format = %s\n", "UYVY");
			return OMX_COLOR_FormatCbYCrY;
	}
        else if (!strcmp (format, "NV12")){
			printf(" **** pixel format = %s\n", "NV12");
			return OMX_COLOR_FormatYUV420SemiPlanar;
	} else {
                printf("unsupported pixel format!\n");
                return -1;
        }

			return -1;
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
			printf("\nD Port Disabled port number = %d \n", nData2);
			/* Nothing to do over here */
		} else if (OMX_CommandPortEnable == nData1) {
			TIMM_OSAL_SemaphoreRelease (pContext->hStateSetEvent);
			printf("\nD Port Enable semaphore released \n");
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

static int omx_fillthisbuffer(int index, int PortNum)
{
	struct port_param *sPortParam;
	sPortParam = &(pContext->sPortParam[PortNum]);
	eError = OMX_FillThisBuffer(pContext->hComp,
				sPortParam->bufferheader[index]);

	OMX_TEST_BAIL_IF_ERROR(eError);
        return eError;

OMX_TEST_BAIL:
        if (eError != OMX_ErrorNone) {
                printf("\n D ERROR FROM SEND Q_BUF PARAMS\n");
        }
        return eError;
}

//******* sendbuffer to DSS  **********************//

void SendbufferToDss()
{
	struct v4l2_buffer filledbuffer;
	int result;

printf("\n IN SendbufferToDss \n");
	filledbuffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	filledbuffer.memory = V4L2_MEMORY_MMAP;
	filledbuffer.flags = 0;
	filledbuffer.index = 0;


printf("\n BEFORE QBUF of index 0\n");

	result = ioctl(vid1_fd, VIDIOC_QBUF, &filledbuffer);
	if (result != 0) {
		perror("VIDIOC_QBUF FAILED FAILED FAILED FAIL");
	}



if ( streamon == 1)
{printf("\n STREAMING ON SO NOW DQ buffer index 1\n");
			filledbuffer.index = 1;

			result = ioctl(vid1_fd, VIDIOC_DQBUF, &filledbuffer);

}
printf("\n BEFORE MEMCPY \n");
	memcpy(buffers[1].start, buffers[0].start, buffers[0].length);
printf("\n AFTER MEMCPY \n");

	filledbuffer.flags = 0;
	filledbuffer.index = 1;
printf("\n lets call Q buffer of buff index 1 \n");
	result = ioctl(vid1_fd, VIDIOC_QBUF, &filledbuffer);
	if (result != 0) {
		perror("VIDIOC_QBUF FAILED FAILED FAILED FAIL");
	}

printf("\nAFTER QBUF  index 1 \n");

	if ( streamon == 0)	{
		printf("\n STREAM ON DONE !!! \n");
		result = ioctl(vid1_fd, VIDIOC_STREAMON,
							&filledbuffer.type);
printf("\n STREAMON result = %d\n",result);
		if (result != 0) {
			perror("VIDIOC_STREAMON FAILED FAILED FAILED FAILED");

		}
		streamon = 1;
	}
		filledbuffer.flags = 0;
		filledbuffer.index = 0;
		printf("\n DQBUF CALLED for buffer index 0\n");
		result = ioctl(vid1_fd, VIDIOC_DQBUF, &filledbuffer);
		printf("\n DQBUF index 0 eresult = %d\n",result);
		if (result != 0){
			perror("VIDIOC_DQBUF FAILED FAILED FAILED FAILED");
		}


	return;



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

        printf("D FBD port = %d ", (int)pBuffHeader->nOutputPortIndex );
	if(TotalCapFrame == 10)
	{
		TIMM_OSAL_SemaphoreRelease (pContext->hExitSem);
		return eError;
	}

	pPortParam = &(pContext->sPortParam[pBuffHeader->nOutputPortIndex]);
        if (pBuffHeader->nOutputPortIndex == OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW) {
		printf("D Preview port capture frame Done number = %d\n",
			       			(int)pPortParam->nCapFrame);
		pPortParam->nCapFrame++;
		TotalCapFrame++;
		/*TODO assuming that right now we are working with only one
		 * buffer, we aren't passing the index number right now
		 * anyway at this point of time I don't know how to get the
		 * index number of the buffer which we have received
		 */
		printf("\n BEFORE SendbufferToDss \n");
#ifdef DSS_ENABLE
		SendbufferToDss();
#endif
		printf("\n AFTER SendbufferToDss and called Fillthis Buffer again\n");
		omx_fillthisbuffer(0, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW);
printf("\n == CAP FRAMES = %d ==\n",TotalCapFrame);
        }

        printf("\n ===FBD DONE ===\n");
        return OMX_ErrorNone;
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone){
                printf("\n D Error in FillBufferDone\n");
                return eError;
        }
	return OMX_ErrorNone;
}


uint getDSSBuffers(uint count)
{
	int result;
	struct v4l2_requestbuffers reqbuf;
	int i;

	reqbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = count;
	result = ioctl(vid1_fd, VIDIOC_REQBUFS, &reqbuf);
	if (result != 0) {
		perror("VIDEO_REQBUFS");
		return -1;
	}
	printf("\nD DSS driver allocated %d buffers \n", reqbuf.count);
	if (reqbuf.count != count)
	{
		printf("\nD DSS DRIVER FAILED TO ALLOCATE THE REQUESTED");
		printf(" NUMBER OF BUFFERS. NOT FORCING STRICT ERROR CHECK");
		printf(" AND SO THE APPLIACTION MAY FAIL.\n");
	}
	buffers = calloc(reqbuf.count, sizeof(*buffers));

	for (i = 0; i < reqbuf.count ; ++i) {
		struct v4l2_buffer buffer;
		buffer.type = reqbuf.type;
		buffer.index = i;

		result = ioctl(vid1_fd, VIDIOC_QUERYBUF, &buffer);
		if (result != 0) {
			perror("VIDIOC_QUERYBUF");
			return -1;
		}
		printf("%d: buffer.length=%d, buffer.m.offset=%d\n",
				i, buffer.length, buffer.m.offset);
		buffers[i].length = buffer.length;
		buffers[i].start = mmap(NULL, buffer.length, PROT_READ|
						PROT_WRITE, MAP_SHARED,
						vid1_fd, buffer.m.offset);
		if (buffers[i].start == MAP_FAILED) {
			perror("mmap");
			return -1;
		}

		printf("Buffers[%d].start = %x  length = %d\n", i,
			buffers[i].start, buffers[i].length);
	}
	return 0;

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

#ifdef TILER_BUFFERS
	MemAllocBlock *MemReqDescTiler;
	OMX_PTR TilerAddr=NULL;
#endif

	printf("\nD number of buffers to aloocated =0x%x \n",
			(uint)pPortDef->nBufferCountActual);
	printf("D SampleTest_AllocateBuffer width = %d height = %d\n",
			(int)pPortDef->format.video.nFrameWidth,
			(int)pPortDef->format.video.nFrameHeight);

	/* struct to hold data particular to Port */
	sPort = &(pContext->sPortParam[pPortDef->nPortIndex]);

#ifdef DSS_ENABLE
	getDSSBuffers(pPortDef->nBufferCountActual);
#else
	/* Allocate the buffers now */
	buffersize = pPortDef->format.video.nFrameWidth
		* pPortDef->format.video.nFrameHeight * 2 + PAGE_SIZE;

#ifdef TILER_BUFFERS
	MemReqDescTiler=(MemAllocBlock*)TIMM_OSAL_Malloc((sizeof(MemAllocBlock) * 2), TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
	if (!MemReqDescTiler)
		printf("\nD can't allocate memory for Tiler block allocation \n");

#endif
#endif
	/* Loop to allocate buffers */
	for (i = 0; i < pPortDef->nBufferCountActual; i++) {
#ifndef DSS_ENABLE
#ifdef BUF_HEAP
		/* change the buffer size to be allocated here Ashutosh */
		pTmpBuffer = HeapBuf_alloc(heapHandle, buffersize, 0);
		printf("\npBuffer %d address at A9 side = %x\n",i,(uint)pTmpBuffer);

#elif defined (TILER_BUFFERS)
		memset((void *) MemReqDescTiler , 0,
				(sizeof(MemAllocBlock) * 2));
		MemReqDescTiler[0].pixelFormat=PIXEL_FMT_8BIT;
		MemReqDescTiler[0].dim.area.width=  pPortDef->format.video.nFrameWidth;/*width*/
		MemReqDescTiler[0].dim.area.height=  pPortDef->format.video.nFrameHeight;/*height*/
		MemReqDescTiler[0].stride=STRIDE_8BIT;

		MemReqDescTiler[1].pixelFormat=PIXEL_FMT_16BIT;
		MemReqDescTiler[1].dim.area.width=pPortDef->format.video.nFrameWidth/2;/*width*/
		MemReqDescTiler[1].dim.area.height= pPortDef->format.video.nFrameHeight/2;/*height*/
		MemReqDescTiler[1].stride=STRIDE_16BIT;

		//MemReqDescTiler.reserved
		/*call to tiler Alloc*/
		printf("\nBefore tiler alloc for the Codec Internal buffer\n");
		TilerAddr=MemMgr_Alloc(MemReqDescTiler,2);
		printf("\nTiler buffer allocated is %x\n",
			(unsigned int)TilerAddr);
		pTmpBuffer = (OMX_U8 *)TilerAddr;
#endif
		if (pTmpBuffer== TIMM_OSAL_NULL) {
			printf("OMX_ErrorInsufficientResources\n");
			return -1;
		}
		sPort->hostbufaddr[i] = (OMX_U32)pTmpBuffer;
#ifdef BUF_HEAP

		pTmpBuffer = (OMX_U8 *)SharedRegion_getSRPtr(pTmpBuffer, 2);
		printf("\npBuffer Address for Ducati side  = %x\n",
							(uint)pTmpBuffer);
#endif

#else  //DSS buffers

	pTmpBuffer = (OMX_U8 *) buffers[i].start;
	buffersize = buffers[i].length;
	sPort->hostbufaddr[i] = (OMX_U32)pTmpBuffer;

#endif  // DSS buffers
		//printf("D Just going to call UseBuffer \n");
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

		printf("D buffer address = %x \n",
				(unsigned int)sPort->hostbufaddr[i]);
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

	for (j = 0; j < MAX_NO_BUFFERS; j++) {
		pBufferHdr = pContext->
		sPortParam[OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW].bufferheader[j];
		printf("D deinit buffer header = %x \n",
						(unsigned int )pBufferHdr);
		if (pBufferHdr){
#ifdef TILER_BUFFERS
			MemMgr_Free(pBufferHdr->pBuffer);
#endif
			eError = OMX_FreeBuffer (pContext->hComp, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW, pBufferHdr);
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
        OMX_PARAM_PORTDEFINITIONTYPE  tPortDefPreview;

        /* send command to change the state from Loaded to Idle */
        printf("\nTransitionWait send command to transition comp to %d\n",
								eToState );
        eError = OMX_SendCommand (pContext->hComp, OMX_CommandStateSet,
                                                        eToState, NULL);
        OMX_TEST_BAIL_IF_ERROR(eError);

        /* Command is sent to the Component but it will not change the state
        * to Idle untill we call UseThisBuffer or AllocateBuffer for
        * nBufferCountActual times.
        */
        /* Verify that the component is still in Loaded state */
        eError = OMX_GetState (pContext->hComp, &pContext->eState);
        if(!eError)
        	printf("\nD get state from sample test transition wait failed\n");
        OMX_TEST_BAIL_IF_ERROR(eError);
        if ((eToState == OMX_StateIdle) &&
		       		(pContext->eState == OMX_StateLoaded)) {
		OMX_U8 *pTmpBuffer;

                /* call GetParameter for preview port */
                OMX_TEST_INIT_STRUCT (tPortDefPreview,
                                                OMX_PARAM_PORTDEFINITIONTYPE);
                tPortDefPreview.nPortIndex =  pContext->nPrevPortIndex;
                eError = OMX_GetParameter(pContext->hComp,
						OMX_IndexParamPortDefinition,
						(OMX_PTR)&tPortDefPreview);
                OMX_TEST_BAIL_IF_ERROR(eError);
                /* now allocate the desired number of buffers for preview port*/
                eError = SampleTest_AllocateBuffers (&tPortDefPreview,
								pTmpBuffer);
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



static int omx_switch_to_loaded()
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

void SetFormatforDSSvid( unsigned int width, unsigned int height)
{
	struct v4l2_format format;
	int result;
	format.fmt.pix.width  = width;
	format.fmt.pix.height = height;
	format.type           = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	format.fmt.pix.pixelformat = v4l2pixformat;
	/* set format of the picture captured */
	result = ioctl(vid1_fd, VIDIOC_S_FMT, &format);
	if (result != 0) {
		perror("VIDIOC_S_FMT");
	}
	return;
}


static int SetFormat( int width, int height, OMX_COLOR_FORMATTYPE pix_fmt)
{
	OMX_PARAM_PORTDEFINITIONTYPE portCheck;
        pContext->nPorts=OUTPUT_NUMPORTS;
	struct port_param *PrevPort;
	int nStride;

        OMX_TEST_INIT_STRUCT_PTR (&portCheck, OMX_PARAM_PORTDEFINITIONTYPE);
        printf("\nD SetFormat Calling GetParameter \n");
        portCheck.nPortIndex = pContext->nPrevPortIndex;
        printf("\nD Preview portindex = %d \n",(int)portCheck.nPortIndex);
        eError = OMX_GetParameter (pContext->hComp,
				OMX_IndexParamPortDefinition, &portCheck);
        OMX_TEST_BAIL_IF_ERROR(eError);

        portCheck.format.video.nFrameWidth = width;
        portCheck.format.video.nFrameHeight = height;
        portCheck.format.video.eColorFormat = pix_fmt;
	portCheck.format.video.nStride = 4096;
	nStride = 4096;
        /* FIXME the BPP is hardcoded here but it should not be less*/
        portCheck.nBufferSize = nStride * height;

	PrevPort = &(pContext->sPortParam[pContext->nPrevPortIndex]);
	PrevPort->nWidth= width;
	PrevPort->nHeight=height;
	PrevPort->nStride= nStride;
	PrevPort->eColorFormat = pix_fmt;

        /* fill some default buffer count as of now.  */
        portCheck.nBufferCountActual = PrevPort->nActualBuffer = DEFAULT_BUFF_CNT;
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

	SetFormatforDSSvid( width, height);
OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone)
                printf("\n D ERROR FROM SEND S_FMT PARAMS\n");
        return eError;
}


static int omx_comp_release()
{
        /* Free the OMX handle and call Deinit */
        if ( hComp)
        {
        	printf("D Calling OMX_FreeHandle \n");

        	eError = OMX_FreeHandle (hComp);
        	printf("\n Done with FREEHANDLE with error = %d\n", eError);
        	OMX_TEST_BAIL_IF_ERROR(eError);
		}

        eError = OMX_Deinit();
        printf("\n Done with DEINIT with error = %d\n", eError);
		OMX_TEST_BAIL_IF_ERROR(eError);
		if(vid1_fd)
		{
			printf("closing the video pipeline \n");
			close(vid1_fd);
			vid1_fd = 0;
		}
        return 0;

OMX_TEST_BAIL:
        if(eError != OMX_ErrorNone)
                printf("\n D ERROR FROM PROCESS_CLOSE\n");
        return eError;

}

static void usage(void)
{
	printf("\nUsage:\n");
        printf("./camera_proxy.out <width> <height> <format> \n");
        printf("\twidth in pixels min=%d, max= %d\n",MIN_PREVIEW_WIDTH,
	     						MAX_PREVIEW_WIDTH );
        printf("\theight in pixels min = %d, max = %d",MIN_PREVIEW_HEIGHT,
							MAX_PREVIEW_HEIGHT);
        printf("\tformat supported= YUYV|UYVY|NV12\n");
}


//int main (int argc, char *argv[])
int main()
{
	OMX_U32 test_case_id;
//	getcmdoptions(argc, argv);

        /* to load the images on the ducati side through CCS this call is
        * essential
        */
        printf("\nCalling platform init\n");
        mmplatform_init(2);
        printf("\nWait until RCM Server is created on other side. Press any key after that\n");
        getchar();

        printf(" \n Please Select test case ID (1-20):");
        fflush(stdout);
        scanf("%d", &test_case_id);


	switch(test_case_id) {
		case 1:   {
			printf("\n Going to test resolution 640x480 format UYVY\n");
			eError = test_camera_preview(640, 480,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 2:   {
			printf("\n Going to test resolution 864x480 format UYVY\n");
			eError = test_camera_preview(864, 480,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 3:   {
			printf("\n Going to test resolution 320x240 format UYVY\n");
			eError = test_camera_preview(320, 240,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 4:   {
			printf("\n Going to test resolution 800x600 format UYVY\n");
			eError = test_camera_preview(800, 600,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 5:   {
			printf("\n Going to test resolution 176x144 format UYVY\n");
			eError = test_camera_preview(176, 144,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 6:   {
			printf("\n Going to test resolution 768x576format UYVY\n");
			eError = test_camera_preview(768, 576,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 7:   {
			printf("\n Going to test resolution 128x96 format UYVY\n");
			eError = test_camera_preview(128, 96,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 8:   {
			printf("\n Going to test resolution 64x64 format UYVY\n");
			eError = test_camera_preview(64, 64,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 9:   {
			printf("\n Going to test resolution 80x60 formatUYVY\n");
			eError = test_camera_preview(80, 60,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 10:   {
			printf("\n Going to test resolution 864x486 format UYVY\n");
			eError = test_camera_preview(864, 486,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 11:   {
			printf("\n Going to test resolution 720x480 format YUVY\n");
			eError = test_camera_preview(720, 480,"UYVY");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 12:   {
			printf("\n Going to test resolution 864x480 format NV12\n");
			eError = test_camera_preview(864, 480,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

 		case 13:   {
			printf("\n Going to test resolution 640x480 format NV12\n");
			eError = test_camera_preview(640, 480,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}


		case 14:   {
			printf("\n Going to test resolution 320x240 format NV12\n");
			eError = test_camera_preview(320, 240,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 15:   {
			printf("\n Going to test resolution 800x600 format NV12\n");
			eError = test_camera_preview(800, 600,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 16:   {
			printf("\n Going to test resolution 176x144 format NV12\n");
			eError = test_camera_preview(176, 144,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 17:   {
			printf("\n Going to test resolution 768x576format NV12\n");
			eError = test_camera_preview(768, 576,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 18:   {
			printf("\n Going to test resolution 128x96 format NV12\n");
			eError = test_camera_preview(128, 96,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 19:   {
			printf("\n Going to test resolution 64x64 format NV12\n");
			eError = test_camera_preview(64, 64,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 20:   {
			printf("\n Going to test resolution 80x60 formatNV12\n");
			eError = test_camera_preview(80, 60,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 21:   {
			printf("\n Going to test resolution 864x486 format NV12\n");
			eError = test_camera_preview(864, 486,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}

		case 22:   {
			printf("\n Going to test resolution 720x480 format NV12\n");
			eError = test_camera_preview(720, 480,"NV12");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
			OMX_TEST_BAIL_IF_ERROR(eError);
			break;
		}


 		case 41:   {
 			printf("\n Going to test resolution 864x480 format YUYV\n");
 			eError = test_camera_preview(864, 480,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

  		case 42:   {
 			printf("\n Going to test resolution 640x480 format YUYV\n");
 			eError = test_camera_preview(640, 480,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}


 		case 43:   {
 			printf("\n Going to test resolution 320x240 format YUYV\n");
 			eError = test_camera_preview(320, 240,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 44:   {
 			printf("\n Going to test resolution 800x600 format YUYV\n");
 			eError = test_camera_preview(800, 600,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 45:   {
 			printf("\n Going to test resolution 176x144 format YUYV\n");
 			eError = test_camera_preview(176, 144,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 46:   {
 			printf("\n Going to test resolution 768x576format YUYV\n");
 			eError = test_camera_preview(768, 576,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 47:   {
 			printf("\n Going to test resolution 128x96 format YUYV\n");
 			eError = test_camera_preview(128, 96,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 48:   {
 			printf("\n Going to test resolution 64x64 format YUYV\n");
 			eError = test_camera_preview(64, 64,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 49:   {
 			printf("\n Going to test resolution 80x60 formatYUYV\n");
 			eError = test_camera_preview(80, 60,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 50:   {
 			printf("\n Going to test resolution 864x486 format YUYV\n");
 			eError = test_camera_preview(864, 486,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}

 		case 51:   {
 			printf("\n Going to test resolution 720x480 format YUYV\n");
 			eError = test_camera_preview(720, 480,"YUYV");
			if(!eError)
				printf("\n eError From Preview test= %d\n",eError);
 			OMX_TEST_BAIL_IF_ERROR(eError);
 			break;
 		}


 		case 130:   {
			printf("\nD 2. Starting image capture test case: \n");
			zoom_prv=0;
                        fun_case2_case3();
                        break;
                }


        case 131:   {
			printf("\nD 3. Starting zoom in preview test case: \n");
			zoom_prv=1;
                        fun_case2_case3();
                        break;
                }



		default: {
			printf(" Invalid test ID selection.");
			return -1;
		}
	};

	printf("\nCalling platform Deinit\n");
	mmplatform_deinit();
	printf("\nPlatform deinitialized\n");
	return 0;

OMX_TEST_BAIL:
if (eError != OMX_ErrorNone) {
	printf("\n ERROR from main()");
	return eError;
        }
        return eError;
}


void open_video1()
{
	int result;
	struct v4l2_capability capability;
	struct v4l2_format format;
	vid1_fd = open(VIDEO_DEVICE1, O_RDWR);
	if ( vid1_fd <=0)
	{
		printf("\nD Failed to open the DSS Video Device\n");
		exit(-1);
	}
	printf("\nD opened Video Device 1 for preview \n");
	result = ioctl(vid1_fd, VIDIOC_QUERYCAP, &capability);
	if (result != 0) {
		perror("VIDIOC_QUERYCAP");
		goto ERROR_EXIT;
	}
	if (capability.capabilities & V4L2_CAP_STREAMING == 0) {
		printf("VIDIOC_QUERYCAP indicated that driver is not "
			"capable of Streaming \n");
		goto ERROR_EXIT;
	}
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	result = ioctl(vid1_fd, VIDIOC_G_FMT, &format);

	if (result != 0) {
		perror("VIDIOC_G_FMT");
		goto ERROR_EXIT;
	}


	return;
ERROR_EXIT:
	close(vid1_fd);
	exit (-1);
}

/* function to test the camera preview test case
** called from the main() and passed arguments
** width, height and image format
*/

int test_camera_preview( int width, int height, char *image_fmt)
{

	unsigned int cmd_width,cmd_height;
	cmd_width = width;
	cmd_height = height;

	open_video1();
	printf("\n BACK FROM VIDEO DEV OPEN CALL\n");
	v4l2pixformat = getv4l2pixformat((const char *)image_fmt);
	printf("\n back form getv4l2pixformat function\n");
    if( -1 == v4l2pixformat )
    {
        printf(" pixel format not supported \n");
		close(vid1_fd);
		return -1;
    }

    omx_pixformat = getomxformat((const char *)image_fmt);
    printf("\n back form getomxformat function\n");

    if( -1 == omx_pixformat )
    {
        printf(" pixel cmdformat not supported \n");
		close(vid1_fd);
		return -1;
    }
	printf("\n OMX FMT = %d\n",omx_pixformat);

    pContext = &oAppData;
    memset(pContext, 0x0, sizeof(SampleCompTestCtxt));

	/* Initialize the callback handles */
    oCallbacks.EventHandler    = SampleTest_EventHandler;
    oCallbacks.EmptyBufferDone = SampleTest_EmptyBufferDone;
    oCallbacks.FillBufferDone  = SampleTest_FillBufferDone;

	/* video and pre port indexes */
	pContext->nVideoPortIndex = OMX_CAMERA_PORT_VIDEO_OUT_VIDEO;
	pContext->nPrevPortIndex = OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW;

	/* Initialize the Semaphores 1. for events and other for
	 * FillBufferDone */
    TIMM_OSAL_SemaphoreCreate(&pContext->hStateSetEvent, 0);
    TIMM_OSAL_SemaphoreCreate(&pContext->hExitSem, 0);

    printf("D call OMX_init \n");
    /* Initialize the OMX component */
    eError = OMX_Init();
    OMX_TEST_BAIL_IF_ERROR(eError);

	printf("D call OMX_getHandle \n");
    /* Get the handle of OMX camera component */

    eError = OMX_GetHandle (&hComp,
			(OMX_STRING)"OMX.TI.DUCATI1.VIDEO.CAMERA", pContext,
								&oCallbacks);
    OMX_TEST_BAIL_IF_ERROR(eError);

    /* store the handle in global structure */
    pContext->hComp = hComp;

    /* disable all ports and then enable preview and video out port
    * enabling two ports are necessary right now for OMX Camera component
    * to work. */
    printf("D Disabling all the ports \n");
    eError = OMX_SendCommand(pContext->hComp, OMX_CommandPortDisable,
                                                        OMX_ALL, NULL);

    OMX_TEST_BAIL_IF_ERROR(eError);

	/* Enable PREVIEW and VIDEO PORT */
    eError = OMX_SendCommand(pContext->hComp, OMX_CommandPortEnable,
                                OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW, NULL);
	TIMM_OSAL_SemaphoreObtain (pContext->hStateSetEvent, TIMM_OSAL_SUSPEND);
	OMX_TEST_BAIL_IF_ERROR(eError);

    printf("D preview port enabled successfully \n");

	eError = SetFormat(cmd_width, cmd_height , omx_pixformat);
    OMX_TEST_BAIL_IF_ERROR(eError);

	/* change state to idle. in between allocate the buffers and
	 * submit them to port
	 */
	printf("\nD Changing state from loaded to Idle \n");
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

	omx_fillthisbuffer(0, OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW);

	TIMM_OSAL_SemaphoreObtain (pContext->hExitSem, TIMM_OSAL_SUSPEND);
	omx_switch_to_loaded();
	printf("\nCalling platform deinit()\n");


OMX_TEST_BAIL:
    if (eError != OMX_ErrorNone) {
            printf("\n ERROR test_camera_preview() function");
    }
	omx_comp_release();
	return eError;
}

static OMX_U32 OMXColorFormatGetBytesPerPixel(OMX_COLOR_FORMATTYPE eColorFormat)
{
    OMX_U32 nBytesPerPix;

    switch (eColorFormat) {
        case OMX_COLOR_FormatYUV420SemiPlanar:
            nBytesPerPix = 1;
            break;
        case OMX_COLOR_FormatRawBayer10bit:
        case OMX_COLOR_Format16bitRGB565:
        case OMX_COLOR_FormatCbYCrY:
            nBytesPerPix = 2;
            break;
        case OMX_COLOR_Format32bitARGB8888:
            nBytesPerPix = 4;
            break;
        default:
            nBytesPerPix = 2;
    }
    return nBytesPerPix;
}


static OMX_ERRORTYPE WaitForState(OMX_HANDLETYPE* pHandle, OMX_STATETYPE DesiredState)
{
    OMX_STATETYPE CurState = OMX_StateInvalid;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_U32 pRetrievedEvents;

    /* get the state of the component */
    eError = OMX_GetState(pHandle, &CurState);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

    /* wait while the state of the component is not desired state */
	while (CurState != DesiredState) {
	/* wait for the STATE change event occured on the camera component */
        retval = TIMM_OSAL_EventRetrieve(CameraEvents,
		       	CAM_EVENT_CMD_STATE_CHANGE, TIMM_OSAL_EVENT_AND_CONSUME,
		       	&pRetrievedEvents, TIMM_OSAL_SUSPEND);
	/* go to exit if we have woken up because of time out or if some
	 * error have occured
	 * */
        GOTO_EXIT_IF(((retval != TIMM_OSAL_ERR_NONE) &&
			(retval == TIMM_OSAL_WAR_TIMEOUT)), OMX_ErrorTimeout);
	/* retrive the current state of the component */
        eError = OMX_GetState(pHandle, &CurState);
        GOTO_EXIT_IF(((eError != OMX_ErrorNone)), eError);
    }

EXIT:
    return eError;
}

/* Event handler of the test case for campture test */
static OMX_ERRORTYPE test_OMX_CAM_EventHandler(OMX_HANDLETYPE hComponent,
	       	OMX_PTR ptrAppData, OMX_EVENTTYPE eEvent,
		OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;

	printf("\n test_OMX_CAM_EventHandler \n");
	if (!ptrAppData) {
		eError = OMX_ErrorBadParameter;
		goto EXIT;
	}

	switch (eEvent) {
		case OMX_EventCmdComplete: {
			if (nData1 == OMX_CommandStateSet) {
				retval = TIMM_OSAL_EventSet(CameraEvents,
				CAM_EVENT_CMD_STATE_CHANGE, TIMM_OSAL_EVENT_OR);
				GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),
					       	OMX_ErrorBadParameter);

			}
			break;
		}

		case OMX_EventError: {
			printf("\n OMX_EventError !!! \n");
			break;
		}

		case OMX_EventBufferFlag:
		case OMX_EventMark:
		case OMX_EventPortSettingsChanged:
		case OMX_EventResourcesAcquired:
		case OMX_EventComponentResumed:
		case OMX_EventDynamicResourcesAvailable:
		case OMX_EventPortFormatDetected:
		case OMX_EventMax: {
			printf("\n OMX_EventError !!!! \n");
			eError = OMX_ErrorNotImplemented;
			break;
		}
		default: {
			printf("\n OMX_ErrorBadParameter !!!\n");

			eError = OMX_ErrorBadParameter;
			break;
		}
	}

EXIT:
    return eError;
}


static OMX_ERRORTYPE test_OMX_CAM_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR ptrAppData,
    OMX_BUFFERHEADERTYPE *pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
//    TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_OMXCAM, E("Received Empty buffer done call!!!\n"));
printf("\n  Received Empty buffer done call!!!! \n ");
    goto EXIT;

EXIT:
    return eError;
}


static OMX_ERRORTYPE test_OMX_CAM_FillBufferDone(OMX_HANDLETYPE hComponent,
	       	OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE *pBuffer)
{
	OMX_ERRORTYPE eError = OMX_ErrorUndefined;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	test_OMX_CAM_AppData_t *appData = ptrAppData;
	OMX_CONFIG_BOOLEANTYPE bOMX;
	size_t byteswritten=0;
	printf("\nD inside FillBufferDone \n");

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
			    "frame no: %d \n", appData->nCapruredFrames);

	if (!pBuffer) {
		printf("\nD pBuffer in the FillBufferDone is NULL\n");
		goto EXIT;
	}

	if (appData->nPreviewPortIndex == pBuffer->nOutputPortIndex) {
		OMX_TEST_INIT_STRUCT_PTR(&bOMX, OMX_CONFIG_BOOLEANTYPE);

		/* Make zoom */
		if (   (CAM_ZOOM_IN_START_ON_FRAME <= appData->nCapruredFrames)
			&& (appData->nCapruredFrames < (CAM_ZOOM_IN_START_ON_FRAME
			+CAM_ZOOM_IN_FRAMES)) )
		{
			OMX_CONFIG_SCALEFACTORTYPE tScaleFactor;

			printf("\nD inside ZOOM \n");
			OMX_TEST_INIT_STRUCT_PTR(&tScaleFactor,
					OMX_CONFIG_SCALEFACTORTYPE);

			eError = OMX_GetConfig(hComponent,
				OMX_IndexConfigCommonDigitalZoom,&tScaleFactor);

			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			tScaleFactor.xWidth += CAM_ZOOM_IN_STEP;
			tScaleFactor.xHeight += CAM_ZOOM_IN_STEP;

			eError = OMX_SetConfig(hComponent,
			OMX_IndexConfigCommonDigitalZoom, &tScaleFactor);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
		}

		if( zoom_prv==0){
			if ((appData->nCapruredFrames
					== CAM_FRAME_TO_MAKE_STILL_CAPTURE)) {
				printf("\nD inside enabling Still capture \n");
				OMX_TEST_INIT_STRUCT_PTR(&bOMX,
						OMX_CONFIG_BOOLEANTYPE);
				bOMX.bEnabled = OMX_TRUE;

				eError = OMX_SetConfig(hComponent,
					OMX_IndexConfigCapturing, &bOMX);
				GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
			}

		}
		if ((appData->nCapruredFrames ==
				CAM_FRAME_TO_MAKE_STILL_CAPTURE)
				&& (zoom_prv == 1)) {
			printf("\n captured 6 frames  \n");
		}

		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				    "PRV: Start write to file...\n");
		printf("\nD pBuffer=%x, FilledLen=%x, outputfile=%x \n",
				(uint)pBuffer->pBuffer,
				(uint)pBuffer->nFilledLen,
				(uint)appData->pPrvOutputFile);
		byteswritten = fwrite(pBuffer->pBuffer, 1,
					pBuffer->nFilledLen,
					appData->pPrvOutputFile);
		printf("\n PRV OP FILE bytes_written = %d \n", byteswritten);
		printf("\n pBuffer->nFilledLen = %d \n",
					(int)pBuffer->nFilledLen);

		if ( byteswritten != pBuffer->nFilledLen) {
			printf("\n D ERROR IN WRITING FILE \n");
		}
		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
			    "PRV: File write done.\n");

		if (appData->nCapruredFrames < CAM_FRAMES_TO_CAPTURE) {
			OMX_FillThisBuffer(hComponent, pBuffer);
		} else {
			retval = TIMM_OSAL_EventSet(CameraEvents,
					CAM_EVENT_MAIN_LOOP_EXIT,
				       	TIMM_OSAL_EVENT_OR);
			GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),
						OMX_ErrorBadParameter);
		}
		appData->nCapruredFrames++;

	} else if (appData->nCapturePortIndex == pBuffer->nOutputPortIndex) {
		OMX_CONFIG_BOOLEANTYPE bOMX;

		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
			    "CAP: Start write to file...\n");
		byteswritten=fwrite(pBuffer->pBuffer,  1,pBuffer->nFilledLen,
			   appData->pCapOutputFile);
		printf("\n PRV OP FILE bytes_written = %d \n",byteswritten);
		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
		    "CAP: File write done.\n");

		OMX_TEST_INIT_STRUCT_PTR(&bOMX, OMX_CONFIG_BOOLEANTYPE);
bOMX.bEnabled = OMX_FALSE;

		eError = OMX_SetConfig(hComponent,
					OMX_IndexConfigCapturing, &bOMX);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}

	if (eError == OMX_ErrorUndefined)
	eError = OMX_ErrorNone;
EXIT:
	return eError;
}

static OMX_ERRORTYPE test_OMX_CAM_SetParams(test_OMX_CAM_AppData_t *appData)
{
    OMX_HANDLETYPE pHandle;
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_CONFIG_CAMOPERATINGMODETYPE tCamOpMode;
    OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
    OMX_CONFIG_ROTATIONTYPE      tCapRot;
    OMX_CONFIG_MIRRORTYPE        tCapMir;

    /* if component has not been initialized, go to error */
    GOTO_EXIT_IF((!appData || !(appData->pHandle)), eError);


    /* initialize omx struct to be used to set parameter */
    OMX_TEST_INIT_STRUCT_PTR(&tCamOpMode, OMX_CONFIG_CAMOPERATINGMODETYPE);
    OMX_TEST_INIT_STRUCT_PTR(&tPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

if (zoom_prv == 0){
    OMX_TEST_INIT_STRUCT_PTR(&tCapRot, OMX_CONFIG_ROTATIONTYPE);
    OMX_TEST_INIT_STRUCT_PTR(&tCapMir, OMX_CONFIG_MIRRORTYPE);
}
    pHandle = appData->pHandle;


    /* Select Usecase */
/* as per borislav this is a default setting so can be commented for now
    tCamOpMode.eCamOperatingMode = OMX_CaptureImageProfileBase;
    eError = OMX_SetParameter(pHandle, OMX_IndexCameraOperatingMode, &tCamOpMode);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);*/


	printf("\n D (4) n");
    /* Disable all ports except Preview (VPB+1) port and Image (IPB+0) port */
    eError = OMX_SendCommand(pHandle, OMX_CommandPortDisable, OMX_ALL, NULL);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
printf("\n ALL PORTS DISABLED");
    eError = OMX_SendCommand(pHandle, OMX_CommandPortEnable, appData->nPreviewPortIndex, NULL);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
printf("\nENABLED PREVIEW PORT ");


	if(zoom_prv == 0){ printf("\n\n\n ++++++++++ \n");
    eError = OMX_SendCommand(pHandle, OMX_CommandPortEnable, appData->nCapturePortIndex, NULL);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

printf("\n ENABLED IMAGE CAPTURE PORT \n");
}
    /* Setup Preview Port */
    tPortDef.nPortIndex = appData->nPreviewPortIndex;
    eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

printf("\n D (5) n");

    tPortDef.format.video.eColorFormat = appData->ePrvFormat;
    tPortDef.format.video.nFrameWidth = appData->nPrvWidth;
    tPortDef.format.video.nFrameHeight = appData->nPrvHeight;
    tPortDef.format.video.nStride = appData->nPrvStride;
    tPortDef.format.video.xFramerate = 15;
    eError = OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

printf("\n D (6) n");
if (zoom_prv == 0) {

    /* Setup Image Capture Port */
    tPortDef.nPortIndex = appData->nCapturePortIndex;
    eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

printf("\n D (7) n");

    tPortDef.format.image.eCompressionFormat = appData->eCapCompressionFormat;
    tPortDef.format.image.eColorFormat = appData->eCapColorFormat;
    tPortDef.format.image.nFrameWidth = appData->nCapWidth;
    tPortDef.format.image.nFrameHeight = appData->nCapHeight;
    tPortDef.format.image.nStride = appData->nCapStride;
    eError = OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, &tPortDef);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

printf("\n D (8) n");
    tCapRot.nPortIndex = appData->nCapturePortIndex;
    tCapRot.nRotation = appData->nCaptureRot;
    eError = OMX_SetConfig(pHandle, OMX_IndexConfigCommonRotate, &tCapRot);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
printf("\n D (9) n");
    tCapMir.nPortIndex = appData->nCapturePortIndex;
    tCapMir.eMirror = appData->eCaptureMirror;
    eError = OMX_SetConfig(pHandle, OMX_IndexConfigCommonMirror, &tCapMir);
    GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
}
printf("\n D (10) n");

	return OMX_ErrorNone;

EXIT:
	printf("\n\n\n D ERROR OCCURED IN SETCONG F returning \n");
    return eError;
}



int fun_case2_case3(void)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	TIMM_OSAL_ERRORTYPE retval = TIMM_OSAL_ERR_UNKNOWN;
	OMX_CALLBACKTYPE appCallbacks;
	OMX_HANDLETYPE pHandle;
	test_OMX_CAM_AppData_t appData;
	OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
	TIMM_OSAL_U32 pRetrievedEvents;
	OMX_U32 i;
#ifdef OMXCAM_USE_BUFFER
	MemAllocBlock       tMemBlocks;
	OMX_U8 *pPreviewBuf[6];
	OMX_U8 *pCaptureBuf[6];
#endif

/*****************************************
* Initialize
*****************************************/
	OMX_TEST_INIT_STRUCT_PTR(&tPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

	appData.nPreviewPortIndex = OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW;
	appData.nCapruredFrames = 0;
	appData.nPrvWidth = 320;
	appData.nPrvHeight = 240;
	appData.ePrvFormat = OMX_COLOR_FormatCbYCrY;
	appData.nPrvBytesPerPixel = OMXColorFormatGetBytesPerPixel(appData.ePrvFormat);
	appData.nPrvStride = ROUND_UP16(appData.nPrvWidth) * appData.nPrvBytesPerPixel;

	appData.pPrvOutputFile = fopen("/camera_bin/prvout.yuv", "wb");
	printf("\nD prev output file = %d\n",(int) appData.pPrvOutputFile);
	GOTO_EXIT_IF((appData.pPrvOutputFile == NULL),
					OMX_ErrorInsufficientResources);

	/* we work with image port in case of image capture */
	if( zoom_prv==0){
		appData.nCapturePortIndex = OMX_CAMERA_PORT_IMAGE_OUT_IMAGE;
		appData.nCapWidth = 640;
		appData.nCapHeight = 480;
		appData.eCapColorFormat = OMX_COLOR_FormatCbYCrY;
		appData.eCapCompressionFormat = OMX_IMAGE_CodingJPEG;
		appData.nCapBytesPerPixel = OMXColorFormatGetBytesPerPixel(appData.eCapColorFormat);
		appData.nCapStride = ROUND_UP16(appData.nCapWidth) * appData.nCapBytesPerPixel;
		appData.nCaptureRot = 0;
		appData.eCaptureMirror = OMX_MirrorNone;
		if (OMX_IMAGE_CodingJPEG == appData.eCapCompressionFormat) {
			appData.pCapOutputFile = fopen(
					"/camera_bin/capout_jpeg.jpg", "wb");
		} else {
			appData.pCapOutputFile = fopen(
					"/camera_bin/capout_yuv.yuv", "wb");
		}
		GOTO_EXIT_IF((appData.pCapOutputFile == NULL),
						OMX_ErrorInsufficientResources);
	}


	retval = TIMM_OSAL_EventCreate(&CameraEvents);
	GOTO_EXIT_IF((retval != TIMM_OSAL_ERR_NONE),
				OMX_ErrorInsufficientResources);
		printf("\n D STRUCT INIT + parameters set \n");

	eError = OMX_Init();
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	printf("\n D OMX_INIT DONE \n");
	appCallbacks.EventHandler = test_OMX_CAM_EventHandler; // TODO:
	appCallbacks.EmptyBufferDone = test_OMX_CAM_EmptyBufferDone; // TODO:
	appCallbacks.FillBufferDone = test_OMX_CAM_FillBufferDone; // TODO:

	/*****************************************
	* Get components handle
	*****************************************/
	eError = OMX_GetHandle(&pHandle,
			(OMX_STRING)"OMX.TI.DUCATI1.VIDEO.CAMERA",
			&appData, &appCallbacks);

	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	appData.pHandle = pHandle;
	printf("\n D OMX_GETHANDLE DONE\n");

	/*****************************************
	* Set component parameters
	*****************************************/
	eError = test_OMX_CAM_SetParams(&appData);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	printf("\n D SetParamas Done \n");

	/*****************************************
	* Transition Loaded -> Idle
	*****************************************/
	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
			    "Send Command Loaded -> Idle\n");

	eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle,
		       							NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	printf("\n D Now lets allocate Buffers \n");
	/* Allocate buffers */
	/* Preview Port */
	tPortDef.nPortIndex = appData.nPreviewPortIndex;
	eError = OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition,
								&tPortDef);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	appData.nPreviewBuffers = tPortDef.nBufferCountActual;
	printf("\n PRV BUFFERS = %d\n",(int)tPortDef.nBufferCountActual);

	appData.ppPreviewBuffers = TIMM_OSAL_Malloc(
			sizeof(OMX_BUFFERHEADERTYPE *)*appData.nPreviewBuffers,
		       	TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);

	GOTO_EXIT_IF((appData.ppPreviewBuffers == TIMM_OSAL_NULL),
			OMX_ErrorInsufficientResources);

#ifdef OMXCAM_USE_BUFFER
#ifdef OMX_TILERTEST

	for (i = 0; i < appData.nPreviewBuffers; i++) {
		memset((void *) &tMemBlocks, 0,sizeof(tMemBlocks) );
		tMemBlocks.dim.len = tPortDef.nBufferSize;
		tMemBlocks.pixelFormat = PIXEL_FMT_PAGE;
		pPreviewBuf[i] =MemMgr_Alloc(&tMemBlocks, 1);

		eError = OMX_UseBuffer(pHandle, appData.ppPreviewBuffers + i,
				tPortDef.nPortIndex, &appData,
				tPortDef.nBufferSize, pPreviewBuf[i]);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}
#else /* OMX_TILERTEST */

	for (i = 0; i < appData.nPreviewBuffers; i++) {
		 pPreviewBuf[i] = TIMM_OSAL_Malloc(tPortDef.nBufferSize,
				 TIMM_OSAL_TRUE, 0 ,TIMMOSAL_MEM_SEGMENT_EXT);
		eError = OMX_UseBuffer(pHandle, appData.ppPreviewBuffers + i,
			       tPortDef.nPortIndex, &appData,
			       tPortDef.nBufferSize, pPreviewBuf[i]);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}

#endif /* OMX_TILERTEST */
#else /* OMXCAM_USE_BUFFER */

	for (i = 0; i < appData.nPreviewBuffers; i++) {
		eError = OMX_AllocateBuffer(pHandle,
				appData.ppPreviewBuffers + i,
				tPortDef.nPortIndex, &appData,
				tPortDef.nBufferSize);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}

#endif /* OMXCAM_USE_BUFFER */


	if( zoom_prv==0){
	/* Capture Port */
		tPortDef.nPortIndex = appData.nCapturePortIndex;
		eError = OMX_GetParameter(pHandle,
			       OMX_IndexParamPortDefinition, &tPortDef);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

		appData.nCaptureBuffers = tPortDef.nBufferCountActual;
		printf("\n CAPTURE BUFFERS = %d\n",
					(int)tPortDef.nBufferCountActual);
		appData.ppCaptureBuffers = TIMM_OSAL_Malloc(sizeof
			(OMX_BUFFERHEADERTYPE *) * appData.nCaptureBuffers,
			TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
		GOTO_EXIT_IF((appData.ppCaptureBuffers == TIMM_OSAL_NULL),
					OMX_ErrorInsufficientResources);
#ifdef OMXCAM_USE_BUFFER
#ifdef OMX_TILERTEST
		for (i = 0; i < appData.nCaptureBuffers; i++) {
			memset((void *) &tMemBlocks, 0,	sizeof(tMemBlocks));
			tMemBlocks.dim.len = tPortDef.nBufferSize;
			tMemBlocks.pixelFormat = PIXEL_FMT_PAGE;
			pCaptureBuf[i] = MemMgr_Alloc(&tMemBlocks, 1);
			eError = OMX_UseBuffer(pHandle,
					appData.ppCaptureBuffers + i,
					tPortDef.nPortIndex, &appData,
					tPortDef.nBufferSize, pCaptureBuf[i]);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}
#else /* OMX_TILERTEST */

		for (i = 0; i < appData.nCaptureBuffers; i++) {
			 pCaptureBuf[i] = TIMM_OSAL_Malloc(tPortDef.nBufferSize,
					 TIMM_OSAL_TRUE, 0 ,
					 TIMMOSAL_MEM_SEGMENT_EXT);
			eError = OMX_UseBuffer(pHandle,
					appData.ppCaptureBuffers + i,
					tPortDef.nPortIndex, &appData,
					tPortDef.nBufferSize, pCaptureBuf[i]);

			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
		}

#endif /* OMX_TILERTEST */
#else
		for (i = 0; i < appData.nCaptureBuffers; i++) {
			eError = OMX_AllocateBuffer(pHandle,
					appData.ppCaptureBuffers + i,
					tPortDef.nPortIndex, &appData,
					tPortDef.nBufferSize);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
		}
#endif
	}

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
					"Waiting for Loaded -> Idle\n");
	WaitForState(pHandle, OMX_StateIdle);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
					"Waiting for Loaded -> Idle Done\n");


	/*****************************************
	* Transition Idle -> Executing
	*****************************************/
	eError = OMX_SendCommand(pHandle, OMX_CommandStateSet,
			    OMX_StateExecuting, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
		    			"\nWaiting for Idle -> Executing\n");

	WaitForState(pHandle, OMX_StateExecuting);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"\nWaiting for Idle -> Executing Done\n");


	/*****************************************
	* Main Loop
	*****************************************/
	if( zoom_prv==0){
		for (i = 0; i < 1/*appData.nCaptureBuffers*/; i++) {
			eError = OMX_FillThisBuffer(pHandle,
						appData.ppCaptureBuffers[i]);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
		}
	}

	for (i = 0; i < 1/*appData.nPreviewBuffers*/; i++) {
		eError = OMX_FillThisBuffer(pHandle,
						appData.ppPreviewBuffers[i]);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}

	printf("\n Fill Buffer called \n");
	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM, "\nMain loop\n");

	retval = TIMM_OSAL_EventRetrieve(CameraEvents, CAM_EVENT_MAIN_LOOP_EXIT,
			     TIMM_OSAL_EVENT_AND_CONSUME, &pRetrievedEvents,
			     TIMM_OSAL_SUSPEND);
	GOTO_EXIT_IF(((retval != TIMM_OSAL_ERR_NONE) &&
			    (retval == TIMM_OSAL_WAR_TIMEOUT)),
			    OMX_ErrorTimeout);


	/*****************************************
	* Transition Executing -> Idle
	*****************************************/
	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM, "Executing -> Idle\n");

	eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle,
									NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"Waiting for Executing -> Idle\n");

	WaitForState(pHandle, OMX_StateIdle);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"Waiting for Executing -> Idle Done\n");


	/*****************************************
	* Transition Idle -> Loaded
	*****************************************/
	TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM, "Idle -> Loaded\n");

	eError = OMX_SendCommand(pHandle, OMX_CommandStateSet,
							OMX_StateLoaded, NULL);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);



	for (i = 0; i < appData.nPreviewBuffers; i++) {
#ifdef OMXCAM_USE_BUFFER
#ifdef OMX_TILERTEST
		eError = MemMgr_Free(pPreviewBuf[i]);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#else /* OMX_TILERTEST */
		TIMM_OSAL_Free(pPreviewBuf[i]);
#endif /* OMX_TILERTEST */
#endif
		eError = OMX_FreeBuffer(pHandle, appData.nPreviewPortIndex,
						appData.ppPreviewBuffers[i]);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	}



	if( zoom_prv==0){
		for (i = 0; i < appData.nCaptureBuffers; i++) {
#ifdef OMXCAM_USE_BUFFER
#ifdef OMX_TILERTEST
			eError = MemMgr_Free(pCaptureBuf[i]);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
#else /* OMX_TILERTEST */
			TIMM_OSAL_Free(pCaptureBuf[i]);
#endif /* OMX_TILERTEST */
#endif
			eError = OMX_FreeBuffer(pHandle,
					appData.nCapturePortIndex,
					appData.ppCaptureBuffers[i]);
			GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
		}


		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"Waiting for Idle -> Loaded\n");

		WaitForState(pHandle, OMX_StateLoaded);
		GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"Waiting for Idle -> Loaded Done\n");

	}
	/*****************************************
	* Finalize
	*****************************************/
	eError = OMX_FreeHandle(pHandle);
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);

	printf("\n D FreeHandle Done \n");

	eError = OMX_Deinit();
	GOTO_EXIT_IF((eError != OMX_ErrorNone), eError);
	printf("\n D Deinit() done \n");

	/* TODO Need to check the EXIT routine */
EXIT:
	TIMM_OSAL_EventDelete(CameraEvents);
	printf("\nD TIMM_OSAL_EventDelete Done \n");
	TIMM_OSAL_Free(appData.ppPreviewBuffers);
	printf("\nD TIMM_OSAL_Free Done \n");

	if (eError != OMX_ErrorNone)
		TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_OMXCAM, "\n\tError!!!\n");
	else
		TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_OMXCAM,
				"\n\tSuccess!!!\n");

	return eError;
}
