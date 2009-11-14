/******************************************************************************\
 * Copyright © 2009 Texas Instruments Incorporated - http://www.ti.com/
\******************************************************************************/
/** @file     omtbHDVPSSVfdc.h
 *
 *  @brief    This file contains structure specific to HDVPSS VFDC OMX
 *            Component.
 *
 * <B> History: </B>
 *
 *      -# October-12-2009   AVME System Test   Inital Version
 *
 */

#ifndef __OMTB_HD_VPSS_VFDC_H__
#define __OMTB_HD_VPSS_VFDC_H__


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

#define OMTB_VPSS_VFDC_MAX_INBUFF_SIZE  (100 * 1024)/* Max size of I/P buffer */
#define OMTB_VPSS_VFDC_COMPONENT_NAME   ("OMX.TI.DUCATI1.HDVPSS.VFDC")  
                                                    /* Comp name */
#define OMTB_VPSS_VFDC_COMP_REF_STR     ("vfdc")    /* comp refernce string */
#define OMTB_VPSS_VFDC_GROUP_PRIORITY   (2)         /* Task group priority */
#define OMTB_VPSS_VFDC_TSK_STACK_SIZE   (30 * 1024) /* Task stack size */

#define OMTB_VPSS_VFDC_DEF_WIDTH        (720)       /* VFDC def width */
#define OMTB_VPSS_VFDC_DEF_HEIGHT       (480)       /* VFDC def height */


/******************************************************************************\
 *      Data type definitions
\******************************************************************************/

/* Application's private structure for OMX HDVPSS VFDC Component*/

typedef struct OMTB_VPSS_VFDC_PRIVATE_T 
{

  OMX_HANDLETYPE handle;                  /* Component handle */
  OMTB_INPORT_PARAMS  inPortParams;       /* Input port params */
  OMTB_OUTPORT_PARAMS outPortParams;      /* Output port params */
  OMX_CALLBACKTYPE* cb;                   /* callback struct var */
  char  compressionFormat;                /* encoder compression format */
  void* eventSem;                         /* Event Semaphore */
  void* eofSem;                           /* End of Stream Semaphore */
  unsigned int instanceNo;                /* VPSS VFDC Instance number */
  unsigned int flagThrCreated;            /* Func/API thread created Flag */
  char compName[OMX_MAX_STRINGNAME_SIZE]; /* OMX component name */
  
}OMTB_VPSS_VFDC_PRIVATE;


/* Configuration structure for OMX HDVPSS VFDC Component */

typedef struct OMTB_VPSS_VFDC_CONFIG_PARAMS_T 
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
  char compName[OMX_MAX_STRINGNAME_SIZE];  /* OMX component name */

}OMTB_VPSS_VFDC_CONFIG_PARAMS;

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
 *      VPSSVFDC_AllocateResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Allocate I/O port buffers for VPSS VFDC.
 *
 *  @param in:
 *            vpssVfdcClient: Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std error/return code
 *
 */

OMX_ERRORTYPE VPSSVFDC_AllocateResources(
                      OMTB_VPSS_VFDC_PRIVATE* vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_SetPortDefinition Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Set parameter for input port and output port for VPSS VFDC.
 *
 *  @param in:
 *            vpssVfdcClient:Pointer to appPrivateType structure
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFDC_SetPortDefinition(
                      OMTB_VPSS_VFDC_PRIVATE* vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_GetStructIdx Function Prototype
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

OmtbRet VPSSVFDC_GetStructIdx(
  char *indexStr,
  unsigned int template,
  unsigned int portIdx,
  unsigned int *nConfigIndex,
  void **pCompCfgStruct);


/******************************************************************************\
 *      VPSSVFDC_SetParameter Function Prototype
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

OMX_ERRORTYPE VPSSVFDC_SetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFDC_SetConfig Function Prototype
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

OMX_ERRORTYPE VPSSVFDC_SetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFDC_GetParameter Function Prototype
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

OMX_ERRORTYPE VPSSVFDC_GetParameter(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFDC_GetConfig Function Prototype
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

OMX_ERRORTYPE VPSSVFDC_GetConfig(
  OMX_HANDLETYPE handle,
  unsigned int portIdx,
  char *indexStr,
  unsigned int template);


/******************************************************************************\
 *      VPSSVFDC_UseInitialResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Tells VPSS VFDC component to empty/fill the initial buffers.
 *
 *  @param in:
 *            vpssVfdcClient: Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFDC_UseInitialResources(
                          OMTB_VPSS_VFDC_PRIVATE * vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_FreeResources Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the private resources allocated for the component
 *
 *  @param in:
 *            vpssVfdcClient: Pointer to OMTB_VPSS_VFDC_PRIVATE struct
 *
 *  @return   OMX_ERRORTYPE - OMX std return/error code
 *
 */

OMX_ERRORTYPE VPSSVFDC_FreeResources(OMTB_VPSS_VFDC_PRIVATE *vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_FreeMemory Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    Free the memory allocated for the component
 *
 *  @param in:
 *            vpssVfdcClient: Pointer to OMTB_VPSS_VFDC_PRIVATE struct
 *
 *  @return   None
 *
 */

void VPSSVFDC_FreeMemory(OMTB_VPSS_VFDC_PRIVATE *vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_FuncTsk Thread Prototype
\******************************************************************************/
/**
 *
 *  @brief    This is a VPSS VFDC thread which invokes during func command
 *
 *  @param in:void *threads_arg - Pointer to thread arg structure
 *
 *  @return   None
 *
 */

void VPSSVFDC_FuncTsk(void *threads_arg);


/******************************************************************************\
 *      VPSSVFDC_ApiTsk Thread Prototype
\******************************************************************************/
/**
 *
 *  @brief    VPSS VFDC API mode Execute Thread
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

void VPSSVFDC_ApiTsk(void *threads_arg);


/******************************************************************************\
 *      VPSSVFDC_InputDataRead Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs i/p data read for VPSS VFDC.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void VPSSVFDC_InputDataRead(void *threadsArg);

/******************************************************************************\
 *      VPSSVFDC_OutputDataWrite Function Prototype
\******************************************************************************/
/**
 *
 *  @brief    This Function performs o/p data write for VPSS VFDC.
 *
 *  @param    threadsArg  : Pointer to thread arguments
 *
 *  @return   None
 *
 */
void VPSSVFDC_OutputDataWrite(void *threadsArg);


/******************************************************************************\
 *      VPSSVFDC_FillData Function Prototype
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
 
unsigned int VPSSVFDC_FillData( OMTB_VPSS_VFDC_PRIVATE* pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf,
                                unsigned int count);


/******************************************************************************\
 *      VPSSVFDC_AllocateMemory Prototype
\******************************************************************************/
/**
 *  @brief    Allocate memory for private data structure for VPSS VFDC
 *            component
 *  @param in:
 *            vpssVfdcClient:Pointer to appPrivateType structure
 *
 *  @param Out:
 *            None
 *
 *  @return   OmtbRet - OMTB return/error code
 *
 */
 
OmtbRet VPSSVFDC_AllocateMemory( OMTB_VPSS_VFDC_PRIVATE *vpssVfdcClient);


/******************************************************************************\
 *      VPSSVFDC_EventHandler callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    Event handler Callback of the HDVPSS VFDC component
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

OMX_ERRORTYPE VPSSVFDC_EventHandler(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_EVENTTYPE eEvent,
  OMX_OUT OMX_U32 Data1,
  OMX_OUT OMX_U32 Data2,
  OMX_OUT OMX_PTR pEventData);


/******************************************************************************\
 *      VPSSVFDC_EmptyBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    EmptyBufferDone Callback of the HDVPSS VFDC component
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

OMX_ERRORTYPE VPSSVFDC_EmptyBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


/******************************************************************************\
 *      VPSSVFDC_FillBufferDone callback Function Definition
\******************************************************************************/
/**
 *
 *  @brief    FillBufferDone Callback of the HDVPSS VFDC component
 *
 *  @param in:
 *            hComponent: Component Handle
 *            ptrAppData: Pointer to Private Data Structure
 *            pBuffer   : Pointer to o/p Buffer
 *
 *  @return   err       : OMX_ErrorType
 *
 */

OMX_ERRORTYPE VPSSVFDC_FillBufferDone(
  OMX_OUT OMX_HANDLETYPE hComponent,
  OMX_OUT OMX_PTR pAppData,
  OMX_OUT OMX_BUFFERHEADERTYPE* pBuffer);


#ifdef __cplusplus    /* matches __cplusplus construct above */
 }
#endif

#endif


/*--------------------------------- end of file -----------------------------*/
