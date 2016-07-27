
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "extension_wpe/wpe_driver/wpe_driver_common.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

int cmdQueueId = 0;
WDCommandBuf cmdBuff;

int stsQueueId = 0;
WDStatusBuf stsBuff = {};

void* WpeHandle = NULL;
pthread_t WpeDriverThreadId;

#define WPE_SEND_COMMAND(cmd, msg)     \
    {                                      \
        cmdBuff.command = cmd;             \
        strcpy(cmdBuff.message, msg);  \
        if ((msgsnd(cmdQueueId, &cmdBuff, WD_CMD_SIZE, 0 )) < 0) { \
            printf ("Error in send command %d, Error no: %s\n", cmd, strerror(errno)); \
        }                                               \
    }
#define WPE_WAIT_FOR_STATUS(retStatus)  \
    {                       \
         printf("WPE_WAIT %s:%s:%d\n",__FILE__, __func__, __LINE__); \
         if ((msgrcv(stsQueueId, &stsBuff, WD_STATUS_SIZE, 0, 0) >= 0)) { \
             if (WD_SUCCESS == stsBuff.status)          \
                 printf("Command executed successfully %d\n", stsBuff.status); \
             else                                           \
             {                                              \
                 printf("Error in execution %d\n", stsBuff.status); \
                 retStatus =  stsBuff.status;               \
             }                                              \
         }                                                  \
         printf ("Receive Status : %s\n", strerror(errno)); \
    }


WPEDriver::~WPEDriver() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeHandle = NULL;
}

void* WPEDriver::RunWpeProxy(void *pArg) {
    int status;
    pid_t wpeProxyPid;
    //WPEDriver* WpeDriver = (WPEDriver*) pArg;
   
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    wpeProxyPid = fork();
    if (wpeProxyPid == 0) {
        printf("This is %d from %s in %s Pid = %d\n",__LINE__,__func__,__FILE__, wpeProxyPid);
        int execStatus = execl ("/usr/bin/WPEProxy", "/usr/bin/WPEProxy", NULL);
        if (execStatus ==-1) {
            printf ("Error in loading WPEProxy\n");
        }
        return 0;
    } else if (wpeProxyPid < 0) {
        printf ("Failed start WPEProxy\n");
    } else { 
        printf("This is %d from %s in %s Pid = %d\n",__LINE__,__func__,__FILE__, wpeProxyPid);
        waitpid(wpeProxyPid, &status, 0);   
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    }

   printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return 0;
}

int WPEDriver::WpeCreateView ( void **handle) {

    int ret = 0;
    
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    ret = pthread_create(&WpeDriverThreadId, NULL, RunWpeProxy, this);
    if (ret != 0)
        printf("Can't start RunWpeProxy Thread\n");
 
    if ((cmdQueueId = msgget(WPE_WD_CMD_KEY, IPC_CREAT | 0666)) < 0) {
         printf("Error in message queue creation \n");
         return 0;
    } 

    if ((stsQueueId = msgget(WPE_WD_STATUS_KEY, IPC_CREAT | 0666)) < 0) {
         printf("Error in message queue creation \n");
         return 0;
    }
         
    //Send message for CreateView
    WPE_SEND_COMMAND (WD_CREATE_VIEW, "")
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WPE_WAIT_FOR_STATUS(ret);

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeHandle = *handle = (void*) WpeDriverThreadId;
    return ret;
}

void* WPEDriver::GetViewHandle () {
   printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
   if (WpeHandle)
       return WpeHandle;
   else
       return NULL;
}

bool WPEDriver::isUrlSupported (const std::string& mimeType) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
#if 0 
   if (WpeDriverImpl)
        return WpeDriverImpl->isUrlSupported(mimeType);
    else
#endif
        return true;//false;
}

void WPEDriver::WpeReload () {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        //Send Reload Command
        WPE_SEND_COMMAND(WD_RELOAD, "");
        //wait for the completion
    }
    else
        printf("View doesn't exisit\n");

    return;
}

void WPEDriver::WpeRemoveView () {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_REMOVE_VIEW, "");
        pthread_join (WpeDriverThreadId, NULL);
        msgctl(cmdQueueId, IPC_RMID, NULL);
        msgctl(stsQueueId, IPC_RMID, NULL);
        WpeHandle = NULL;
        cmdQueueId = stsQueueId = 0;
    }
    else
        printf("View already removed\n");

    return;
}
