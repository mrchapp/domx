#ifndef __CAMERA_TEST_H__
#define __CAMERA_TEST_H__

#define TILER_BUFFERS

#include <OMX_TI_IVCommon.h>

/* FIXME check min and max values of different use cases */
#define MAX_PREVIEW_WIDTH	2500
#define MAX_PREVIEW_HEIGHT	2500
#define MAX_CAPTURE_WIDTH	2500
#define MAX_CAPTURE_HEIGHT	2500
#define MAX_VIDEO_WIDTH		1920
#define MAX_VIDEO_HEIGHT	1080

#define MIN_PREVIEW_WIDTH	100
#define MIN_PREVIEW_HEIGHT	100
#define MIN_CAPTURE_WIDTH	100
#define MIN_CAPTURE_HEIGHT	100
#define MIN_VIDEO_WIDTH		100
#define MIN_VIDEO_HEIGHT	100

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

#define OMX_TEST_BAIL_IF_ERROR(_eError)		\
        if(OMX_ErrorNone != (eError = _eError)){	\
                goto OMX_TEST_BAIL;		\
        }

#define OMX_TEST_SET_ERROR_BAIL(_eCode, _desc)	\
{						\
    eError = _eCode;				\
    printf(_desc);\
    goto OMX_TEST_BAIL;			\
}

#define OMX_TEST_INIT_STRUCT(_s_, _name_)	\
    memset(&(_s_), 0x0, sizeof(_name_));	\
    (_s_).nSize = sizeof(_name_);		\
    (_s_).nVersion.s.nVersionMajor = 0x1;	\
    (_s_).nVersion.s.nVersionMinor = 0x1;	\
    (_s_).nVersion.s.nRevision = 0x0;		\
    (_s_).nVersion.s.nStep = 0x0

 struct port_param {
	OMX_U32 hostbufaddr[MAX_NO_BUFFERS];
	OMX_BUFFERHEADERTYPE *bufferheader[MAX_NO_BUFFERS];

	OMX_U32 nWidth;
	OMX_U32 nHeight;
	OMX_U32 nStride;
	OMX_COLOR_FORMATTYPE eColorFormat;
	OMX_PARAM_VIDEONOISEFILTERTYPE tVNFMode;
	OMX_PARAM_VIDEOYUVRANGETYPE tYUVRange;
	OMX_CONFIG_BOOLEANTYPE tVidStabParam;
	OMX_CONFIG_FRAMESTABTYPE tVidStabConfig;
	OMX_U32 nCapFrame;
	OMX_U32 nActualBuffer;

};


typedef struct SampleCompTestCtxt{
        OMX_HANDLETYPE hComp;
        OMX_U32 nPorts;
        OMX_STATETYPE eState ;
        OMX_HANDLETYPE hStateSetEvent;
        OMX_HANDLETYPE hExitSem;
        FILE *pOutputFile;
	OMX_U32 nVideoPortIndex;
	OMX_U32 nPrevPortIndex;
	struct port_param sPortParam[6];
        OMX_PARAM_VIDEONOISEFILTERTYPE tVNFMode;
        OMX_PARAM_VIDEOYUVRANGETYPE tYUVRange;
        OMX_CONFIG_BOOLEANTYPE tVidStabParam;
        OMX_CONFIG_FRAMESTABTYPE tVidStabConfig;
	
}SampleCompTestCtxt;


/* ****** from omx_iss_cam_def.h ******************************************* */
/* need to find a better place for this, because right now same thing is
 * cut/paste in gstomx_camera.h as well as here... maybe this goes in one
 * of the OMX_TI*.h headers??
 */
/* Default portstartnumber of Camera component */
#define OMX_CAMERA_DEFAULT_START_PORT_NUM 0

/* Define number of ports for differt domains */
#define OMX_CAMERA_PORT_OTHER_NUM 1
#define OMX_CAMERA_PORT_VIDEO_NUM 4
#define OMX_CAMERA_PORT_IMAGE_NUM 1
#define OMX_CAMERA_PORT_AUDIO_NUM 0
#define OMX_CAMERA_NUM_PORTS (OMX_CAMERA_PORT_OTHER_NUM + OMX_CAMERA_PORT_VIDEO_NUM + OMX_CAMERA_PORT_IMAGE_NUM + OMX_CAMERA_PORT_AUDIO_NUM)

/* Define start port number for differt domains */
#define OMX_CAMERA_PORT_OTHER_START OMX_CAMERA_DEFAULT_START_PORT_NUM
#define OMX_CAMERA_PORT_VIDEO_START (OMX_CAMERA_PORT_OTHER_START + OMX_CAMERA_PORT_OTHER_NUM)
#define OMX_CAMERA_PORT_IMAGE_START (OMX_CAMERA_PORT_VIDEO_START + OMX_CAMERA_PORT_VIDEO_NUM)
#define OMX_CAMERA_PORT_AUDIO_START (OMX_CAMERA_PORT_IMAGE_START + OMX_CAMERA_PORT_IMAGE_NUM)

/* Port index for camera component */
#define OMX_CAMERA_PORT_OTHER_IN (OMX_CAMERA_PORT_OTHER_START + 0)
#define OMX_CAMERA_PORT_VIDEO_IN_VIDEO (OMX_CAMERA_PORT_VIDEO_START + 0)
#define OMX_CAMERA_PORT_VIDEO_OUT_PREVIEW (OMX_CAMERA_PORT_VIDEO_START + 1)
#define OMX_CAMERA_PORT_VIDEO_OUT_VIDEO (OMX_CAMERA_PORT_VIDEO_START + 2)
#define OMX_CAMERA_PORT_VIDEO_OUT_MEASUREMENT (OMX_CAMERA_PORT_VIDEO_START + 3)
#define OMX_CAMERA_PORT_IMAGE_OUT_IMAGE (OMX_CAMERA_PORT_IMAGE_START + 0)
/* ************************************************************************** */

#endif

