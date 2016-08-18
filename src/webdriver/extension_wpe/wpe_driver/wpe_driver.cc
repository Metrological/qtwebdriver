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
                 printf("Error in command execution %d\n", stsBuff.status); \
                 retStatus =  stsBuff.status;               \
             }                                              \
         }                                                  \
         printf ("Message Receive Status : %s\n", strerror(errno)); \
    }

WPEDriver::WPEDriver() 
         : WpeDriverThreadId(0) {
    
}


WPEDriver::~WPEDriver() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void* WPEDriver::RunWpeProxy(void *pArg) {
    int status;
    pid_t wpeProxyPid;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    wpeProxyPid = fork();
    if (wpeProxyPid == 0) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        int execStatus = execl ("/usr/bin/WPEProxy", "/usr/bin/WPEProxy", NULL);
        if (execStatus ==-1) {
            printf ("Error in loading WPEProxy\n");
        }
        return 0;
    } else if (wpeProxyPid < 0) {
        printf ("Failed start WPEProxy\n");
    } else { 
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        waitpid(wpeProxyPid, &status, 0);   
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return 0;
}

int WPEDriver::WpeCreateView ( ) {

    int ret = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    
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
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    WPE_WAIT_FOR_STATUS(ret);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return ret;
}

bool WPEDriver::isUrlSupported (const std::string& mimeType) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
#if 0 
   if (WpeDriverImpl)
        return WpeDriverImpl->isUrlSupported(mimeType);
    else
#endif
        return true;//false;
}

int WPEDriver::WpeLoadURL(const std::string* url) {
    int ret = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (WpeHandle) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_LOAD_URL, url->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    }
    else
        printf("View doesn't exist\n");

    return ret;
}

int WPEDriver::WpeReload () {
    int ret = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (WpeHandle) {
        //Send Reload Command
        WPE_SEND_COMMAND(WD_RELOAD, "");
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        WPE_WAIT_FOR_STATUS(ret);
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    }
    else
        printf("View doesn't exist\n");

    return ret;
}

int WPEDriver::WpeFindElement(std::string* arg, std::string* output) {
    int ret = 0;

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_FIND_ELEMENT, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        printf("View doesn't exist\n");
    return ret;
}

int WPEDriver::WpeFindElements(std::string* arg, std::string* output) {
    int ret = 0;

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_FIND_ELEMENTS, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        printf("View doesn't exist\n");
    return ret;
}

int WPEDriver::WpeGetAttribute(std::string* arg, std::string* output) {
    int ret = 0;

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_GET_ATTRIBUTE, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        printf("View doesn't exist\n");
    return ret;
}

int WPEDriver::WpeGetURL(std::string* url) {
    int ret = 0;
    if (WpeHandle) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_GET_URL, "");
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            url->assign(stsBuff.rspMsg);
        }
    }
    else
        printf("View doesn't exist\n");

    return ret;
}

int WPEDriver::WpeRemoveView () {
    int ret = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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

    return ret;
}

int CreateWpeView ( void **handle) {
    int ret = 0;
    WPEDriver* WpeDriver = new WPEDriver();
    ret = WpeDriver->WpeCreateView();
    WpeHandle = *handle = (void*) WpeDriver;
    return ret;
}

void* GetWpeViewHandle () {
   printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
   if (WpeHandle)
       return WpeHandle;
   else
       return NULL;
}

int ExecuteCommand (void *handle, WPEDriverCommand command, void* arg, void* ret) {

    int retStatus = 0;
    if (!handle)
    {
        printf ("Invalid handle\n");
    }
    WPEDriver* WpeDriver = (WPEDriver*) handle;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);

    switch (command) {
        case WPE_WD_LOAD_URL:{
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            retStatus = WpeDriver->WpeLoadURL((const std::string*) arg);
            break;
        }
        case WPE_WD_RELOAD:{
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            retStatus = WpeDriver->WpeReload();
            break;
        }
        case WPE_WD_REMOVE_VIEW: {
            WpeDriver->WpeRemoveView();
             
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            delete WpeHandle;
            WpeHandle = NULL;
            break;
        }
        case WPE_WD_IS_URL_SUPPORTED: {
            //WpeDriver->isUrlSupported(arg);
            break;
        }
        case WPE_WD_GET_URL: {
            retStatus = WpeDriver->WpeGetURL((std::string*)ret);
            break;
        }
        case WPE_WD_FIND_ELEMENT: {
            retStatus = WpeDriver->WpeFindElement((std::string*)arg, (std::string*)ret);
            break;
        }
        case WPE_WD_FIND_ELEMENTS: {
            retStatus = WpeDriver->WpeFindElements((std::string*)arg, (std::string*)ret);
            break;
        }
        case WPE_WD_GET_ATTRIBUTE: {
            retStatus = WpeDriver->WpeGetAttribute((std::string*)arg, (std::string*)ret);
            break;
        }
        default:
            break;
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);

    return retStatus;
}
