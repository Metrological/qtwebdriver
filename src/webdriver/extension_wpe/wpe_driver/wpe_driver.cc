#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "extension_wpe/wpe_driver/wpe_driver.h"

void* WpeHandle = NULL;

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

WPEDriver::WPEDriver() 
         : WpeDriverThreadId(0) {
    
}


WPEDriver::~WPEDriver() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
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

int WPEDriver::WpeCreateView ( ) {

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
    return ret;
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

void WPEDriver::WpeLoadURL(const std::string* url) {
    int ret = 0;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_LOAD_URL, url->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    }
    else
        printf("View doesn't exist\n");

    return;
}

void WPEDriver::WpeReload () {
    int ret = 0;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        //Send Reload Command
        WPE_SEND_COMMAND(WD_RELOAD, "");
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        WPE_WAIT_FOR_STATUS(ret);

        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    }
    else
        printf("View doesn't exisit\n");

    return;
}

void WPEDriver::WpeGetURL(std::string* url) {
    int ret = 0;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_GET_URL, "");
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
            url->assign(stsBuff.rspMsg); 
        }
    }
    else
        printf("View doesn't exist\n");

    return;
}

void WPEDriver::WpeRemoveView () {
    int ret = 0;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeHandle) {
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_REMOVE_VIEW, "");
        WPE_WAIT_FOR_STATUS(ret);

        //pthread_join (WpeDriverThreadId, NULL);
        msgctl(cmdQueueId, IPC_RMID, NULL);
        msgctl(stsQueueId, IPC_RMID, NULL);
        WpeHandle = NULL;
        cmdQueueId = stsQueueId = 0;
    }
    else
        printf("View already removed\n");

    return;
}

int CreateWpeView ( void **handle) {
    int ret = 0;
    WPEDriver* WpeDriver = new WPEDriver();
    ret = WpeDriver->WpeCreateView();
    WpeHandle = *handle = (void*) WpeDriver;
    return ret;
}

void* GetWpeViewHandle () {
   printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
   if (WpeHandle)
       return WpeHandle;
   else
       return NULL;
}

int ExecuteCommand (void *handle, WPEDriverCommand command, void* arg) {

    if (!handle)
    {
        printf ("Invalid handle\n");
    }
    WPEDriver* WpeDriver = (WPEDriver*) handle;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 

    switch (command) {
        case WPE_WD_LOAD_URL:{
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
            WpeDriver->WpeLoadURL((const std::string*) arg);
            break;
        }
        case WPE_WD_RELOAD:{
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
            WpeDriver->WpeReload();
            break;
        }
        case WPE_WD_REMOVE_VIEW: {
            WpeDriver->WpeRemoveView();
             
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
            delete WpeHandle;
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
            WpeHandle = NULL;
            break;
        }
        case WPE_WD_IS_URL_SUPPORTED: {
            //WpeDriver->isUrlSupported(arg);
            break;
        }
        case WPE_WD_GET_URL: {
            WpeDriver->WpeGetURL((std::string*)arg);
        }
        default:
            break;
    }
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 

    return 0;
}
