/*
 * Copyright (C) 2016 TATA ELXSI
 * Copyright (C) 2016 Metrological
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/prctl.h>
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
    strcpy(cmdBuff.message, msg);      \
    if ((msgsnd(cmdQueueId, &cmdBuff, WD_CMD_SIZE, 0 )) < 0) { \
        printf ("Error in send command %d, Error no: %s\n", cmd, strerror(errno)); \
    }                                               \
}
#define WPE_WAIT_FOR_STATUS(retStatus)  \
{                       \
    if ((msgrcv(stsQueueId, &stsBuff, WD_STATUS_SIZE, 0, 0) >= 0)) { \
         if (WD_SUCCESS != stsBuff.status)              \
         {                                              \
             printf("Error in command execution %d\n", stsBuff.status); \
             retStatus =  stsBuff.status;               \
         }                                              \
     }                                                  \
}

WPEDriver::WPEDriver() 
    : logger(NULL),
      cmdQueueId(0),
      stsQueueId(0),
      WpeDriverThreadId(0) {
}

WPEDriver::WPEDriver(const Logger* logger)
    : logger(logger),
      cmdQueueId(0),
      stsQueueId(0),
      WpeDriverThreadId(0) {
}

WPEDriver::~WPEDriver() {
}

void* WPEDriver::RunWpeProxy(void* pArg) {
    int status;
    pid_t wpeProxyPid;
    WPEDriver *wpeDriver = (WPEDriver *) pArg;
    wpeDriver->logger->Log(kInfoLogLevel, LOCATION);

    if ((wpeDriver->cmdQueueId = msgget(WPE_WD_CMD_KEY, IPC_CREAT | 0666)) < 0) {
         wpeDriver->logger->Log(kSevereLogLevel, "Error in message queue creation");
         return 0;
    }
    if ((wpeDriver->stsQueueId = msgget(WPE_WD_STATUS_KEY, IPC_CREAT | 0666)) < 0) {
         wpeDriver->logger->Log(kSevereLogLevel, "Error in message queue creation");
         return 0;
    }

    wpeProxyPid = fork();
    if (0 == wpeProxyPid) {
        prctl(PR_SET_PDEATHSIG, SIGTERM);
        int execStatus = execl ("/usr/bin/WPEProxy", "/usr/bin/WPEProxy", NULL);
        if (execStatus ==-1) {
            wpeDriver->logger->Log(kSevereLogLevel, "Error in loading WPEProxy");
        }
        return 0;
    } else if (0 > wpeProxyPid) {
        wpeDriver->logger->Log(kSevereLogLevel, "Failed start WPEProxy");
    } else { 
        waitpid(wpeProxyPid, &status, 0);   
    }
    msgctl(wpeDriver->cmdQueueId, IPC_RMID, NULL);
    msgctl(wpeDriver->stsQueueId, IPC_RMID, NULL);

    wpeDriver->cmdQueueId = wpeDriver->stsQueueId = 0;
    wpeDriver->logger->Log(kInfoLogLevel, LOCATION);
    return 0;
}

int WPEDriver::WpeCreateView() {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);
    
    ret = pthread_create(&WpeDriverThreadId, NULL, RunWpeProxy, this);
    if (ret != 0)
        logger->Log(kSevereLogLevel, "Can't start RunWpeProxy Thread");

    do { // Wait till the Queues are created
        if (stsQueueId) {
            //Send message for CreateView
            WPE_SEND_COMMAND (WD_CREATE_VIEW, "")
            WPE_WAIT_FOR_STATUS(ret);
            break;
        } else {
            logger->Log(kSevereLogLevel, "Queue is not yet created");
            sleep(1);
            // queues not created */
        }
    } while(1);
    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

bool WPEDriver::WpeIsUrlSupported(const char* mimeType, bool* status) {
    int ret = 0;
    *status = false;
    logger->Log(kInfoLogLevel, LOCATION);
    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_IS_URL_SUPPORTED, mimeType);
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) {
            *status = true;
        }
    }

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeLoadURL(const std::string* url) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);
    if (WpeHandle) {
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_LOAD_URL, url->c_str());
        WPE_WAIT_FOR_STATUS(ret);
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeReload() {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);
    if (WpeHandle) {
        //Send Reload Command
        WPE_SEND_COMMAND(WD_RELOAD, "");
        WPE_WAIT_FOR_STATUS(ret);
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeFindElement(std::string* arg, std::string* output) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_FIND_ELEMENT, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeFindElements(std::string* arg, std::string* output) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_FIND_ELEMENTS, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeGetAttribute(std::string* arg, std::string* output) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);

    if (WpeHandle) {
        WPE_SEND_COMMAND(WD_GET_ATTRIBUTE, arg->c_str());
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            output->assign(stsBuff.rspMsg);
        }
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeGetURL(std::string* url) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);
    if (WpeHandle) {
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_GET_URL, "");
        WPE_WAIT_FOR_STATUS(ret);
        if (!ret) { //success
            url->assign(stsBuff.rspMsg);
        }
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int WPEDriver::WpeRemoveView() {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);
    if (WpeHandle) {
        // Send Remove View Command
        WPE_SEND_COMMAND(WD_REMOVE_VIEW, "");
        WPE_WAIT_FOR_STATUS(ret);

        //pthread_join (WpeDriverThreadId, NULL);
        WpeHandle = NULL;
        cmdQueueId = stsQueueId = 0;
    }
    else
        logger->Log(kSevereLogLevel, "View doesn't exist");

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

int CreateWpeView(const Logger* logger, void **handle) {
    int ret = 0;
    logger->Log(kInfoLogLevel, LOCATION);

    WPEDriver* WpeDriver = new WPEDriver(logger);
    ret = WpeDriver->WpeCreateView();
    WpeHandle = *handle = (void*) WpeDriver;

    logger->Log(kInfoLogLevel, LOCATION);
    return ret;
}

void* GetWpeViewHandle() {
   if (WpeHandle)
       return WpeHandle;
   else
       return NULL;
}

int ExecuteCommand(void* handle, WPEDriverCommand command, void* arg, void* ret) {

    int retStatus = 0;
    if (handle)
    {
        WPEDriver* WpeDriver = (WPEDriver*) handle;

        switch (command) {
            case WPE_WD_LOAD_URL:{
                retStatus = WpeDriver->WpeLoadURL((const std::string*) arg);
                break;
            }
            case WPE_WD_RELOAD:{
                retStatus = WpeDriver->WpeReload();
                break;
            }
            case WPE_WD_REMOVE_VIEW: {
                WpeDriver->WpeRemoveView();
             
                delete(WPEDriver*)WpeHandle;
                WpeHandle = NULL;
                break;
            }
            case WPE_WD_IS_URL_SUPPORTED: {
                retStatus = WpeDriver->WpeIsUrlSupported((char *) arg, (bool*) ret);
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
    }
    return retStatus;
}
