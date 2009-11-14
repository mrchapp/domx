/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbH264Venc.h
 *
 *  @brief    This file contains structure specific to H264 Venc OMX
 *            Component.
 *
 * <B> History: </B>
 *
 *      -# October-2-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_H264_VENC_H__
#define __OMTB_H264_VENC_H__


/******************************************************************************\
 *      Includes
\******************************************************************************/

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "OMX_Types.h"
#include "OMX_Video.h"      
#include "OMX_IVCommon.h"
#ifdef WTSD
#include "omx_ti_video.h"
#else
#include "wtsd17_omx_ti_video.h"
#endif

#include "omtbPortParam.h"


/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/

#define OMTB_H264_VENC_MAX_OUTBUF_SIZE  (100 * 1024)/* Max size of I/P buffer */
#ifdef WTSD
#define OMTB_H264_VENC_COMPONENT_NAME   ("OMX.TI.DUCATI1.VIDEO.H264E")  /* Comp name */
#else
#define OMTB_H264_VENC_COMPONENT_NAME   ("OMX.TI.VIDEO.H264E")  /* Comp name */
#endif
#define OMTB_H264_VENC_COMP_REF_STR     ("h264venc")/* comp refernce string */
#define OMTB_H264_VENC_GROUP_PRIORITY   (2)         /* Task group priority */
#define OMTB_H264_VENC_TSK_STACK_SIZE   (30 * 1024) /* Task stack size */

#define OMTB_H264_VENC_MIN_I_FRM_INTERVAL   (0)
#define OMTB_H264_VENC_DEF_I_FRM_INTERVAL   (128)
#define OMTB_H264_VENC_MAX_I_FRM_INTERVAL   (512)

#define OMTB_H264_VENC_MIN_P_FRM_INTERVAL   (1)
#define OMTB_H264_VENC_DEF_P_FRM_INTERVAL   (1)
#define OMTB_H264_VENC_MAX_P_FRM_INTERVAL   (256)

#define OMTB_H264_VENC_MIN_B_FRM_INTERVAL   (0)
#define OMTB_H264_VENC_DEF_B_FRM_INTERVAL   (2)
#define OMTB_H264_VENC_MAX_B_FRM_INTERVAL   (256)

#define OMTB_H264_VENC_MIN_NUM_REF_FRM      (1)
#define OMTB_H264_VENC_DEF_NUM_REF_FRM      (2)
#define OMTB_H264_VENC_MAX_NUM_REF_FRM      (16)

#define OMTB_H264_VENC_DEF_REF_IDX_10       (0)
#define OMTB_H264_VENC_DEF_REF_IDX_11       (0)

/* This params values need to be verified/corrected if wrong */
#define OMTB_H264_VENC_DEF_SLICE_HDR_SPACING      (0)
#define OMTB_H264_VENC_DEF_ALLOWED_PIC_TYPE       (0)
#define OMTB_H264_VENC_DEF_WEIGHTED_BIPRED_MODE   (0)
#define OMTB_H264_VENC_DEF_CABAC_INIT_IDX         (0)
#define OMTB_H264_VENC_DEF_NUM_SLICE_GRP          (0)
#define OMTB_H264_VENC_DEF_SLICE_GRP_MAP_TYPE     (0)

#define OMTB_H264_VENC_DEF_LUMA_INTRA_4x4_PRED    (0x1FF)
#define OMTB_H264_VENC_MAX_LUMA_INTRA_4x4_PRED    (0x1FF)

#define OMTB_H264_VENC_DEF_LUMA_INTRA_8x8_PRED    (0x1FF)
#define OMTB_H264_VENC_MAX_LUMA_INTRA_8x8_PRED    (0x1FF)

#define OMTB_H264_VENC_DEF_LUMA_INTRA_16x16_PRED  (0x0F)
#define OMTB_H264_VENC_MAX_LUMA_INTRA_16x16_PRED  (0x0F)

#define OMTB_H264_VENC_DEF_CHROMA_INTRA_8x8_PRED  (0x0F)
#define OMTB_H264_VENC_MAX_CHROMA_INTRA_8x8_PRED  (0x0F)




#define OMTB_H264_VENC_MAX_FRM_RATE     (85)        /* Max framerate */
#define OMTB_H264_VENC_DEF_FRM_RATE     (30)        /* Def framerate */

#define OMTB_H264_VENC_MAX_BIT_RATE     (32000000)  /* Max bitrate */
#define OMTB_H264_VENC_DEF_BIT_RATE     (1000000)   /* Def bitrate */

#define OMTB_H264_VENC_DEF_MESR_HORP    (16)        /* Def ME Search range: P */
#define OMTB_H264_VENC_DEF_MESR_VERP    (16)        /* Def ME Search range: P */
#define OMTB_H264_VENC_DEF_MESR_HORB    (16)        /* Def ME Search range: B */
#define OMTB_H264_VENC_DEF_MESR_VERB    (16)        /* Def ME Search range: B */

#define OMTB_H264_VENC_MIN_QP_IPB       (0)         /* Min QP - For I, P & B */
#define OMTB_H264_VENC_MAX_QP_IPB       (51)        /* Max QP - For I, P & B */
#define OMTB_H264_VENC_DEF_QP_I         (25)        /* Def QP - For I frames */
#define OMTB_H264_VENC_DEF_QP_P         (25)        /* Def QP - For P frames */
#define OMTB_H264_VENC_DEF_QP_B         (4)         /* Def QP - For B frames */

#define OMTB_H264_VENC_MAX_SLICE_SIZE   (500)       /* Max QP - For I, P & B */
/* This params values need to be verified/corrected if wrong */
#define OMTB_H264_VENC_DEF_SLICE_SIZE   (0)         /* Def QP - For I, P & B */

#define OMTB_H264_VENC_MAX_NUM_MB       ( (OMTB_MAX_VID_WIDTH >> 4) * \
                                          (OMTB_MAX_VID_HEIGHT >> 4))
  
/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Application's private structure for OMX H264 Video Encoder Component*/

typedef struct OMTB_H264_VENC_PRIVATE_T 
{

  OMX_HANDLETYPE handle;            /* Component handle */
  OMTB_INPORT_PARAMS  inPortParams; /* Input port params */
  OMTB_OUTPORT_PARAMS outPortParams;/* Output port params */
  OMX_CALLBACKTYPE* cb;             /* callback struct var */
  OMX_VIDEO_PARAM_AVCTYPE* pH264;   /* H264 video param struct */
  char  compressionFormat;          /* encoder compression format */
  void* eventSem;                   /* Event Semaphore */
  void* eofSem;                     /* End of Stream Semaphore */
  unsigned int instanceNo;          /* H264 encoder Instance number */
  unsigned int flagThrCreated;      /* Func/API thread created Flag */
  char compName[OMX_MAX_STRINGNAME_SIZE]; 
                                    /* OMX component name */
  OMX_VIDEO_PARAM_PROFILELEVELTYPE profileLevel;
                                    /* H264 profile & level */
  OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE inputImgFmt;
                                    /* Input Image format */
  OMX_VIDEO_PARAM_AVCTYPE avcParams;
                                    /* AVC config params */
  OMX_VIDEO_PARAM_AVCSLICEFMO avcFMO;
                                    /* FMO slice mode params */
  OMX_VIDEO_CONTROLRATETYPE rateControlAlg; 
                                    /* Rate control algorithm */
  OMX_VIDEO_TRANSFORMBLOCKSIZETYPE transformBlockSize;
                                    /* Transform BLK size */
  OMX_VIDEO_PARAM_ENCODER_PRESETTYPE encoderPreset;
                                    /* Encoding & Rate control preset */
  OMX_VIDEO_AVCBITSTREAMFORMATTYPE bitStreamFormat;   
                                    /* Bitstream format*/
  unsigned int maxInterFrameInterval; 
                                    /* Max inter frame interval */
  
}OMTB_H264_VENC_PRIVATE;


/* Configuration structure for OMX H264 Video Enocder Component */

typedef struct OMTB_H264_VENC_CONFIG_PARAMS_T 
{
  /* Basic configuration parameters */
  char inputFile[OMTB_MAX_LEN];     /* Array to store i/p File Name */
  char outputFile[OMTB_MAX_LEN];    /* Array to store o/p File Name */
  char codecType[OMTB_MAX_LEN];     /* Array to store code type */
  unsigned int frameWidth;          /* Variable to store I/P Frame Width */
  unsigned int frameHeight;         /* Variable to store I/P Frame Height */
  OMX_COLOR_FORMATTYPE colorFormat; /* Variable to store I/P Color Format */
  
  OmtbOutPutMode dataOutPutMode;    /* For selecting O/P Mode */
  unsigned int numInBuf;            /* Number of i/p buffers */
  unsigned int numOutBuf;           /* Number of o/p buffers */
  unsigned int flagUseInBuffer;     /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  unsigned int flagUseOutBuffer;    /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  char compName[OMX_MAX_STRINGNAME_SIZE];  
                                    /* OMX component name */
  OMX_VIDEO_PARAM_PROFILELEVELTYPE profileLevel;
                                    /* H264 profile & level */
  OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE inputImgFmt;
                                    /* Input Image format */
  OMX_VIDEO_PARAM_AVCTYPE avcParams;
                                    /* AVC config params */
  OMX_VIDEO_PARAM_AVCSLICEFMO avcFMO;
                                    /* FMO slice mode params */
  OMX_VIDEO_CONTROLRATETYPE rateControlAlg; 
                                    /* Rate control algorithm */
  OMX_VIDEO_TRANSFORMBLOCKSIZETYPE transformBlockSize;
                                    /* Transform BLK size */
  OMX_VIDEO_PARAM_ENCODER_PRESETTYPE encoderPreset;
                                    /* Encoding & Rate control preset */
  OMX_VIDEO_AVCBITSTREAMFORMATTYPE bitStreamFormat;   
                                    /* Bitstream format*/
                                    
  /* Dynamic configuration params */
  OMX_CONFIG_FRAMERATETYPE frameRate;
                                    /* Framerate */
  OMX_VIDEO_CONFIG_BITRATETYPE bitRate;
                                    /* Bitrate */
  OMX_VIDEO_CONFIG_MESEARCHRANGETYPE meSearchrange;
                                    /* Motion estimation search range */
  OMX_CONFIG_INTRAREFRESHVOPTYPE forceFrame;
                                    /* Intra referesh VOP type */
  OMX_VIDEO_CONFIG_QPSETTINGSTYPE qpSettings;
                                    /* QP settings */
  OMX_VIDEO_CONFIG_AVCINTRAPERIOD avcIntraPeriod;
                                    /* Intra frame period */
  OMX_VIDEO_CONFIG_SLICECODINGTYPE  sliceSettings;
                                    /* Slice settings */
  OMX_VIDEO_CONFIG_PIXELINFOTYPE pixelInfo;
                                    /* Pikel information like aspect ratio */
                                    
  /* Advanced configuration params */
  OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE naluSettings;
                                    /* NALU settings */
  OMX_VIDEO_PARAM_MEBLOCKSIZETYPE meBlockSize;
                                    /* ME block size */
  OMX_VIDEO_PARAM_INTRAREFRESHTYPE intraRefresh;
                                    /* Intra refresh period */
  OMX_VIDEO_PARAM_VUIINFOTYPE vuiSettings;
                                    /* VUI settings */
  OMX_VIDEO_PARAM_INTRAPREDTYPE intraPred;
                                    /* Intra prediction modes */
  OMX_VIDEO_PARAM_AVCADVANCEDFMOTYPE advancedFMO;
                                    /* Advanced FMO settings */
  OMX_VIDEO_PARAM_DATASYNCMODETYPE vidDataMode;
                                    /* Video data mode */
  

}OMTB_H264_VENC_CONFIG_PARAMS;

/******************************************************************************\
 *      Macros
\******************************************************************************/


/******************************************************************************\
 *      Globals
\******************************************************************************/


/******************************************************************************\
 *      External Object Definitions
\******************************************************************************/


/******************************************************************************\
 *      Public Function Prototypes
\******************************************************************************/

/******************************************************************************\
 *      H264VENC_AllocateResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for H264 encoder.
 *
 *  @param in:
 *            h264VencClient: Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std error/return code
 *
 */

OMX_ERRORTYPE H264VENC_AllocateResources(
  OMTB_H264_VENC_PRIVATE* h264VencClient);


/******************************************************************************\
 *      H264VENC_SetPortDefinition Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for H264 encoder.
 *
 *  @param in:
 *            h264VencClient:Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VENC_SetPortDefinition(
  OMTB_H264_VENC_PRIVATE* h264VencClient);


/******************************************************************************\
 *      H264VENC_GetStructIdx Function Prototype
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
  void **pCompCfgStruct);


/******************************************************************************\
 *      H264VENC_SetParameter Function Prototype
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
  unsigned int template);


/******************************************************************************\
 *      H264VENC_SetConfig Function Prototype
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
  unsigned int template);


/******************************************************************************\
 *      H264VENC_GetParameter Function Prototype
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
  unsigned int template);


/******************************************************************************\
 *      H264VENC_GetConfig Function Prototype
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
  unsigned int template);


/******************************************************************************\
 *      H264VENC_UseInitialResources Function Prototype
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
                          OMTB_H264_VENC_PRIVATE * h264VencClient);


/******************************************************************************\
 *      H264VENC_FreeResources Function Prototype
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

OMX_ERRORTYPE H264VENC_FreeResources(OMTB_H264_VENC_PRIVATE *h264VencClient);


/******************************************************************************\
 *      H264VENC_FreeMemory Function Prototype
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

void H264VENC_FreeMemory(OMTB_H264_VENC_PRIVATE *h264VencClient);


/******************************************************************************\
 *      H264VENC_FuncTsk Thread Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a H264 thread which invokes during func command
 *
 *  @param in:void *threads_arg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void H264VENC_FuncTsk(void *threads_arg);


/******************************************************************************\
 *      H264VENC_ApiTsk Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a H264 thread which invokes during exec command
 *
 *  @param in:
 *            void *threads_arg : Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void H264VENC_ApiTsk(void *threads_arg);


/******************************************************************************\
 *      H264VENC_InputDataRead Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs i/p data read for H264 encoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void H264VENC_InputDataRead(void *threadsArg);

/******************************************************************************\
 *      H264VENC_OutputDataWrite Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs o/p data write for H264 encoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void H264VENC_OutputDataWrite(void *threadsArg);


/******************************************************************************\
 *      H264VENC_FillData Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *
 *            pBuf - Pointer to buffer to be filled
 *  @param Out:
 *            Pointer of I/P filled Buffer
 *
 *  @return   No of bytes read
 *
 */
 
unsigned int H264VENC_FillData( OMTB_H264_VENC_PRIVATE* pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf);


/******************************************************************************\
 *      H264VENC_AllocateMemory Prototype
\******************************************************************************/
/**
 *  @brief    Allocate memory for private data structure for h264 video encode
 *            component
 *  @param in:
 *            h264VencClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */
 
OmtbRet H264VENC_AllocateMemory( OMTB_H264_VENC_PRIVATE *h264VencClient);


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
  OMX_OUT OMX_PTR pEventData);


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
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


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
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file -----------------------------*/
