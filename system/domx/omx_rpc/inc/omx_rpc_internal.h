/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc_internal.h
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework RPC.
 *
 *  @path \WTSD_DucatiMMSW\framework\domx\omx_rpc\inc 
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 19-August-2009 B Ravi Kiran ravi.kiran@ti.com: Initial Version
 *================================================================*/
 

#ifndef OMXRPC_INTERNAL_H
#define OMXRPC_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <RcmClient.h>
#include <HeapBuf.h>

#include <OMX_Component.h>
#include <OMX_Core.h>
#include <OMX_Audio.h>
#include <OMX_Video.h>
#include <OMX_Types.h>
#include <OMX_Index.h>
#include <OMX_TI_Index.h>
#include <OMX_TI_Common.h>

/******************************************************************
 *   DEFINES - CONSTANTS
 ******************************************************************/
/* *********************** OMX RPC DEFINES******************************************/
#define MAX_CORENAME_LENGTH 10
#define MAX_NUM_OF_RPC_USERS 5
#define MAX_SERVER_NAME_LENGTH 40

/* ************************* MESSAGE BUFFER DEFINES *********************************/
#define DEFAULTBUFSIZE 1024
#define MAX_MSG_FRAME_LENGTH DEFAULTBUFSIZE
#define MESSAGE_BODY_LENGTH (MAX_MSG_FRAME_LENGTH - sizeof(RPC_MSG_HEADER))

/* **************************PACKET SIZES-TEMPORARY FIX*****************************/
#define CHIRON_PACKET_SIZE 0x90
#define DUCATI_PACKET_SIZE 0x100

/* ************************* OFFSET DEFINES ******************************** */
#define GET_PARAM_DATA_OFFSET    (sizeof(RPC_OMX_HANDLE) + sizeof(OMX_INDEXTYPE) + sizeof(OMX_U32) /*4 bytes for offset*/ )
#define USE_BUFFER_DATA_OFFSET   (sizeof(OMX_U32)*5)

/******************************************************************
 *   MACROS - ASSERTS
 ******************************************************************/
#define RPC_assert  RPC_paramCheck
#define RPC_require RPC_paramCheck
#define RPC_ensure  RPC_paramCheck

#define RPC_paramCheck(C,V,S)  if (!(C)) { eRPCError = V;\
TIMM_OSAL_TraceFunction("##Error:: %s::in %s::line %d \n",S,__FUNCTION__, __LINE__); \
goto EXIT; }

/******************************************************************
 *   MACROS - COMMON MARSHALLING UTILITIES
 ******************************************************************/
#define RPC_SETFIELDVALUE(MSGBODY, POS, VALUE, TYPE) \
*((TYPE *) ((OMX_U32)MSGBODY+POS)) = VALUE; \
POS+=sizeof(TYPE);

#define RPC_SETFIELDOFFSET(MSGBODY, POS, OFFSET, TYPE) \
*((TYPE *) ((OMX_U32)MSGBODY+POS)) = OFFSET; \
POS+=sizeof(TYPE);

#define RPC_SETFIELDCOPYGEN(MSGBODY, POS, PTR, SIZE) \
TIMM_OSAL_Memcpy((OMX_U8*) ((OMX_U32)MSGBODY+POS),PTR,SIZE);

#define RPC_SETFIELDCOPYTYPE(MSGBODY, POS, PSTRUCT, TYPE) \
*((TYPE *) ((OMX_U32)MSGBODY+POS)) = *PSTRUCT;

/******************************************************************
 *   MACROS - COMMON UNMARSHALLING UTILITIES
 ******************************************************************/
#define RPC_GETFIELDVALUE(MSGBODY, POS, VALUE, TYPE) \
VALUE = *((TYPE *) ((OMX_U32)MSGBODY+POS)); \
POS+=sizeof(TYPE);

#define RPC_GETFIELDOFFSET(MSGBODY, POS, OFFSET, TYPE) \
OFFSET = *((TYPE *) ((OMX_U32)MSGBODY+POS)); \
POS+=sizeof(TYPE);

#define RPC_GETFIELDCOPYGEN(MSGBODY, POS, PTR, SIZE) \
TIMM_OSAL_Memcpy(PTR,(OMX_U8*) ((OMX_U32)MSGBODY+POS),SIZE);

#define RPC_GETFIELDCOPYTYPE(MSGBODY, POS, PSTRUCT, TYPE) \
*PSTRUCT = *((TYPE *) ((OMX_U32)MSGBODY+POS));

#define RPC_GETFIELDPATCHED(MSGBODY, OFFSET, PTR, TYPE) \
PTR = (TYPE *) (MSGBODY+OFFSET);

/* ************************* OMX RPC DATA TYPES *************************** */
typedef OMX_U32 	     RPC_OMX_HANDLE;
typedef OMX_U8  	     RPC_OMX_BYTE;
typedef OMX_U32          RPC_INDEX;
typedef OMX_HANDLETYPE RPC_HANDLE_TYPE;
typedef OMX_ERRORTYPE    RPC_OMX_CMD_STATUS;
typedef OMX_U32          RPC_OMX_SIZE;
typedef OMX_PTR		     RPC_OMX_PTR;
typedef OMX_U32	         RPC_OMX_ARG;
typedef OMX_U32	         RPC_OMX_ID;

/*******************************************************************************
* Enumerated Types
*******************************************************************************/
/** 
  *  @brief           CORE Identification Enum. This gets configured from
  *                   CFG file ( each RCM will have a different value based on CORE) 
  */
typedef enum COREID
{
        CORE_CHIRON = 0,
        CORE_TESLA = 1,
        CORE_SYSM3 = 2,
        CORE_APPM3 = 3,
        CORE_MAX = 4
}COREID; 


/************************************* OMX RPC MESSAGE STRUCTURE ******************************/
typedef struct RPC_MSG_HEADER
{ 
        RPC_OMX_ERRORTYPE     nRPCCmdStatus; // command status is for reflecting successful remote OMX return of call
        OMX_ERRORTYPE         nOMXReturn; // omx error      
}RPC_MSG_HEADER;

typedef struct RPC_OMX_MESSAGE
{
        RPC_MSG_HEADER msgHeader;
        RPC_OMX_BYTE   msgBody[MESSAGE_BODY_LENGTH];
}RPC_OMX_MESSAGE;


/**************************** RPC FUNCTION INDEX STRUCTURES FOR STUB AND SKEL **********************/
typedef struct rpcFxnArr {
        RPC_INDEX rpcFxnIdx;
        char *FxnName;
} rpcFxnArr;

typedef struct rpcSkelArr {
        OMX_PTR FxnPtr;
} rpcSkelArr;

typedef enum ProxyType {
    ClientProxy = 0,
    TunnelProxy = 1,
}ProxyType;

typedef enum CacheType{
    Cache_SameProc = 0, // Same processor cache (Inter Ducati case)
    Cache_DMA = 1,  // No cache required as data access is through DMA - flag to be filled in by OMX component(BASE)
    //placeholder future types to follow
}CacheType;

/** 
  *  @brief           Index for Remote Function Index Array. 
  */
typedef enum rpc_fxn_list{
    RPC_OMX_FXN_IDX_SET_PARAMETER = 0,
	RPC_OMX_FXN_IDX_GET_PARAMETER = 1,
	RPC_OMX_FXN_IDX_GET_HANDLE = 2,
	RPC_OMX_FXN_IDX_USE_BUFFER = 3,
	RPC_OMX_FXN_IDX_FREE_HANDLE = 4,
	RPC_OMX_FXN_IDX_SET_CONFIG    = 5,
	RPC_OMX_FXN_IDX_GET_CONFIG    = 6,
	RPC_OMX_FXN_IDX_GET_STATE     = 7,
	RPC_OMX_FXN_IDX_SEND_CMD      = 8,
	RPC_OMX_FXN_IDX_GET_VERSION   = 9,
	RPC_OMX_FXN_IDX_GET_EXT_INDEX = 10,
	RPC_OMX_FXN_IDX_FILLTHISBUFFER = 11,
	RPC_OMX_FXN_IDX_FILLBUFFERDONE = 12,
	RPC_OMX_FXN_IDX_FREE_BUFFER = 13,
	RPC_OMX_FXN_IDX_EMPTYTHISBUFFER = 14,
	RPC_OMX_FXN_IDX_EMPTYBUFFERDONE = 15,
	RPC_OMX_FXN_IDX_EVENTHANDLER = 16,
	RPC_OMX_FXN_IDX_ALLOCATE_BUFFER = 17,
	RPC_OMX_FXN_IDX_COMP_TUNNEL_REQUEST = 18,
	RPC_OMX_FXN_IDX_MAX = 19
}rpc_fxn_list;

/*******************************************************************************
* STRUCTURES
*******************************************************************************/
/* ********************************************* RPC LAYER HANDLE ************************************** */
typedef struct RPC_Object
{
//RCM init params
RcmClient_Handle rcmHndl[CORE_MAX]; //RCM Handle on that respective core
//RcmClient_Params rcmParams[CORE_MAX];
OMX_U32 heapId[CORE_MAX]; //these heap IDs need to be configured during ModInit to the correct values after heaps for TILER and other apps have been allocated
//Functions to be registered and indices
rpcFxnArr rpcFxns[RPC_OMX_FXN_IDX_MAX];
//Number of users per transport or RCM client
RPC_OMX_SIZE NumOfTXUsers;

}RPC_Object;

//NOTE: Need to check if we need this. It should be defined in some OMX header.
/* this is used for GetComponentVersion****/
typedef struct VERSION_INFO
{
char cName[128]; /***like "OMX.TI.Video.encoder" **/
OMX_VERSIONTYPE sVersion;
OMX_VERSIONTYPE sSpecVersion;
OMX_UUIDTYPE sUUID;
} VERSION_INFO; 

#endif
