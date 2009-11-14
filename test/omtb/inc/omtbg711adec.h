/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbG711Adec.h
 *
 *  @brief    This file contains structure specific to G711 Audio Decode OMX
 *            Component.
 *
 * <B> History: </B>
 *
 *     -# July-09-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_G711_ADEC_H__
#define __OMTB_G711_ADEC_H__


/******************************************************************************\
 *      Includes
\******************************************************************************/

#include "omtbPortParam.h"


/******************************************************************************/

#ifdef __cplusplus   /* required for headers that might */
 extern "C" {         /* be compiled by a C++ compiler */
#endif


/******************************************************************************\
 *      Defines
\******************************************************************************/

/* Buffer Count */

#define OMTB_G711_ADEC_MAX_INBUFF_SIZE   (80)        /* max i/p buf size */
#define OMTB_G711_ADEC_MAX_OUTBUFF_SIZE  (320)       /* max o/p buf size */
#define OMTB_G711_ADEC_COMPONENT_NAME    ("OMX.TI.ADEC")  /* component name */
#define OMTB_G711_ADEC_COMP_REF_STR      ("g711adec")/* comp refernce string */
#define OMTB_G711_ADEC_GROUP_PRIORITY    (2)         /* Task group priority */
#define OMTB_G711_ADEC_TSK_STACK_SIZE    (30 * 1024) /* Task stack size */


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Application's private structure for OMX G711 AudioDecoder Component*/

typedef struct OMTB_G711_ADEC_PRIVATE_T 
{

  OMX_HANDLETYPE handle;                  /* Component handle */
  OMTB_INPORT_PARAMS  inPortParams;       /* Input port params */
  OMTB_OUTPORT_PARAMS outPortParams;      /* Output port params */
  OMX_CALLBACKTYPE* cb;                   /* callback struct var */
  char  compressionFormat;                /* encoder compression format */
  void* eventSem;                         /* Event Semaphore */
  void* eofSem;                           /* End of Stream Semaphore */
  unsigned int instanceNo;                /* G711 decoder Instance number */
  unsigned int flagThrCreated;            /* Func/API thread created Flag */
  char compName[OMX_MAX_STRINGNAME_SIZE]; /* OMX component name */
  
} OMTB_G711_ADEC_PRIVATE;


/* Configuration structure for OMX G711 Audio Decoder Component */

typedef struct OMTB_G711_ADEC_CONFIG_PARAMS_T 
{

  char inputFile[OMTB_MAX_LEN];     /* Array to store i/p File Name */
  char outputFile[OMTB_MAX_LEN];    /* Array to store o/p File Name */
  OmtbOutPutMode dataOutPutMode;    /* For selecting O/P Mode */
  unsigned int numInBuf;            /* Number of i/p buffers */
  unsigned int numOutBuf;           /* Number of o/p buffers */
  unsigned int flagUseInBuffer;     /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  unsigned int flagUseOutBuffer;    /* 0: OMX_AllocateBuffer & 1:OMX_UseBuffer*/
  char compName[OMX_MAX_STRINGNAME_SIZE];  /* OMX component name */
  
} OMTB_G711_ADEC_CONFIG_PARAMS;


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
 *      G711ADEC_SetPortDefinition Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for inport and outport port params
 *
 *  @param in:
 *            g711AdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_SetPortDefinition(
                      OMTB_G711_ADEC_PRIVATE* g711AdecClient);


/******************************************************************************\
 *      G711ADEC_GetStructIdx Function Prototype
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

OmtbRet G711ADEC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct);


/******************************************************************************\
 *      G711ADEC_SetParameter Function Prototype
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

OMX_ERRORTYPE G711ADEC_SetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      G711ADEC_SetConfig Function Prototype
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

OMX_ERRORTYPE G711ADEC_SetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      G711ADEC_GetParameter Function Prototype
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

OMX_ERRORTYPE G711ADEC_GetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      G711ADEC_GetConfig Function Prototype
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

OMX_ERRORTYPE G711ADEC_GetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      G711ADEC_AllocateResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate input port and output port buffers
 *
 *  @param in:
 *            g711AdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std error/return code
 *
 */

OMX_ERRORTYPE G711ADEC_AllocateResources(
                    OMTB_G711_ADEC_PRIVATE* g711AdecClient);


/******************************************************************************\
 *      G711ADEC_UseInitialResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Tells G711 decode component to empty/fill the initial buffers.
 *
 *  @param in:
 *            g711AdecClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_UseInitialResources(
                          OMTB_G711_ADEC_PRIVATE* g711AdecClient);


/******************************************************************************\
 *      G711ADEC_FreeResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            g711AdecClient: Pointer to OMTB_G711_ADEC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE G711ADEC_FreeResources(OMTB_G711_ADEC_PRIVATE *g711AdecClient);


/******************************************************************************\
 *      G711ADEC_FreeMemory Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            g711AdecClient: Pointer to OMTB_G711_ADEC_PRIVATE struct
 *
 *  @return   None
 *
 */

void G711ADEC_FreeMemory(OMTB_G711_ADEC_PRIVATE *g711AdecClient);


/******************************************************************************\
 *      G711ADEC_ApiTsk Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a G711 thread which invokes during exec command
 *
 *  @param in:
 *            void *threadsArg : Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void G711ADEC_ApiTsk(void *threadsArg);


/******************************************************************************\
 *      G711ADEC_FuncTsk Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a G711 thread which invokes during exec command
 *
 *  @param in:
 *            void *threadsArg : Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void G711ADEC_FuncTsk(void *threadsArg);


/******************************************************************************\
 *      G711ADEC_InputDataRead Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs i/p data read for G711 decoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
 
void G711ADEC_InputDataRead(void *threadsArg);


/******************************************************************************\
 *      G711ADEC_OutputDataWrite Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs o/p data write for G711 decoding.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
 
void G711ADEC_OutputDataWrite(void *threadsArg);


/******************************************************************************\
 *      G711ADEC_AllocateMemory Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate memory for private data structure for g711 Audio decode
 *            component.
 *  @param in:
 *            g711AdecClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB error codes
 *
 */
OmtbRet G711ADEC_AllocateMemory( OMTB_G711_ADEC_PRIVATE *g711AdecClient);


/******************************************************************************\
 *      G711ADEC_FillData Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This function reads from I/P file and fill date in i/p buffer
 *
 *  @param in:
 *            pAppData: Pointer to private data structure
 *
 *  @param Out:
 *            Pointer of I/P filled Buffer
 *
 *  @return   No of bytes read
 *
 */
unsigned int G711ADEC_FillData( OMTB_G711_ADEC_PRIVATE* pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf);


/******************************************************************************\
 *      G711ADEC_EventHandler callback Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the g711 Audio decoder component
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

OMX_ERRORTYPE G711ADEC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData);


/******************************************************************************\
 *      G711ADEC_EmptyBufferDone callback Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the g711 Audio decoder component
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

OMX_ERRORTYPE G711ADEC_EmptyBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


/******************************************************************************\
 *      G711ADEC_FillBufferDone callback Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the g711 Audio decoder component
 *
 *  @param in:
 *            hComponent: Component Handle
 *
 *            ptrAppData: Pointer to Private Data Structure
 *
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE G711ADEC_FillBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file ------------------------------*/
