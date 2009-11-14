/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbHDVPSSVfcc.h
 *
 *  @brief    This file contains structure specific to HDVPSS VFCC OMX
 *            Component.
 *
 * <B> History: </B>
 *
 *      -# October-12-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_HD_VPSS_VFCC_H__
#define __OMTB_HD_VPSS_VFCC_H__


/******************************************************************************\
 *      Includes
\******************************************************************************/

#include "omx_ti_common.h"
#include "omtbPortParam.h"


/******************************************************************************/

#ifdef __cplusplus    /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/

#define OMTB_VPSS_VFCC_MAX_INBUFF_SIZE  (100 * 1024)/* Max size of I/P buffer */
#define OMTB_VPSS_VFCC_COMPONENT_NAME   ("OMX.TI.DUCATI1.HDVPSS.VFCC")  
                                                    /* Comp name */
#define OMTB_VPSS_VFCC_COMP_REF_STR     ("vfcc")    /* comp refernce string */
#define OMTB_VPSS_VFCC_GROUP_PRIORITY   (2)         /* Task group priority */
#define OMTB_VPSS_VFCC_TSK_STACK_SIZE   (30 * 1024) /* Task stack size */

#define OMTB_VPSS_VFCC_DEF_WIDTH        (720)       /* VFCC def width */
#define OMTB_VPSS_VFCC_DEF_HEIGHT       (480)       /* VFCC def height */


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Application's private structure for OMX HDVPSS VFCC Component*/

typedef struct OMTB_VPSS_VFCC_PRIVATE_T 
{

  OMX_HANDLETYPE handle;                  /* Component handle */
  OMTB_INPORT_PARAMS  inPortParams;       /* Input port params */
  OMTB_OUTPORT_PARAMS outPortParams;      /* Output port params */
  OMX_CALLBACKTYPE* cb;                   /* callback struct var */
  char  compressionFormat;                /* encoder compression format */
  void* eventSem;                         /* Event Semaphore */
  void* eofSem;                           /* End of Stream Semaphore */
  unsigned int instanceNo;                /* VPSS VFCC Instance number */
  unsigned int flagThrCreated;            /* Func/API thread created Flag */
  char compName[OMX_MAX_STRINGNAME_SIZE]; /* OMX component name */
  
}OMTB_VPSS_VFCC_PRIVATE;


/* Configuration structure for OMX HDVPSS VFCC Component */

typedef struct OMTB_VPSS_VFCC_CONFIG_PARAMS_T 
{
  char inputFile[OMTB_MAX_LEN];     /* Array to store i/p File Name */
  char outputFile[OMTB_MAX_LEN];    /* Array to store o/p File Name */
  char codecType[OMTB_MAX_LEN];     /* Array to store code type */
  unsigned int frameWidth;          /* Variable to store I/P Frame Width */
  unsigned int frameHeight;         /* Variable to store I/P Frame Height */
  OMX_COLOR_FORMATTYPE colorFormat; /* Variable to store O/P Color Format */
  OmtbOutPutMode dataOutPutMode;    /* For selecting O/P Mode */
  unsigned int numInBuf;            /* Number of i/p buffers */
  unsigned int numOutBuf;           /* Number of o/p buffers */
  unsigned int flagUseInBuffer;     /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  unsigned int flagUseOutBuffer;    /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  char compName[OMX_MAX_STRINGNAME_SIZE];  
                                    /* OMX component name */
  OMX_CONFIG_CHANNELNAME sChannelName;
                                    /* Capture channel name */

}OMTB_VPSS_VFCC_CONFIG_PARAMS;

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
 *      VPSSVFCC_AllocateResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for VPSS VFCC.
 *
 *  @param in:
 *            vpssVfccClient: Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std error/return code
 *
 */

OMX_ERRORTYPE VPSSVFCC_AllocateResources(
                      OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_SetPortDefinition Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for VPSS VFCC.
 *
 *  @param in:
 *            vpssVfccClient:Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_SetPortDefinition(
                      OMTB_VPSS_VFCC_PRIVATE* vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_GetStructIdx Function Prototype
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

OmtbRet VPSSVFCC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct);


/******************************************************************************\
 *      VPSSVFCC_SetParameter Function Prototype
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

OMX_ERRORTYPE VPSSVFCC_SetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFCC_SetConfig Function Prototype
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

OMX_ERRORTYPE VPSSVFCC_SetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFCC_GetParameter Function Prototype
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

OMX_ERRORTYPE VPSSVFCC_GetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFCC_GetConfig Function Prototype
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

OMX_ERRORTYPE VPSSVFCC_GetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFCC_UseInitialResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Tells VPSS VFCC component to empty/fill the initial buffers.
 *
 *  @param in:
 *            vpssVfccClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_UseInitialResources(
                          OMTB_VPSS_VFCC_PRIVATE * vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_FreeResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            vpssVfccClient: Pointer to OMTB_VPSS_VFCC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFCC_FreeResources(OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_FreeMemory Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            vpssVfccClient: Pointer to OMTB_VPSS_VFCC_PRIVATE struct
 *
 *  @return   None
 *
 */

void VPSSVFCC_FreeMemory(OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_FuncTsk Thread Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a VPSS VFCC thread which invokes during func command
 *
 *  @param in:void *threads_arg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void VPSSVFCC_FuncTsk(void *threads_arg);


/******************************************************************************\
 *      VPSSVFCC_ApiTsk Thread Prototype
\******************************************************************************/
/**
 *
 *  @brief    VPSS VFCC API mode Execute Thread
 *
 *  @param in:
 *            void *threadsArg - Pointer to thread arg structure
 *
 *  @param Out:
 *            None
 *
 *  @return   None
 *
 */

void VPSSVFCC_ApiTsk(void *threads_arg);


/******************************************************************************\
 *      VPSSVFCC_InputDataRead Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs i/p data read for VPSS VFCC.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void VPSSVFCC_InputDataRead(void *threadsArg);

/******************************************************************************\
 *      VPSSVFCC_OutputDataWrite Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs o/p data write for VPSS VFCC.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void VPSSVFCC_OutputDataWrite(void *threadsArg);


/******************************************************************************\
 *      VPSSVFCC_FillData Function Prototype
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
 
unsigned int VPSSVFCC_FillData( OMTB_VPSS_VFCC_PRIVATE* pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf,
                                unsigned int count);


/******************************************************************************\
 *      VPSSVFCC_AllocateMemory Prototype
\******************************************************************************/
/**
 *  @brief    Allocate memory for private data structure for VPSS VFCC
 *            component
 *  @param in:
 *            vpssVfccClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */
 
OmtbRet VPSSVFCC_AllocateMemory( OMTB_VPSS_VFCC_PRIVATE *vpssVfccClient);


/******************************************************************************\
 *      VPSSVFCC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the HDVPSS VFCC component
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

OMX_ERRORTYPE VPSSVFCC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData);


/******************************************************************************\
 *      VPSSVFCC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the HDVPSS VFCC component
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

OMX_ERRORTYPE VPSSVFCC_EmptyBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


/******************************************************************************\
 *      VPSSVFCC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the HDVPSS VFCC component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE VPSSVFCC_FillBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file -----------------------------*/
