/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc_stub.h
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
 
#ifndef OMX_RPC_STUBH
#define OMX_RPC_STUBH
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include "omx_rpc.h"
#include "omx_rpc_internal.h"

extern RPC_Object rpcHndl[CORE_MAX];
extern RcmClient_Handle rcmHndl;
extern COREID TARGET_CORE_ID;

RPC_OMX_ERRORTYPE RPC_GetHandle(RPC_OMX_HANDLE* remoteHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks,OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_FreeHandle(RPC_OMX_HANDLE remoteHandle, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_UseBuffer(RPC_OMX_HANDLE remoteHandle,OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer, OMX_U32* pBufferMapped, OMX_U32* pBufHeaderRemote, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_SetParameter(RPC_OMX_HANDLE hComp, OMX_INDEXTYPE nParamIndex, OMX_PTR pCompParam,OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_GetParameter(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nParamIndex,OMX_PTR pCompParam,OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_FillThisBuffer(RPC_OMX_HANDLE remoteHandle, OMX_BUFFERHEADERTYPE* pBufferHdr, OMX_U32 BufHdrRemote, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_EmptyThisBuffer(RPC_OMX_HANDLE remoteHandle, OMX_BUFFERHEADERTYPE* pBufferHdr, OMX_U32 BufHdrRemote, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_FreeBuffer(RPC_OMX_HANDLE hComp,OMX_IN  OMX_U32 nPortIndex, OMX_IN  OMX_U32 BufHdrRemote , OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_SetConfig(RPC_OMX_HANDLE hComp, OMX_INDEXTYPE nConfigIndex, OMX_PTR pCompConfig, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_GetConfig(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nConfigIndex,OMX_PTR pCompConfig, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_GetComponentVersion(RPC_OMX_HANDLE hComp,OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion,  OMX_VERSIONTYPE* pSpecVersion,  OMX_UUIDTYPE* pComponentUUID, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_SendCommand(RPC_OMX_HANDLE hComp, OMX_COMMANDTYPE eCmd, OMX_U32 nParam, OMX_PTR pCmdData,OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_GetState(RPC_OMX_HANDLE hComp, OMX_STATETYPE * pState,OMX_ERRORTYPE * nCmdStatus); 
RPC_OMX_ERRORTYPE RPC_GetExtensionIndex(RPC_OMX_HANDLE hComponent,OMX_STRING cParameterName,OMX_INDEXTYPE* pIndexType, OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_AllocateBuffer(RPC_OMX_HANDLE remoteHandle, OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,OMX_IN OMX_U32 nPortIndex,OMX_U32* pBufHeaderRemote,OMX_U32* pBufferMapped, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes,OMX_ERRORTYPE * nCmdStatus);
RPC_OMX_ERRORTYPE RPC_ComponentTunnelRequest(RPC_OMX_HANDLE remoteHandle, OMX_IN  OMX_U32 nPort,RPC_OMX_HANDLE hTunneledremoteHandle,OMX_U32 nTunneledPort, OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup,OMX_ERRORTYPE * nCmdStatus);

/*callback stubs with errors added*/
RPC_OMX_ERRORTYPE RPC_EventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
RPC_OMX_ERRORTYPE  RPC_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);
RPC_OMX_ERRORTYPE RPC_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer);




#endif
