/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc_stub.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework RPC Stub implementations.
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


/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <string.h>
#include <stdio.h>

#include "omx_rpc.h"
#include "omx_rpc_utils.h"
#include "omx_proxy_common.h"
#include "omx_rpc_stub.h"
#include "omx_ti_common.h"
#include <timm_osal_interfaces.h>
#include <MultiProc.h>

extern OMX_U8 rpcOmxInit; //this flag is to make sure we run RPC_Init() once during initialization
extern OMX_U32 PACKET_SIZE; // different packet sizes required for INTER-M3 case and MPU-APPM3

/* ===========================================================================*/
/**
 * @name RPC_EmptyThisBuffer() 
 * @brief 
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/

RPC_OMX_ERRORTYPE RPC_EmptyThisBuffer(RPC_OMX_HANDLE remoteHandle, OMX_BUFFERHEADERTYPE* pBufferHdr, OMX_U32 BufHdrRemote, OMX_ERRORTYPE * nCmdStatus)
{
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_BYTE * msgBody = NULL;
	OMX_U32 offset = 0;
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
        
    DOMX_DEBUG("\n Entering: %s __________ BUFFER READ-WRITE ", __FUNCTION__);
    
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_EMPTYTHISBUFFER].rpcFxnIdx; //HACK !!! THE Index required to find whether it is ETB or FTB is obtained from the RPC command ID
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
	
	if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
		goto EXIT;
    }
		
    rcmMsg->fxnIdx = fxnIdx;
    pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);  
    msgBody = &pRPCMsg->msgBody[0];
    // Marshalling RPC Messaging fields in RPCMsg struct
    
	TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
	offset+=sizeof(RPC_OMX_HANDLE);
		
	TIMM_OSAL_Memcpy(msgBody+offset,&BufHdrRemote, sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Note: Could avoid entire buffer header copy by just copying relavent information.
//TODO : Just copy relevant info OR on skel update only relevant fielsd in Ducati bufhdr.
/* relevant information includes nFilledlen, noffset, flags (same as callback) */
//	TIMM_OSAL_Memcpy(msgBody+offset, pBufferHdr, sizeof(OMX_BUFFERHEADERTYPE));

    TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nFilledLen),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nOffset
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nOffset),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nFlags
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nFlags),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	DOMX_DEBUG("\n pBufferHdr = %x BufHdrRemote %x",pBufferHdr,BufHdrRemote);
	
    status = RcmClient_exec(rcmHndl, rcmMsg);
    
    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
    
	
	
	if(rpcError==RPC_OMX_ErrorNone)
	{

		if(pRPCMsg->msgHeader.nRPCCmdStatus== RPC_OMX_ErrorNone)
		{
			if(pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone)
			 {
				DOMX_DEBUG("\n %s: OMX Error received: 0x%x\n",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			 }
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
		else
		{
			DOMX_DEBUG("%s: RPC Error received: 0x%x\n",__FUNCTION__,pRPCMsg->msgHeader.nRPCCmdStatus);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
			
	}
	else
	{
			DOMX_DEBUG("\n %s: Error Sending Message: 0x%x\n",__FUNCTION__,rpcError);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}

	//Freeing Msg Frame
	 RcmClient_free(rcmHndl, rcmMsg);

	EXIT:
        
        return rpcError;
}


/* ===========================================================================*/
/**
 * @name RPC_FillThisBuffer() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_FillThisBuffer(RPC_OMX_HANDLE remoteHandle, OMX_BUFFERHEADERTYPE* pBufferHdr, OMX_U32 BufHdrRemote, OMX_ERRORTYPE * nCmdStatus)
{
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_BYTE * msgBody = NULL;
	OMX_U32 offset = 0;
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
        
    DOMX_DEBUG("\n Entering: %s __________ BUFFER READ-WRITE ", __FUNCTION__);
    
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_FILLTHISBUFFER].rpcFxnIdx; //HACK !!! THE Index required to find whether it is ETB or FTB is obtained from the RPC command ID

    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
	
	if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
		goto EXIT;
    }
		
    rcmMsg->fxnIdx = fxnIdx;
    pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);   
    msgBody = &pRPCMsg->msgBody[0];
    // Marshalling RPC Messaging fields in RPCMsg struct
    
	TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
    offset+=sizeof(RPC_OMX_HANDLE);
		
	TIMM_OSAL_Memcpy(msgBody+offset,&BufHdrRemote, sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Note: Could avoid entire buffer header copy by just copying relavent information.
//Note: Could avoid entire buffer header copy by just copying relavent information.
//TODO : Just copy relevant info OR on skel update only relevant fielsd in Ducati bufhdr.
/* relevant information includes nFilledlen, noffset, flags (same as callback) */
	//	TIMM_OSAL_Memcpy(msgBody+offset, pBufferHdr, sizeof(OMX_BUFFERHEADERTYPE));
    
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nFilledLen),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nOffset
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nOffset),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nFlags
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBufferHdr->nFlags),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	DOMX_DEBUG("\n pBufferHdr = %x BufHdrRemote %x",pBufferHdr,BufHdrRemote);
	
	status = RcmClient_exec(rcmHndl, rcmMsg);
	
	if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
	    rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;
        }
	
	if(rpcError==RPC_OMX_ErrorNone)
	{

		if(pRPCMsg->msgHeader.nRPCCmdStatus== RPC_OMX_ErrorNone)
		{
			if(pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone)
			 {
				DOMX_DEBUG("\n %s: OMX Error received: 0x%x\n",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			 }
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
		else
		{
			DOMX_DEBUG("%s: RPC Error received: 0x%x\n",__FUNCTION__,pRPCMsg->msgHeader.nRPCCmdStatus);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
			
	}
	else
	{
			DOMX_DEBUG("\n %s: Error Sending Message: 0x%x\n",__FUNCTION__,rpcError);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}
	
	//Freeing Msg Frame
	 RcmClient_free(rcmHndl, rcmMsg);

	EXIT:
        
        return rpcError;
}


/* ===========================================================================*/
/**
 * @name RPC_SetParameter() 
 * @brief 
 * @param hComp: This is the handle on the Remote core, the proxy will replace
 *        it's handle with actual OMX Component handle that recides on the specified core
 * @param nParamIndex: same as nParamIndex received at the Proxy
 * @param pCompParam: same as nPCompParam recieved at the Proxy
 * @param nCmdStatus: This is return value that will be supplied by Proxy to the caller.
          This is actual return value returned by the Remote component 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SetParameter(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nParamIndex,OMX_PTR pCompParam, OMX_ERRORTYPE * nCmdStatus)
{

	RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	RPC_OMX_SIZE structSize;
    RPC_OMX_BYTE * msgBody;
    OMX_U32 offset=0;
	OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
    
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_SET_PARAMETER].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
	
    if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
		goto EXIT;
    }
    rcmMsg->fxnIdx = fxnIdx;
    
    DOMX_DEBUG("\n Entering: %s\n", __FUNCTION__);

	if(rpcError != RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("%s:	 Error Getting Frame\n",__FUNCTION__);
		goto EXIT;
	}

       pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);   
       msgBody = &pRPCMsg->msgBody[0];

        //Filling msgBody with input arguements
        //FORMAT AS FOLLOWS
        /*                                                                     | SET_PARAM_DATA_OFFSET
                                                                                V
                        _____________________________________________________________________________
        msgBody: | remoteHandle | nParamIndex |offset|---- data: pCompParam strcuture is filled here ----|
                        -----------------------------------------------------------------------------
        */

        //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &hComp,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        //Marshalling nParamIndex value
        TIMM_OSAL_Memcpy((msgBody+offset),&nParamIndex,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);     //assuming sizeof (OMX_INDEXTYPE) to be of OMX_U32

        //Retrieve size based on the nParamIndex in order to copy the struct into the RPCMsg argStruct union 
	    //rpcError = RPC_GetStructSize(nParamIndex,&structSize);
	    rpcError = RPC_GetStructSize(pCompParam,&structSize);

        //marshall the offset of the data structure(skip the arguments)
        *(OMX_U32 *)(msgBody+offset) = (OMX_U32)SET_PARAM_DATA_OFFSET;
        offset+=sizeof(OMX_U32);

        //*************************************************************************
        //IMP: TO DO
        //Doing shallow copy; need to look for spl case where we need to more specific deep copies
        //*************************************************************************
        if(rpcError!=RPC_OMX_ErrorNone && structSize!=0){
		return rpcError;
			 }
        else{
		TIMM_OSAL_Memcpy((msgBody+ (OMX_U32)SET_PARAM_DATA_OFFSET),pCompParam,structSize);		
		   	 }
	
    	//RcmClient_exec 
    	status = RcmClient_exec(rcmHndl, rcmMsg);
    	
    	if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
    	
   	    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
	
    	if(rpcError == RPC_OMX_ErrorNone) {
		    if(pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone) {
	            DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
                }
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}

    	else {
		    DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
    		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
   		}
   		
	//Freeing Msg Frame
	RcmClient_free(rcmHndl, rcmMsg);

	EXIT:
		return rpcError;

}

/* ===========================================================================*/
/**
 * @name RPC_GetParameter() 
 * @brief 
 * @param hComp: This is the handle on the Remote core, the proxy will replace
 *        it's handle with actual OMX Component handle that recides on the specified core
 * @param nParamIndex: same as nParamIndex received at the Proxy
 * @param pCompParam: same as nPCompParam recieved at the Proxy
 * @param nCmdStatus: This is return value that will be supplied by Proxy to the caller.
          This is actual return value returned by the Remote component 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetParameter(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nParamIndex,OMX_PTR pCompParam, OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_BYTE * msgBody;
        OMX_U32 offset=0;
        RPC_OMX_SIZE structSize;
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
        
        OMX_S16 status;
        RPC_INDEX fxnIdx;

        DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
        
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_PARAMETER].rpcFxnIdx;
        
        //Allocating remote command message
        
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
		
        if (rcmMsg == NULL) {
		    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
		    rpcError = RPC_OMX_ErrorInsufficientResources;
			goto EXIT;
        }
        rcmMsg->fxnIdx = fxnIdx;
        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);   
        msgBody = &pRPCMsg->msgBody[0];

        //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &hComp,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        //Marshalling nParamIndex value
        TIMM_OSAL_Memcpy((msgBody+offset),&nParamIndex,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);     //assuming sizeof (OMX_INDEXTYPE) to be of int
           
          //marshall the offset of the data structure(skip the arguments)
        *(OMX_U32 *)(msgBody+offset) = (OMX_U32)GET_PARAM_DATA_OFFSET;
        DOMX_DEBUG("RPC_GetParam: Data offset : %d\n",*(OMX_U32 *)(msgBody+offset));
        offset+=sizeof(OMX_U32);
		
        rpcError = RPC_GetStructSize(pCompParam,&structSize);

        DOMX_DEBUG("RPC_GetParam: Struct Size retrieve : %d\n",structSize);
        
        if(rpcError!=RPC_OMX_ErrorNone && structSize!=0){
		return rpcError;
			 }
        else{
            //even for a GetParameter we are filling some fields in the pCompParam structure
            //hence we are copying the entire stucture as is.
            TIMM_OSAL_Memcpy((msgBody+ (OMX_U32)GET_PARAM_DATA_OFFSET),pCompParam,structSize);
	}
         
         
        //RcmClient_exec
        status = RcmClient_exec(rcmHndl, rcmMsg);
        
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }


	    if(rpcError == RPC_OMX_ErrorNone)
	    {
	 	
		if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone)
         	{
         		if(pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone)
         		{
				TIMM_OSAL_Memcpy(pCompParam,(msgBody+ (OMX_U32)GET_PARAM_DATA_OFFSET),structSize);
				
         		}
			else
			{
				DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			}
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
         	}
		else
		{
			DOMX_DEBUG("%s: RPC Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nRPCCmdStatus);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
			 
	 }
	 else
	{
		 DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
		 *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}

	     //Freeing Msg Frame
	     //RcmClient_free
	     RcmClient_free(rcmHndl, rcmMsg);
	
	EXIT:
		return rpcError;    
}

/* ===========================================================================*/
/**
 * @name RPC_SetConfig() 
 * @brief 
 * @param hComp: This is the handle on the Remote core, the proxy will replace
 *        it's handle with actual OMX Component handle that recides on the specified core
 * @param nParamIndex: same as nParamIndex received at the Proxy
 * @param pCompParam: same as nPCompParam recieved at the Proxy
 * @param nCmdStatus: This is return value that will be supplied by Proxy to the caller.
          This is actual return value returned by the Remote component 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SetConfig(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nConfigIndex,OMX_PTR pCompConfig, OMX_ERRORTYPE * nCmdStatus)
{

	RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	RPC_OMX_SIZE structSize=0;
    RPC_OMX_BYTE * msgBody=NULL;
    OMX_U32 offset=0;
	
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    OMX_S16 status;
    RPC_INDEX fxnIdx;    
               
    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
    
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_SET_CONFIG].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    
	if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
        rpcError = RPC_OMX_ErrorInsufficientResources;
		goto EXIT;
    }
	
    rcmMsg->fxnIdx = fxnIdx;

    DOMX_DEBUG("[%s] (%d) message_frame \n", __FUNCTION__,__LINE__);
	if(rpcError != RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("%s: Error Getting Frame\n",__FUNCTION__);
		goto EXIT;
	}

	pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
    msgBody = &pRPCMsg->msgBody[0];
	   
   //Marshalling hComp handle
    TIMM_OSAL_Memcpy(msgBody, &hComp,sizeof(RPC_OMX_HANDLE));
    offset+=sizeof(RPC_OMX_HANDLE);

    //Marshalling nConfigIndex value
    TIMM_OSAL_Memcpy((msgBody+offset),&nConfigIndex,sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);     

    *(OMX_U32 *)(msgBody+offset) = (OMX_U32)SET_CONFIG_DATA_OFFSET;
    offset+=sizeof(OMX_U32);
	
	rpcError = RPC_GetStructSize(pCompConfig,&structSize);
    DOMX_DEBUG("RPC_GetParam: Struct Size retrieve : %d\n",structSize);

	
	

        
    //*************************************************************************
        //IMP: TO DO
        //Doing shallow copy; need to look for spl case where we need to more specific deep copies
    //*************************************************************************
       if(rpcError!=RPC_OMX_ErrorNone && structSize!=0){
		return rpcError;
			 }
        
        else {
		TIMM_OSAL_Memcpy((msgBody+ (OMX_U32)SET_CONFIG_DATA_OFFSET),pCompConfig,structSize);
			}

	
	    //RcmClient_exec
        status = RcmClient_exec(rcmHndl, rcmMsg);
        
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }

	if(rpcError == RPC_OMX_ErrorNone)
	{
	       //DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);

		 if(pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone)
		 {
			DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
		 }
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}
	else
	{
		 DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}

	//Freeing Msg Frame
	//RcmClient_free
	 RcmClient_free(rcmHndl, rcmMsg);
       
	EXIT:
		return rpcError;

}

/* ===========================================================================*/
/**
 * @name RPC_GetConfig() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetConfig(RPC_OMX_HANDLE hComp,OMX_INDEXTYPE nConfigIndex,OMX_PTR pCompConfig, OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_BYTE * msgBody;
        OMX_U32 offset=0;
        RPC_OMX_SIZE structSize;
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
        
        OMX_S16 status;
        RPC_INDEX fxnIdx;
        
        DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
        
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_CONFIG].rpcFxnIdx;
        
        //Allocating remote command message
        
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        
		if (rcmMsg ==NULL) {
		    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
		    rpcError = RPC_OMX_ErrorInsufficientResources;
			goto EXIT;
        }
		
        rcmMsg->fxnIdx = fxnIdx;
        
        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
        msgBody = &pRPCMsg->msgBody[0];
           
        //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &hComp,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        //Marshalling nConfigIndex value
        TIMM_OSAL_Memcpy((msgBody+offset),&nConfigIndex,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);     //assuming sizeof (OMX_INDEXTYPE) to be of int
           
         //marshall the offset of the data structure(skip the arguments)
        *(OMX_U32 *)(msgBody+offset) = (OMX_U32)GET_CONFIG_DATA_OFFSET;
        DOMX_DEBUG("RPC_GetConfig: Data offset : %d\n",*(OMX_U32 *)(msgBody+offset));

        offset+=sizeof(OMX_U32);
        rpcError = RPC_GetStructSize(pCompConfig,&structSize);
        DOMX_DEBUG("RPC_GetParam: Struct Size retrieve : %d\n",structSize);

         //Retrieve size based on the nConfigIndex in order to copy the struct into the RPCMsg argStruct union   

         if(rpcError!=RPC_OMX_ErrorNone && structSize!=0){
		      return rpcError;
		      }
         else {
            //even for a GetConfig we are filling some fields in the pCompConfig structure
            //hence we are copying the entire stucture as is.
            TIMM_OSAL_Memcpy((msgBody+ (OMX_U32)GET_CONFIG_DATA_OFFSET),pCompConfig,structSize);
            }

         //RcmClient_exec
         status = RcmClient_exec(rcmHndl, rcmMsg);
         
         if (status < 0) {
	     DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
         rpcError = RPC_OMX_RCM_ErrorExecFail;
         goto EXIT;        
         }
	
         if(rpcError == RPC_OMX_ErrorNone) {

             if( pRPCMsg->msgHeader.nRPCCmdStatus== RPC_OMX_ErrorNone) {
                 if( pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone) {
		             TIMM_OSAL_Memcpy(pCompConfig,(msgBody+ (OMX_U32)GET_CONFIG_DATA_OFFSET),structSize); 
	      	       }
		       else	{	
		           DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
		       }
			 
               *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	        	    }
		     else {
			   *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		       }
	      }
	      else {
		     DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
		     *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	      }	 
     
	 RcmClient_free(rcmHndl, rcmMsg);

	EXIT:
		return rpcError;
    
}



/* ===========================================================================*/
/**
 * @name RPC_SendCommand() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_SendCommand(RPC_OMX_HANDLE  nRemoteHandle,OMX_COMMANDTYPE eCmd,OMX_U32 nParam,OMX_PTR pCmdData,OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_BYTE * msgBody;     
        OMX_U32 offset = 0;
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
        OMX_S16 status;
        RPC_INDEX fxnIdx;

        DOMX_DEBUG("\nEntering: %s", __FUNCTION__);
  
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_SEND_CMD].rpcFxnIdx;
  
        //Allocating remote command message
  
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        
        if (rcmMsg ==NULL) {
		    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
		    rpcError = RPC_OMX_ErrorInsufficientResources;
            goto EXIT;
        }
  
        rcmMsg->fxnIdx = fxnIdx;
  
        if (NULL == nCmdStatus) {
            goto EXIT;
            }

        if(rpcError == RPC_OMX_ErrorNone){
  
            pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);  
            msgBody = &pRPCMsg->msgBody[0];

            /***cp handler*/
            TIMM_OSAL_Memcpy(msgBody, &nRemoteHandle,sizeof(RPC_OMX_HANDLE));
            offset+=sizeof(RPC_OMX_HANDLE);

            /***cp cmd*/
            TIMM_OSAL_Memcpy((msgBody+offset),&eCmd,sizeof(OMX_COMMANDTYPE));
            offset+=sizeof(OMX_COMMANDTYPE);     

            /***cp parameter*/  
            TIMM_OSAL_Memcpy((msgBody+offset),&nParam,sizeof(OMX_U32));
            offset+=sizeof(OMX_U32);
  
            /***remember the offset for more data***/ /* Parameter not being unmarshalled at remote core !!  NOT USED*/
            *((OMX_U32*)(msgBody+offset)) = (OMX_U32) (offset + sizeof(OMX_U32));
            offset+=sizeof(OMX_U32);
            
        DOMX_DEBUG("\n\n SEND COMMAND MPU: Just before sending marshalled packet:");
        DOMX_DEBUG("\n handle: 0x%x Command: %x nParam: %x Offset: %x",nRemoteHandle,eCmd,nParam); 
    
            //RcmClient_exec
   	    status = RcmClient_exec(rcmHndl, rcmMsg);
   	
	    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }

       if(rpcError == RPC_OMX_ErrorNone){
          DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);
          if( pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone){
		      DOMX_DEBUG("%s, Warning REMOTE OMAX RETURNS ERR : 0x%x\n",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
	   }
	   *nCmdStatus =pRPCMsg->msgHeader.nOMXReturn;
       }
       else {
           DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
          *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
       }
    
    //RcmClient_free    
   	 RcmClient_free(rcmHndl, rcmMsg);
	
  }
  else
  {
    DOMX_DEBUG("%s: Error Getting Frame\n",__FUNCTION__);   
  }


 EXIT:
 DOMX_DEBUG("\nExiting rpc send command with error = %d\n",rpcError);
  return rpcError;
}


/* ===========================================================================*/
/**
 * @name RPC_GetState() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetState(RPC_OMX_HANDLE  nRemoteHandle,OMX_STATETYPE* pState, OMX_ERRORTYPE * nCmdStatus)          
{
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    RPC_OMX_MESSAGE* pRPCMsg = NULL;
    RPC_OMX_BYTE * msgBody;
    OMX_U32 offset = 0;
    OMX_U32 i = 0;
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    OMX_S16 status;
    RPC_INDEX fxnIdx;
        
    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
  
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_STATE].rpcFxnIdx;
  
    //Allocating remote command message
  
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
	    goto EXIT;
	}
	
	//Inserting appropritate Remote Function Index
    rcmMsg->fxnIdx = fxnIdx;


  if (NULL == nCmdStatus)      
    {
      goto EXIT;
    }

    *pState = OMX_StateInvalid; /***this is the default return****/
	pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
    msgBody = &pRPCMsg->msgBody[0];           
    
    TIMM_OSAL_Memcpy(msgBody, &nRemoteHandle, sizeof(RPC_OMX_HANDLE));
    offset+=sizeof(RPC_OMX_HANDLE);
           
    /***this is the index value****/
    i = offset + sizeof(OMX_U32);
    *((OMX_U32*)(msgBody+offset)) = (OMX_U32)i;

    //RcmClient_exec
   	status = RcmClient_exec(rcmHndl, rcmMsg);
	
	if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
        
    if(rpcError == RPC_OMX_ErrorNone){
      
	  if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone)
	  { 
	  	 if( pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone )
		{
			*pState = *((OMX_STATETYPE *) (msgBody + i ));
		
		 }
		else
		{
	  		DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
		}
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	  	}
	  else
	  {
	      DOMX_DEBUG("%s: RPC Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nRPCCmdStatus);
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	  }
     }
     else {
     		DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
      		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
    }
   
    //Freeing Msg Frame
    //RcmClient_free
    RcmClient_free(rcmHndl, rcmMsg);
    
  EXIT:
  
  return rpcError;

}



/* ===========================================================================*/
/**
 * @name RPC_GetComponentVersion() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetComponentVersion(RPC_OMX_HANDLE nRemoteHandle, OMX_STRING pComponentName, OMX_VERSIONTYPE* pComponentVersion, OMX_VERSIONTYPE* pSpecVersion, OMX_UUIDTYPE* pComponentUUID, OMX_ERRORTYPE * nCmdStatus)      
{
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    RPC_OMX_MESSAGE* pRPCMsg = NULL;
    RPC_OMX_BYTE * msgBody;
    OMX_U32 offset = 0;
    VERSION_INFO *pVer;
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    OMX_S16 status;
    RPC_INDEX fxnIdx;
        
    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
  
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_VERSION].rpcFxnIdx;
  
    //Allocating remote command message
  
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
  
    if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
		rpcError = RPC_OMX_ErrorInsufficientResources;
		goto EXIT;
		}
  
  //Inserting appropritate Remote Function Index
  rcmMsg->fxnIdx = fxnIdx;

  /*** gabbage in, error out****/
  if ((NULL == pComponentName)
      || (NULL == pComponentVersion)
      || (NULL == pSpecVersion)
      || (NULL == nCmdStatus))
    {
      goto EXIT;
    }
  
  pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);	
  msgBody = &pRPCMsg->msgBody[0];           

  /***hander***/
  TIMM_OSAL_Memcpy(msgBody, &nRemoteHandle, sizeof(RPC_OMX_HANDLE));
  offset+=sizeof(RPC_OMX_HANDLE);
           
  *((OMX_U32 *)(msgBody+offset)) = (OMX_U32)GET_PARAM_DATA_OFFSET;
          
  //RcmClient_exec
  status = RcmClient_exec(rcmHndl, rcmMsg);

  if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
           rpcError = RPC_OMX_RCM_ErrorExecFail;
           goto EXIT;        
  }
  if(rpcError == RPC_OMX_ErrorNone){

    DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);
    *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;

    if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone) {          

      if( pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone) {/*** OMX return***/
	pVer = ((VERSION_INFO *) (msgBody + (OMX_U32)GET_PARAM_DATA_OFFSET));         
	strcpy(pComponentName, pVer->cName);
	TIMM_OSAL_Memcpy(pComponentVersion, &(pVer->sVersion.s), sizeof(pVer->sVersion.s));
	TIMM_OSAL_Memcpy(pSpecVersion, &(pVer->sSpecVersion.s), sizeof(pVer->sSpecVersion.s));
    
      }
      else{
	DOMX_DEBUG("\n %s, *****Warning REMOTE CORE RETURNS ERR\n",__FUNCTION__);
      }
    }
    else {
      DOMX_DEBUG("\n %s, *****Warning REMOTE RPC RETURNS ERR\n",__FUNCTION__);
    }
  }else {
    DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
    /***how to set the name and version here?****/
    *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
  }   

  //Freeing Msg Frame
  //RcmClient_free
  RcmClient_free(rcmHndl, rcmMsg);
  

 EXIT:
  
  return rpcError;
}



/* ===========================================================================*/
/**
 * @name RPC_GetExtensionIndex() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetExtensionIndex(RPC_OMX_HANDLE nRemoteHandle,OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType, OMX_ERRORTYPE * nCmdStatus) 
{       

  RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
  RPC_OMX_MESSAGE* pRPCMsg=NULL;
  RPC_OMX_BYTE * msgBody;
  OMX_U32  offset = 0;  
  OMX_U32 i_offset;
  
  OMX_U32 packetSize = PACKET_SIZE;
  RcmClient_Message *rcmMsg;
  
  OMX_S16 status;
  RPC_INDEX fxnIdx;

  fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_EXT_INDEX].rpcFxnIdx;
  
  //Allocating remote command message
  
  rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
  
  if (rcmMsg ==NULL) {
        DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
  }
  
  rcmMsg->fxnIdx = fxnIdx;

  DOMX_DEBUG("Entering: %s\n", __FUNCTION__);

  if ((NULL == pIndexType)
      || (NULL == nCmdStatus))
    {
      goto EXIT;
    }

  
  if(rpcError == RPC_OMX_ErrorNone) {   
     
    
    pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
    msgBody = &pRPCMsg->msgBody[0];
          
    TIMM_OSAL_Memcpy(msgBody, &nRemoteHandle,sizeof(RPC_OMX_HANDLE));
    offset+=sizeof(RPC_OMX_HANDLE);

    /*** handle, offset, string, index***/
    i_offset = offset + 256 +sizeof(OMX_U32) ;   
    *((OMX_U32 *)(msgBody+offset))= (OMX_U32)i_offset;
    
    offset+=sizeof(OMX_U32);
    
    if(strlen(cParameterName) <= 256) {

      /*** investigate the max length***/      
      /***cp string*/
      strcpy((msgBody+offset), cParameterName);           
            
      //RcmClient_exec
      status = RcmClient_exec(rcmHndl, rcmMsg);
      
      if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }

      DOMX_DEBUG("\n %s, received response from remote core",__FUNCTION__);
      *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;           
            
      if(rpcError == RPC_OMX_ErrorNone){   
    
	if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone) {/*** RPC return***/
	  if( pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone) {/*** OMX return***/
	    TIMM_OSAL_Memcpy( pIndexType,  (msgBody + i_offset), sizeof(OMX_INDEXTYPE));	 
	  }
	  else {
	    DOMX_DEBUG("\n %s, *****Warning REMOTE Component RETURNS ERR\n",__FUNCTION__);
	  }
	}
	else {      
	  DOMX_DEBUG("%s, *****Warning REMOTE RPC RETURNS ERR\n",__FUNCTION__);
	}
      }
      else {
	DOMX_DEBUG("%s Failed to RCM_SendMsg_Sync\n", __FUNCTION__);
      }
    }

    //RcmClient_free
    RcmClient_free(rcmHndl, rcmMsg);
    
  }
  else {    
    DOMX_DEBUG("%s: Error Getting Frame\n",__FUNCTION__);      
  }
    
 EXIT:
  return rpcError;

}


/* ===========================================================================*/
/**
 * @name RPC_GetHandle() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param remoteHandle: This is the handle into which the remote Component handle is returned.
                        it's handle with actual OMX Component handle that recides on the specified core
 * @param cComponentName: Name of the component that is to be created on Remote core
 * @param pCallBacks:  
 * @param pAppData: equal to pAppData passed in original GetHandle.
 * @param nCmdStatus: This is return value that will be supplied by Proxy to the caller.
 *                    This is actual return value returned by the Remote component
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_GetHandle(RPC_OMX_HANDLE* remoteHandle, OMX_STRING cComponentName, OMX_PTR pAppData, OMX_CALLBACKTYPE* pCallBacks, OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_BYTE * msgBody;
        OMX_U32 dataOffset = 0;
        OMX_U32 dataOffset2 = 0;	
        OMX_U32 offset=0;
        OMX_PTR * testVal;
//	 RPC_OMX_CONTEXT *pStubCtxt;
	    OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
	    RPC_OMX_CONTEXT *pRpcCtxt;
        
        OMX_S16 status;
        RPC_INDEX fxnIdx;
        OMX_U8 CallingCore = 0;
        OMX_STRING rcmServerName;
        OMX_STRING CallingCorercmServerName;
       
        DOMX_DEBUG("\nEntering: %s\n", __FUNCTION__);
        DOMX_DEBUG("\nRPC_GetHandle: Recieved GetHandle request from %s\n", cComponentName);

    	//CHECK required here for determing if it is a Client calling or a RPC_SKEL calling
    	// This is done based on target component location core id and client core id -this is ascertained in the proxy (wrapper)
        
	 pRpcCtxt = TIMM_OSAL_Malloc(sizeof(RPC_OMX_CONTEXT),TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_INT);

	if (pRpcCtxt  == NULL) {
           DOMX_DEBUG("\n ERROR ALLOCATING RPC CONTEXT STRUCTURE");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
	    //rpcHandle = 0x0;
	    goto EXIT;
	}
	
	// rpcHandle = pRpcCtxt;

	 //Filling in Proxy callback information
/*
	 pRpcCtxt->CbInfo = {pCallBacks->EventHandler, pCallBacks->EmptyBufferDone,pCallBacks->FillBufferDone};
	 pRpcCtxt->Tprxy = ClientProxy;
	 pRpcCtxt->pAppData = pAppData;
*/
	  /* RCM Client Instance Creation*/
	
        RPC_GetTargetServerName(cComponentName,&rcmServerName);
        
        DOMX_DEBUG("\n RCM Server Name To connected to: %s",rcmServerName);
        RPC_InstanceInit(rcmServerName);
        RPC_GetLocalServerName(cComponentName,&CallingCorercmServerName);
        DOMX_DEBUG("\n RCM Server Name Calling on Current Core: %s",CallingCorercmServerName);
        CallingCore = MultiProc_getId(NULL);
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_GET_HANDLE].rpcFxnIdx;
        
        //Allocating remote command message
        
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        
        if (rcmMsg ==NULL) {
		    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	        rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
        }
        
        rcmMsg->fxnIdx = fxnIdx;
       
       
        DOMX_DEBUG("\n Successfully RETURNED FROM RcmClient_alloc\n");
	 if(rpcError!=RPC_OMX_ErrorNone) {

		DOMX_DEBUG("\nRPC ERROR IN GETHANDLE");
		goto EXIT;
	 }
			

    	//CHECK required here for determing if it is a Client calling or a RPC_SKEL calling
    	// This is done based on target component location core id and client core id -this is ascertained in the proxy (wrapper)
    	

        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);

        msgBody =  &pRPCMsg->msgBody[0];		
        (*msgBody) = (RPC_OMX_HANDLE)0x0;
        offset+=sizeof(RPC_OMX_HANDLE);
		
		//Getting first data offset.
        dataOffset = GET_HANDLE_DATA_OFFSET;
        dataOffset2 = GET_HANDLE_DATA_OFFSET2;
        //DOMX_DEBUG("\nDATAOFFSET now: %d",dataOffset);
		*(OMX_U32 *)(msgBody+offset) = (OMX_U32)dataOffset;
		
		strcpy((char *)(msgBody + dataOffset),cComponentName);
		
	    offset+=sizeof(OMX_U32);
        testVal =(OMX_PTR*)(msgBody+offset);
        *testVal = pAppData;

	 offset+=sizeof(OMX_PTR);
        *(msgBody+offset) = CallingCore; 
        offset+=sizeof(OMX_U8);
	 *(msgBody+offset) = dataOffset2;
	 offset+=sizeof(OMX_U8);
        strcpy((char *)(msgBody + dataOffset2),CallingCorercmServerName);
		
        status = RcmClient_exec(rcmHndl, rcmMsg);
        
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
        
        if (status < 0) {
        DOMX_DEBUG("\n Error in GetHandle");
        DOMX_DEBUG("\nRcm exec failed");
        goto EXIT;
        }

	    if(rpcError==RPC_OMX_ErrorNone){
	    	TIMM_OSAL_Memcpy(remoteHandle,msgBody,sizeof(RPC_OMX_HANDLE));
	    	DOMX_DEBUG("%s: Received Remote Handle 0x%x\n",__FUNCTION__, *remoteHandle);
	    	*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	    }	
	    else
    	{
	    	*remoteHandle = 0x00;
	     	DOMX_DEBUG("\n Failed to obtain the Remote Handle:Filling in Zeros");
	    }
        DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);
      	//Freeing Msg Frame
	    RcmClient_free(rcmHndl, rcmMsg);
	
	EXIT:
		return rpcError;
}




/* ===========================================================================*/
/**
 * @name RPC_FreeHandle() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_FreeHandle(RPC_OMX_HANDLE remoteHandle, OMX_ERRORTYPE * nCmdStatus)
{
	   RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
       RPC_OMX_MESSAGE* pRPCMsg=NULL;
       RPC_OMX_BYTE * msgBody;
       OMX_U32 packetSize = PACKET_SIZE;
       RcmClient_Message *rcmMsg;
       OMX_S16 status;
       RPC_INDEX fxnIdx;
	   
       DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
       
       fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_FREE_HANDLE].rpcFxnIdx;
       
       //Allocating remote command message
       
       rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
       if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
       }
       rcmMsg->fxnIdx = fxnIdx;
       

	   if(rpcError!=RPC_OMX_ErrorNone)
		    goto EXIT;
	
        //Filling header information.
        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
        msgBody =  &pRPCMsg->msgBody[0];

 	  //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
	
        //RcmClient_exec
        status = RcmClient_exec(rcmHndl, rcmMsg);
	
	    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }
        

	    if(rpcError==RPC_OMX_ErrorNone){
		    *nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	    }	
	    else {
		*nCmdStatus= rpcError;
	    }
      	//Freeing Msg Frame
	    //RcmClient_free
        RcmClient_free(rcmHndl, rcmMsg);
        RPC_InstanceDeInit();
	
	EXIT:
		return rpcError;
	
}



/* ===========================================================================*/
/**
 * @name RPC_UseBuffer() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param hComp: This is the handle on the Remote core, the proxy will replace
                 it's handle with actual OMX Component handle that recides on the specified core
 * @param ppBufferHdr: 
 * @param nPortIndex: 
 * @param pAppPrivate: 
 * @param nCmdStatus: This is return value that will be supplied by Proxy to the caller.
 *                    This is actual return value returned by the Remote component
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_UseBuffer(RPC_OMX_HANDLE remoteHandle,OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr, OMX_U32 nPortIndex, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes, OMX_U8* pBuffer,OMX_U32* pBufferMapped,OMX_U32* pBufHeaderRemote, OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_BYTE * msgBody;
        OMX_U32 offset=0;
        OMX_U32 mappedAddress =0x00;
	 #ifdef TILER_BUFF
        OMX_U32 mappedAddress2 =0x00;
        #endif
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
        
        OMX_U32 pPlatformPrivate = 0;
        
        OMX_S16 status;
        RPC_INDEX fxnIdx;

        DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
        
        // Error block init and Function Index assignment
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_USE_BUFFER].rpcFxnIdx;
        
        //Allocating remote command message
        
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        if (rcmMsg ==NULL) {
		    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	        rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
        }
        rcmMsg->fxnIdx = fxnIdx;
     
        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
        msgBody = &pRPCMsg->msgBody[0];
        // Marshalling RPC Messaging fields in RPCMsg struct
        //Note: We Pack the message,nPortIndex, pAppData, nSize, pBuffer (post Mapping)
        //MapBuffer - null call - does nothing
        //rpcError = OMX_RPC_MapBuffer(pBuffer, nSizeBytes, &mappedAddress);
        mappedAddress = *(OMX_U32 *)(&((*ppBufferHdr)->pBuffer)); //this is mapping function needs to call RCM mapp call here

	#ifdef TILER_BUFF
    DOMX_DEBUG("\n Getting aux buf\n");
//	mappedAddress2 = *(OMX_U32 *)((*(*ppBufferHdr))->pPlatformPrivate->pAuxBuf1); 
       mappedAddress2 = *(OMX_U32 *)(&((OMX_TI_PLATFORMPRIVATE *)((*ppBufferHdr)->pPlatformPrivate))->pAuxBuf1);
	#endif
        
        DOMX_DEBUG("\n DEBUG - MAPPING - pBuffer = %x",pBuffer);
        DOMX_DEBUG("\n DEBUG - MAPPING - mappedAddress = %x",mappedAddress);
        
    	if(rpcError!=RPC_OMX_ErrorNone)	{
		DOMX_DEBUG("\n %s Failed to Map Buffer\n",__FUNCTION__);
		//RcmClient_free
		//RcmClient_free(rcmHndl, rcmMsg);
		goto EXIT;
		}

        //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        TIMM_OSAL_Memcpy(msgBody+offset,&nPortIndex,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);
        
        TIMM_OSAL_Memcpy(msgBody+offset,&pAppPrivate,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);

        TIMM_OSAL_Memcpy(msgBody+offset, &nSizeBytes, sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);
        
        TIMM_OSAL_Memcpy(msgBody+offset,&mappedAddress,sizeof(OMX_U32));

	 #ifdef TILER_BUFF
	 offset+=sizeof(OMX_U32);
	 TIMM_OSAL_Memcpy(msgBody+offset,&mappedAddress2,sizeof(OMX_U32));
	 #endif

	 

	    //RcmClient_exec
        status = RcmClient_exec(rcmHndl, rcmMsg);
        
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }

	    DOMX_DEBUG("\n RPC STUB ------------------------- msgBody = %x",msgBody);
    	DOMX_DEBUG("\n RPC STUB ------------------------- msgBody+ USE_BUFFER_DATA_OFFSET= %x",msgBody+ USE_BUFFER_DATA_OFFSET);
	  
	
		 //If you are here then Send is successful and you have go the return value
	    if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone) {
		if(pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone){
			DOMX_DEBUG("\nNo errors!\n");
			offset = USE_BUFFER_DATA_OFFSET;
			#ifdef TILER_BUFF
	              offset += sizeof(OMX_U32);
	              #endif
			TIMM_OSAL_Memcpy(pBufHeaderRemote, (msgBody+offset), sizeof(OMX_U32));
			//offset = USE_BUFFER_DATA_OFFSET + sizeof(OMX_U32);
            offset += sizeof(OMX_U32);
            
            pPlatformPrivate = (*ppBufferHdr)->pPlatformPrivate;
			TIMM_OSAL_Memcpy(*ppBufferHdr, (msgBody + offset), sizeof(OMX_BUFFERHEADERTYPE));
            (*ppBufferHdr)->pPlatformPrivate = pPlatformPrivate;
			*pBufferMapped = mappedAddress;
			#ifdef TILER_BUFF
				//Updating the pPlatformPrivate
			offset += sizeof(OMX_BUFFERHEADERTYPE);
			TIMM_OSAL_Memcpy((OMX_TI_PLATFORMPRIVATE *)((*ppBufferHdr)->pPlatformPrivate), (msgBody + offset), sizeof(OMX_TI_PLATFORMPRIVATE));
				
			#endif
		}
		else {
			DOMX_DEBUG("\n %s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			*ppBufferHdr = NULL;
			RPC_UnMapBuffer(mappedAddress);
			*pBufferMapped = 0;
		}
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;

	    }
	    else {
		*ppBufferHdr = NULL;
		*pBufferMapped = 0;
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	    }

	    DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);
	
	
	    //Freeing Msg Frame
	    //RcmClient_free
	    RcmClient_free(rcmHndl, rcmMsg);

	
	
EXIT:
        
        return rpcError;
    
}


/* ===========================================================================*/
/**
 * @name RPC_FreeBuffer() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_FreeBuffer(RPC_OMX_HANDLE remoteHandle,OMX_IN  OMX_U32 nPortIndex, OMX_IN  OMX_U32 BufHdrRemote, OMX_ERRORTYPE * nCmdStatus)
{
        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_BYTE * msgBody;
        OMX_U32 offset=0;
        
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
        
        OMX_S16 status;
        RPC_INDEX fxnIdx;

        DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
        
        // Error block init and Function Index assignment
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_FREE_BUFFER].rpcFxnIdx;
        
        //Allocating remote command message
        
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        if (rcmMsg ==NULL) {
            DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	        rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
        }
        rcmMsg->fxnIdx = fxnIdx;

        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
        msgBody = &pRPCMsg->msgBody[0];
        // Marshalling RPC Messaging fields in RPCMsg struct
     
	    //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        TIMM_OSAL_Memcpy(msgBody+offset,&nPortIndex,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);
        
        TIMM_OSAL_Memcpy(msgBody+offset,&BufHdrRemote,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);

        //RcmClient_exec   
	    status = RcmClient_exec(rcmHndl, rcmMsg);
	    
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }  

	    if(rpcError == RPC_OMX_ErrorNone) {
	       //DOMX_DEBUG("%s, Received Response from DSP\n",__FUNCTION__);
		if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone) {
         	
			 if(pRPCMsg->msgHeader.nOMXReturn != OMX_ErrorNone) {
				DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			 }
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
		else
		{
			DOMX_DEBUG("%s: RPC Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nRPCCmdStatus);
			*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
		}
	}
	else
	{
		 DOMX_DEBUG("%s, Error Sending msg to Remote Core\n",__FUNCTION__);
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	}
	
    	 //Freeing Msg Frame
	    //RcmClient_free
	    RcmClient_free(rcmHndl, rcmMsg);

	EXIT:

		return rpcError;
	
}


/* ===========================================================================*/
/**
 * @name RPC_EventHandler() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_EventHandler(OMX_HANDLETYPE hComponent,OMX_PTR pAppData,OMX_EVENTTYPE eEvent,OMX_U32 nData1, OMX_U32 nData2, OMX_PTR pEventData)
{

	RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_BYTE * msgBody;
	RPC_OMX_ERRORTYPE  rpcError = RPC_OMX_ErrorNone;
	OMX_U32 offset =0;	
	OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
    
    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
        
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_EVENTHANDLER].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    if (rcmMsg ==NULL) {
        DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
    } 

    rcmMsg->fxnIdx = fxnIdx;
    pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
    msgBody =  &pRPCMsg->msgBody[0];
	pRPCMsg->msgHeader.nOMXReturn = OMX_ErrorNone;
		
	//pAppData Contains real component on MPU side
	//This information was passed to Component during the GetHandle Call

	DOMX_DEBUG(" REMOTE_CORE: RPC_EventHandler : 0%x\n",pAppData);
	TIMM_OSAL_Memcpy(msgBody+offset,&pAppData,sizeof(OMX_PTR));
	offset+=sizeof(OMX_PTR);

	TIMM_OSAL_Memcpy(msgBody+offset,&eEvent,sizeof(OMX_EVENTTYPE));
	offset+=sizeof(OMX_EVENTTYPE);

	TIMM_OSAL_Memcpy(msgBody+offset,&nData1,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);
	
	TIMM_OSAL_Memcpy(msgBody+offset,&nData2,sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Shallow copy of a pointer done here
	TIMM_OSAL_Memcpy(msgBody+offset,&pEventData,sizeof(OMX_PTR));
	offset+=sizeof(OMX_PTR);

	status = RcmClient_exec(rcmHndl, rcmMsg);

	if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        DOMX_DEBUG("RPC_EventHandler: Error Sending Message\n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }

EXIT:        
	RcmClient_free(rcmHndl, rcmMsg);
	return rpcError;
	
}


/* ===========================================================================*/
/**
 * @name RPC_EmptyBufferDone() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE  RPC_EmptyBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{
	RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_BYTE * msgBody;
	RPC_OMX_ERRORTYPE  rpcError = RPC_OMX_ErrorNone;
	OMX_U32 offset =0;
	OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
       
    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
               
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_EMPTYBUFFERDONE].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    rcmMsg->fxnIdx = fxnIdx;
	//DOMX_DEBUG("RPC_EmptyBufferDone: pMsgBuf: %x\n",pMsgFrame->pMsgBuf);
	
	pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
	msgBody =(RPC_OMX_BYTE *) &pRPCMsg->msgBody[0];
	pRPCMsg->msgHeader.nOMXReturn = OMX_ErrorNone;
		
	//pAppData Contains real component on MPU side
	//This information was passed to Component during the GetHandle Call
	TIMM_OSAL_Memcpy(msgBody+offset,&pAppData,sizeof(OMX_PTR));
	offset+=sizeof(OMX_PTR);
	
	TIMM_OSAL_Memcpy(msgBody+offset,&pBuffer,sizeof(OMX_BUFFERHEADERTYPE* ));
	offset+=sizeof(OMX_BUFFERHEADERTYPE*);

	//Filling up nOffset and nFilledLen
    TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nFilledLen),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nOffset
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nOffset),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nFlags
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nFlags),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

    status = RcmClient_exec(rcmHndl, rcmMsg);
    
	if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        DOMX_DEBUG("RPC_EmptyBufferDone: Error Sending Message\n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }  
			
	RcmClient_free(rcmHndl, rcmMsg);
	
	if(rpcError!=RPC_OMX_ErrorNone)
	{
		DOMX_DEBUG("RPC_EmptyBufferDone: Error Freeing Msg Frame\n");
	}

EXIT:
	return rpcError;
}

/* ===========================================================================*/
/**
 * @name RPC_FillBufferDone() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_FillBufferDone(OMX_HANDLETYPE hComponent, OMX_PTR pAppData, OMX_BUFFERHEADERTYPE* pBuffer)
{

	RPC_OMX_MESSAGE* pRPCMsg=NULL;
	RPC_OMX_BYTE * msgBody;
	RPC_OMX_ERRORTYPE  rpcError = RPC_OMX_ErrorNone;
	OMX_U32 offset =0;
	OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    RPC_INDEX fxnIdx;
       
    DOMX_DEBUG("\n Entering: %s", __FUNCTION__);
               
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_FILLBUFFERDONE].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    if (rcmMsg ==NULL) {
	    DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    rcmMsg->fxnIdx = fxnIdx;
	
	pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
	msgBody =(RPC_OMX_BYTE *) &pRPCMsg->msgBody[0];
    pRPCMsg->msgHeader.nOMXReturn = OMX_ErrorNone;
		
	//pAppData Contains real component on MPU side
	//This information was passed to Component during the GetHandle Call
	TIMM_OSAL_Memcpy(msgBody+offset,&pAppData,sizeof(OMX_PTR));
	offset+=sizeof(OMX_PTR);
	
	TIMM_OSAL_Memcpy(msgBody+offset,&pBuffer,sizeof(OMX_BUFFERHEADERTYPE* ));
	offset+=sizeof(OMX_BUFFERHEADERTYPE*);

	//Filling up nOffset and nFilledLen
	//DOMX_DEBUG("DSP: RPC level: nFilledLen %d\n", pBuffer->nFilledLen);
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nFilledLen),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nOffset
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nOffset),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

	//Filling up nFlags
	TIMM_OSAL_Memcpy(msgBody+offset,&(pBuffer->nFlags),sizeof(OMX_U32));
	offset+=sizeof(OMX_U32);

    status = RcmClient_exec(rcmHndl, rcmMsg);
    
    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        DOMX_DEBUG("\n RPC_FillBufferDone: Error Sending Message\n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }  
EXIT:
    RcmClient_free(rcmHndl, rcmMsg);
	return rpcError;
}
/* ###################################### CALLBACK STUBS END ############################################ */


/* ===========================================================================*/
/**
 * @name RPC_AllocateBuffer() 
 * @brief Remote invocation stub for OMX_AllcateBuffer()
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_AllocateBuffer(RPC_OMX_HANDLE remoteHandle, OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,OMX_IN OMX_U32 nPortIndex,OMX_U32* pBufHeaderRemote,OMX_U32* pBufferMapped, OMX_PTR pAppPrivate, OMX_U32 nSizeBytes,OMX_ERRORTYPE * nCmdStatus)
{
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    RPC_OMX_MESSAGE* pRPCMsg=NULL;
    RPC_OMX_BYTE * msgBody;     
    OMX_U32 offset = 0;
    OMX_U32 mappedAddress =0x00;
    OMX_U8 * pBuffer;
    OMX_U32 pPlatformPrivate = NULL;

  
    OMX_U32 packetSize = PACKET_SIZE;
    RcmClient_Message *rcmMsg;
    
    OMX_S16 status;
    
    RPC_INDEX fxnIdx;

    DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
  
    fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_ALLOCATE_BUFFER].rpcFxnIdx;
    
    //Allocating remote command message
    
    rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
    if (rcmMsg ==NULL) {
        DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	    rpcError = RPC_OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    rcmMsg->fxnIdx = fxnIdx;
     
    pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
    msgBody = &pRPCMsg->msgBody[0];
    
    //rpcError = OMX_RPC_MapBuffer(pBuffer, nSizeBytes, &mappedAddress);
    //mappedAddress = (OMX_U32 )(ppBufferHdr); //his is mapping function needs to call RCM mapp call here
        
    //DOMX_DEBUG("\n DEBUG - MAPPING - pBufferHeader = %x",ppBufferHdr);
    //DOMX_DEBUG("\n DEBUG - MAPPING - mappedAddress = %x",mappedAddress);  
    
    //Marshalling hComp handle
    TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
    offset+=sizeof(RPC_OMX_HANDLE);

    TIMM_OSAL_Memcpy(msgBody+offset,&nPortIndex,sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);
        
    TIMM_OSAL_Memcpy(msgBody+offset,&pAppPrivate,sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(msgBody+offset, &nSizeBytes, sizeof(OMX_U32));
    offset+=sizeof(OMX_U32);

    TIMM_OSAL_Memcpy(msgBody+offset,&ppBufferHdr,sizeof(OMX_BUFFERHEADERTYPE**));
    offset+=sizeof(OMX_BUFFERHEADERTYPE**);

    //RcmClient_exec
    status = RcmClient_exec(rcmHndl, rcmMsg);
	    
	    if (status < 0) {
	        DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
               DOMX_DEBUG("\n Allocate Buffer: Error Sending Message\n");
               rpcError = RPC_OMX_RCM_ErrorExecFail;
               goto EXIT;        
        }  	
        DOMX_DEBUG( "\n Returned from exec \n");        
	    //If you are here then Send is successful and you have go the return value
	    if(pRPCMsg->msgHeader.nRPCCmdStatus == RPC_OMX_ErrorNone) {
            if(pRPCMsg->msgHeader.nOMXReturn == OMX_ErrorNone){
                TIMM_OSAL_Memcpy(pBufHeaderRemote, (msgBody+USE_BUFFER_DATA_OFFSET), sizeof(OMX_U32));
		        offset = USE_BUFFER_DATA_OFFSET + sizeof(OMX_U32);
          
                pPlatformPrivate = (*ppBufferHdr)->pPlatformPrivate;
		        TIMM_OSAL_Memcpy(*ppBufferHdr, (msgBody + offset), sizeof(OMX_BUFFERHEADERTYPE));
                (*ppBufferHdr)->pPlatformPrivate = pPlatformPrivate;
				#ifdef TILER_BUFF
                DOMX_DEBUG("\n Copying plat pvt. \n");
                offset += sizeof(OMX_BUFFERHEADERTYPE);
				TIMM_OSAL_Memcpy((OMX_TI_PLATFORMPRIVATE *)((*ppBufferHdr)->pPlatformPrivate), (msgBody + offset), sizeof(OMX_TI_PLATFORMPRIVATE));
   			    DOMX_DEBUG("\nDone copying plat pvt., aux buf = 0x%x\n", ((OMX_TI_PLATFORMPRIVATE *)((*ppBufferHdr)->pPlatformPrivate))->pAuxBuf1);	
				#endif
                pBuffer = (*ppBufferHdr )->pBuffer;
                mappedAddress = *(OMX_U32 *)(&pBuffer);
                *pBufferMapped = (OMX_U32)mappedAddress;
				
		
		}
		else {
			DOMX_DEBUG("%s: OMX Error received: 0x%x",__FUNCTION__,pRPCMsg->msgHeader.nOMXReturn);
			*ppBufferHdr = NULL;
			RPC_UnMapBuffer(mappedAddress);
		}
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;

	    }
	    else {
		*ppBufferHdr = NULL;
		*pBufferMapped = 0;
		*nCmdStatus = pRPCMsg->msgHeader.nOMXReturn;
	    }

	    DOMX_DEBUG("\n%s, Received Response from Remote Core\n",__FUNCTION__);
	
EXIT:
        
	    //Freeing Msg Frame
        RcmClient_free(rcmHndl, rcmMsg);

        return rpcError;
    
}

/* ===========================================================================*/
/**
 * @name RPC_ComponentTunnelRequest() 
 * @brief Remote Tunneling of OMX components
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_ComponentTunnelRequest(RPC_OMX_HANDLE remoteHandle, OMX_IN  OMX_U32 nPort,RPC_OMX_HANDLE hTunneledremoteHandle,OMX_U32 nTunneledPort, OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup,OMX_ERRORTYPE * nCmdStatus)
{

        RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
        RPC_OMX_MESSAGE* pRPCMsg=NULL;
        RPC_OMX_BYTE * msgBody;     
        OMX_U32 offset = 0;
        OMX_TUNNELSETUPTYPE pTunnelSetupAtStub;
     
        OMX_U32 packetSize = PACKET_SIZE;
        RcmClient_Message *rcmMsg;
    
        OMX_S16 status;
        RPC_INDEX fxnIdx;

        DOMX_DEBUG("Entering: %s\n", __FUNCTION__);
  
        fxnIdx = rpcHndl[TARGET_CORE_ID].rpcFxns[RPC_OMX_FXN_IDX_COMP_TUNNEL_REQUEST].rpcFxnIdx;
    
        //Allocating remote command message
    
        rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
        
        if (rcmMsg ==NULL) {
            DOMX_DEBUG("\n Error Allocating RCM Message Frame");
	        rpcError = RPC_OMX_ErrorInsufficientResources;
            goto EXIT;
        }
        rcmMsg->fxnIdx = fxnIdx;

        pTunnelSetupAtStub = *pTunnelSetup;
          
        pRPCMsg = (RPC_OMX_MESSAGE*)(&rcmMsg->data);
        msgBody = &pRPCMsg->msgBody[0];

       
        //Marshalling hComp handle
        TIMM_OSAL_Memcpy(msgBody, &remoteHandle,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);

        TIMM_OSAL_Memcpy(msgBody+offset,&nPort,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);

	    TIMM_OSAL_Memcpy(msgBody, &hTunneledremoteHandle,sizeof(RPC_OMX_HANDLE));
        offset+=sizeof(RPC_OMX_HANDLE);	

        TIMM_OSAL_Memcpy(msgBody+offset,&nTunneledPort,sizeof(OMX_U32));
        offset+=sizeof(OMX_U32);
        
        TIMM_OSAL_Memcpy(msgBody+offset,&pTunnelSetupAtStub,sizeof(OMX_TUNNELSETUPTYPE));
        offset+=sizeof(OMX_U32);

        
	    //RcmClient_exec
        status = RcmClient_exec(rcmHndl, rcmMsg);
        
        if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        DOMX_DEBUG("\n CTR: Error Sending Message\n");
        rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto EXIT;        
        }  		
        

EXIT:

	    //Freeing Msg Frame

        RcmClient_free(rcmHndl, rcmMsg);
        return rpcError;
}
