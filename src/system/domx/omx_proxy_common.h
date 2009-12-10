/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_proxy_common.h
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework OMX Common Proxy.
 *
 *  @path \WTSD_DucatiMMSW\framework\domx\omx_proxy_common\ 
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 19-August-2009 B Ravi Kiran ravi.kiran@ti.com: Initial Version
 *================================================================*/
 
#ifndef OMX_PROXY_H
#define OMX_PROXY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include "omx_rpc.h"
#include "omx_rpc_internal.h"

#define MAX_NUM_PROXY_BUFFERS             25
#define MAX_COMPONENT_NAME_LENGTH         128

typedef struct PROXY_BUFFER_INFO{
	OMX_BUFFERHEADERTYPE* pBufHeader;
	OMX_U32 pBufHeaderRemote;
	OMX_U32 pBufferMapped;
	OMX_U32 pBufferActual;
	OMX_U32 actualContent;
	OMX_U32 pAlloc_localBuffCopy;
}PROXY_BUFFER_INFO;

typedef RPC_OMX_ERRORTYPE (*PROXY_EMPTYBUFFER_DONE)(OMX_HANDLETYPE, OMX_U32, OMX_U32, OMX_U32,OMX_U32);
typedef RPC_OMX_ERRORTYPE (*PROXY_FILLBUFFER_DONE)(OMX_HANDLETYPE, OMX_U32, OMX_U32, OMX_U32, OMX_U32);
typedef RPC_OMX_ERRORTYPE (*PROXY_EVENTHANDLER)(OMX_HANDLETYPE,OMX_EVENTTYPE,OMX_U32, OMX_U32, OMX_PTR);

typedef struct PROXY_COMPONENT_PRIVATE
{
	/* OMX Related Information */
	OMX_CALLBACKTYPE cbInfo;
	OMX_COMPONENTTYPE * pHandle;
	OMX_PTR pApplicationPrivate;
	
	/* PROXY specific data - PROXY PRIVATE DATA */
       char *cCompName;
	/*RPC Related information
	This is the handle of the real component on the remote side.
	The pHandle received by the Proxy is replaced by the following handle */
	RPC_OMX_HANDLE remoteHandle; 
	PROXY_EMPTYBUFFER_DONE proxyEmptyBufferDone;
	PROXY_FILLBUFFER_DONE proxyFillBufferDone;
	PROXY_EVENTHANDLER proxyEventHandler;
       PROXY_BUFFER_INFO bufferList[MAX_NUM_PROXY_BUFFERS];
	OMX_U16 numOfBuffers; 
}PROXY_COMPONENT_PRIVATE;

OMX_ERRORTYPE OMX_ProxyCommonInit(OMX_HANDLETYPE hComponent);
#endif
