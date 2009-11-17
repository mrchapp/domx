/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/

/**
 *  @file  omx_rpc.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework RPC.
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

/*  ********************** SYSTEM Headers ************************ */

//#include <xdc/std.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Std.h>

/*  ****************** XDC.RUNTIME module Headers ****************** */
/*
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/knl/Semaphore.h>
#include <xdc/runtime/knl/Thread.h>
*/
/* ************************ Link IPC Headers *********************** */
/*
#include <ti/sdo/ipc/include/biostypes.h>
#include <ti/sdo/ipc/include/ipctypes.h>
*/
/* ********************* Notify module Headers ********************* */
/*
#include <notifyerr.h>
#include <Notify.h>
#include <INotifyDriver.h>
#include <NotifyDriverIPI.h>
#include <NameServerRemoteNotify.h>
*/
/* ********************* BIOS6 module Headers ********************* */
/*
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/ipc/Semaphore.h>
#include <ti/sysbios/BIOS.h>
*/
#include <MultiProc.h>

/* ************** To get globals from .cfg Header ***************** */
#include <RcmClient.h>
#include <RcmServer.h>
//#include <ti/sysbios/hal/ammu/AMMU.h>
#include <SharedRegion.h>

/*
 *  ======== ipc_setup ========
 */
/*
#include <ti/sdo/ipc/MessageQ.h>
#include <ti/sdo/ipc/GatePeterson.h>
#include <ti/sdo/ipc/HeapBuf.h>
#include <ti/sdo/ipc/MessageQTransportShm.h>
#include <ti/sdo/ipc/SharedRegion.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/sysbios/syncs/SyncSem.h>
#include <ti/sdo/ipc/HeapBuf.h>
#include <ti/sysbios/gates/GateMutex.h>
*/
/* ********************** RPC Headers ************************* */
#include "omx_rpc.h"
#include "omx_rpc_stub.h"
#include "omx_rpc_skel.h"
#include "omx_rpc_internal.h"
#include "omx_rpc_utils.h"

/*  ********************** OMX OSAL Other Headers ************************* */
#include <OMX_Types.h>
#include <timm_osal_interfaces.h>
#include <timm_osal_trace.h>
//#include <xdc/cfg/global.h> /* Wrong global.h being included */
 
 /* **************************** MACROS DEFINES *************************** */
#define APPM3_PROC 3
#define SYSM3_PROC 2
#define TESLA_PROC 1
#define CHIRON_PROC 0

#define MAX_INDEX 1
#define MAX_NUMBER_OF_HEAPS 4
#define MAX_FUNCTION_NAME_LENGHT 50
#define MAX_FUNCTION_LIST 20
#define DO_SPIN 0

/* ******************************* GLOBALS ********************************* */
extern TIMM_OSAL_PTR testSem;
extern TIMM_OSAL_PTR testSemSys;
extern OMX_U8 CHIRON_IPC_FLAG;

RcmClient_Handle rcmHndl = NULL;
RcmServer_Handle rcmSrvHndl;
TIMM_OSAL_PTR rcmServerSem;

COREID TARGET_CORE_ID = CORE_MAX; //Should be configured in the CFG or header file for SYS APP split header.   
COREID LOCAL_CORE_ID = CORE_MAX; 
RPC_Object rpcHndl[CORE_MAX];
OMX_U32 PACKET_SIZE;// different packet sizes required for INTER-M3 case and MPU-APPM3 

static OMX_U8 rpcOmxInit = 0; //this flag is to make sure we run RPC_Init() once during initialization
static OMX_U8 flag_client=0;// flag to reflect the number of users/clients
static OMX_U8 flag_server=0;//flag to reflect the number of users/servers
/*Mutex to be used while updating flag_client. This mtx is currently created in 
  ModInit*/
TIMM_OSAL_PTR client_flag_mtx = NULL;

static char rpcFxns[][80]= {
          "RPC_SKEL_SetParameter",
          "RPC_SKEL_GetParameter",
          "RPC_SKEL_GetHandle",
          "RPC_SKEL_UseBuffer",
          
          "RPC_SKEL_FreeHandle",
          
          "RPC_SKEL_SetConfig",
          "RPC_SKEL_GetConfig",
          "RPC_SKEL_GetState",
          "RPC_SKEL_SendCommand",
          "RPC_SKEL_GetComponentVersion",
          "RPC_SKEL_GetExtensionIndex",
          "RPC_SKEL_FillThisBuffer",
          "RPC_SKEL_FillBufferDone",
          "RPC_SKEL_FreeBuffer",
         
          "RPC_SKEL_EmptyThisBuffer",
          "RPC_SKEL_EmptyBufferDone",
          "RPC_SKEL_EventHandler",
          "RPC_SKEL_AllocateBuffer",
          "RPC_SKEL_ComponentTunnelRequest",
          "RPC_SKEL_GetAdditionalServer"
          };
          
static rpcSkelArr rpcSkelFxns[] =
{
          RPC_SKEL_SetParameter,
          RPC_SKEL_GetParameter,
          RPC_SKEL_GetHandle,
          RPC_SKEL_UseBuffer,
          RPC_SKEL_FreeHandle,
          RPC_SKEL_SetConfig,
          RPC_SKEL_GetConfig,
          RPC_SKEL_GetState,
          RPC_SKEL_SendCommand,
          RPC_SKEL_GetComponentVersion,
          RPC_SKEL_GetExtensionIndex,
          RPC_SKEL_FillThisBuffer,
          RPC_SKEL_FillBufferDone,
          RPC_SKEL_FreeBuffer,
          RPC_SKEL_EmptyThisBuffer,
          RPC_SKEL_EmptyBufferDone,
          RPC_SKEL_EventHandler,
          RPC_SKEL_AllocateBuffer,
          RPC_SKEL_ComponentTunnelRequest,
          RPC_SKEL_GetAdditionalServer
		  
};

char *RCM_SERVER_NAME;
char *RCM_SERVER_NAME_LOCAL;

static UInt32 fxnExitidx,getFxnIndexFromRemote_skelIdx;

/*This list needs to be a comprehensive list of all possible communicating RCM servers avalilable across the whole system (core 0 core 1, tesla, chiron)*/
char Core_Array[][20] = {"CHIRON","TESLA","DUCATI0","DUCATI1"};
char rcmservertable[][MAX_SERVER_NAME_LENGTH] = {"RSrv_Chiron","RSrv_Tesla","RSrv_Ducati0","RSrv_Ducati1"}; 
static OMX_U32 heapIdArray[MAX_NUMBER_OF_HEAPS] = {1,0,0,1};

/* ************************* EXTERNS, FUNCTION DECLARATIONS ***************************** */
extern Int32 ipc_finalize(Void);
static Int32 fxnExit(UInt32 size, UInt32 *data);
static Int32 getFxnIndexFromRemote_skel(UInt32 size, UInt32 *data);
void getFxnIndexFromRemote_stub(void);
RPC_OMX_ERRORTYPE fxn_exit_caller(void);
RPC_OMX_ERRORTYPE appRcmServerThrFxn(void);

typedef struct {
	RPC_INDEX FxnIdxArr[MAX_FUNCTION_LIST];
}FxnList;



/* ===========================================================================*/
/**
 * @name RPC_InstanceInit() 
 * @brief RPC instance init is used to bring up a instance of a client - this should be ideally invokable from any core
 *        For this the parameters it would require are
 *        Heap ID - this needs to be configured at startup (CFG) and indicates the heaps available for a RCM client to pick from 
 *        Server - this contains the RCM server name that the client should connect to 
 *        rcmHndl - Contains the Client once the call is completed
 *        rcmParams - 
 *        These values can be picked up from the RPC handle. But an unique identifier is required -Server
 * @param cComponentName  : Pointer to the Components Name that is requires the RCM client to be initialized
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_InstanceInit(OMX_STRING ServerName)
{

	OMX_U8 i;
	OMX_S32 status;
    RcmClient_Config cfgParams;
	RcmClient_Params rcmParams;
    OMX_BOOL bCreateClient = OMX_FALSE;
    
    /* RCM client configuration added in Bridge release 0.9-P1*/
/* Added New */
    cfgParams.maxNameLen = 20;
    cfgParams.defaultHeapIdArrayLength = 4;
    TIMM_OSAL_MutexObtain(client_flag_mtx, TIMM_OSAL_SUSPEND);
        if(flag_client == 0)
            bCreateClient = OMX_TRUE;
        flag_client++;
    TIMM_OSAL_MutexRelease(client_flag_mtx);
    
  //Create client for 1st component
  if(bCreateClient == OMX_TRUE)
  {

    RcmClient_getConfig(&cfgParams);
    DOMX_DEBUG( "\nPrinting Config Parameters\n");
    DOMX_DEBUG( "\nMaxNameLen %d\nHeapIdArrayLength %d\n",cfgParams.maxNameLen,cfgParams.defaultHeapIdArrayLength);
		 
    DOMX_DEBUG( "\nRPC_InstanceInit: creating rcm instance\n");

    RcmClient_Params_init(NULL,&rcmParams); 
    
    rcmParams.heapId = heapIdArray[LOCAL_CORE_ID];
    DOMX_DEBUG("\n Heap ID configured : %d\n",rcmParams.heapId);
	//rcmParams.heapId = rpcHndl[LOCAL_CORE_ID].heapId;
	
	if(CHIRON_IPC_FLAG&&(LOCAL_CORE_ID==CORE_APPM3))
	    rcmParams.heapId = 1; // Hardcoded as heapId 0 when running on APPM3
		//This is done only for WHEN RUNNING ON MPU-APPM3 - "Both use HeapId 0"
		// Work around will be fixed when Independent Heaps are available across MPU and APPM3
		

/* Component Name based Server Name*/
    RCM_SERVER_NAME = ServerName;
/* Added New */    
    DOMX_DEBUG("\nCalling client setup\n");
    status = RcmClient_setup(&cfgParams);
    DOMX_DEBUG("\nClient setup done\n");
    if(status < 0 )
    {
        DOMX_DEBUG( "Client  exist.Error Code:%d\n",status);
        goto leave;
    }

    while (rcmHndl == NULL) {
    DOMX_DEBUG("\nCalling client create with server name = %s\n", RCM_SERVER_NAME);
              status = RcmClient_create(RCM_SERVER_NAME, &rcmParams, &rcmHndl);
    DOMX_DEBUG("\nClient create done\n");

		 if (status < 0) {
			 DOMX_DEBUG( "\nCannot Establish the connection\n");
			 goto leave;
		 }
		 else{
			 DOMX_DEBUG( "\nConnected to Server\n");
		 }
        
    }
	
    DOMX_DEBUG( "\nRPC_InstanceInit: calling RCM_getSymbolIndex(rpcFxns array)\n");
	DOMX_DEBUG("\n Getting Symbols\n");
	/* cached indices recovery*/
	
	status = RcmClient_getSymbolIndex(rcmHndl, "getFxnIndexFromRemote_skel", &getFxnIndexFromRemote_skelIdx);
    
     if(status < 0) {
            DOMX_DEBUG("\nInvalid Symbol.Error Code:%d\n",status);
            goto leave;
        }
		/* cached indices recovery*/		
	//DOMX_DEBUG("\n Calling stub to cache remote function indices");		
	//getFxnIndexFromRemote_stub();
	//DOMX_DEBUG("\n Returned from stub :caching Done");
	
    
    for (i=0;i<MAX_FUNCTION_LIST;i++) {
    
        status = RcmClient_getSymbolIndex(rcmHndl, rpcHndl[TARGET_CORE_ID].rpcFxns[i].FxnName, &rpcHndl[TARGET_CORE_ID].rpcFxns[i].rpcFxnIdx);
        
        if(status < 0) {
            DOMX_DEBUG("\nInvalid Symbol.Error Code:%d\n",status);
            goto leave;
            }

        DOMX_DEBUG("\n%d.FUNCTION INDEX OBTAINED: %d for %s",i+1,rpcHndl[TARGET_CORE_ID].rpcFxns[i].rpcFxnIdx,rpcHndl[TARGET_CORE_ID].rpcFxns[i].FxnName); 
    
        
    }
    
    DOMX_DEBUG( "\nmain: calling RCM_getSymbolIndex(fxnExit)");
    status = RcmClient_getSymbolIndex(rcmHndl, "fxnExit", &fxnExitidx);
    
     if(status < 0) {
            DOMX_DEBUG("\nInvalid Symbol.Error Code:%d\n",status);
            goto leave;
            }
  }
	
leave:
    if(status < 0)
    {
        TIMM_OSAL_MutexObtain(client_flag_mtx, TIMM_OSAL_SUSPEND);
          flag_client--;
        TIMM_OSAL_MutexRelease(client_flag_mtx);
        return RPC_OMX_ErrorUndefined;
    }
    return RPC_OMX_ErrorNone;
}
	
/* ===========================================================================*/
/**
 * @name RPC_InstanceDeInit() 
 * @brief This function Removes or deinitializes RCM client instances. This also manages the number of active users    
 *        of a given RCM client
 * @param cComponentName  : Pointer to the Components Name that is active user of the RCM client to be deinitialized
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/

RPC_OMX_ERRORTYPE RPC_InstanceDeInit(void)
{
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_S32 status;
    TIMM_OSAL_MutexObtain(client_flag_mtx, TIMM_OSAL_SUSPEND);
        flag_client--;
	
	if(flag_client==0) {
	
	    status = RcmClient_delete(&rcmHndl);
		
		if(status < 0) {
		    DOMX_DEBUG("\nError in RcmClient_delete. Error Code:%d\n",status);
			rpcError = RPC_OMX_RCM_ClientFail;
            goto leave;
		}
		
        status = RcmClient_destroy ();
        if(status < 0) {
		    DOMX_DEBUG("\nError in RcmClient_destroy. Error Code:%d\n",status);
			rpcError = RPC_OMX_RCM_ClientFail;
            goto leave;
	}
	}	
leave:
    TIMM_OSAL_MutexRelease(client_flag_mtx);
    DOMX_DEBUG("\n Leaving %s",__FUNCTION__);
    return rpcError;	
}

/* ===========================================================================*/
/**
 * @name RPC_ModDeInit() 
 * @brief This function Removes or deinitializes RCM client instances. This also manages the number of active users    
 *        of a given RCM client
 * @param cComponentName  : Pointer to the Components Name that is active user of the RCM client to be deinitialized
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
 /* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_ModDeInit(void)
{
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
	OMX_S32 status;
	
	DOMX_DEBUG("\nEntered %s",__FUNCTION__);
	
	if(flag_server==0) {
        if(client_flag_mtx)
        {
            TIMM_OSAL_MutexDelete(client_flag_mtx);
            client_flag_mtx = NULL;
        }
            RcmServer_delete(&rcmSrvHndl);
            DOMX_DEBUG("\nRCM server deleted\n");
	
	    status = RcmServer_destroy(); 
		//This function checks the reference count of the RCM server(internal variable to RCM srevre module)
		if(status < 0 ) {
            DOMX_DEBUG( "Client does not exist.Error Code:%d\n",status);
			rpcError = RPC_OMX_RCM_ClientFail;
            goto leave;
        }
	 
  	    flag_server = 1;
    	}
		
leave:
    DOMX_DEBUG("\nLeaving %s",__FUNCTION__);
    return rpcError;	
}
	

/* ===========================================================================*/
/**
 * @name RPC_ModInit() 
 * @brief This function Creates the Default RCM servers on current processor       
 * @param Void 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
 /* ===========================================================================*/
RPC_OMX_ERRORTYPE RPC_ModInit(void)
{

    OMX_U8 i,j;
    //Semaphore_Params semParams;
    //Thread_Params thrParams;
    volatile Bool spin = DO_SPIN; // Needed to connect to CCS debugger
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;

    //initializing global structures (RPC handle)

    DOMX_DEBUG( "\nEntered: RPC_ModInit()");
    bReturnStatus = TIMM_OSAL_MutexCreate(&client_flag_mtx);
    if(bReturnStatus != TIMM_OSAL_ERR_NONE)
    {
        TIMM_OSAL_Error("Mutex create failed");
        goto leave;
    }

    //fetching RCM server name - This needs to be fetched from the default RCM server table
    RCM_SERVER_NAME_LOCAL = rcmservertable[MultiProc_getId(NULL)];
   
    DOMX_DEBUG("\nIn ModInit, MP getId = %d\n",MultiProc_getId(NULL));
    DOMX_DEBUG("\nrcmservertable[0] = %s\n", rcmservertable[0]);
    DOMX_DEBUG("\nrcmservertable[MultiProc_getId(NULL)] = %s\n", rcmservertable[MultiProc_getId(NULL)]);
    DOMX_DEBUG("\nRCM_SERVER_NAME_LOCAL = %s\n", RCM_SERVER_NAME_LOCAL);
   
	if(CHIRON_IPC_FLAG) {
        if(MultiProc_getId(NULL) == APPM3_PROC) {
		    TARGET_CORE_ID = CORE_CHIRON; // This Value needs to be parsed from the Component Name
		    LOCAL_CORE_ID = CORE_APPM3;
	    }
        else {
		    TARGET_CORE_ID = CORE_APPM3; // This Value needs to be parsed from the Component Name	
		    LOCAL_CORE_ID = CORE_CHIRON;
		}
		PACKET_SIZE = CHIRON_PACKET_SIZE;
	}
	else
	{
   
        if(MultiProc_getId(NULL) == SYSM3_PROC) {
		    TARGET_CORE_ID = CORE_APPM3; // This Value needs to be parsed from the Component Name
		    LOCAL_CORE_ID = CORE_SYSM3;
	    }
        else {
		    TARGET_CORE_ID = CORE_SYSM3; // This Value needs to be parsed from the Component Name	
		    LOCAL_CORE_ID = CORE_APPM3;
		}
		PACKET_SIZE = DUCATI_PACKET_SIZE;
	}
	
	for (i=0;i<CORE_MAX;i++)
    {
    rpcHndl[i].rcmHndl[LOCAL_CORE_ID]= (RcmClient_Handle)0;
    rpcHndl[i].heapId[LOCAL_CORE_ID] = heapIdArray[LOCAL_CORE_ID];
	
	
    for(j=0;j<MAX_FUNCTION_LIST;j++)
    {
    rpcHndl[i].rpcFxns[j].rpcFxnIdx = 0;
    rpcHndl[i].rpcFxns[j].FxnName = rpcFxns[j];
    }

    rpcHndl[i].NumOfTXUsers = 0;
            
    }

    /* temporary, wait here for CCS connection */
    while (spin);

	bReturnStatus = TIMM_OSAL_SemaphoreCreate(&rcmServerSem, 0);

        if (bReturnStatus != TIMM_OSAL_ERR_NONE) {
		DOMX_DEBUG("Error: Semaphore_create() failed\n");
		rpcError = RPC_OMX_ErrorInsufficientResources;
             goto leave;
		}


    rpcError = appRcmServerThrFxn(); //calling in same context - not a different thread

	if (rpcError != RPC_OMX_ErrorNone) {
		DOMX_DEBUG("\n Error in Bringing up RCM server Thread");
		goto leave;
		}
	
leave:
    DOMX_DEBUG("\nLeaving %s",__FUNCTION__);
    if (rpcError != RPC_OMX_ErrorNone) 
    {
        if(client_flag_mtx)
        {
            TIMM_OSAL_MutexDelete(client_flag_mtx);
            client_flag_mtx = NULL;
        }
    }
    return rpcError;  	
    }
	
	
	
/* ===========================================================================*/
/**
 * @name appRcmServerThrFxn() 
 * @brief This function Creates the Default RCM servers on current processor       
 * @param Void 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
 /* ===========================================================================*/
 
 
RPC_OMX_ERRORTYPE appRcmServerThrFxn(void)
{
    RcmServer_Params rcmSrvParams;
    OMX_S32 i,status;
    OMX_U32 fxIndx;
    RcmServer_Config cfgParams;
    RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    // Need to verify What this value is ? Is it the function registration table maximum size
    cfgParams.maxTables = 8;//RcmServer_MAX_TABLES; 
    cfgParams.maxNameLen = 20;
    DOMX_DEBUG( "\nPrinting Config Parameters");
    DOMX_DEBUG( "\nMaxNameLen %d \nmaxTables %d\n",cfgParams.maxNameLen,cfgParams.maxTables);
    RcmServer_getConfig(&cfgParams);

    // create an rcm server instance
    DOMX_DEBUG("\n GetConfig done - calling Sercer params init\n");
    RcmServer_Params_init(NULL,&rcmSrvParams);
    DOMX_DEBUG("\n Server params init done - calling Server setup\n");
    status = RcmServer_setup(&cfgParams);
    if(status < 0 )
    {
        DOMX_DEBUG("\nServer Exit. Error Code:%d\n",status);
		rpcError = RPC_OMX_RCM_ServerFail;
        goto leave;
    }
    
    //rcmSrvParams.priority = 1;  // TODO what does this mean?
    DOMX_DEBUG("\n Server setup done - calling Server create\n");
    status = RcmServer_create(RCM_SERVER_NAME_LOCAL, &rcmSrvParams, &rcmSrvHndl);
    //status = RcmServer_create("abc", &rcmSrvParams, &rcmSrvHndl);
DOMX_DEBUG("\nServer create done\n");
	if(status < 0)
        {
		DOMX_DEBUG("\nError occured while RcmServer_Create() \n");
		rpcError = RPC_OMX_RCM_ServerFail;
		goto leave;
		}


    DOMX_DEBUG("\n RCM SERVER NAME: (1 for SYS 2 for APP) = %s",RCM_SERVER_NAME_LOCAL);

    DOMX_DEBUG("\nRcmServer Created");

    /* Local Function Registration starts on  RCM server */
	
	status = RcmServer_addSymbol(rcmSrvHndl, "fxnExit", fxnExit,&fxIndx);
	 
	if(status < 0 || fxIndx == 0xFFFFFFFF)
    {
        DOMX_DEBUG("\nError occured while RcmServer_addSymbol Status:%d\n",status);
		rpcError = RPC_OMX_RCM_ServerFail;
        goto leave;
    }
	
	status = RcmServer_addSymbol(rcmSrvHndl, "getFxnIndexFromRemote_skel", getFxnIndexFromRemote_skel,&getFxnIndexFromRemote_skelIdx);
	
	if(status < 0 || getFxnIndexFromRemote_skelIdx == 0xFFFFFFFF)
    {
        DOMX_DEBUG("\nError occured while RcmServer_addSymbol Status:%d\n",status);
		rpcError = RPC_OMX_RCM_ServerFail;
        goto leave;
    }

     
     for(i=0;i<MAX_FUNCTION_LIST;i++)
    {
        status = RcmServer_addSymbol(rcmSrvHndl, rpcFxns[i], rpcSkelFxns[i].FxnPtr,&rpcHndl[LOCAL_CORE_ID].rpcFxns[i].rpcFxnIdx);
        
        if(status < 0 || rpcHndl[LOCAL_CORE_ID].rpcFxns[i].rpcFxnIdx == 0xFFFFFFFF)
		{
        DOMX_DEBUG("\nError occured while RcmServer_addSymbol Status:%d\n",status);
		rpcError = RPC_OMX_RCM_ServerFail;
        goto leave;
		}

	    DOMX_DEBUG("\n %d.Function %s registered", i+1,rpcFxns[i]);
        
    }
	
	//Start the RCM server thread
	RcmServer_start(rcmSrvHndl);
    DOMX_DEBUG("\nRunning RcmServer\n");
	
leave:
      return rpcError;
}
/* ===========================================================================*/
/**
 * @name fxnExit() 
 * @brief 
 * @param size   : Size of the incoming RCM message (parameter used in the RCM alloc call)
 * @param *data  : Pointer to the RCM message/buffer received
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/

static
Int32 fxnExit(UInt32 size, UInt32 *data)
{
    OMX_S16 status = 0;  /* success */

    DOMX_DEBUG("Executing fxnExit \n");
    DOMX_DEBUG("\nReleasing testcase semaphore:");
/*
    if(MultiProc_getId(NULL) == APPM3_PROC)	
        TIMM_OSAL_SemaphoreRelease (testSem);
    else
        TIMM_OSAL_SemaphoreRelease (testSemSys);
*/	
    return status;
}

/* ===========================================================================*/
/**
 * @name fxn_exit_caller() 
 * @brief 
 * @param void 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
RPC_OMX_ERRORTYPE fxn_exit_caller(void)
{
    RcmClient_Message *rcmMsg = NULL;
    OMX_S32 status;
	RPC_OMX_ERRORTYPE rpcError = RPC_OMX_ErrorNone;
    
    DOMX_DEBUG("\n Entered %s",__FUNCTION__);	
    rcmMsg = RcmClient_alloc(rcmHndl, sizeof(RcmClient_Message));

    if (rcmMsg == NULL) {
        DOMX_DEBUG("\n Error in allocating RCM msg");
		rpcError = RPC_OMX_ErrorInsufficientResources;
		goto leave;
    }

    rcmMsg->fxnIdx = fxnExitidx;
    //Sending Terminate messsage to remote processor
    status = RcmClient_execDpc(rcmHndl, rcmMsg); 
    
    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_execDpc failed \n");
		rpcError = RPC_OMX_RCM_ErrorExecFail;
        goto leave;        
        }	

leave:
    DOMX_DEBUG("\n Exitting %s",__FUNCTION__);
	return rpcError;

}

/* ===========================================================================*/
/**
 * @name getFxnIndexFromRemote_stub() 
 * @brief 
 * @param void 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/
void getFxnIndexFromRemote_stub(void)
{
    //RPC_INDEX FxnIdxArr[MAX_FUNCTION_LIST];
	OMX_U32 packetSize = 0x100;
	RPC_INDEX *FxnIdxArr;
	RcmClient_Message *rcmMsg;
	OMX_S32 status;
	RPC_INDEX fxnIdx;
	OMX_U8 i;
	FxnList FxnIdxList;
	
	DOMX_DEBUG("\nENTERED %s",__FUNCTION__);
	
	rcmMsg = RcmClient_alloc(rcmHndl, packetSize);
	
	if(rcmMsg ==NULL) {
	    DOMX_DEBUG("\nError in Allocting rcm message");        
        goto EXIT;
    }
	
	rcmMsg->fxnIdx = getFxnIndexFromRemote_skelIdx;
       FxnIdxArr = (RPC_INDEX *)(&rcmMsg->data);
	TIMM_OSAL_Memcpy(FxnIdxArr,0,sizeof(RPC_INDEX)*MAX_FUNCTION_LIST);

	status = RcmClient_exec(rcmHndl, rcmMsg);
    
    if (status < 0) {
	    DOMX_DEBUG( "\n Error RcmClient_exec failed \n");
        goto EXIT;        
        }
		
	for(i=0;i<MAX_FUNCTION_LIST;i++)
	{
	
	    DOMX_DEBUG("\n function index from remote side %x",  (OMX_U32)(*(FxnIdxArr+i)));
	    //rpcHndl[TARGET_CORE_ID].rpcFxns[i].rpcFxnIdx =  (RPC_INDEX *)(FxnIdxArr+i);
		
	}
EXIT:
    return;
}

/* ===========================================================================*/
/**
 * @name getFxnIndexFromRemote_skel() 
 * @brief 
 * @param void 
 * @return RPC_OMX_ErrorNone = Successful 
 * @sa TBD
 *
 */
/* ===========================================================================*/

static
Int32 getFxnIndexFromRemote_skel(UInt32 size, UInt32 *data)
{
    Int status = 0;  /* success */
	RPC_INDEX *FxnIdxArr;
	OMX_U32 i;
	
	FxnIdxArr = (RPC_INDEX *)(data);
	
	DOMX_DEBUG("\nENTERED %s",__FUNCTION__);
	
	for(i=0;i<MAX_FUNCTION_LIST;i++)
	//TIMM_OSAL_Memcpy(FxnIdxArr+i,rpcHndl[LOCAL_CORE_ID].rpcFxns[i].rpcFxnIdx,sizeof(RPC_INDEX));
	TIMM_OSAL_Memcpy(FxnIdxArr+i,i,sizeof(RPC_INDEX));
	//    (RPC_INDEX )(*(FxnIdxArr+i)) = rpcHndl[LOCAL_CORE_ID].rpcFxns[i].rpcFxnIdx;

    return status;
}


