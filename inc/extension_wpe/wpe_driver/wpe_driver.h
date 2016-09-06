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
#ifndef WPE_DRIVER_H_
#define WPE_DRIVER_H_
#include <string>
#include <pthread.h>
#include "extension_wpe/wpe_driver/wpe_driver_common.h"

enum WPEDriverCommand {
    WPE_WD_LOAD_URL,
    WPE_WD_RELOAD,
    WPE_WD_REMOVE_VIEW,
    WPE_WD_IS_URL_SUPPORTED,
    WPE_WD_GET_URL,
    WPE_WD_FIND_ELEMENT,
    WPE_WD_FIND_ELEMENTS,
    WPE_WD_GET_ATTRIBUTE
};


class WPEDriver {
public:
    int cmdQueueId;
    int stsQueueId;
    WDStatusBuf  stsBuff;
    WDCommandBuf cmdBuff;
    pthread_t    WpeDriverThreadId;
    
    WPEDriver();
    ~WPEDriver();

    int   WpeCreateView();
    void* GetViewHandle();
   
    bool WpeIsUrlSupported(const char* mimeType, bool* status);
    int WpeLoadURL(const std::string* url);
    int WpeReload();
    int WpeGetURL(std::string* url);
    int WpeFindElement(std::string* arg, std::string* output);
    int WpeFindElements(std::string* arg, std::string* output);
    int WpeGetAttribute(std::string* arg, std::string* output);
    int WpeRemoveView();

    static void* RunWpeProxy(void* arg);
};

int CreateWpeView(void** handle);
void* GetWpeViewHandle();
int ExecuteCommand(void* handle, WPEDriverCommand command, void* arg, void* ret);

#endif // __WPE_DRIVER_H_
