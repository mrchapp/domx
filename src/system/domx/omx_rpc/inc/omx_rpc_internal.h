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

//#include <xdc/runtime/Error.h>
#include "omx_component.h"
#include "omx_core.h"      
#include "omx_audio.h"
#include "omx_video.h"	
#include "omx_types.h"
#include "omx_index.h"
#include "omx_ti_index.h"
#include "omx_ti_video.h"

/* ************************************************************************ */

/* ************************* OMX RPC DATA TYPES *************************** */
typedef OMX_U32 	     RPC_OMX_HANDLE;
typedef OMX_HANDLETYPE RPC_HANDLE_TYPE;
//typedef OMX_ERRORTYPE    RPC_OMX_CMD_STATUS;
typedef OMX_U32          RPC_OMX_SIZE;
typedef OMX_PTR		     RPC_OMX_PTR;
typedef OMX_U8  	     RPC_OMX_BYTE;
typedef OMX_U32	         RPC_OMX_ARG;
typedef OMX_U32	         RPC_OMX_ID;
typedef OMX_U32          RPC_INDEX;
/* *********************** OMX RPC DEFINES**************************************** */
#define MAX_CORENAME_LENGTH 10
#define MAX_NUM_OF_RPC_USERS 5
#define MAX_SERVER_NAME_LENGTH 40
/* ************************* OFFSET DEFINES ******************************** */
#define GET_HANDLE_DATA_OFFSET 55 //This offset contains a slack to hold further additions to be made in the MsgBody - Right now contains RemoteHandle(OMX_U32-4b), pAppData(OMX_PTR-4b), CallingCore(OMX_U8-1b), Dataoffset2 - reference for Corename Being passed (OMX_U8-1b)
#define GET_HANDLE_DATA_OFFSET2 35 //Jump required to reach the ServerName as discussed above - This contains slack 
#define SET_PARAM_DATA_OFFSET   (sizeof(RPC_OMX_HANDLE) + sizeof(OMX_INDEXTYPE) + sizeof(OMX_U32) /*4 bytes for offset*/ )
#define GET_PARAM_DATA_OFFSET    (sizeof(RPC_OMX_HANDLE) + sizeof(OMX_INDEXTYPE) + sizeof(OMX_U32) /*4 bytes for offset*/ )
#define USE_BUFFER_DATA_OFFSET   (sizeof(OMX_U32)*5) // sizeof(RPC_OMX_HANDLE) + sizeof(OMX_32) + sizeof(OMX_U32) + sizeof(OMX_32) + sizeof(unsigned long)
#define SET_CONFIG_DATA_OFFSET   (sizeof(RPC_OMX_HANDLE) + sizeof(OMX_INDEXTYPE) + sizeof(OMX_U32) /*4 bytes for offset*/ )
#define GET_CONFIG_DATA_OFFSET    (sizeof(RPC_OMX_HANDLE) + sizeof(OMX_INDEXTYPE) + sizeof(OMX_U32) /*4 bytes for offset*/ )
/* ************************* MESSAGE BUFFER DEFINES ******************************** */
#define DEFAULTBUFSIZE                 				1024    /* Default buffer size in bytes. What value is to be used to get optimal usage?*/
#define MAX_MSG_FRAME_LENGTH   				DEFAULTBUFSIZE
#define MESSAGE_BODY_LENGTH     (MAX_MSG_FRAME_LENGTH - sizeof(RPC_MSG_HEADER))
/* **************************PACKET SIZES - TEMPORARY FIX FOR PACKETSIZES************************* */
#define CHIRON_PACKET_SIZE 0x90
#define DUCATI_PACKET_SIZE 0x100
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
        OMX_ERRORTYPE             nOMXReturn; // omx error      
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

 /*******************************************************************************
* Enumerated Types
*******************************************************************************/
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
	RPC_OMX_FXN_IDX_EVENTHANDLER = 16, //new
	RPC_OMX_FXN_IDX_ALLOCATE_BUFFER = 17,//new
	RPC_OMX_FXN_IDX_COMP_TUNNEL_REQUEST = 18,//new
	RPC_OMX_FXN_IDX_MAX = 19 //16
}rpc_fxn_list;

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

} RPC_Object;

typedef struct RPC_OMX_CONTEXT
{
RPC_OMX_HANDLE remoteHandle; //real components handle
RcmClient_Handle ClientHndl[2]; //This is filled in after the ClientCreate() call goes through
OMX_U32 HeapId[2]; //This needs to be filled in before client create() - fetch this head ID from Global table
COREID RealCore; // Target core of component - To be parsed from component name and put in
OMX_CALLBACKTYPE CbInfo; // should contain the updated pointers
ProxyType Tprxy; // Tunnel or Client Proxy
CacheType CacheMode; //DMA, same processor etc
OMX_PTR pAppData;
}RPC_OMX_CONTEXT;

/* **************************************************************************************************** */

typedef struct RPC_MSG_FRAME
{
	RPC_OMX_ID		nRPCFrameID;
    RPC_OMX_BYTE * pMsgBuf;
}RPC_MSG_FRAME;


typedef struct RPC_OMX_STRUCTURE_INFO
{
	OMX_INDEXTYPE indexType;
	RPC_OMX_SIZE indexSize;
}RPC_OMX_STRUCT_INFO;

//NOTE: Need to check if we need this. It should be defined in some OMX header.
/* this is used for GetComponentVersion****/
typedef struct VERSION_INFO
{
  char cName[128]; /***like "OMX.TI.Video.encoder"  **/
  OMX_VERSIONTYPE sVersion;
  OMX_VERSIONTYPE sSpecVersion;
  OMX_UUIDTYPE sUUID;
} VERSION_INFO;

/* ************************************** PROXY CONTEXT - VISIBLE AND MODIFIABLE TO RPC ONLY********************************* */
/* Tunnel manager structures */
typedef struct  TunnelManager
{
OMX_HANDLETYPE hComp;
OMX_HANDLETYPE hTunneledComp;
OMX_HANDLETYPE hProxy; // Tunnel Proxy handle
OMX_U32 refCount;
OMX_U32 ctrPass;
} TunnelManager;

#endif
