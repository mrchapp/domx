#include <stdio.h>
#include <omx_rpc.h>

extern int ipc_setup(int);
OMX_U8 CHIRON_IPC_FLAG=1;


//todo: synchronize access
static int cnt = 0;


void mmplatform_init(int setup)
{
	cnt++;
	if (cnt > 1)
		return;

    printf("Calling ipc setup\n");
    
    if(ipc_setup(setup) < 0)
        printf("\nipc_setup failed\n");
    else
        printf("\nipc_setup successful\n");

    printf("\nCreating Rcm Server\n");
    RPC_ModInit();
    printf("\nRCM Server created\n");

    return;
}

void mmplatform_deinit()
{
	cnt--;
	if (cnt > 0)
		return;

    printf("\nCalling RPC Mod deinit\n");
    RPC_ModDeInit();
    printf("\nRPC mod deinit done\n");

    return;
}
