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

#ifndef WPE_DRIVER_PROXY_H_
#define WPE_DRIVER_PROXY_H_

#include <string>
#include <cstdio>
#include <glib.h>
#include <initializer_list>
#include <semaphore.h>

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>
#include <WPE/WebKit/WKWebAutomation.h>
#include <extension_wpe/wpe_driver/wpe_logger.h>

using namespace wpedriver;

enum WPEDriverStatus {
    WPE_WD_STOP,
    WPE_WD_RUN
};

class WPEDriverProxy {

public:
    WKViewRef        view_;
    WKPageRef        page_;
    WKContextRef     context_;
    WKPageGroupRef   pageGroup_;
    WKPreferencesRef preferences_;
    WKPageConfigurationRef pageConfiguration_;

    GMainLoop*       loop_;
    pthread_t        WpeViewThreadId_;
    WPEDriverStatus  WDStatus_;

    Logger*          logger_;
    int              requestID_;
    std::string      browsingContext_;
    WKWebAutomationSessionRef webAutomationSession_;

    WPEDriverProxy();
    ~WPEDriverProxy();
    WPEDriverProxy(bool enableConsole);

    WDStatus CreateView();
    WDStatus LoadURL(const char* url);
    WDStatus Reload();
    WDStatus GetURL(char* command);
    WDStatus GetAttribute(const char* reqParams, char* value);
    WDStatus FindElement(bool isElements, const char* reqParams, char* element);
    WDStatus IsUrlSupported(const char* mimeType);
    WDStatus RemoveView();
    static void* RunWpeView(void*);

private:

    void CreateJSScript(const char* methodName, const char* handleStr, const char* jsScript, 
                        const char* argList, std::string& command);
    void ExecuteJSCommand(const  char* methodName, const char* handleStr, 
                          const char* jsScript, const char* argList);
    WDStatus ParseJSResponse(const char* response, const char* attrib, std::string& attribValue);

    WDStatus FindElementById(const char* query, std::string& element);
    WDStatus FindElementByName(const char* rootElement, const char* query, std::string& element);
    WDStatus FindElementByNameType(const char* rootElement, const char* type, const char* query, std::string& element);
    WDStatus FindElementByCss(bool isElements, const char* rootElement, const char* query, std::string& element);
    WDStatus FindElementByXPath(bool isElements, const char* rootElement, const char* query, std::string& element);
	
    void CreateBrowsingContext();
    void CloseBrowsingContext();
};

#endif // __WPE_DRIVER_PROXY_H_
