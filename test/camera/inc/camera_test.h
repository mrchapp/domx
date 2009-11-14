
#define TILER_BUFFERS

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

	OMX_U32 nPrvWidth;
	OMX_U32 nPrvHeight;
	OMX_U32 nPrvStride;
	OMX_U32 nCapWidth;
	OMX_U32 nCapHeight;
	OMX_U32 nCapStride;
	OMX_PARAM_VIDEONOISEFILTERTYPE tVNFMode;
	OMX_PARAM_VIDEOYUVRANGETYPE tYUVRange;
	OMX_CONFIG_BOOLEANTYPE tVidStabParam;
	OMX_CONFIG_FRAMESTABTYPE tVidStabConfig;
	OMX_U32 nCapFrame;

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
}SampleCompTestCtxt;

