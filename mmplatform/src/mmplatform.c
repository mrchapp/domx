#include <stdio.h>
#include <omx_rpc.h>

extern int ipc_setup(int);
extern int ipc_finalize();

OMX_U8 CHIRON_IPC_FLAG=1;


//todo: synchronize access
static int cnt = 0;


void mmplatform_init(int setup)
{
	cnt++;
	if (cnt > 1)
		return;

	DOMX_DEBUG("Calling ipc setup\n");
    
    if(ipc_setup(setup) < 0)
        TIMM_OSAL_Error("\nipc_setup failed\n");
    else
        DOMX_DEBUG("\nipc_setup successful\n");

    DOMX_DEBUG("\nCreating Rcm Server\n");
    RPC_ModInit();
    DOMX_DEBUG("\nRCM Server created\n");

    return;
}

void mmplatform_deinit()
{
	cnt--;
	if (cnt > 0)
		return;

    DOMX_DEBUG("\nCalling RPC Mod deinit\n");
    RPC_ModDeInit();
    DOMX_DEBUG("\nRPC mod deinit done\n");
    if(ipc_finalize() < 0)
        TIMM_OSAL_Error("\nIPC finalize failed\n");
    else
        TIMM_OSAL_Info("\nIPC Deinitialized\n");
    return;
}
