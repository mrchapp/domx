/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbH264Vdec.h
 *
 *  @brief    This file contains structure specific to H264 Vdec OMX
 *            Component.
 *
 * <B> History: </B>
 *
 *      -# May-11-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_H264_VDEC_H__
#define __OMTB_H264_VDEC_H__


/******************************************************************************\
 *      Includes
\******************************************************************************/

#include "omtbPortParam.h"


/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/

#define OMTB_H264_VDEC_MAX_INBUFF_SIZE  (100 * 1024)/* Max size of I/P buffer */
#ifdef WTSD
#define OMTB_H264_VDEC_COMPONENT_NAME   ("OMX.TI.DUCATI1.VIDEO.H264D")  /* Comp name */
#else
#define OMTB_H264_VDEC_COMPONENT_NAME   ("OMX.TI.VIDEO.H264D")  /* Comp name */
#endif
#define OMTB_H264_VDEC_COMP_REF_STR     ("h264vdec")/* comp refernce string */
#define OMTB_H264_VDEC_GROUP_PRIORITY   (2)         /* Task group priority */
#define OMTB_H264_VDEC_TSK_STACK_SIZE   (30 * 1024) /* Task stack size */

/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Application's private structure for OMX H264 Video Decoder Component*/

typedef struct OMTB_H264_VDEC_PRIVATE_T 
{

  OMX_HANDLETYPE handle;                  /* Component handle */
  OMTB_INPORT_PARAMS  inPortParams;       /* Input port params */
  OMTB_OUTPORT_PARAMS outPortParams;      /* Output port params */
  OMX_CALLBACKTYPE* cb;                   /* callback struct var */
  OMX_VIDEO_PARAM_AVCTYPE* pH264;         /* H264 video param struct */
  char  compressionFormat;                /* encoder compression format */
  void* eventSem;                         /* Event Semaphore */
  void* eofSem;                           /* End of Stream Semaphore */
  unsigned int instanceNo;                /* H264 decoder Instance number */
  unsigned int flagThrCreated;            /* Func/API thread created Flag */
  char compName[OMX_MAX_STRINGNAME_SIZE]; /* OMX component name */
  
}OMTB_H264_VDEC_PRIVATE;


/* Configuration structure for OMX H264 Video Decoder Component */

typedef struct OMTB_H264_VDEC_CONFIG_PARAMS_T 
{
  char inputFile[OMTB_MAX_LEN];     /* Array to store i/p File Name */
  char outputFile[OMTB_MAX_LEN];    /* Array to store o/p File Name */
  char frameSizeFile[OMTB_MAX_LEN]; /* Array to store Frame Size File Name */
  char codecType[OMTB_MAX_LEN];     /* Array to store code type */
  unsigned int frameWidth;          /* Variable to store I/P Frame Width */
  unsigned int frameHeight;         /* Variable to store I/P Frame Height */
  OMX_COLOR_FORMATTYPE colorFormat; /* Variable to store O/P Color Format */
  OmtbOutPutMode dataOutPutMode;    /* For selecting O/P Mode */
  unsigned int numInBuf;            /* Number of i/p buffers */
  unsigned int numOutBuf;           /* Number of o/p buffers */
  unsigned int flagUseInBuffer;     /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  unsigned int flagUseOutBuffer;    /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  char compName[OMX_MAX_STRINGNAME_SIZE];  /* OMX component name */

}OMTB_H264_VDEC_CONFIG_PARAMS;

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
 *      H264VDEC_AllocateResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for H264 decoder.
 *
 *  @param in:
 *            h264VdecClient: Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std error/return code
 *
 */

OMX_ERRORTYPE H264VDEC_AllocateResources(
                      OMTB_H264_VDEC_PRIVATE* h264VdecClient);


/******************************************************************************\
 *      H264VDEC_SetPortDefinition Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for H264 decoder.
 *
 *  @param in:
 *            h264VdecClient:Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_SetPortDefinition(
                      OMTB_H264_VDEC_PRIVATE* h264VdecClient);


/******************************************************************************\
 *      H264VDEC_GetStructIdx Function Prototype
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

OmtbRet H264VDEC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct);


/******************************************************************************\
 *      H264VDEC_SetParameter Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for the appropriate index passsed
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

OMX_ERRORTYPE H264VDEC_SetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      H264VDEC_SetConfig Function Prototype
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

OMX_ERRORTYPE H264VDEC_SetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      H264VDEC_GetParameter Function Prototype
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

OMX_ERRORTYPE H264VDEC_GetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      H264VDEC_GetConfig Function Prototype
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

OMX_ERRORTYPE H264VDEC_GetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      H264VDEC_UseInitialResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Tells H264 decode component to empty/fill the initial buffers.
 *
 *  @param in:
 *            h264VdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_UseInitialResources(
                          OMTB_H264_VDEC_PRIVATE * h264VdecClient);


/******************************************************************************\
 *      H264VDEC_FreeResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            h264VdecClient: Pointer to OMTB_H264_VDEC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE H264VDEC_FreeResources(OMTB_H264_VDEC_PRIVATE *h264VdecClient);


/******************************************************************************\
 *      H264VDEC_FreeMemory Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            h264VdecClient: Pointer to OMTB_H264_VDEC_PRIVATE struct
 *
 *  @return   None
 *
 */

void H264VDEC_FreeMemory(OMTB_H264_VDEC_PRIVATE *h264VdecClient);


/******************************************************************************\
 *      H264VDEC_FuncTsk Thread Prototype
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

void H264VDEC_FuncTsk(void *threads_arg);


/******************************************************************************\
 *      H264VDEC_ApiTsk Function Prototype
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

void H264VDEC_ApiTsk(void *threads_arg);


/******************************************************************************\
 *      H264VDEC_InputDataRead Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs i/p data read for H264 decoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void H264VDEC_InputDataRead(void *threadsArg);

/******************************************************************************\
 *      H264VDEC_OutputDataWrite Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs o/p data write for H264 decoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void H264VDEC_OutputDataWrite(void *threadsArg);


/******************************************************************************\
 *      H264VDEC_FillData Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *
 *            count: Frame number
 *  @param Out:
 *            Pointer of I/P filled Buffer
 *
 *  @return   No of bytes read
 *
 */
 
unsigned int H264VDEC_FillData( OMTB_H264_VDEC_PRIVATE* pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf,
                                unsigned int count);


/******************************************************************************\
 *      H264VDEC_AllocateMemory Prototype
\******************************************************************************/
/**
 *  @brief    Allocate memory for private data structure for h264 video decode
 *            component
 *  @param in:
 *            h264VdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */
 
OmtbRet H264VDEC_AllocateMemory( OMTB_H264_VDEC_PRIVATE *h264VdecClient);


/******************************************************************************\
 *      H264VDEC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the h264 video decoder component
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

OMX_ERRORTYPE H264VDEC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData);


/******************************************************************************\
 *      H264VDEC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the h264 video decoder component
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

OMX_ERRORTYPE H264VDEC_EmptyBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


/******************************************************************************\
 *      H264VDEC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the h264 video decoder component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE H264VDEC_FillBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);

#if 0
static unsigned short outputDisplayFrame(IH264VDEC_OutArgs *outArgs, unsigned short xoff, unsigned short yoff,
                   unsigned short ref_width, unsigned short ref_height,
                   unsigned short width, unsigned short height, unsigned short pic_struct, unsigned short hrd,
                   unsigned int fieldBuf, FILE *fout, unsigned int complianceMode);


static unsigned short outputDisplayField(IH264VDEC_OutArgs *outArgs, unsigned short xoff, unsigned short yoff,
                   unsigned short ref_width, unsigned short ref_height,
                   unsigned short width, unsigned short height, unsigned short pic_struct, unsigned short hrd,
                   unsigned int fieldBuf, unsigned short bottom_field_flag, FILE *fout, unsigned int complianceMode);

static XDAS_Void TestApp_WriteOutputData
(
  FILE            *fOutFile,
  IH264VDEC_OutArgs   *outArgs,
  unsigned int              fieldBuf,
  unsigned int              complianceMode
);
#endif


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file -----------------------------*/
