/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc_utils.h
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

#ifndef OMX_RPC_UTILSH
#define OMX_RPC_UTILSH

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
 
#include "omx_rpc_internal.h"

extern char rcmservertable[CORE_MAX][MAX_SERVER_NAME_LENGTH];
RPC_OMX_ERRORTYPE RPC_UnMapBuffer(OMX_U32 mappedBuffer );
RPC_OMX_ERRORTYPE RPC_MapBuffer(OMX_U32 mappedBuffer );
RPC_OMX_ERRORTYPE RPC_FlushBuffer(OMX_U8 * pBuffer, OMX_U32 size );
RPC_OMX_ERRORTYPE RPC_GetStructSize(OMX_PTR pCompParam, RPC_OMX_SIZE* nSize);
RPC_OMX_ERRORTYPE RPC_GetTargetServerName(OMX_STRING ComponentName, OMX_STRING * ServerName);
RPC_OMX_ERRORTYPE RPC_GetLocalServerName(OMX_STRING ComponentName, OMX_STRING * ServerName);


#endif
