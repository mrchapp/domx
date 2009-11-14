/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbPortParam.h
 *
 *  @brief    This file contains structure specific to OMX components port
 *            definition related parameters for all the components.
 *
 * <B> History: </B>
 *
 *     -# August-11-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_PORT_PARAM_H__
#define __OMTB_PORT_PARAM_H__

/******************************************************************************\
 *      Includes
\******************************************************************************/


/******************************************************************************/

#ifdef __cplusplus   /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/

/* No of i/p and o/p buffers */
#define OMTB_H264_VDEC_NUM_IN_BUFS        (1)  /* No of i/p Buffers for H264 */
#ifdef WTSD
#define OMTB_H264_VDEC_NUM_OUT_BUFS       (20)  /* No of o/p Buffers for H264 */
#else
#define OMTB_H264_VDEC_NUM_OUT_BUFS       (1)  /* No of o/p Buffers for H264 */
#endif
#define OMTB_H264_VENC_NUM_IN_BUFS        (1)  /* No of i/p Buffers for H264 */
#define OMTB_H264_VENC_NUM_OUT_BUFS       (1)  /* No of o/p Buffers for H264 */
#define OMTB_G711_ADEC_NUM_IN_BUFS        (3)  /* No of i/p Buffers for G711 */
#define OMTB_G711_ADEC_NUM_OUT_BUFS       (3)  /* No of o/p Buffers for G711 */
#define OMTB_VPSS_VFCC_NUM_IN_BUFS        (3)  /* No of i/p Buffers for VFCC */
#define OMTB_VPSS_VFCC_NUM_OUT_BUFS       (3)  /* No of o/p Buffers for VFCC */
#define OMTB_VPSS_VFDC_NUM_IN_BUFS        (3)  /* No of i/p Buffers for VFDC */
#define OMTB_VPSS_VFDC_NUM_OUT_BUFS       (3)  /* No of o/p Buffers for VFDC */


/* Maximum no of i/p and o/p buffers */
#define OMTB_H264_VDEC_MAX_NUM_IN_BUFS    (32) /* Max i/p Buffers for H264 */
#define OMTB_H264_VDEC_MAX_NUM_OUT_BUFS   (32) /* Max o/p Buffers for H264 */
#define OMTB_H264_VENC_MAX_NUM_IN_BUFS    (32) /* Max i/p Buffers for H264 */
#define OMTB_H264_VENC_MAX_NUM_OUT_BUFS   (32) /* Max o/p Buffers for H264 */
#define OMTB_G711_ADEC_MAX_NUM_IN_BUFS    (32) /* Max i/p Buffers for G711 */
#define OMTB_G711_ADEC_MAX_NUM_OUT_BUFS   (32) /* Max o/p Buffers for G711 */
#define OMTB_VPSS_VFCC_MAX_NUM_IN_BUFS    (32) /* Max i/p Buffers for VFCC */
#define OMTB_VPSS_VFCC_MAX_NUM_OUT_BUFS   (32) /* Max o/p Buffers for VFCC */
#define OMTB_VPSS_VFDC_MAX_NUM_IN_BUFS    (32) /* Max i/p Buffers for VFDC */
#define OMTB_VPSS_VFDC_MAX_NUM_OUT_BUFS   (32) /* Max o/p Buffers for VFDC */


/* Max video frame height and width */
#define OMTB_MAX_VID_WIDTH                (1920)
#define OMTB_MAX_VID_HEIGHT               (1080)


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/*!
 *******************************************************************************
 *  \struct OMTB_H264_VDEC_INPORT_PARAMS
 *  \brief  H264 video decoder specific input port parameters
 *******************************************************************************
 */

typedef struct OMTB_H264_VDEC_INPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pInBuff[OMTB_H264_VDEC_MAX_NUM_IN_BUFS];/* I/P Buffers*/
  FILE *frameFile;                          /* Frame Size File Pointer */
  unsigned int frameSize[OMTB_MAX_FRAME];   /* Buffer for keeping frame sizes */
  unsigned int frameCount;            /* Frame counter used to get frame size */
    
}OMTB_H264_VDEC_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_H264_VDEC_OUTPORT_PARAMS
 *  \brief  H264 video decoder specific output port parameters
 *******************************************************************************
 */
 
typedef struct OMTB_H264_VDEC_OUTPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pOutBuff[OMTB_H264_VDEC_MAX_NUM_OUT_BUFS];/* O/P Bufs */
  OMX_COLOR_FORMATTYPE colorFormat;   /* O/P Color Format */
  unsigned int nWidth;                /* I/P Frame Width */
  unsigned int nHeight;               /* I/P Frame Height */
  
}OMTB_H264_VDEC_OUTPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_G711_ADEC_INPORT_PARAMS
 *  \brief  G711 audio decoder specific input port parameters
 *******************************************************************************
 */

typedef struct OMTB_G711_ADEC_INPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pInBuff[OMTB_G711_ADEC_MAX_NUM_IN_BUFS];/* I/P Buffers*/
  
}OMTB_G711_ADEC_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_G711_ADEC_OUTPORT_PARAMS
 *  \brief  G711 audio decoder specific output port parameters
 *******************************************************************************
 */

typedef struct OMTB_G711_ADEC_OUTPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pOutBuff[OMTB_G711_ADEC_MAX_NUM_OUT_BUFS];/* O/P Bufs */
  char *tempBuff;                 /* Temporary Pointer to copy pcm buffer */
  char *stereoBuff;               /* Pointer to stereo buffer for alsa driver */
  
}OMTB_G711_ADEC_OUTPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_H264_VENC_INPORT_PARAMS
 *  \brief  H264 video encoder specific input port parameters
 *******************************************************************************
 */

typedef struct OMTB_H264_VENC_INPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pInBuff[OMTB_H264_VENC_MAX_NUM_IN_BUFS];/* I/P Buffers*/
  OMX_COLOR_FORMATTYPE colorFormat;   /* O/P Color Format */
  unsigned int nWidth;                /* I/P Frame Width */
  unsigned int nHeight;               /* I/P Frame Height */
  unsigned int frameCount;            /* Frame counter */
    
}OMTB_H264_VENC_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_H264_VENC_OUTPORT_PARAMS
 *  \brief  H264 video encoder specific output port parameters
 *******************************************************************************
 */
 
typedef struct OMTB_H264_VENC_OUTPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pOutBuff[OMTB_H264_VENC_MAX_NUM_OUT_BUFS];/* O/P Bufs */
  
}OMTB_H264_VENC_OUTPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_VPSS_VFCC_INPORT_PARAMS
 *  \brief  VPSS VFCC specific input port parameters
 *******************************************************************************
 */

typedef struct OMTB_VPSS_VFCC_INPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pInBuff[OMTB_VPSS_VFCC_MAX_NUM_IN_BUFS];/* I/P Buffers*/
  unsigned int frameCount;            /* Frame counter used to get frame size */
    
}OMTB_VPSS_VFCC_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_VPSS_VFCC_OUTPORT_PARAMS
 *  \brief  VPSS VFCC specific output port parameters
 *******************************************************************************
 */
 
typedef struct OMTB_VPSS_VFCC_OUTPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pOutBuff[OMTB_VPSS_VFCC_MAX_NUM_OUT_BUFS];/* O/P Bufs */
  OMX_COLOR_FORMATTYPE colorFormat;   /* O/P Color Format */
  unsigned int nWidth;                /* I/P Frame Width */
  unsigned int nHeight;               /* I/P Frame Height */
  
}OMTB_VPSS_VFCC_OUTPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_VPSS_VFDC_INPORT_PARAMS
 *  \brief  VPSS VFDC specific input port parameters
 *******************************************************************************
 */

typedef struct OMTB_VPSS_VFDC_INPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pInBuff[OMTB_VPSS_VFDC_MAX_NUM_IN_BUFS];/* I/P Buffers*/
  unsigned int frameCount;            /* Frame counter used to get frame size */
    
}OMTB_VPSS_VFDC_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_VPSS_VFDC_OUTPORT_PARAMS
 *  \brief  VPSS VFDC specific output port parameters
 *******************************************************************************
 */
 
typedef struct OMTB_VPSS_VFDC_OUTPORT_PARAMS_T
{

  OMX_BUFFERHEADERTYPE* pOutBuff[OMTB_VPSS_VFDC_MAX_NUM_OUT_BUFS];/* O/P Bufs */
  OMX_COLOR_FORMATTYPE colorFormat;   /* O/P Color Format */
  unsigned int nWidth;                /* I/P Frame Width */
  unsigned int nHeight;               /* I/P Frame Height */
  
}OMTB_VPSS_VFDC_OUTPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_INPORT_PARAMS
 *  \brief  Structure for openmax component input port related param
 *******************************************************************************
 */

typedef struct OMTB_INPORT_PARAMS_T
{

  OMX_PARAM_PORTDEFINITIONTYPE* pInPortDef;   /* input port def var */
  OMX_PTR ipBufPipe;              /* input pipe */
  FILE *fin;                      /* I/P File Pointer */
  void* inputDataReady;           /* EmptyBuffer Done Semaphore */
  unsigned int flagInputEos;      /* EOS indication flag */
  unsigned int bufferInSize;      /* I/P buffer size */
  void* inDataStrmThrdId;         /* I/P data streaming thread id */
  unsigned int flagUseInBuffer;   /* 0: OMX_AllocateBuffer & 1: OMX_UseBuffer*/
  unsigned int numInBuf;          /* Number of i/p buffers */
  void* inputDataReadThrExit;     /* I/P data read thread exit semaphore */
  
  union
  {
  
    OMTB_H264_VDEC_INPORT_PARAMS h264Vdec;  /* H264Vdec specific params */
    OMTB_G711_ADEC_INPORT_PARAMS g711Adec;  /* G711Adec specific params */
    OMTB_H264_VENC_INPORT_PARAMS h264Venc;  /* H264Venc specific params */
    OMTB_VPSS_VFCC_INPORT_PARAMS vpssVfcc;  /* VPSSVfcc specific params */
    OMTB_VPSS_VFDC_INPORT_PARAMS vpssVfdc;  /* VPSSVfdc specific params */
    
  }comp;                          /* Component specific parameters */
  
}OMTB_INPORT_PARAMS;


/*!
 *******************************************************************************
 *  \struct OMTB_OUTPORT_PARAMS
 *  \brief  Structure for openmax component output port related param
 *******************************************************************************
 */

typedef struct OMTB_OUTPORT_PARAMS_T
{

  OMX_PARAM_PORTDEFINITIONTYPE* pOutPortDef;  /* output port def var */
  OMX_PTR opBufPipe;              /* output pipe */
  FILE *fout;                     /* O/P File Pointer */
  void* outputDataReady;          /* FillBuffer Done Semaphore */
  unsigned int flagEos;           /* EOS indication flag */
  unsigned int bufferOutSize;     /* O/P buffer size */
  void* outDataStrmThrdId;        /* O/P data streaming thread id */
  unsigned int flagUseOutBuffer;  /* 0: OMX_AllocateBuffer & 1: OMX_UseBuffer*/
  OmtbOutPutMode dataOutPutMode;  /* For selecting O/P Mode */
  unsigned int numOutBuf;         /* Number of o/p buffers */
  void* outputDataWriteThrExit;   /* O/P data read thread exit semaphore */
  
  union
  {
  
    OMTB_H264_VDEC_OUTPORT_PARAMS h264Vdec; /* H264Vdec specific params */
    OMTB_G711_ADEC_OUTPORT_PARAMS g711Adec; /* G711Adec specific params */
    OMTB_H264_VENC_OUTPORT_PARAMS h264Venc; /* H264Venc specific params */
    OMTB_VPSS_VFCC_OUTPORT_PARAMS vpssVfcc; /* VPSSVfcc specific params */
    OMTB_VPSS_VFDC_OUTPORT_PARAMS vpssVfdc; /* VPSSVfdc specific params */
    
  }comp;                          /* Component specific parameters */
  
}OMTB_OUTPORT_PARAMS;


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


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/

