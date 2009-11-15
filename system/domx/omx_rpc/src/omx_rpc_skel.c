/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc_skel.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework RPC Skeleton implementations.
 *
 *  @path \WTSD_DucatiMMSW\framework\domx\omx_rpc\src 
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 19-August-2009 B Ravi Kiran ravi.kiran@ti.com: Initial Version
 *================================================================*/
 
/* FUNCTION LIST*/
/*
release 1.05
Added functions Allocatebuffer and CompoenentTunnelRequest

release 1.07
split common EBDFDB call to separate implementations

1)  RPC_OMX_ERRORTYPE RPC_SKEL_GetHandle(UInt32 size, UInt32 *data)
2)  RPC_OMX_ERRORTYPE RPC_SKEL_SetParameter(UInt32 size, UInt32 *data)
3)  RPC_OMX_ERRORTYPE RPC_SKEL_GetParameter(UInt32 size, UInt32 *data)
4)  RPC_OMX_ERRORTYPE RPC_SKEL_FreeHandle(UInt32 size, UInt32 *data)
5)  RPC_OMX_ERRORTYPE RPC_SKEL_EmptyThisBuffer(UInt32 size, UInt32 *data)
6)  RPC_OMX_ERRORTYPE RPC_SKEL_FillThisBuffer(UInt32 size, UInt32 *data)
7)  RPC_OMX_ERRORTYPE RPC_SKEL_UseBuffer(UInt32 size, UInt32 *data)
8)  RPC_OMX_ERRORTYPE RPC_SKEL_FreeBuffer(UInt32 size, UInt32 *data)
9)  RPC_OMX_ERRORTYPE RPC_SKEL_SetConfig(UInt32 size, UInt32 *data)
10) RPC_OMX_ERRORTYPE RPC_SKEL_GetConfig(UInt32 size, UInt32 *data)
11) RPC_OMX_ERRORTYPE RPC_SKEL_GetState(UInt32 size, UInt32 *data)
12) RPC_OMX_ERRORTYPE RPC_SKEL_SendCommand(UInt32 size, UInt32 *data)
13) RPC_OMX_ERRORTYPE RPC_SKEL_GetComponentVersion(UInt32 size, UInt32 *data)
14) RPC_OMX_ERRORTYPE RPC_SKEL_GetExtensionIndex(UInt32 size, UInt32 *data)
15) RPC_OMX_ERRORTYPE RPC_SKEL_EmptyBufferDone(UInt32 size, UInt32 *data) 
16) RPC_OMX_ERRORTYPE RPC_SKEL_FillBufferDone(UInt32 size, UInt32 *data) 
17) RPC_OMX_ERRORTYPE RPC_SKEL_ComponentTunnelRequest(UInt32 size, UInt32 *data) - Not Implemented : TBD for 1.07 release
18) RPC_OMX_ERRORTYPE RPC_SKEL_AllocateBuffer(UInt32 size, UInt32 *data)
*/

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

#include <stdio.h>
#include <string.h>

#include "omx_rpc.h"
#include "omx_proxy_common.h"
#include "omx_rpc_skel.h"
#include <timm_osal_interfaces.h>
#include "omx_ti_common.h"

#include <SharedRegion.h>
static TunnelManager tMgr[10]; //Static implementation of Tunnel Manager - Will change to link list if memory constraints appear
static OMX_U32 tMgr_free_slot = 0; //Counter for free tunnel manager table entry

/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetHandle() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetHandle(UInt32 size, UInt32 *data)
{
    OMX_COMPONENTTYPE * hComponent;
    OMX_U32 dataOffset = 0;
    OMX_U32 dataOffset2 = 0;
    OMX_U32 offset=0;
    OMX_PTR pAppData;
    char  cComponentName[128] = {0};
    char CallingCorercmServerName[20]={0};	
    OMX_ERRORTYPE omxErr = OMX_ErrorNone;
    OMX_CALLBACKTYPE rpcCallBackInfo = {(void *)RPC_EventHandler, (void*)RPC_EmptyBufferDone, (void*)RPC_FillBufferDone};
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody;
    OMX_U8 CallingCore;
    recdMsg = (RPC_OMX_MESSAGE*)(data);

    //Place Holder for RCM client for the calling components callbacks
    
    msgBody = &recdMsg->msgBody[0];
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);
    DOMX_DEBUG("EventHandler address : 0x%x\n", rpcCallBackInfo.EventHandler);
    //skipping the offset for handle
    offset = offset+sizeof(int);
    dataOffset = *(msgBody+ offset);
    //Need to check length so that we dont exceed 128 bytes
    strcpy((char *)&cComponentName[0],msgBody+dataOffset);

   //moving past the 'cComponentName offset'
   offset+=sizeof(OMX_U32);
   TIMM_OSAL_Memcpy(&pAppData,msgBody+offset,sizeof(OMX_PTR));
   offset+=sizeof(OMX_PTR);
   TIMM_OSAL_Memcpy(&CallingCore,msgBody+offset,sizeof(OMX_U8));
   offset+=sizeof(OMX_U8);
   TIMM_OSAL_Memcpy(&dataOffset2,msgBody+offset,sizeof(OMX_U8));
   offset+=sizeof(OMX_U8);
   strcpy((char *)&CallingCorercmServerName[0],msgBody+dataOffset2);

   DOMX_DEBUG("\n CALLING CORE SERVERNAME RECOVERED FROM MESSAGE BODY:");
   DOMX_DEBUG("\n %s ",CallingCorercmServerName);

   RPC_InstanceInit(CallingCorercmServerName);
   	
   DOMX_DEBUG("RPC_SKEL_GetHandle: cComponentName received : %s\n", cComponentName);
   omxErr = OMX_ErrorNone;
   omxErr = OMX_GetHandle((OMX_HANDLETYPE *)&hComponent,(OMX_STRING)&cComponentName[0], pAppData,&rpcCallBackInfo);
   DOMX_DEBUG("\n handle obtained locally of new component %x\n",hComponent);

   if(omxErr !=OMX_ErrorNone) 
   {
   	DOMX_DEBUG("\nError Getting Handle: Error : %d\n", omxErr);
   }

    DOMX_DEBUG("RPC_SKEL_GetHandle: Remote Component Handle : 0x%x\n", hComponent);
   //Filling the handle of the remote component
    TIMM_OSAL_Memcpy(msgBody,&hComponent, sizeof(OMX_COMPONENTTYPE*));
    recdMsg->msgHeader.nOMXReturn = omxErr;
    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    return RPC_OMX_ErrorNone;
}




/* ===========================================================================*/
/**
 * @name RPC_SKEL_SetParameter() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_SetParameter(UInt32 size, UInt32 *data)
{
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle =NULL;
    OMX_ERRORTYPE eError;
    OMX_INDEXTYPE nParamIndex;
    OMX_U32 offset=0;
    OMX_PTR pCompParamStruct = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];

    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    nParamIndex = *(OMX_INDEXTYPE *)(msgBody+offset);
    offset+=sizeof(OMX_INDEXTYPE);
    //The offset of Index Structure is added with the base address of the msgBody.
    pCompParamStruct = (OMX_PTR)(*(msgBody+offset)+msgBody);

    //Now calling the the component SetParameter;
    pHandle = (OMX_COMPONENTTYPE * )hComp;

    eError = pHandle->SetParameter(hComp,nParamIndex,pCompParamStruct);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    return RPC_OMX_ErrorNone;
}

/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetParameter() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetParameter(UInt32 size, UInt32 *data)
{

    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    //OMX_PARAM_PORTDEFINITIONTYPE * pCompPrivateStruct=NULL;
    OMX_INDEXTYPE nParamIndex;
    OMX_PTR pCompParamStruct = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    
    DOMX_DEBUG("\n FUNCTION ENETRED_____________________ : %s",__FUNCTION__);
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;

    offset+=sizeof(OMX_HANDLETYPE);

    //Retrieving Index type
    nParamIndex= *(OMX_INDEXTYPE *)(msgBody+offset);
    offset+=sizeof(int);
    //The offset of Index Structure is added with the base address of the msgBody.
    pCompParamStruct = (OMX_PTR)(*(msgBody+offset)+msgBody);
    
    eError= pHandle->GetParameter(hComp,nParamIndex,pCompParamStruct);
    
    //DOMX_DEBUG("\n NOTE!!!!!!   --   --  Address of msgbody = %x\n",&msgBody);
    DOMX_DEBUG("\n Remote handle value after call on APP going to SYS: %x",hComp);

  
    //pCompPrivateStruct=(OMX_PARAM_PORTDEFINITIONTYPE *)pCompParamStruct;
    
    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;

    return RPC_OMX_ErrorNone;
}

/* ===========================================================================*/
/**
 * @name RPC_SKEL_FreeHandle() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_FreeHandle(UInt32 size, UInt32 *data)
{
	OMX_HANDLETYPE pHandle=NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);
	
	TIMM_OSAL_Memcpy(&pHandle,msgBody,sizeof(OMX_HANDLETYPE *));
	DOMX_DEBUG("RPC_SKEL_FreeHandle: Freeing Handle :0x%x\n",pHandle);

	OMX_FreeHandle(pHandle);

	recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = OMX_ErrorNone;
    RPC_InstanceDeInit();
	return RPC_OMX_ErrorNone;
}



/* ===========================================================================*/
/**
 * @name RPC_SKEL_EmptyThisBuffer() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_EmptyThisBuffer(UInt32 size, UInt32 *data)
{
    
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle=NULL;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    OMX_BUFFERHEADERTYPE * pBufHdr = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];

    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    TIMM_OSAL_Memcpy(&pBufHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE * ));
    offset+=sizeof(OMX_BUFFERHEADERTYPE * );
	TIMM_OSAL_Memcpy(&(pBufHdr->nFilledLen),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&(pBufHdr->nOffset),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(&(pBufHdr->nFlags),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

    //Calling the actual ETB onthe TC
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    DOMX_DEBUG("\n%s: ETB: filledLen  %d",__FUNCTION__, pBufHdr->nFilledLen);
    DOMX_DEBUG("\n%s: ETB: buffer header %x",__FUNCTION__, pBufHdr);

    //BCACHE_invalidate(pBufHdr->pBuffer,pBufHdr->nAllocLen);
    //REPLACE  WITH NEW CACHE INAVALIDATION API
    
#ifdef BUF_HEAP       
//AD try
DOMX_DEBUG("\npBuffer SR in skel before ETB = %x\n",pBufHdr->pBuffer);
pBufHdr->pBuffer = (char *)SharedRegion_getPtr(pBufHdr->pBuffer);
DOMX_DEBUG("\npBuffer in skel before ETB = %x\n",pBufHdr->pBuffer);
//AD try ends  
#endif  

    eError=pHandle->EmptyThisBuffer(hComp,pBufHdr);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;

    return RPC_OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_FillThisBuffer() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_FillThisBuffer(UInt32 size, UInt32 *data)
{
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle=NULL;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    OMX_BUFFERHEADERTYPE * pBufHdr = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);
    
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    TIMM_OSAL_Memcpy(&pBufHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE * ));
    offset+=sizeof(OMX_BUFFERHEADERTYPE * );

//Copying only relevant information (No bufferheader copy)
//    TIMM_OSAL_Memcpy(pBufHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE));
//extracting nfilledLen & nOffsetVal
	TIMM_OSAL_Memcpy(&(pBufHdr->nFilledLen),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&(pBufHdr->nOffset),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(&(pBufHdr->nFlags),msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

    //Calling the actual FTB onthe
    pHandle = (OMX_COMPONENTTYPE * )hComp;
#ifdef BUF_HEAP       
//AD try
DOMX_DEBUG("\npBuffer SR in skel before FTB = %x\n",pBufHdr->pBuffer);
pBufHdr->pBuffer = (char *)SharedRegion_getPtr(pBufHdr->pBuffer);
DOMX_DEBUG("\npBuffer in skel before FTB = %x\n",pBufHdr->pBuffer);
//AD try ends  
#endif 
    eError=pHandle->FillThisBuffer(hComp,pBufHdr);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;

    return RPC_OMX_ErrorNone;
}



/* ===========================================================================*/
/**
 * @name RPC_SKEL_UseBuffer() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_UseBuffer(UInt32 size, UInt32 *data)
{

    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle=NULL;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    OMX_BUFFERHEADERTYPE* ppBufferHdr;
    OMX_U32 nPortIndex;
    OMX_U32 nSizeBytes;
    OMX_PTR pAppPrivate;
    OMX_U8* pBuffer;

    #ifdef TILER_BUFF
    OMX_U32 pBufferUV;
    #endif

    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];

    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    
    DOMX_DEBUG("\n Remote handle value: %x",hComp);
    
    TIMM_OSAL_Memcpy(&nPortIndex,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(&pAppPrivate,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(&nSizeBytes,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(&pBuffer, msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

	#ifdef TILER_BUFF
//TODO : only for UV and move to ducati as well    
	TIMM_OSAL_Memcpy(&pBufferUV, msgBody+offset, sizeof(OMX_U32));
       offset+=sizeof(OMX_U32);
	#endif

	

    //Now calling Use buffer
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    
    DOMX_DEBUG("\n DEBUG - UNMAPPING - pBuffer = %x",pBuffer);
#ifdef BUF_HEAP_UB       
//AD try
DOMX_DEBUG("\npBuffer SR in skel before UB = %x\n",pBuffer);
pBuffer = (char *)SharedRegion_getPtr(pBuffer);
DOMX_DEBUG("\npBuffer in skel before UB = %x\n",pBuffer);
//AD try ends  
#endif    
    eError = pHandle->UseBuffer(hComp, &ppBufferHdr,nPortIndex,pAppPrivate, nSizeBytes,pBuffer);
#ifdef BUF_HEAP_UB       
//AD try
DOMX_DEBUG("\npBuffer in skel after UB = %x\n",ppBufferHdr->pBuffer);
pBuffer = (char *)SharedRegion_getSRPtr(pBuffer, 2);
ppBufferHdr->pBuffer = (char *)SharedRegion_getSRPtr(ppBufferHdr->pBuffer, 2);
DOMX_DEBUG("\npBuffer SR in skel after UB = %x\n",ppBufferHdr->pBuffer);
//AD try ends  
#endif     

    //Need the local BufferHeader address for filling it up during a ETB and FTB calls

    #ifdef TILER_BUFF
//TODO : only for UV and move to ducati as well    
//TODO : TI_PLAT type    
        ((OMX_TI_PLATFORMPRIVATE *)(ppBufferHdr->pPlatformPrivate))->pAuxBuf1 = pBufferUV;
	
    #endif

    if(eError == OMX_ErrorNone)
    {
           offset = USE_BUFFER_DATA_OFFSET;
	    #ifdef TILER_BUFF
	    offset += sizeof(OMX_U32);
	    #endif
	    TIMM_OSAL_Memcpy((msgBody+ offset),&ppBufferHdr, sizeof(OMX_BUFFERHEADERTYPE *));
	    offset = USE_BUFFER_DATA_OFFSET+sizeof(OMX_BUFFERHEADERTYPE *);
           DOMX_DEBUG("\n RPC SKEL ------------------------- msgBody = %x",msgBody);
	    DOMX_DEBUG("\n RPC SKEL ------------------------- msgBody+ USE_BUFFER_DATA_OFFSET= %x",msgBody+ USE_BUFFER_DATA_OFFSET);
	    //Now we have received a pointer to local BUFFERHEADER  in *ppBufferHdr
	    //The entire buffer header information retrieved from ppBufferHdr pointer is copied into the message buffer
	    //and sent to MPU side for make a local copy. THe contents are copied at the end of argument list
	    TIMM_OSAL_Memcpy((msgBody+ offset),(ppBufferHdr), sizeof(OMX_BUFFERHEADERTYPE));
		#ifdef TILER_BUFF
		offset += sizeof(OMX_BUFFERHEADERTYPE);
		if(ppBufferHdr->pPlatformPrivate) {
		//Checking for NON-NULL pPlatformPrivate to copy the Structure
		TIMM_OSAL_Memcpy((msgBody+ offset),ppBufferHdr->pPlatformPrivate,sizeof(OMX_TI_PLATFORMPRIVATE));
		}
		#endif
    }
    else
    {
    	DOMX_DEBUG("\n RPC_SKEL_UseBuffer: Error in UseBuffer 0x%x \n", eError);
    }

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;

    return RPC_OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_FreeBuffer() 
 * @brief Skeleton for OMX_FreeBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_FreeBuffer(UInt32 size, UInt32 *data)
{

	OMX_HANDLETYPE hComp;
	OMX_COMPONENTTYPE * pHandle =NULL;
	OMX_ERRORTYPE eError;
	OMX_U32 offset=0;
	OMX_U32 nPortIndex =0;
	OMX_BUFFERHEADERTYPE* pBufHdr;
	RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);

	hComp = *(OMX_HANDLETYPE *)(msgBody);
	offset+=sizeof(OMX_HANDLETYPE);

	TIMM_OSAL_Memcpy(&nPortIndex,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&pBufHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE * ));
	offset+=sizeof(OMX_BUFFERHEADERTYPE * );

	DOMX_DEBUG("\nSKEL-FREEBUFFER: buffer header being freed: %x",&pBufHdr);


	 pHandle = (OMX_COMPONENTTYPE * )hComp;
	 eError = pHandle->FreeBuffer(hComp,nPortIndex,pBufHdr);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
	recdMsg->msgHeader.nOMXReturn = eError;
	return RPC_OMX_ErrorNone;

}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_SetConfig() 
 * @brief Skeleton for OMX_GetComponentVersion()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_SetConfig(UInt32 size, UInt32 *data)
{

    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle =NULL;
    OMX_ERRORTYPE eError;
    OMX_INDEXTYPE nConfigIndex;
    OMX_PTR nCompConfig;
    OMX_U32 offset=0;
    OMX_PTR pCompConfigStruct = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);

    nConfigIndex = *(OMX_INDEXTYPE *)(msgBody+offset);
    offset+=sizeof(OMX_INDEXTYPE);

    //The offset of Index Structure is added with the base address of the msgBody.
    pCompConfigStruct = (OMX_PTR)(*(msgBody+offset)+msgBody);

    //Now calling the the component SetConfig;
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    eError = pHandle->SetConfig(hComp,nConfigIndex,pCompConfigStruct);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    return RPC_OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetConfig() 
 * @brief Skeleton for OMX_GetComponentVersion()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetConfig(UInt32 size, UInt32 *data)
{
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    OMX_INDEXTYPE nConfigIndex;
    OMX_PTR pCompConfigStruct = NULL;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 

    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\nEntering __________________________ : %s\n", __FUNCTION__);


    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;

    offset+=sizeof(OMX_HANDLETYPE);

    //Retrieving Index type
    nConfigIndex= *(OMX_INDEXTYPE *)(msgBody+offset);
    offset+=sizeof(int);

     //The offset of Index Structure is added with the base address of the msgBody.
    pCompConfigStruct = (OMX_PTR)(*(msgBody+offset)+msgBody);

    eError= pHandle->GetConfig(hComp,nConfigIndex,pCompConfigStruct);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    return RPC_OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetState() 
 * @brief Skeleton for OMX_GetComponentVersion()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetState(UInt32 size, UInt32 *data)
{
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_ERRORTYPE eError;
    OMX_U32 offset=0;
    OMX_STATETYPE* pState;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;

    offset+=sizeof(OMX_HANDLETYPE);

    /*** point to the msg***/
    pState = (OMX_PTR)(*(msgBody+offset)+msgBody);

    eError= pHandle->GetState(hComp,pState);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    
    return RPC_OMX_ErrorNone;

}



/* ===========================================================================*/
/**
 * @name RPC_SKEL_SendCommand() 
 * @brief Skeleton for OMX_GetComponentVersion()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_SendCommand(UInt32 size, UInt32 *data)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_U32 offset = 0;
    OMX_COMMANDTYPE * pCmd;
    OMX_U32 nParam1;
    
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    offset+=sizeof(OMX_HANDLETYPE);

    pCmd = ((OMX_COMMANDTYPE *)(msgBody+offset));
    offset+=sizeof(OMX_COMMANDTYPE);

    nParam1 = *((OMX_U32 *)(msgBody+offset));

    eError= pHandle->SendCommand(hComp,*pCmd, nParam1,NULL);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    
    return RPC_OMX_ErrorNone;

}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetComponentVersion() 
 * @brief Skeleton for OMX_GetComponentVersion()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetComponentVersion(UInt32 size, UInt32 *data)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_U32 offset = 0;
    
    /***temp ptr****/
    VERSION_INFO *p;

    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];

    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);
    /*parse the incoming msg***/
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    offset+=sizeof(OMX_HANDLETYPE);

    p= (VERSION_INFO*)(*(msgBody+offset) + msgBody);

    /*** call real handler to get the value back***/
    eError= pHandle->GetComponentVersion(hComp, p->cName,
					 &(p->sVersion), &(p->sSpecVersion),
					 &(p->sUUID));

    /*** check error***/
    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    
    return RPC_OMX_ErrorNone;
} 


/* ===========================================================================*/
/**
 * @name RPC_SKEL_GetExtensionIndex() 
 * @brief Skeleton for OMX_GetExtensionIndex()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_GetExtensionIndex(UInt32 size, UInt32 *data){
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_U32 offset = 0;
    OMX_U32 x;
    OMX_INDEXTYPE *pIndex;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    /*parse the incoming msg***/
    hComp = *(OMX_HANDLETYPE *)(msgBody);
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    offset+=sizeof(OMX_HANDLETYPE);
    
    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);

    /** get index***/

    x =*((OMX_U32*) (msgBody+offset))  ;

    pIndex = (OMX_INDEXTYPE *)(x + msgBody);

    offset+=sizeof(OMX_U32);/**this is the string***/

    eError= pHandle->GetExtensionIndex(hComp, (msgBody+offset), pIndex);

    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;

    return RPC_OMX_ErrorNone;

}




/* ===========================================================================*/
/**
 * @name RPC_SKEL_FillBufferDone() 
 * @brief Skeleton for OMX_AllocateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_EmptyBufferDone(UInt32 size, UInt32 *data)
{
	
	OMX_HANDLETYPE hComp;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_U32 nfilledLen =0;
	OMX_U32 nOffset=0;
	OMX_U32 nFlags =0;
	OMX_U32 bufferHdr;
	OMX_COMPONENTTYPE * pHandle=NULL;  
	PROXY_COMPONENT_PRIVATE *pCompPrivate  =NULL;  
	OMX_U32 offset=0;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
	
    msgBody = &recdMsg->msgBody[0];

	DOMX_DEBUG("\n%s:  Entered Function\n",__FUNCTION__);
	//Extracting the handle
	hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    
	//Assigning component type to access private variables
	pHandle = (OMX_COMPONENTTYPE *)hComp;
	pCompPrivate = (PROXY_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
	
	//extracting buffer header information.
	TIMM_OSAL_Memcpy(&bufferHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE *));
    offset+=sizeof(OMX_BUFFERHEADERTYPE *);

	//extracting nfilledLen & nOffsetVal
	TIMM_OSAL_Memcpy(&nfilledLen,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&nOffset,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(&nFlags,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	  
	DOMX_DEBUG("\n%s:  hComp : 0x%x, bufHdr : 0x%x, nFilledLen : 0x%x, nOffset: 0x%x\n",__FUNCTION__,hComp,bufferHdr,nfilledLen, nOffset);

	rpcError = (*pCompPrivate->proxyEmptyBufferDone)(hComp,bufferHdr,nfilledLen,nOffset,nFlags);
	
	return rpcError;
}

/* ===========================================================================*/
/**
 * @name RPC_SKEL_FillBufferDone() 
 * @brief Skeleton for OMX_AllocateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_FillBufferDone(UInt32 size, UInt32 *data)
{
	
	OMX_HANDLETYPE hComp;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_U32 nfilledLen =0;
	OMX_U32 nOffset=0;
	OMX_U32 nFlags =0;
	OMX_U32 bufferHdr;
	OMX_COMPONENTTYPE * pHandle=NULL;  
	PROXY_COMPONENT_PRIVATE *pCompPrivate  =NULL;  
	OMX_U32 offset=0;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
	
    msgBody = &recdMsg->msgBody[0];

	DOMX_DEBUG("\n%s:  Entered Function\n",__FUNCTION__);
    //Extracting the handle	
	hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    
	//Assigning component type to access private variables
	pHandle = (OMX_COMPONENTTYPE *)hComp;
	pCompPrivate = (PROXY_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
	
	//extracting buffer header information.
	TIMM_OSAL_Memcpy(&bufferHdr,msgBody+offset,sizeof(OMX_BUFFERHEADERTYPE *));
    offset+=sizeof(OMX_BUFFERHEADERTYPE *);

	//extracting nfilledLen & nOffsetVal
	TIMM_OSAL_Memcpy(&nfilledLen,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&nOffset,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(&nFlags,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	  
	DOMX_DEBUG("\n%s:  hComp : 0x%x, bufHdr : 0x%x, nFilledLen : 0x%x, nOffset: 0x%x\n",__FUNCTION__,hComp,bufferHdr,nfilledLen, nOffset);

    rpcError = (*pCompPrivate->proxyFillBufferDone)(hComp,bufferHdr,nfilledLen,nOffset,nFlags);		

	return rpcError;
}



/* ===========================================================================*/
/**
 * @name RPC_SKEL_EventHandler() 
 * @brief Skeleton for OMX_AllocateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_EventHandler(UInt32 size, UInt32 *data)
{

	
	OMX_HANDLETYPE hComp;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_COMPONENTTYPE * pHandle=NULL;  
	PROXY_COMPONENT_PRIVATE *pCompPrivate  =NULL;  
	OMX_EVENTTYPE eEvent;
	OMX_U32 nData1;
	OMX_U32 nData2;
	OMX_PTR pEventData;
	OMX_U32 offset=0;
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
	msgBody = &recdMsg->msgBody[0];
	
    DOMX_DEBUG("%s:  Entered Function\n",__FUNCTION__);
	//Extracting the handle
	hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);

	 //Assigning component type to access private variables
	pHandle = (OMX_COMPONENTTYPE *)hComp;
	pCompPrivate = (PROXY_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

	TIMM_OSAL_Memcpy(&eEvent,msgBody+offset,sizeof(OMX_EVENTTYPE));
	offset+=sizeof(OMX_EVENTTYPE);

	TIMM_OSAL_Memcpy(&nData1,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(&nData2,msgBody+offset,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	TIMM_OSAL_Memcpy(&pEventData,msgBody+offset,sizeof(OMX_PTR));
	offset+=sizeof(OMX_PTR);

    DOMX_DEBUG("\n\n**__**Got eventhandler from Remote Core succesffuly - values are : **__**");
    DOMX_DEBUG("\nhComp :0x%x,eEvent: 0x%x,nData1: 0x%x,nData2: 0x%x,pEventData: 0x%x",hComp,eEvent,nData1,nData2,pEventData);
    DOMX_DEBUG("\nJust Before Entering Proxy Event handler call");

	rpcError = (*pCompPrivate->proxyEventHandler)(hComp, eEvent, nData1, nData2, pEventData);
	//PROXY_EventHandler(hComponent, eEvent, nData1,nData2, pEventData)


	DOMX_DEBUG("\nAfter from Proxy event handler");
    
    return rpcError;
	
}


/* ===========================================================================*/
/**
 * @name RPC_SKEL_AllocateBuffer() 
 * @brief Skeleton for OMX_AllocateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_AllocateBuffer(UInt32 size, UInt32 *data)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hComp;
    OMX_COMPONENTTYPE * pHandle;
    OMX_U32 offset = 0;
    OMX_U32 nPortIndex;
    OMX_PTR pAppPrivate;
    OMX_U8* pBuffer;
    OMX_BUFFERHEADERTYPE** ppBufferHdr;
    OMX_BUFFERHEADERTYPE* ppBufferHdr_app;	
    OMX_U32 nSizeBytes;
    OMX_U32 pBufferMapped;
    
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    
    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    
    TIMM_OSAL_Memcpy(&nPortIndex,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(&pAppPrivate,msgBody+offset, sizeof(int));
    offset+=sizeof(int);

    TIMM_OSAL_Memcpy(&nSizeBytes,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(&ppBufferHdr, msgBody+offset, sizeof(OMX_BUFFERHEADERTYPE**));
    offset+=sizeof(OMX_BUFFERHEADERTYPE**);

    //Now calling Allocate buffer
    pHandle = (OMX_COMPONENTTYPE * )hComp;
    
    DOMX_DEBUG("\n DEBUG - UNMAPPING - pBuffer = %x",pBuffer);

    eError= pHandle->AllocateBuffer(hComp,&ppBufferHdr_app,nPortIndex,pAppPrivate,nSizeBytes);
   
    if(eError == OMX_ErrorNone)
    {
	    offset = USE_BUFFER_DATA_OFFSET;
	    TIMM_OSAL_Memcpy((msgBody+ offset),&ppBufferHdr_app, sizeof(OMX_BUFFERHEADERTYPE *));
	    offset = USE_BUFFER_DATA_OFFSET+sizeof(OMX_BUFFERHEADERTYPE *);
	    //Now we have received a pointer to local BUFFERHEADER  in *ppBufferHdr
	    //The entire buffer header information retrieved from ppBufferHdr pointer is copied into the message buffer
	    //and sent to MPU side for make a local copy. THe contents are copied at the end of argument list
	    TIMM_OSAL_Memcpy((msgBody+ offset),(ppBufferHdr_app), sizeof(OMX_BUFFERHEADERTYPE ));
		#ifdef TILER_BUFF
		offset += sizeof(OMX_BUFFERHEADERTYPE);
		if(ppBufferHdr_app->pPlatformPrivate) {
		//Checking for NON-NULL pPlatformPrivate to copy the Structure
		TIMM_OSAL_Memcpy((msgBody+ offset),ppBufferHdr_app->pPlatformPrivate,sizeof(OMX_TI_PLATFORMPRIVATE));
		
		}
        #endif
		
    }
    else
    {
    	DOMX_DEBUG("\n %s: Error in AllocateBuffer 0x%x \n", eError, __FUNCTION__);
    }
    
    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    
    return RPC_OMX_ErrorNone;

}

/* ===========================================================================*/
/**
 * @name RPC_SKEL_ComponentTunnelRequest() 
 * @brief Skeleton for ComponentTunnel Request
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SKEL_ComponentTunnelRequest(UInt32 size, UInt32 *data)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_HANDLETYPE hComp,hTunneledComp;
    OMX_COMPONENTTYPE * pInComp, * pOutComp;
    OMX_U32 offset = 0;
    OMX_U32 nPort,nTunneledPort;
    OMX_PTR pAppPrivate,pAppData;
    OMX_TUNNELSETUPTYPE pTunnelSetupAtSkel,*pTunnelSetup;
    OMX_CALLBACKTYPE rpcCallBackInfo = {(void *)RPC_EventHandler, (void*)RPC_EmptyBufferDone, (void*)RPC_FillBufferDone}; 
    // rpcCallBackInfo should contain the call back implemetations of the component - EBD and FDB - no need for EH - that can be left NULL - and a check provided
	
      
    RPC_OMX_MESSAGE * recdMsg;
    RPC_OMX_BYTE * msgBody; 
    recdMsg = (RPC_OMX_MESSAGE*)(data);
    msgBody = &recdMsg->msgBody[0];
    pTunnelSetup = &pTunnelSetupAtSkel;
    
    DOMX_DEBUG("\n Entering function ________ %s ",__FUNCTION__);

    hComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
       
    TIMM_OSAL_Memcpy(&nPort,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);
    
    hTunneledComp = *(OMX_HANDLETYPE *)(msgBody);
    offset+=sizeof(OMX_HANDLETYPE);
    
    TIMM_OSAL_Memcpy(&nTunneledPort,msgBody+offset, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);
    
    TIMM_OSAL_Memcpy(&pTunnelSetupAtSkel,msgBody+offset, sizeof(OMX_TUNNELSETUPTYPE));
    offset+=sizeof(OMX_U32);

	//NOTE:pAppData needs to be copied here from the msgBody to be used in the consequent Gethandle call

	// need to get OMX_GetHandle("OMX_SAMPLE_PROXY,...)
	//Callbacks need to be set to the Sample components implementations of EBD and FBD

    eError = OMX_GetHandle(&hComp, (OMX_STRING)"OMX.TI.MISC.SAMPLE",
                                  &pAppData, &rpcCallBackInfo);

    if(OMX_ErrorNone == eError)
    {

	DOMX_DEBUG("\n SAMPLE COMPONENT TUNNEL PROXY INITIALIZED SUCCESSFULLY:");
    }
    else
    {
        DOMX_DEBUG("\n Failed to initialize Tunnel Proxy");
		goto EXIT;
    }
    
    tMgr_free_slot++;

    if(tMgr_free_slot == 10)
    {
         DOMX_DEBUG("\nTUNNEL MANAGER TABLE OVERFLOW");
	  eError = OMX_ErrorInsufficientResources;
	  goto EXIT;
    }
	

    pInComp  = (OMX_COMPONENTTYPE *)hComp;
    pOutComp = (OMX_COMPONENTTYPE *)hTunneledComp;

    
    if(tMgr[tMgr_free_slot].hComp == pOutComp && tMgr[tMgr_free_slot].hComp == pInComp)
    {
	tMgr[tMgr_free_slot].ctrPass = 2;
	DOMX_DEBUG("\n CTR PASS 2: First call to CTR SKEL made");
    }
    else
    {
        tMgr[tMgr_free_slot].hComp = pInComp;
        tMgr[tMgr_free_slot].hTunneledComp = pOutComp;
	 tMgr[tMgr_free_slot].ctrPass = 1;
	 DOMX_DEBUG("\n CTR PASS 1: First call to CTR SKEL made");
    }
    	
    if(hComp == NULL && hTunneledComp == NULL) {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }

    

    /* setting default values before  tunneling */
    //tunnelSetupAtSkel.nTunnelFlags = 0;
    //tunnelSetupAtSkel.eSupplier    = OMX_BufferSupplyUnspecified;
    // This should have occured at OMX core at the IL Client side

    if(tMgr[tMgr_free_slot].ctrPass == 1)
    {
        if(hTunneledComp) {

        /* requesting tunnel on the component with the output port */
           // eError = pOutComp->ComponentTunnelRequest(hComp,nPort,hTunneledComp,nTunneledPort,pTunnelSetup);
           DOMX_DEBUG("\n Value of hcomp,hTunneledComp = %x,%x",hComp,hTunneledComp);
	    hComp =(OMX_HANDLETYPE) 0x1;hTunneledComp = (OMX_HANDLETYPE)0x1;
	    eError = OMX_ErrorNone ;
	     if(OMX_ErrorNone == eError)
		 	DOMX_DEBUG("\n Component Tunnel request 1 succesful");
        }
    }
    else
    {
        if(hComp) {

        /* requesting tunnel on the component with the output port */
           // eError = pInComp->ComponentTunnelRequest(hTunneledComp,nTunneledPort,hComp,nPort,pTunnelSetup);
		eError = OMX_ErrorNone ;
		DOMX_DEBUG("\n Value of hcomp,hTunneledComp = %x,%x",hComp,hTunneledComp);
		hComp = (OMX_HANDLETYPE)0x2;hTunneledComp = (OMX_HANDLETYPE)0x2;
	     if(OMX_ErrorNone == eError)
		 	DOMX_DEBUG("\n Component Tunnel request 2 succesful");
        }
    }
    
    recdMsg->msgHeader.nRPCCmdStatus = RPC_OMX_ErrorNone;
    recdMsg->msgHeader.nOMXReturn = eError;
    
EXIT:
    DOMX_DEBUG("\n Leaving %s",__FUNCTION__);
    return RPC_OMX_ErrorNone;
            
}

RPC_OMX_ERRORTYPE RPC_SKEL_GetAdditionalServer(UInt32 size, UInt32 *data)
{
        RPC_OMX_MESSAGE * recdMsg;
        RPC_OMX_BYTE * msgBody; 
        recdMsg = (RPC_OMX_MESSAGE*)(data);
        msgBody = &recdMsg->msgBody[0];
	 DOMX_DEBUG("\n EMPTY IMPLEMENTATION");
	 DOMX_DEBUG("\nSKEL is being called to bring up new server");
	 DOMX_DEBUG("\n Server create and symbol registration to proceed");
	 //RCM server create and register symbols
	 return RPC_OMX_ErrorNone;

}
