/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_proxy_common.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework OMX Common Proxy .
 *
 *  @path \WTSD_DucatiMMSW\framework\domx\omx_proxy_common\src 
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 19-August-2009 B Ravi Kiran ravi.kiran@ti.com: Initial Version
 *================================================================*/


/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "omx_proxy_common.h"
#include "omx_rpc.h"
#include "omx_rpc_stub.h"
#include "omx_rpc_utils.h"
#include "timm_osal_memory.h"
#include "omx_ti_common.h"

#define TILER_BUFF

#ifdef TILER_BUFF
#include <ProcMgr.h>
#include <SysLinkMemUtils.h>
#include "memmgr/mem_types.h"
//#include "utils/utils.h"
//#include "debug_utils.h"
//#include "list_utils.h"
//#include "tilermem_utils.h"
#include "d2c/phase1_d2c_remap.h"
#include "memmgr/memmgr.h"
//#include "tiler.h"
#define PortFormatIsNotYUV 0
#endif

static OMX_ERRORTYPE PROXY_GetParameter(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_INDEXTYPE nParamIndex,  OMX_INOUT OMX_PTR pComponentParameterStructure);
static OMX_ERRORTYPE PROXY_SetParameter(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_INDEXTYPE nParamIndex, OMX_IN  OMX_PTR pComponentParameterStructure);
static OMX_ERRORTYPE PROXY_EmptyThisBuffer (OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE* pBufferHdr);
static OMX_ERRORTYPE PROXY_FillThisBuffer (OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE* pBufferHdr);
static OMX_ERRORTYPE PROXY_ComponentDeInit (OMX_HANDLETYPE hComponent);
static OMX_ERRORTYPE PROXY_SetCallbacks (OMX_HANDLETYPE pComponent, OMX_CALLBACKTYPE* pCallBacks, OMX_PTR pAppData);
static OMX_ERRORTYPE PROXY_UseBuffer (OMX_IN OMX_HANDLETYPE hComponent, OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,OMX_IN OMX_U32 nPortIndex,OMX_IN OMX_PTR pAppPrivate, OMX_IN OMX_U32 nSizeBytes, OMX_IN OMX_U8* pBuffer);
static OMX_ERRORTYPE PROXY_EmptyBufferDone(OMX_HANDLETYPE hComponent,OMX_U32 remoteBufHdr, OMX_U32 nfilledLen, OMX_U32 nOffset, OMX_U32 nFlags);
static OMX_ERRORTYPE PROXY_FillBufferDone(OMX_HANDLETYPE hComponent,OMX_U32 remoteBufHdr, OMX_U32 nfilledLen, OMX_U32 nOffset, OMX_U32 nFlags);
static OMX_ERRORTYPE PROXY_EventHandler(OMX_HANDLETYPE hComponent,OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData);
static OMX_ERRORTYPE PROXY_SetConfig(OMX_IN  OMX_HANDLETYPE hComponent,OMX_IN  OMX_INDEXTYPE nConfigIndex, OMX_IN  OMX_PTR pComponentConfigStructure);
static OMX_ERRORTYPE PROXY_FreeBuffer( OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_U32 nPortIndex, OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);
static OMX_ERRORTYPE PROXY_GetConfig (OMX_HANDLETYPE hComp,OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);
static OMX_ERRORTYPE PROXY_SendCommand(OMX_IN  OMX_HANDLETYPE hComponent,OMX_IN  OMX_COMMANDTYPE eCmd,OMX_IN  OMX_U32 nParam,OMX_IN  OMX_PTR pCmdData);
static OMX_ERRORTYPE PROXY_GetComponentVersion(OMX_IN  OMX_HANDLETYPE hComponent,OMX_OUT OMX_STRING pComponentName,OMX_OUT OMX_VERSIONTYPE* pComponentVersion,OMX_OUT OMX_VERSIONTYPE* pSpecVersion,OMX_OUT OMX_UUIDTYPE* pComponentUUID);
static OMX_ERRORTYPE PROXY_GetState(OMX_IN  OMX_HANDLETYPE hComponent,OMX_OUT OMX_STATETYPE* pState);
static OMX_ERRORTYPE PROXY_GetExtensionIndex( OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_STRING cParameterName,OMX_OUT OMX_INDEXTYPE* pIndexType); 
static OMX_ERRORTYPE PROXY_ComponentRoleEnum(OMX_IN OMX_HANDLETYPE hComponent,OMX_OUT OMX_U8 *cRole,OMX_IN OMX_U32 nIndex);

/* NEW APIs ADDED */
static OMX_ERRORTYPE PROXY_ComponentTunnelRequest(
        OMX_IN  OMX_HANDLETYPE hComp, 
        OMX_IN  OMX_U32 nPort, 
        OMX_IN  OMX_HANDLETYPE hTunneledComp, 
        OMX_IN  OMX_U32 nTunneledPort, 
        OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup);

static OMX_ERRORTYPE PROXY_AllocateBuffer( 
            OMX_IN OMX_HANDLETYPE hComponent, 
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr, 
            OMX_IN OMX_U32 nPortIndex, 
            OMX_IN OMX_PTR pAppPrivate, 
            OMX_IN OMX_U32 nSizeBytes);

/* TILER BUFFER TESTING*/
#ifdef TILER_BUFF
 static OMX_ERRORTYPE PROXY_UTIL_GetUVPtr(OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE * pBufferHeader,OMX_U32 * nFrameHeight, bytes_t * uvPtr, OMX_U32 nPortIndex);
#endif


static OMX_ERRORTYPE PROXY_EmptyBufferDone(OMX_HANDLETYPE hComponent,OMX_U32 remoteBufHdr, OMX_U32 nfilledLen, OMX_U32 nOffset, OMX_U32 nFlags)
{
	OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)hComponent;
	PROXY_COMPONENT_PRIVATE *pComponentPrivate =(PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
	OMX_U16 count;
	OMX_BUFFERHEADERTYPE * pBufHdr = NULL;
	DOMX_DEBUG("\n%s Entered",__FUNCTION__);
	for(count = 0; count<pComponentPrivate->numOfBuffers;++count)
	{
		if(pComponentPrivate->bufferList[count].pBufHeaderRemote == remoteBufHdr)
		{
			
			pBufHdr = pComponentPrivate->bufferList[count].pBufHeader;
			pBufHdr->nFilledLen = nfilledLen;
			pBufHdr->nOffset = nOffset;
			pBufHdr->nFlags = nFlags;
			pBufHdr->pBuffer = (OMX_U8 *)pComponentPrivate->bufferList[count].pBufferActual;
			pComponentPrivate->cbInfo.EmptyBufferDone(hComponent,pHandle->pApplicationPrivate,pBufHdr);
			break;
		}
	}
	DOMX_DEBUG("\n%s Leaving",__FUNCTION__);
	return OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name PROXY_EventHandler() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_EventHandler(OMX_HANDLETYPE hComponent,OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{
	OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)hComponent;
	PROXY_COMPONENT_PRIVATE *pComponentPrivate =(PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
	DOMX_DEBUG("\nEntered Proxy event handler__________________________________________PROXY EH");
    DOMX_DEBUG("hComponent:0x%x,pHandle->pApplicationPrivate: 0x%x,eEvent: 0x%x,nData1: 0x%x,nData2: 0x%x,pEventData: 0x%x\n",hComponent,pHandle->pApplicationPrivate,eEvent,nData1,nData2,pEventData);
    pComponentPrivate->cbInfo.EventHandler(hComponent,pHandle->pApplicationPrivate,eEvent,nData1,nData2,pEventData);
	return OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name PROXY_FillBufferDone() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_FillBufferDone(OMX_HANDLETYPE hComponent,OMX_U32 remoteBufHdr, OMX_U32 nfilledLen, OMX_U32 nOffset, OMX_U32 nFlags)
{
	
	OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)hComponent;
	PROXY_COMPONENT_PRIVATE *pComponentPrivate =(PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
	OMX_U16 count;
	OMX_BUFFERHEADERTYPE * pBufHdr = NULL;
	DOMX_DEBUG("\n%s Entered",__FUNCTION__);
	
	for(count = 0; count<pComponentPrivate->numOfBuffers;++count)
	{
		if(pComponentPrivate->bufferList[count].pBufHeaderRemote == remoteBufHdr)
		{
			pBufHdr = pComponentPrivate->bufferList[count].pBufHeader;
			pBufHdr->nFilledLen = nfilledLen;
			pBufHdr->nOffset = nOffset;
			pBufHdr->nFlags = nFlags;
			pBufHdr->pBuffer = (OMX_U8 *)pComponentPrivate->bufferList[count].pBufferActual;
			pComponentPrivate->cbInfo.FillBufferDone(hComponent,pHandle->pApplicationPrivate,pBufHdr);
			break;
		}
	}
	return OMX_ErrorNone;
}


/* ===========================================================================*/
/**
 * @name PROXY_ComponentDeInit() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_ComponentDeInit (OMX_HANDLETYPE hComponent)
{
	OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)hComponent;
	PROXY_COMPONENT_PRIVATE *pComponentPrivate =(PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
	OMX_U32 count;
	OMX_ERRORTYPE nCmdStatus;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;

	DOMX_DEBUG("\n ____PROXY DEINIT_______ %s Entered",__FUNCTION__ );

	//Freeing Remote Component
	rpcError = RPC_FreeHandle(pComponentPrivate->remoteHandle, &nCmdStatus);
	if(rpcError == RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("\n%s RPC_FreeHandle Success",__FUNCTION__);
	}
	else
		DOMX_DEBUG("\n%s Error in RPC_FreeHandle",__FUNCTION__);
	
	//Unmap any buffers previously mapped
	for(count=0;count<pComponentPrivate->numOfBuffers;++count)
	{
		DOMX_DEBUG("%s Cleaning up Buffers\n",__FUNCTION__);
		if(pComponentPrivate->bufferList[count].pBufferMapped)
			RPC_UnMapBuffer(pComponentPrivate->bufferList[count].pBufferMapped);

		if(pComponentPrivate->bufferList[count].pBufHeader){
                     TIMM_OSAL_Free(pComponentPrivate->bufferList[count].pBufHeader);
			pComponentPrivate->bufferList[count].pBufHeader = NULL;
		}
	}
	
	/* Freeing up allocated Component Name */
	if(pComponentPrivate->cCompName){
		TIMM_OSAL_Free((void *)pComponentPrivate->cCompName);
		pComponentPrivate->cCompName=NULL;
	}

	/* Freeing up pCompPrivateStructure*/ 	
	if(pComponentPrivate){
		TIMM_OSAL_Free((void *)pComponentPrivate);
		pComponentPrivate=NULL;
	}

	return OMX_ErrorNone;
	
}



/* ===========================================================================*/
/**
 * @name OMX_ProxyCommonInit() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_ProxyCommonInit(OMX_HANDLETYPE hComponent)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_COMPONENTTYPE *pHandle = NULL;
	RPC_OMX_HANDLE  remoteHandle;
	OMX_ERRORTYPE nCmdStatus;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate;
	OMX_STRING cComponentName; //(Component name to be obatined from remote handle pcompprivate
	pHandle = (OMX_COMPONENTTYPE *)hComponent;
	OMX_CALLBACKTYPE pProxyCBinfo = { PROXY_EventHandler, PROXY_EmptyBufferDone,PROXY_FillBufferDone};

    DOMX_DEBUG("\n_____________________INSIDE PROXY COMMON__________________________\n");
    DOMX_DEBUG("%s:  hComponent: 0x%x\n",__FUNCTION__, hComponent);

    pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate; 

    //Obtaining component name from proxyConfigPrivate
	cComponentName = pComponentPrivate->cCompName;
	
	//Setting its core information in private data
	
	// Storing my own handle in my private data 
	pComponentPrivate->pHandle = pHandle;
	pComponentPrivate->numOfBuffers = 0;
	//This is not a standard OMX Component Function, This is used to fwd the EmptyBufferDone call recieved from Real Component.
	pComponentPrivate->proxyEmptyBufferDone = PROXY_EmptyBufferDone;
	
	pComponentPrivate->proxyFillBufferDone = PROXY_FillBufferDone;

	pComponentPrivate->proxyEventHandler = PROXY_EventHandler;
	
	/*The Proxy component does a get RPC_GetHandle to create corresponding remote components */
	//Here we send the proxy components local handle for pAppData, this is useful in maintaining a handle Match in Facilitator/OMXRPC Remote side
	//In case of multile tunnel requests for the same remote components, this {proxy handle , real remote handle} pair list would be used to check if we
	//have already created the tunneled component.
	//Also during call backs to 'Facilitator' on remote side, pAppData would be returned by the real component to 'Facilitator' based on this handle matching pair
	// we can know who is calling the call back functions and replicate the call back from respective proxy components on the Local core.
	
        rpcError = RPC_GetHandle(&remoteHandle, cComponentName,(OMX_PTR)hComponent, &pProxyCBinfo, &nCmdStatus);
	
        if(rpcError==RPC_OMX_ErrorNone)
	{
		//storing the remote handle as a private information.
		//this is the handle that is sent accross to remote core instead of pHandle of the local component
             if(nCmdStatus==OMX_ErrorNone)
		pComponentPrivate->remoteHandle = remoteHandle;
	      else
	      {
	          eError = nCmdStatus;
		   DOMX_DEBUG("\n ERROR executing OMX_GetHandle remotely");
	      	}
	 }
	else
	{
		//Need to do more accurate Error Handling
		eError = OMX_ErrorHardware;
		DOMX_DEBUG("\nFailed in RPC_GetHandle - Error in RPC");
		
		if(pComponentPrivate)
			{TIMM_OSAL_Free ((void*)pComponentPrivate);
		         
			}

		goto EXIT;

	}

	
	pHandle->SetCallbacks	= PROXY_SetCallbacks;
	pHandle->ComponentDeInit = PROXY_ComponentDeInit;
	pHandle->UseBuffer = PROXY_UseBuffer;
	pHandle->GetParameter = PROXY_GetParameter;
	pHandle->SetParameter = PROXY_SetParameter;
	pHandle->EmptyThisBuffer= PROXY_EmptyThisBuffer;
	pHandle->FillThisBuffer= PROXY_FillThisBuffer;
	pHandle->GetComponentVersion= PROXY_GetComponentVersion;
	pHandle->SendCommand = PROXY_SendCommand;
	pHandle->GetConfig= PROXY_GetConfig;
	pHandle->SetConfig= PROXY_SetConfig;
	pHandle->GetState= PROXY_GetState;
	pHandle->GetExtensionIndex  = PROXY_GetExtensionIndex;
	pHandle->FreeBuffer = PROXY_FreeBuffer;

	
	#ifdef KHRONOS_1_1
	pHandle->ComponentRoleEnum = PROXY_ComponentRoleEnum;
	#endif

	// Will be extended to all these following calls later.
	
	pHandle->ComponentTunnelRequest         = PROXY_ComponentTunnelRequest;
	pHandle->AllocateBuffer                 = PROXY_AllocateBuffer;
		
    DOMX_DEBUG("\n Proxy Initted");
	EXIT:
		return eError;
}



/* ===========================================================================*/
/**
 * @name PROXY_SetCallbacks() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_SetCallbacks (OMX_HANDLETYPE pComponent, OMX_CALLBACKTYPE* pCallBacks, OMX_PTR pAppData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE*)pComponent;
    PROXY_COMPONENT_PRIVATE *pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
    DOMX_DEBUG("Entered : %s\n",__FUNCTION__);

	if (pCallBacks == NULL)
	{
		eError = OMX_ErrorBadParameter;
		DOMX_DEBUG ("%d :: Received the empty callbacks from the application\n",__LINE__);
		goto EXIT;
	}

	if (pAppData == NULL) {
	DOMX_DEBUG ("%d :: Received the empty Private data from the application\n", __LINE__);
	eError = OMX_ErrorBadParameter;
	goto EXIT;
	}

    	/* Copy the callbacks of the application to the component private  */
    	TIMM_OSAL_Memcpy(&(pComponentPrivate->cbInfo), pCallBacks, sizeof(OMX_CALLBACKTYPE));

        /* copy the application private data to component memory */
        pHandle->pApplicationPrivate = pAppData;

EXIT:
    return eError;
}



/* ===========================================================================*/
/**
 * @name PROXY_UseBuffer() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_UseBuffer (OMX_IN OMX_HANDLETYPE hComponent, OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,OMX_IN OMX_U32 nPortIndex,OMX_IN OMX_PTR pAppPrivate, OMX_IN OMX_U32 nSizeBytes, OMX_IN OMX_U8* pBuffer)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	OMX_BUFFERHEADERTYPE * pBufferHeader = NULL;
	OMX_ERRORTYPE nCmdStatus;
	OMX_U32 pBufferMapped;
	OMX_U32 pBufHeaderRemote;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_U32 currentBuffer;
#ifdef TILER_BUFF
	ProcMgr_MapType mapType;
	SyslinkMemUtils_MpuAddrToMap    MpuAddr_list_1D[1];
	SyslinkMemUtils_MpuAddrToMap    MpuAddr_list_2D[2];
	MemAllocBlock block;
	OMX_U32 pDucBuf,pDucBuf_2D[2];
	OMX_PTR pChironBuffer;
	OMX_U32 nFrameHeight;
	bytes_t uvPtr;
	OMX_U32 status;
#endif
	OMX_TI_PLATFORMPRIVATE * pPlatformPrivate;
    OMX_COMPONENTTYPE * pHandle =(OMX_COMPONENTTYPE *) hComponent;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=(PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;;
    currentBuffer = pComponentPrivate->numOfBuffers;
	//Allocating Local bufferheader to be maintained locally within proxy
	pBufferHeader = (OMX_BUFFERHEADERTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_INT);

	if(pBufferHeader==NULL) {
	        DOMX_DEBUG("Allocation of Buffer Header structure failed");
		 eError = OMX_ErrorUndefined;
		 goto leave;
	} 
	
       pPlatformPrivate = (OMX_TI_PLATFORMPRIVATE *)TIMM_OSAL_Malloc(sizeof(OMX_TI_PLATFORMPRIVATE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_INT);

	if(pPlatformPrivate==NULL) {
	        DOMX_DEBUG("Allocation of Platform Private structure failed");
		 eError = OMX_ErrorUndefined;
		 goto leave;
	}   
       /* Storing the original Content of pPlatformPrivate*/
	//	pComponentPrivate->bufferList[currentBuffer].pPlatformPrivateClient =pBufferHeader->pPlatformPrivate;
	pBufferHeader->pPlatformPrivate = pPlatformPrivate;
    

#ifdef TILER_BUFF
       //If Tiler 1D buffer, get corresponding ducati address and send out buffer to ducati

       if(MemMgr_IsMapped(pBuffer) && MemMgr_Is1DBlock(pBuffer)) {

 
        //retrieve ducati address 
        mapType = ProcMgr_MapType_Tiler;
        MpuAddr_list_1D[0].mpuAddr = (UInt32)pBuffer;
        MpuAddr_list_1D[0].size = nSizeBytes;

	 status = SysLinkMemUtils_map (MpuAddr_list_1D, 1, &pDucBuf, mapType, PROC_APPM3);

	 //pRemoteBufHdr->pBuffer = pDucBuf[0];

	 
       //Buffer to be updated in the Buffer header that returns from the RPCSKEL is pDucBuf[0]

	 pBufferHeader->pBuffer = pDucBuf;
 

       }

//For 2D buffers, in phase1, retrive the ducati address (SSPtrs) for Y and UV buffers and send out buffer to ducati

       else if(MemMgr_IsMapped(pBuffer) && MemMgr_Is2DBlock(pBuffer)) {

       //retrieve ducati address

	 eError=PROXY_UTIL_GetUVPtr(hComponent,NULL,&nFrameHeight,&uvPtr, nPortIndex);

	 if(eError!=OMX_ErrorNone) {
	     DOMX_DEBUG("\nERROR WHILE GETTING FRAME HEIGHT");
	     goto leave;
  	 }

        mapType = ProcMgr_MapType_Tiler;
        MpuAddr_list_2D[0].mpuAddr = (UInt32)pBuffer;
        MpuAddr_list_2D[0].size = nFrameHeight * 4 * 1024;
        MpuAddr_list_2D[1].mpuAddr = (UInt32)pBuffer + nFrameHeight * 4 * 1024;
        MpuAddr_list_2D[1].size = nFrameHeight/2 * 4 * 1024;

        status = SysLinkMemUtils_map (&MpuAddr_list_2D[0], 1, &pDucBuf_2D[0],mapType, PROC_APPM3);
        
        status = SysLinkMemUtils_map (&MpuAddr_list_2D[1], 1, &pDucBuf_2D[1],mapType, PROC_APPM3);

        // pRemoteBufHdr->pBuffer = pDucBuf[0];
        // pRemoteBufHdr->pPlatformPrivate->pAuxBuf1 = pDucBuf[1];
        //This needs to be updated once the allocated Buffer Header returns

        pBufferHeader->pBuffer = pDucBuf_2D[0];
        ((OMX_TI_PLATFORMPRIVATE *)(pBufferHeader->pPlatformPrivate))->pAuxBuf1 = pDucBuf_2D[1];
        }

 
//TODO : remove this bec of SR buffers
/*      else if(!MemMgr_IsMapped(pBuffer)) {

            if(PortFormatIsNotYUV) {
                block.pixelFormat = PIXEL_FMT_PAGE;
                block.dim.len = nSizeBytes;
                block.stride = 0;
                block.ptr    = pBuffer;

	         pChironBuffer = MemMgr_Map(&block, 1);

                //retrieve ducati address
 
                mapType = ProcMgr_MapType_Tiler;
                MpuAddr_list_2D[0].mpuAddr = (UInt32)pChironBuffer;
                MpuAddr_list_2D[0].size = nSizeBytes;

	         status = SysLinkMemUtils_map (MpuAddr_list_2D, 1, &pDucBuf, mapType, PROC_APPM3);

                //pRemoteBufHdr->pBuffer = pDucBuf[0];
                //Need to update this in the Buffer ince it returns from RPCSKEL
   
	         pBufferHeader->pBuffer = pDucBuf;

}
*/
else {

DOMX_DEBUG("mapIn2Dmode not supported");

}

#endif //END OF TILER BUFFER MODE
	
	if(pBufferHeader==NULL) {
	    DOMX_DEBUG("\nBuffer Header Allocation failed");
		eError = OMX_ErrorInsufficientResources;
		goto leave;
	}
	
	//Remote call
	rpcError = RPC_UseBuffer(pComponentPrivate->remoteHandle,&pBufferHeader,nPortIndex,pAppPrivate,nSizeBytes,pBuffer,&pBufferMapped,&pBufHeaderRemote, &nCmdStatus);

	if(rpcError == RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("\n%s Yahoo!!Use Buffer Successful", __FUNCTION__);
		DOMX_DEBUG("\n %s Value of pBufHeaderRemote: 0x%x   LocalBufferHdr :0x%x",__FUNCTION__, pBufHeaderRemote,pBufferHeader);
		
		if(nCmdStatus == OMX_ErrorNone)
		{
			//Storing details of pBufferHeader/Mapped/Actual buffer address locally.
			pComponentPrivate->bufferList[currentBuffer].pBufHeader = pBufferHeader;
			pComponentPrivate->bufferList[currentBuffer].pBufHeaderRemote= pBufHeaderRemote;
			pComponentPrivate->bufferList[currentBuffer].pBufferMapped = pBufferMapped;
			pComponentPrivate->bufferList[currentBuffer].pBufferActual =  (OMX_U32)pBuffer;

			//replacing the pBuffer with local memory address space buffer
/*
			TODO : Ensure that pBuffer is Chiron side local address            
			pBufferHeader->pBuffer = pBuffer;
*/         
/* Replacing the Chiron side pBuffer pointer (this is the contiguos YUV buffer) while the one
present in the pBufferHeader->pBuffer is currently the mapped Y address. Also the pPlatformPrivate data needs to be restored here.
This is done simple replace from the buffer list - this needs to be added to the Table structure*/
			pBufferHeader->pBuffer = pBuffer;

			//caching actual content of pInportPrivate
			pComponentPrivate->bufferList[currentBuffer].actualContent = (OMX_U32)pBufferHeader->pInputPortPrivate;
			//filling pInportPrivate with the mapped address to be later used during ETB and FTB calls
			//Need to think on if we need a global actual buffer to mapped buffer data.
			pBufferHeader->pInputPortPrivate = (OMX_PTR )pBufferMapped;

			//keeping track of number of Buffers
			pComponentPrivate->numOfBuffers++;
			*ppBufferHdr = pBufferHeader;
		}
		else
		{
			TIMM_OSAL_Free((void *)pBufferHeader);
			eError = nCmdStatus;
		}
	}
	else
	{
		eError = OMX_ErrorUndefined;
	}
leave:
	return eError;
}



/* ===========================================================================*/
/**
 * @name PROXY_EmptyThisBuffer() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_EmptyThisBuffer(OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OMX_ERRORTYPE nCmdStatus;
	OMX_COMPONENTTYPE * pHandle =(OMX_COMPONENTTYPE *) hComponent;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=(PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	//extracting Mapped address from received Header. Earlier this was filled during USEBUFFER Call.
	OMX_U32 pBufferMapped =  (OMX_U32)pBufferHdr->pInputPortPrivate;
	//PROXY_BUFFER_INFO * pBufInfo=NULL;
	OMX_U32 count=0;
	OMX_U8 isMatchFound = 0;
	OMX_ERRORTYPE eError = OMX_ErrorNone;

	//This is check, but Ideally we should skip this.
	//These buffer calls should be independent of these loops in order to decrease latency
	for(count=0;count<pComponentPrivate->numOfBuffers;count++)
	{
		if(pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped)
		{
			DOMX_DEBUG("%s: Buffer Index of Match %d \n",__FUNCTION__,count);
			isMatchFound = 1;
			break;
		}
	}
	if(!isMatchFound)
	{
		DOMX_DEBUG("\n%s: Could not find the mapped address in component private buffer list",__FUNCTION__);
		eError = OMX_ErrorBadParameter;
		goto leave;
	}

	DOMX_DEBUG("\n%s:  pBufferHdr->pBuffer : 0x%x, pBufferHdr->nFilledLen : %d ",__FUNCTION__,pBufferHdr->pBuffer,pBufferHdr->nFilledLen);
	//Flushing the buffer
	RPC_FlushBuffer(pBufferHdr->pBuffer, pBufferHdr->nAllocLen);

	//Changing the local buffer address to remote mapped buffer address
//TODO : Remove this if sending the actual remote buffer    
//	pBufferHdr->pBuffer = (OMX_U8 *)pBufferMapped;

	RPC_EmptyThisBuffer(pComponentPrivate->remoteHandle, pBufferHdr, pComponentPrivate->bufferList[count].pBufHeaderRemote, &nCmdStatus);

	//changing back the local buffer address
	pBufferHdr->pBuffer = (OMX_U8 *)pComponentPrivate->bufferList[count].pBufferActual;

	DOMX_DEBUG("\n%s: Exiting: ",__FUNCTION__);

leave:
	return eError;

}



/* ===========================================================================*/
/**
 * @name PROXY_FillThisBuffer() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_FillThisBuffer(OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OMX_ERRORTYPE nCmdStatus;
	OMX_COMPONENTTYPE * pHandle =(OMX_COMPONENTTYPE *) hComponent;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=(PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	//extracting Mapped address from received Header. Earlier this was filled during USEBUFFER Call.
	OMX_U32 pBufferMapped =  (OMX_U32)pBufferHdr->pInputPortPrivate;
	//PROXY_BUFFER_INFO * pBufInfo=NULL;
	OMX_U32 count=0;
	OMX_U8 isMatchFound = 0;
	OMX_ERRORTYPE eError = OMX_ErrorNone;


	//This is check, but Ideally we should skip this.
	//These buffer calls should be independent of these loops in order to decrease latency
	for(count=0;count<pComponentPrivate->numOfBuffers;count++)
	{
		if(pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped)
		{
			DOMX_DEBUG("%s: Buffer Index of Match %d \n",__FUNCTION__,count);
			isMatchFound = 1;
			break;
		}
	}
	if(!isMatchFound)
	{
		DOMX_DEBUG("\n%s: Could not find the mapped address in component private buffer list",__FUNCTION__);
		eError = OMX_ErrorBadParameter;
		goto leave;
	}

	DOMX_DEBUG("\n%s:  pBufferHdr->pBuffer : 0x%x, pBufferHdr->nFilledLen : %d ",__FUNCTION__,pBufferHdr->pBuffer,pBufferHdr->nFilledLen);
	//Flushing the buffer
	RPC_FlushBuffer(pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
	//RPC_FlushBuffer(pComponentPrivate->realCore,pBufferHdr->pBuffer, pBufferHdr->nFilledLen);
	//Changing the local buffer address to remote mapped buffer address
	pBufferHdr->pBuffer = (OMX_U8 *)pBufferMapped;

	RPC_FillThisBuffer(pComponentPrivate->remoteHandle, pBufferHdr, pComponentPrivate->bufferList[count].pBufHeaderRemote,&nCmdStatus);

	//changing back the local buffer address
	pBufferHdr->pBuffer = (OMX_U8 *)pComponentPrivate->bufferList[count].pBufferActual;

	DOMX_DEBUG("\n%s: Exiting: ",__FUNCTION__);

leave:
	return eError;

}




/* ===========================================================================*/
/**
 * @name PROXY_FreeBuffer() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_FreeBuffer( OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_U32 nPortIndex, OMX_IN  OMX_BUFFERHEADERTYPE* pBufferHdr)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=NULL;
	OMX_COMPONENTTYPE * pHandle = hComponent;
	OMX_ERRORTYPE cmdStatus;
	OMX_U16 count;
	OMX_U8 isMatchFound = 0;
	OMX_U32 pBufferMapped;
       pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	pBufferMapped =  (OMX_U32)pBufferHdr->pInputPortPrivate;
	
	DOMX_DEBUG("Entered: %s\n",__FUNCTION__);
	
	//This is check, but Ideally we should skip this.
	//These buffer calls should be independent of these loops in order to decrease latency
	for(count=0;count<pComponentPrivate->numOfBuffers;count++)
	{
		if(pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped)
		{
			DOMX_DEBUG("%s: Buffer Index of Match %d \n",__FUNCTION__,count);
			isMatchFound = 1;
			break;
		}
	}
	if(!isMatchFound)
	{
		DOMX_DEBUG("\n%s: Could not find the mapped address in component private buffer list",__FUNCTION__);
		return OMX_ErrorBadParameter;
	}

	rpcError = RPC_FreeBuffer(pComponentPrivate->remoteHandle,nPortIndex,pComponentPrivate->bufferList[count].pBufHeaderRemote , &cmdStatus);
/*
TODO : Demap although not very critical
Unmapping
#ifdef TILER_BUFF

SysLinkMemUtils_unmap (UInt32 mappedAddr, ProcMgr_ProcId procId)
How do you access UV mapped buffers from here. (Y is accessbile from bufferlist under pBuffermapped. 
The UV is not, may be consider adding this to the table
*/
	
	if(rpcError == RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("%s Yahoo!! Free Buffer Successful\n", __FUNCTION__);
		eError = cmdStatus;
	}
	else{
		DOMX_DEBUG("%s [%d]: Warning: RPC Error",__FUNCTION__,__LINE__);
		eError=OMX_ErrorUndefined;
	}
	return eError;

}


/* ===========================================================================*/
/**
 * @name PROXY_SetParameter() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_SetParameter(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_INDEXTYPE nParamIndex, OMX_IN  OMX_PTR pComponentParameterStructure)
 {
	OMX_ERRORTYPE eError = OMX_ErrorNone;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=NULL;
	OMX_COMPONENTTYPE * pHandle = hComponent;
	OMX_ERRORTYPE cmdStatus;
	pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

	DOMX_DEBUG("Entered: %s\n",__FUNCTION__);

	RPC_SetParameter(pComponentPrivate->remoteHandle,nParamIndex,pComponentParameterStructure, &cmdStatus);

	if(cmdStatus == RPC_OMX_ErrorNone)
		DOMX_DEBUG("%s Yahoo!! Set Parameter Successful\n", __FUNCTION__);
	else
		eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
	return eError;

 }

 
/* ===========================================================================*/
/**
 * @name PROXY_GetParameter() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_GetParameter(OMX_IN  OMX_HANDLETYPE hComponent, OMX_IN  OMX_INDEXTYPE nParamIndex,  OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;
    PROXY_COMPONENT_PRIVATE* pComponentPrivate=NULL;
	OMX_COMPONENTTYPE * pHandle = hComponent;
	OMX_ERRORTYPE cmdStatus;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

    DOMX_DEBUG("Entered: %s\n",__FUNCTION__);

    //Checking the pointers
    if(pComponentParameterStructure == NULL)
        return OMX_ErrorBadParameter;

    rpcError = RPC_GetParameter(pComponentPrivate->remoteHandle,nParamIndex,pComponentParameterStructure, &cmdStatus);

       if(rpcError == RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("%s Yahoo!! Get Parameter Successful\n", __FUNCTION__);
		eError = cmdStatus;
       }
	else{
		DOMX_DEBUG("%s [%d]: Warning: RPC Error",__FUNCTION__,__LINE__);
		eError=OMX_ErrorUndefined;
	}
	return eError;

}


 
/* ===========================================================================*/
/**
 * @name PROXY_GetConfig() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_GetConfig (OMX_HANDLETYPE hComp,
				      OMX_INDEXTYPE nConfigIndex,
				      OMX_PTR pComponentConfigStructure)
{

	OMX_ERRORTYPE eError = OMX_ErrorNone;
	PROXY_COMPONENT_PRIVATE* pComponentPrivate=NULL;
	OMX_COMPONENTTYPE * pHandle = hComp;
	OMX_ERRORTYPE cmdStatus;

	pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	DOMX_DEBUG("Entered: %s\n",__FUNCTION__);

        //Checking the pointers
        if(pComponentConfigStructure == NULL)
	     return OMX_ErrorBadParameter;

	RPC_GetConfig(pComponentPrivate->remoteHandle, nConfigIndex,pComponentConfigStructure, &cmdStatus);

	if(cmdStatus == RPC_OMX_ErrorNone)
	    DOMX_DEBUG("%s Yahoo!! Get config Successful\n", __FUNCTION__);
	else
           eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
	
	return eError;
}


/* ===========================================================================*/
/**
 * @name PROXY_SetConfig() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_SetConfig(OMX_IN  OMX_HANDLETYPE hComponent, 
				     OMX_IN  OMX_INDEXTYPE nConfigIndex, 
				     OMX_IN  OMX_PTR pComponentConfigStructure)
 {

    OMX_ERRORTYPE eError                            = OMX_ErrorNone;    
    OMX_COMPONENTTYPE *pHandle                      = NULL;
    PROXY_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_ERRORTYPE cmdStatus;	
	
    DOMX_DEBUG("Entered: %s\n",__FUNCTION__);	
    pHandle = (OMX_COMPONENTTYPE*) hComponent;
    pComponentPrivate = (PROXY_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

	
    DOMX_DEBUG("Entered: %s\n",__FUNCTION__);
    RPC_SetConfig(pComponentPrivate->remoteHandle,nConfigIndex,pComponentConfigStructure, &cmdStatus);

    if(cmdStatus == RPC_OMX_ErrorNone)
		DOMX_DEBUG("%s Yahoo!! Set Config Successful\n", __FUNCTION__);
    else
		eError=OMX_ErrorUndefined; //Need to map correct OMX Error error

    return eError;	

 }

 
/* ===========================================================================*/
/**
 * @name PROXY_GetState() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_GetState(OMX_IN  OMX_HANDLETYPE hComponent,
				    OMX_OUT OMX_STATETYPE* pState)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;	 
  OMX_ERRORTYPE cmdStatus;
  RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;

  OMX_COMPONENTTYPE * pHandle = hComponent;
  PROXY_COMPONENT_PRIVATE* pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;


  DOMX_DEBUG("\nEnter: %s",__FUNCTION__);
  
  if(pState == NULL)
    return OMX_ErrorBadParameter;      

  rpcError = RPC_GetState(pComponentPrivate->remoteHandle, pState, &cmdStatus);

  DOMX_DEBUG("\n%s: Returned from RPC_GetState",__FUNCTION__);
  if(rpcError == RPC_OMX_ErrorNone)
  {
  	DOMX_DEBUG("\n%s Yahoo!! Get Parameter Successful", __FUNCTION__);
	eError = cmdStatus;
  }
  else
  {
  	DOMX_DEBUG("\n%s [%d]: Warning: RPC Error",__FUNCTION__,__LINE__);
	eError=OMX_ErrorUndefined;
  }
  
   return eError;

}



/* ===========================================================================*/
/**
 * @name PROXY_SendCommand() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_SendCommand(OMX_IN  OMX_HANDLETYPE hComponent,
				   OMX_IN  OMX_COMMANDTYPE eCmd,
				   OMX_IN  OMX_U32 nParam,
				   OMX_IN  OMX_PTR pCmdData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	
  PROXY_COMPONENT_PRIVATE* pComponentPrivate = NULL;
  OMX_COMPONENTTYPE * pHandle = hComponent;
  OMX_ERRORTYPE cmdStatus;

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	
  DOMX_DEBUG("\nEnter: %s",__FUNCTION__);	       	       

  rpcError = RPC_SendCommand (pComponentPrivate->remoteHandle, eCmd, nParam, pCmdData, &cmdStatus);
    
  if(rpcError == RPC_OMX_ErrorNone) {
    DOMX_DEBUG("\n%s Yahoo!! PROXY_Send Successful", __FUNCTION__);
     eError = cmdStatus;
  }
  else {
    eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
  }

  DOMX_DEBUG("\nLeaving: %s",__FUNCTION__);	
  return eError;

}



/* ===========================================================================*/
/**
 * @name PROXY_GetComponentVersion() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_GetComponentVersion(
	    OMX_IN  OMX_HANDLETYPE hComponent,
	    OMX_OUT OMX_STRING pComponentName,
	    OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
	    OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
	    OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
  
  OMX_ERRORTYPE eError       = OMX_ErrorNone;
  
  PROXY_COMPONENT_PRIVATE* pComponentPrivate = NULL;
  OMX_COMPONENTTYPE * pHandle = hComponent;
  OMX_ERRORTYPE uCmdStatus;
  RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	
  DOMX_DEBUG("\nEnter: %s",__FUNCTION__);	       	       

  rpcError = RPC_GetComponentVersion(pComponentPrivate->remoteHandle,
			  pComponentName,
			  pComponentVersion,
			  pSpecVersion,
			  pComponentUUID,
			  &uCmdStatus);

    if(rpcError==RPC_OMX_ErrorNone) {

	 if(uCmdStatus == OMX_ErrorNone) {
            DOMX_DEBUG("\n %s : PROXY_GetComponentVersion Successful\n", __FUNCTION__);
  }
  else {
    eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
  }
    }
    else {
        DOMX_DEBUG("\n %s: Error in RPC",__FUNCTION__);
	 eError = OMX_ErrorUndefined;
    }

	
  DOMX_DEBUG("\nLeaving: %s",__FUNCTION__);	
		
  return  eError;
}



/* ===========================================================================*/
/**
 * @name PROXY_GetExtensionIndex() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_GetExtensionIndex( OMX_IN  OMX_HANDLETYPE hComponent,
					      OMX_IN  OMX_STRING cParameterName,
					      OMX_OUT OMX_INDEXTYPE* pIndexType) 
{

  OMX_ERRORTYPE eError       = OMX_ErrorNone;
  
  PROXY_COMPONENT_PRIVATE* pComponentPrivate = NULL;
  OMX_COMPONENTTYPE * pHandle = hComponent;
  OMX_ERRORTYPE uCmdStatus;
  RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	
  DOMX_DEBUG("\nEnter: %s",__FUNCTION__);	       	       

  
  rpcError = RPC_GetExtensionIndex(pComponentPrivate->remoteHandle,
			cParameterName,
			pIndexType,		
			&uCmdStatus);

    if(rpcError==RPC_OMX_ErrorNone) {
        if(uCmdStatus == RPC_OMX_ErrorNone) {
            DOMX_DEBUG("\n%s : PROXY_GetExtensionIndex Successful", __FUNCTION__);
        }
        else {
           DOMX_DEBUG("\n%s PROXY_GetExtensionIndex FAIL\n", __FUNCTION__);
           eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
        }
		
	}

    else {
  	    DOMX_DEBUG("\n %s : Error in RPC",__FUNCTION__);
	    eError=OMX_ErrorUndefined; //Need to map correct OMX Error error
    }

  DOMX_DEBUG("\nLeaving: %s",__FUNCTION__);	
  return  eError;
}




/* ===========================================================================*/
/**
 * @name PROXY_ComponentRoleEnum() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_ComponentRoleEnum(OMX_IN OMX_HANDLETYPE hComponent,
					     OMX_OUT OMX_U8 *cRole,
					     OMX_IN OMX_U32 nIndex)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  DOMX_DEBUG("\nEntering: %s",__FUNCTION__);	
  DOMX_DEBUG("\n EMPTY IMPLEMENTATION ");	
  DOMX_DEBUG("\nLeaving: %s",__FUNCTION__);	
  goto EXIT;

EXIT:  
    return eError;
};




/* ===========================================================================*/
/**
 * @name PROXY_AllocateBuffer() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_AllocateBuffer( 
            OMX_IN OMX_HANDLETYPE hComponent, 
            OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr, 
            OMX_IN OMX_U32 nPortIndex, 
            OMX_IN OMX_PTR pAppPrivate, 
            OMX_IN OMX_U32 nSizeBytes)
{
        OMX_ERRORTYPE eError = OMX_ErrorNone;
	    OMX_BUFFERHEADERTYPE * pBufferHeader = NULL;
	    OMX_ERRORTYPE nCmdStatus;
	    OMX_U32 pBufferMapped;
	    OMX_U32 pBufHeaderRemote;
	    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	    OMX_U32 currentBuffer;
	    OMX_U8* pBuffer;
	    OMX_TI_PLATFORMPRIVATE * pPlatformPrivate;
#ifdef TILER_BUFF
   	    ProcMgr_MapType mapType;
	    SyslinkMemUtils_MpuAddrToMap MpuAddr_list_1D[1];
           SyslinkMemUtils_MpuAddrToMap MpuAddr_list_2D[2];
	    MemAllocBlock block;
     	    OMX_U32 pDucBuf,pDucBuf_2D[2],numBlocks;
	    OMX_PTR pChironBuffer;
	    OMX_U32 nFrameHeight;
	    DSPtr dsptr[2];
	    bytes_t lengths[2],uvPtr;

OMX_U32 i = 0;        
#endif
        DOMX_DEBUG("\n Entered %s ____ \n",__FUNCTION__);
        
           OMX_COMPONENTTYPE * pHandle =(OMX_COMPONENTTYPE *) hComponent;
	    PROXY_COMPONENT_PRIVATE* pComponentPrivate=(PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;;
           currentBuffer = pComponentPrivate->numOfBuffers;
	    pBufferHeader = (OMX_BUFFERHEADERTYPE*)TIMM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_INT);

	    if(pBufferHeader==NULL) {
	        DOMX_DEBUG("Allocation of Buffer Header structure failed");
		 eError = OMX_ErrorUndefined;
		 goto EXIT;
  	    } 
 		
	    pPlatformPrivate = (OMX_TI_PLATFORMPRIVATE *)TIMM_OSAL_Malloc(sizeof(OMX_TI_PLATFORMPRIVATE),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_INT);

	    if(pPlatformPrivate==NULL) {
	        DOMX_DEBUG("Allocation of Platform Private structure failed");
		 eError = OMX_ErrorUndefined;
		 goto EXIT;
	    }
			
		
        /* Storing the original Content of pPlatformPrivate*/
    	//	pComponentPrivate->bufferList[currentBuffer].pPlatformPrivateClient =pBufferHeader->pPlatformPrivate;
       pBufferHeader->pPlatformPrivate = pPlatformPrivate;
  
	    
       DOMX_DEBUG("\n Calling RPC \n");
       
        rpcError = RPC_AllocateBuffer(pComponentPrivate->remoteHandle,&pBufferHeader,nPortIndex,
                               &pBufHeaderRemote,&pBufferMapped,pAppPrivate,nSizeBytes,&nCmdStatus);

     	if(rpcError == RPC_OMX_ErrorNone) {
	        DOMX_DEBUG("\n%s Yahoo!!Allocate Buffer Successful\n", __FUNCTION__);
		 DOMX_DEBUG("\n%s Value of pBufHeaderRemote: 0x%x   LocalBufferHdr :0x%x\n",__FUNCTION__, pBufHeaderRemote,pBufferHeader);
		//DOMX_DEBUG("%s Value of pBufHeaderRemote: 0x%x   LocalBufferHdr :0x%x\n",__FUNCTION__, pBufHeaderRemote,pBufferHeader);
		if(nCmdStatus == OMX_ErrorNone)
		{
                     pBuffer = pBufferHeader->pBuffer;
			//Storing details of pBufferHeader/Mapped/Actual buffer address locally.

/* ################################################### TILER BUFFER MODE ############################ */
#ifdef TILER_BUFF

        // After the RPC AllocateBuffer call the BufferHeader->pBuffer and pPlatforprivate->AuxBuff1 contains the Required Y and UV pointers (this is providef or by the OMXBASE)
        //If Tiler 1D buffer, get corresponding ducati address and send out buffer to ducati

        //if(MemMgr_Is1DBlock(pBuffer)) {
         if(((OMX_TI_PLATFORMPRIVATE *)pBufferHeader->pPlatformPrivate)->pAuxBuf1 == NULL)
        {
            DOMX_DEBUG("\nOne component buffer\n");
            dsptr[0]=pBuffer;
            numBlocks = 1;
            lengths[0] = (4 * 1024) * ((pBufferHeader->nAllocLen + ((4 * 1024) - 1)) / (4 * 1024));
         }


//For 2D buffers, in phase1, retrive the ducati address (SSPtrs) for Y and UV buffers and send out buffer to ducati

       //else if(MemMgr_Is2DBlock(pBuffer)) {
       else
       {
        DOMX_DEBUG("\nTwo component buffers\n");
           dsptr[0]=pBuffer;
           eError=PROXY_UTIL_GetUVPtr(hComponent,pBufferHeader,&nFrameHeight,&uvPtr, nPortIndex);
           DOMX_DEBUG("\nRecd. uv ptr = 0x%x\n", uvPtr);
	    dsptr[1] = uvPtr;		   
           lengths[0]= nFrameHeight* 4 * 1024;
	    lengths[1]= nFrameHeight/2 * 4 * 1024;
           numBlocks = 2;
        }

      
        //Map back to chiron
        DOMX_DEBUG("\nNumBlocks = %d\n", numBlocks);
        for (i = 0; i < numBlocks; i++)
        {
            DOMX_DEBUG("\ndsptr[%d] = 0x%x\n", i, dsptr[i]);
            DOMX_DEBUG("\nlength[%d] = %d\n", i, lengths[i]);
        }
        pChironBuffer = tiler_assisted_phase1_D2CReMap(numBlocks,dsptr,lengths);
        //populate the buffer header
        pBuffer=pChironBuffer;
        
//TODO : pBufferHeader->pBuffer = pBuffer

       pBufferHeader->pBuffer = pBuffer; // restoring the Chiron pointer here for clinet to access
#endif //END OF TILER BUFFER MODE
/*
pBufferMapped here will contain the Y pointer (basically the unity mapped pBuffer)
pBufferHeaderRemote is the header that contains both Y, UV pointers 
*/
/* ################################################### TILER BUFFER MODE ############################ */
			pComponentPrivate->bufferList[currentBuffer].pBufHeader = pBufferHeader;
			pComponentPrivate->bufferList[currentBuffer].pBufHeaderRemote= pBufHeaderRemote;
			pComponentPrivate->bufferList[currentBuffer].pBufferMapped = pBufferMapped;
		       pComponentPrivate->bufferList[currentBuffer].pBufferActual =  (OMX_U32)pBuffer;
			
			//caching actual content of pInportPrivate
			pComponentPrivate->bufferList[currentBuffer].actualContent = (OMX_U32)pBufferHeader->pInputPortPrivate;
			//filling pInportPrivate with the mapped address to be later used during ETB and FTB calls
			//Need to think on if we need a global actual buffer to mapped buffer data.
			pBufferHeader->pInputPortPrivate = (OMX_PTR )pBufferMapped;

			//keeping track of number of Buffers
			pComponentPrivate->numOfBuffers++;
			*ppBufferHdr = pBufferHeader;


		}
		else
		{
			TIMM_OSAL_Free((void *)pBufferHeader);
			eError = nCmdStatus;
		}
	}
	
	else
	{
		eError = OMX_ErrorUndefined;
	}

EXIT:

	return eError;
}


/* ===========================================================================*/
/**
 * @name PROXY_ComponentTunnelRequest() 
 * @brief 
 * @param void 
 * @return OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE PROXY_ComponentTunnelRequest( 
        OMX_IN  OMX_HANDLETYPE hComp, 
        OMX_IN  OMX_U32 nPort, 
        OMX_IN  OMX_HANDLETYPE hTunneledComp, 
        OMX_IN  OMX_U32 nTunneledPort, 
        OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup)
        
{
    OMX_ERRORTYPE eError       = OMX_ErrorNone;
    //PROXY_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_COMPONENTTYPE * pHandle = hComp;
    OMX_ERRORTYPE CmdStatus;

//    pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
	
    DOMX_DEBUG("\nEnter: %s",__FUNCTION__);	       	       



   eError = RPC_ComponentTunnelRequest((RPC_OMX_HANDLE)hComp,nPort,(RPC_OMX_HANDLE)hTunneledComp,nTunneledPort,&pTunnelSetup,&CmdStatus);

  if(CmdStatus == RPC_OMX_ErrorNone) {
    DOMX_DEBUG("\n___NEW_API _ %s Successful",__FUNCTION__);
  }
  else {
    DOMX_DEBUG("\n___NEW_API _ %s Failed",__FUNCTION__);
    eError = OMX_ErrorUndefined; //Need to map correct OMX Error error
  }

  return eError;
}



#ifdef TILER_BUFF
 OMX_ERRORTYPE PROXY_UTIL_GetUVPtr(OMX_HANDLETYPE hComponent,OMX_BUFFERHEADERTYPE * pBufferHeader,OMX_U32 * nFrameHeight, bytes_t * uvPtr, OMX_U32 nPortIndex)
 {
        OMX_ERRORTYPE eError = OMX_ErrorNone;
        PROXY_COMPONENT_PRIVATE* pComponentPrivate=NULL;
	 OMX_COMPONENTTYPE * pHandle = hComponent;
 	 OMX_ERRORTYPE cmdStatus;
	 RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        pComponentPrivate = (PROXY_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;
 	 OMX_PARAM_PORTDEFINITIONTYPE portDef;
	 OMX_PTR pComponentParameterStructure;
	 OMX_U8 * pBuffer;
	 OMX_PTR pUVBuffer;

	 pComponentParameterStructure = &portDef;

        DOMX_DEBUG("Entered: %s\n",__FUNCTION__);

	 /*initializing Structure*/
 	 portDef.nSize = sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
        portDef.nVersion.s.nVersionMajor = 0x1;     
        portDef.nVersion.s.nVersionMinor = 0x1;     
        portDef.nVersion.s.nRevision  = 0x0;       
        portDef.nVersion.s.nStep   = 0x0;
        
        portDef.nPortIndex = nPortIndex;
 
        rpcError = RPC_GetParameter(pComponentPrivate->remoteHandle,OMX_IndexParamPortDefinition,pComponentParameterStructure, &cmdStatus);

        if(rpcError == RPC_OMX_ErrorNone) {
	     DOMX_DEBUG("\n PROXY_UTIL Get Parameter Successful\n", __FUNCTION__);
	     eError = cmdStatus;
        }
	 else{
		DOMX_DEBUG("\n%s [%d]: Warning: RPC Error",__FUNCTION__,__LINE__);
	 	eError=OMX_ErrorUndefined;
	 }

	 if(cmdStatus==OMX_ErrorNone) {
//TODO : Change for sample 
//TODO : If eDomain = Video then do this else something alse for image
        if(portDef.eDomain==OMX_PortDomainVideo) {
            *nFrameHeight = portDef.format.video.nFrameHeight;
			
		}
		else if(portDef.eDomain==OMX_PortDomainImage) {
		
		    DOMX_DEBUG("\nImage DOMAIN TILER SUPPORT");
		}
        else if(portDef.eDomain==OMX_PortDomainAudio) {
		    DOMX_DEBUG("\nAudio DOMAIN TILER SUPPORT");
		}
		else if(portDef.eDomain==OMX_PortDomainOther) {
		    DOMX_DEBUG("\nOther DOMAIN TILER SUPPORT");
		}
		else { //this is the sample component test
        //Temporary - just to get check functionality
		    DOMX_DEBUG("\nSample component TILER SUPPORT");
            *nFrameHeight = 4;
		}
		if(pBufferHeader) {
		pUVBuffer = ((OMX_TI_PLATFORMPRIVATE *)pBufferHeader->pPlatformPrivate)->pAuxBuf1;
        DOMX_DEBUG("\nIn util uv ptr = 0x%x\n", pUVBuffer);
		*uvPtr = (bytes_t)pUVBuffer;
        DOMX_DEBUG("\nIn util value of uv ptr after typecast = 0x%x\n", *uvPtr);
		}
			
			
	 }
	 else {
	 	DOMX_DEBUG("\n ERROR IN RECOVERING UV POINTER");
	 }
		

	
	return eError;
 	}
#endif
