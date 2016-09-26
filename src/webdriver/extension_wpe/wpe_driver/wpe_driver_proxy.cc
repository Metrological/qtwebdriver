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
#include <unistd.h>
#include <semaphore.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <string.h>
#include <memory>

#include "extension_wpe/wpe_driver/wpe_driver_common.h"
#include "extension_wpe/wpe_driver/wpe_driver_proxy.h"
#include "third_party/webdriver/atoms.h"
#include "base/stringprintf.h"

using std::unique_ptr;

/*transfer callback msg to WebDriverProxy */
std::string respMsg;
sem_t  jsRespWait;

#define WD_REMOVE_QUOTES(str) \
if (str.front() == '"') {     \
    str.erase(0, 1);          \
    str.erase(str.size() - 1);\
}

WKPageNavigationClientV0 s_navigationClient = {
    { 0, nullptr },
    // decidePolicyForNavigationAction
    [](WKPageRef, WKNavigationActionRef, WKFramePolicyListenerRef listener, WKTypeRef, const void*) {
        WKFramePolicyListenerUse(listener);
    },
    // decidePolicyForNavigationResponse
    [](WKPageRef, WKNavigationResponseRef, WKFramePolicyListenerRef listener, WKTypeRef, const void*) {
        WKFramePolicyListenerUse(listener);
    },
    nullptr, // decidePolicyForPluginLoad
    nullptr, // didStartProvisionalNavigation
    nullptr, // didReceiveServerRedirectForProvisionalNavigation
    nullptr, // didFailProvisionalNavigation
    nullptr, // didCommitNavigation
    nullptr, // didFinishNavigation
    nullptr, // didFailNavigation
    nullptr, // didFailProvisionalLoadInSubframe
    // didFinishDocumentLoad
    [](WKPageRef page, WKNavigationRef, WKTypeRef, const void*) {
        WKStringRef messageName = WKStringCreateWithUTF8CString("Hello");
        WKMutableArrayRef messageBody = WKMutableArrayCreate();

        for (auto& item : { "Test1", "Test2", "Test3" }) {
            WKStringRef itemString = WKStringCreateWithUTF8CString(item);
            WKArrayAppendItem(messageBody, itemString);
            WKRelease(itemString);
        }

        fprintf(stderr, "[WPELauncher] Hello InjectedBundle ...\n");
        WKPagePostMessageToInjectedBundle(page, messageName, messageBody);
        WKRelease(messageBody);
        WKRelease(messageName);
    },
    nullptr, // didSameDocumentNavigation
    nullptr, // renderingProgressDidChange
    nullptr, // canAuthenticateAgainstProtectionSpace
    nullptr, // didReceiveAuthenticationChallenge
    nullptr, // webProcessDidCrash
    nullptr, // copyWebCryptoMasterKey
    nullptr, // didBeginNavigationGesture
    nullptr, // willEndNavigationGesture
    nullptr, // didEndNavigationGesture
    nullptr, // didRemoveNavigationGestureSnapshot
};

WKViewClientV0 s_viewClient = {
    { 0, nullptr },
    // frameDisplayed
    [](WKViewRef, const void*) {
        static unsigned s_frameCount = 0;
        static gint64 lastDumpTime = g_get_monotonic_time();

        ++s_frameCount;
        gint64 time = g_get_monotonic_time();
        if (time - lastDumpTime >= 5 * G_USEC_PER_SEC) {
            fprintf(stderr, "[WPELauncher] %.2fFPS\n",
                s_frameCount * G_USEC_PER_SEC * 1.0 / (time - lastDumpTime));
            s_frameCount = 0;
            lastDumpTime = time;
        }
    },
};

WPEDriverProxy::WPEDriverProxy(bool enableConsole)
    : WpeViewThreadId_ (0),
      requestID_(1)
{
    logger_ = new wpedriver::Logger(TRACE, enableConsole);
}

WPEDriverProxy::WPEDriverProxy()
    : WpeViewThreadId_ (0),
      requestID_(1)
{
    logger_ = new wpedriver::Logger(TRACE, false);
    logger_->Log(INFO, LOCATION);
}

WPEDriverProxy::~WPEDriverProxy()
{
    logger_->Log(INFO, LOCATION);
    RemoveView();
    delete logger_;
}

static void AutomationCallback(WKStringRef wkRspMsg) {
    size_t bufferSize = WKStringGetMaximumUTF8CStringSize(wkRspMsg);
    std::unique_ptr<char> buffer(new char[bufferSize]);
    WKStringGetUTF8CString(wkRspMsg, buffer.get(), bufferSize);

    respMsg.assign(buffer.get());
    printf("\n Response Msg : = %s\n", respMsg.c_str());
    sem_post(&jsRespWait);
}

WDStatus WPEDriverProxy::CreateView() {
    logger_->Log(TRACE, LOCATION);
    WDStatus ret = WD_SUCCESS;
    if (0 != (pthread_create(&WpeViewThreadId_, NULL, RunWpeView, this ))) {
        logger_->Log(ERR, "Can't start RunWpeView Thread");
        ret = WD_FAILURE;
    }
    logger_->Log(TRACE, LOCATION);
    return ret; 
}

void* WPEDriverProxy::RunWpeView(void* arg){

    WPEDriverProxy *pWpeDriverProxy =  (WPEDriverProxy*) arg;
    pWpeDriverProxy->logger_->Log(TRACE, LOCATION);
    pWpeDriverProxy->loop_ = g_main_loop_new(nullptr, FALSE);

    auto contextConfiguration = WKContextConfigurationCreate();
    auto injectedBundlePath = WKStringCreateWithUTF8CString("/usr/lib/libWebDriver_wpe_driver_injected_bundle.so");
    WKContextConfigurationSetInjectedBundlePath(contextConfiguration, injectedBundlePath);

    gchar *wpeStoragePath = g_build_filename(g_get_user_cache_dir(), "wpe", "local-storage", nullptr);
    g_mkdir_with_parents(wpeStoragePath, 0700);
    auto storageDirectory = WKStringCreateWithUTF8CString(wpeStoragePath);
    g_free(wpeStoragePath);
    WKContextConfigurationSetLocalStorageDirectory(contextConfiguration, storageDirectory);

    gchar *wpeDiskCachePath = g_build_filename(g_get_user_cache_dir(), "wpe", "disk-cache", nullptr);
    g_mkdir_with_parents(wpeDiskCachePath, 0700);
    auto diskCacheDirectory = WKStringCreateWithUTF8CString(wpeDiskCachePath);
    g_free(wpeDiskCachePath);
    WKContextConfigurationSetDiskCacheDirectory(contextConfiguration, diskCacheDirectory);

    WKRelease(injectedBundlePath);

    pWpeDriverProxy->context_ = WKContextCreateWithConfiguration(contextConfiguration);
    WKRelease(contextConfiguration);

    auto pageGroupIdentifier = WKStringCreateWithUTF8CString("WPEPageGroup");
    pWpeDriverProxy->pageGroup_ = WKPageGroupCreateWithIdentifier(pageGroupIdentifier);
    WKRelease(pageGroupIdentifier);

    pWpeDriverProxy->preferences_ = WKPreferencesCreate();
    // Allow mixed content.
    WKPreferencesSetAllowRunningOfInsecureContent(pWpeDriverProxy->preferences_, true);
    WKPreferencesSetAllowDisplayOfInsecureContent(pWpeDriverProxy->preferences_, true);

    // By default allow console log messages to system console reporting.
    if (!g_getenv("WPE_SHELL_DISABLE_CONSOLE_LOG"))
        WKPreferencesSetLogsPageMessagesToSystemConsoleEnabled(pWpeDriverProxy->preferences_, true);

    WKPageGroupSetPreferences(pWpeDriverProxy->pageGroup_, pWpeDriverProxy->preferences_);

    pWpeDriverProxy->pageConfiguration_  = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(pWpeDriverProxy->pageConfiguration_, pWpeDriverProxy->context_);
    WKPageConfigurationSetPageGroup(pWpeDriverProxy->pageConfiguration_, pWpeDriverProxy->pageGroup_);
    WKPreferencesSetFullScreenEnabled(pWpeDriverProxy->preferences_, true);

    if (!!g_getenv("WPE_SHELL_COOKIE_STORAGE")) {
        gchar *cookieDatabasePath = g_build_filename(g_get_user_cache_dir(), "cookies.db", nullptr);
        auto path = WKStringCreateWithUTF8CString(cookieDatabasePath);
        g_free(cookieDatabasePath);
        auto cookieManager = WKContextGetCookieManager(pWpeDriverProxy->context_);
        WKCookieManagerSetCookiePersistentStorage(cookieManager, path, kWKCookieStorageTypeSQLite);
    }

    pWpeDriverProxy->view_ = WKViewCreate(pWpeDriverProxy->pageConfiguration_);
    WKViewSetViewClient(pWpeDriverProxy->view_, &s_viewClient.base);

    pWpeDriverProxy->page_ = WKViewGetPage(pWpeDriverProxy->view_);
    WKPageSetPageNavigationClient(pWpeDriverProxy->page_, &s_navigationClient.base);

    pWpeDriverProxy->logger_->Log(TRACE, LOCATION);

    //Create Automation session
    pWpeDriverProxy->webAutomationSession_ = WKWebAutomationSessionCreate(pWpeDriverProxy->context_, pWpeDriverProxy->page_);
    pWpeDriverProxy->CreateBrowsingContext();

    pWpeDriverProxy->logger_->Log(DEBUG, LOCATION);
    g_main_loop_run(pWpeDriverProxy->loop_);

    pWpeDriverProxy->logger_->Log(TRACE, LOCATION);
    pWpeDriverProxy->CloseBrowsingContext();
    WKRelease(pWpeDriverProxy->webAutomationSession_);
    WKRelease(pWpeDriverProxy->view_);
    WKRelease(pWpeDriverProxy->pageConfiguration_);
    WKRelease(pWpeDriverProxy->pageGroup_);
    WKRelease(pWpeDriverProxy->context_);
    WKRelease(pWpeDriverProxy->preferences_);

    return 0;
}

WDStatus WPEDriverProxy::LoadURL(const char* url) {
    logger_->Log(TRACE, LOCATION);
    if (NULL != page_) {
        auto shellURL = WKURLCreateWithUTF8CString(url);
        WKPageLoadURL (page_, shellURL);
        sleep (2);
    }
    logger_->Log(TRACE, LOCATION);
    return WD_SUCCESS;
}

WDStatus WPEDriverProxy::Reload() {
    logger_->Log(TRACE, LOCATION);
    if (NULL != page_) {
        WKPageReload (page_);
        sleep (2);
    }
    logger_->Log(TRACE, LOCATION);
    return WD_SUCCESS;
}

WDStatus WPEDriverProxy::IsUrlSupported (const char* mimeType) {
    bool isUrlSupported = false;
    logger_->Log(TRACE, LOCATION);
    if (NULL != page_) {
        isUrlSupported = WKPageCanShowMIMEType(page_, WKStringCreateWithUTF8CString(mimeType));
        if (isUrlSupported){
            return WD_SUCCESS;
        }
    }
    logger_->Log(TRACE, LOCATION);
    return WD_FAILURE;
}

void WPEDriverProxy::CreateJSScript(const char* methodName, const char* handleStr, const char* jsScript,
                                    const char* argList, std::string& command) {
    logger_->Log(TRACE, LOCATION);
    json_object *jobj = json_object_new_object();
    json_object *jint = json_object_new_int(requestID_);
    json_object_object_add(jobj, "id", jint);
    json_object *jmethodStr = json_object_new_string(methodName);
    json_object_object_add(jobj, "method", jmethodStr);

    //Create Parameters
    {
        json_object *subObj = json_object_new_object();
        logger_->Log(INFO, LOCATION);
        if (strcmp (handleStr, "")) {
            json_object *jhandleStr = json_object_new_string(browsingContext_.c_str());
            json_object_object_add(subObj, handleStr, jhandleStr);
        }

        if (strcmp (jsScript, "")) {
            json_object *jscriptStr = json_object_new_string(jsScript);
            json_object_object_add(subObj, "function", jscriptStr);
            json_object *jargs = json_object_new_array();
            json_object_array_add(jargs, json_object_new_string(argList));
            json_object_object_add(subObj, "arguments", jargs);
        }
        json_object_object_add(jobj, "params", subObj);
    }
    
    command.assign(json_object_to_json_string(jobj));
    logger_->Log(TRACE, LOCATION);
}

void WPEDriverProxy::ExecuteJSCommand(const char* methodName, const char* handleStr,
                                      const char* jsScript, const char* argList) {
    std::string command;
    logger_->Log(TRACE, LOCATION);
    CreateJSScript(methodName, handleStr, jsScript, argList, command);
    WKWebAutomationExecuteCommand(webAutomationSession_, WKStringCreateWithUTF8CString(command.c_str()), AutomationCallback);
    logger_->Log(TRACE, LOCATION);
}

WDStatus WPEDriverProxy::ParseJSResponse(const char* response, const char* attrib, std::string& attribValue) {
    WDStatus ret = WD_FAILURE;
    json_object *jsObj;
    jsObj = json_tokener_parse(response);

    logger_->Log(TRACE, LOCATION);
    if (json_object_object_get_ex(jsObj, "result", &jsObj)) {
        if (strcmp (attrib, "")) {
           json_object_object_get_ex(jsObj, attrib, &jsObj);
        } else {
            //parsing of final result will handle outside this function
        }
        if (NULL != jsObj) {
            const char *value = json_object_get_string(jsObj);
            attribValue.assign(value);
            ret = WD_SUCCESS;
        }
    }
    logger_->Log(TRACE, LOCATION);
    return ret;
}

void WPEDriverProxy::CreateBrowsingContext() {
    logger_->Log(TRACE, LOCATION);
    ExecuteJSCommand("Automation.createBrowsingContext", "", "", "");
    sem_wait(&jsRespWait);
    ParseJSResponse(respMsg.c_str(), "handle", browsingContext_);
    logger_->Log(TRACE, LOCATION);
}

void WPEDriverProxy::CloseBrowsingContext() {
    logger_->Log(TRACE, LOCATION);
    ExecuteJSCommand("Automation.closeBrowsingContext", "handle", "", "");
    logger_->Log(TRACE, LOCATION);
}

WDStatus WPEDriverProxy::GetURL(char *url) {
    std::string tmpResponse;
    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction", 
                     "browsingContextHandle", 
                     "function() { return document.URL }", 
                     "");
    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", tmpResponse);
    strcpy(url, tmpResponse.c_str());
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::GetAttribute(const char* reqParams, char* value) {
    std::string element, key;
    WDStatus retStatus = WD_FAILURE;

    logger_->Log(TRACE, LOCATION);
    json_object *jObj = json_tokener_parse(reqParams);
    if (NULL != jObj) {
        json_object *jIdxObj;
        int elementSize = json_object_array_length(jObj);
        logger_->Log(DEBUG, LOCATION);
        if (WD_GET_ATTRIBUTE_MAX_ARGS == elementSize) {
            jIdxObj = json_object_array_get_idx(jObj, 0); //Element Id
            if (jIdxObj) {
                element.assign(json_object_get_string(jIdxObj));
            }
            jIdxObj = json_object_array_get_idx(jObj, 1); //Key
            if (jIdxObj) {
                key.assign(json_object_get_string(jIdxObj));
            }
        }
        else {
             logger_->Log(WARN, LOCATION);
             return retStatus;
        }
    }

    std::string elementNode, script;
    script = base::StringPrintf("function (e) { return e.getAttribute('%s') }", key.c_str());
    elementNode = base::StringPrintf("{\"%s\":\"%s\"}", WPE_SESSION_IDENTIFIER, element.c_str());

    std::string tmpResponse;
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(), elementNode.c_str());

    sem_wait(&jsRespWait);

    retStatus = ParseJSResponse(respMsg.c_str(), "result", tmpResponse);
    WD_REMOVE_QUOTES(tmpResponse);
    if (retStatus == WD_SUCCESS)
        strcpy(value, tmpResponse.c_str());

    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElement(bool isElements, const char* reqParams, char* element) {
    std::string tmpResponse;
    std::string locator, query, rootElement;
    json_object *jsLocator, *jsQuery, *jsRootElement;
    json_object *jsObj = json_tokener_parse(reqParams);
    WDStatus retStatus = WD_FAILURE;

    if (json_object_object_get_ex(jsObj, "locator", &jsLocator)) {
        locator.assign(json_object_get_string(jsLocator));
        logger_->Log(DEBUG, LOCATION);
        if (json_object_object_get_ex(jsObj, "query", &jsQuery)) {
            query.assign(json_object_get_string(jsQuery));
            if (json_object_object_get_ex(jsObj, "rootElement", &jsRootElement)) {
                rootElement = base::StringPrintf("{\"%s\":\"%s\"}",
                                                 WPE_SESSION_IDENTIFIER,
                                                 json_object_get_string(jsRootElement));
            }
        }
    }

    if (!strcmp(locator.c_str(),"id")) {
        retStatus = FindElementById(query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(),"name")) {
        retStatus = FindElementByName(rootElement.c_str(), query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(),"className")) {
        retStatus = FindElementByNameType(rootElement.c_str(), "ClassName", query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(),"tagName")) {
        retStatus = FindElementByNameType(rootElement.c_str(), "TagName", query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(), "css")) {
        retStatus = FindElementByCss(isElements, rootElement.c_str(), query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(), "xpath")) {
        retStatus = FindElementByXPath(isElements, rootElement.c_str(), query.c_str(), tmpResponse);
    } else {
       logger_->Log(WARN, LOCATION);
    }

    strcpy(element, tmpResponse.c_str());
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementById(const char* query, std::string& element) {
    std::string script;

    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    logger_->Log(DEBUG, "Query = " , query);

    script =  base::StringPrintf("function() { return document.getElementById(\"%s\"); }", query);
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(), "");

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByName(const char* rootElement, const char* query, std::string& element) {
    std::string script;

    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    logger_->Log(DEBUG, "Query = " , query);

    script = (strcmp(rootElement, "")?
                  (base::StringPrintf("function(e) { var children, child = [];            \
                                       children = e.children;                             \
                                       for (var i = 0, j = 0; i < children.length; ++i) { \
                                           if (children[i].getAttribute(\"name\") == \"%s\") {\
                                               child[j++] = children[i];  }              \
                                        } return child; }", query)):
                  (base::StringPrintf("function() { return document.getElementsByName(\"%s\"); }", query)));

    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(), rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByNameType(const char* rootElement, const char* type, const char* query, std::string& element) {
    std::string script;

    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    logger_->Log(DEBUG, "Query = " , query);

    script = (strcmp(rootElement, "")? (base::StringPrintf("function(e) { return e.getElementsBy%s(\"%s\"); }", type, query)):
                                       (base::StringPrintf("function() { return document.getElementsBy%s(\"%s\"); }",type, query)));

    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByXPath(bool isElements, const char* rootElement, const char* query, std::string& element) {
    std::string script, funcStart;
    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    logger_->Log(DEBUG, "Query = " , query);

    funcStart = (strcmp(rootElement, "")? (base::StringPrintf("function (e) {\
                                                               var elem = document.evaluate(\"%s\", e,",
                                                               query)):
                                          (base::StringPrintf("function () {\
                                                               var elem = document.evaluate(\"%s\", document,",
                                                               query)));


    script = (isElements? (base::StringPrintf("%s null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);\
                                                var aResult = new Array();\
                                                for ( var i = 0 ; i < elem.snapshotLength ; i++ ) {\
                                                    aResult.push(elem.snapshotItem(i));\
                                                }\
                                                return aResult;\
                                               }", funcStart.c_str())) :
                           (base::StringPrintf("%s null, XPathResult.ANY_TYPE, null ); \
                                                  return elem.iterateNext();\
                                                 }", funcStart.c_str())));

    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    logger_->Log(TRACE, LOCATION);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByCss(bool isElements, const char* rootElement, const char* query, std::string& element) {
    std::string script, function;
    WDStatus retStatus = WD_FAILURE;
    logger_->Log(TRACE, LOCATION);
    logger_->Log(DEBUG, "Query = " , query);

    function.assign(isElements? "querySelectorAll":"querySelector");

    if (strcmp(rootElement, "")) {
        script =  base::StringPrintf("function(e) { return e.%s(\"%s\"); }", function.c_str(), query);
    } else {
        script =  base::StringPrintf("function() { return document.%s(\"%s\"); }", function.c_str(), query);
    }
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    return retStatus;
}

WDStatus WPEDriverProxy::RemoveView() {
    if (loop_ != NULL) {
        g_main_loop_quit (loop_);
        g_main_loop_unref(loop_);
    }
    WDStatus_ = WPE_WD_STOP;
    if (WpeViewThreadId_) {
        pthread_join (WpeViewThreadId_, NULL);
        WpeViewThreadId_ = 0;
    }
    return WD_SUCCESS;
}

void* WPECommandDispatcherThread(void* pArgs)
{
    WDStatusBuf  stsBuf;
    WDCommandBuf cmdBuf;

    int cmdQueueId, stsQueueId;

    cmdQueueId = stsQueueId = 0;
    WPEDriverProxy* WPEProxy = (WPEDriverProxy*) pArgs;
    WPEProxy->logger_->Log(TRACE, LOCATION);
    if ((cmdQueueId = msgget(WPE_WD_CMD_KEY, 0666)) < 0) {
         WPEProxy->logger_->Log(ERR, LOCATION);
         return 0;
    }
    if ((stsQueueId = msgget(WPE_WD_STATUS_KEY, 0666)) < 0) {
         WPEProxy->logger_->Log(ERR, LOCATION);
         return 0;
    }

    while (WPE_WD_RUN == WPEProxy->WDStatus_) {
        WPEProxy->logger_->Log(INFO, LOCATION);
        if (msgrcv (cmdQueueId, &cmdBuf, WD_CMD_SIZE, 0, 0) >= 0) {
            WPEProxy->logger_->Log(DEBUG, LOCATION);
            stsBuf.status = WD_FAILURE;
            switch (cmdBuf.command) {
                case WD_CREATE_VIEW: {
                    stsBuf.status = WPEProxy->CreateView();
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                    sleep(2);
                    break;
                }
                case WD_REMOVE_VIEW: {
                    stsBuf.status = WPEProxy->RemoveView();
                    break;
                }
                case WD_LOAD_URL: {
                    stsBuf.status = WPEProxy->LoadURL(cmdBuf.message);
                    break;
                }
                case WD_RELOAD: {
                    stsBuf.status = WPEProxy->Reload();
                    break;
                }
                case WD_IS_URL_SUPPORTED: {
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                    stsBuf.status = WPEProxy->IsUrlSupported(cmdBuf.message);
                    break;
                }

                case WD_GET_URL: {
                    stsBuf.status = WPEProxy->GetURL(stsBuf.rspMsg);
                    break;
                }
                case WD_FIND_ELEMENT: {
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                    stsBuf.status = WPEProxy->FindElement(false, cmdBuf.message, stsBuf.rspMsg);
                    break;
                }
                case WD_FIND_ELEMENTS: {
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                    stsBuf.status = WPEProxy->FindElement(true, cmdBuf.message, stsBuf.rspMsg);
                    break;
                }
                case WD_GET_ATTRIBUTE: {
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                    stsBuf.status = WPEProxy->GetAttribute(cmdBuf.message, stsBuf.rspMsg);
                    break;
                }
                case WD_JS_CMD_END:
                case WD_JS_CMD_START:
                case WD_CMD_NONE: //fall through
                    WPEProxy->logger_->Log(DEBUG, LOCATION);
                default:
                    WPEProxy->logger_->Log(WARN, LOCATION);
                    break;
            }
            if (cmdBuf.command > WD_JS_CMD_START && cmdBuf.command < WD_JS_CMD_END) {
                WPEProxy->logger_->Log(DEBUG, LOCATION);
            }
            if (msgsnd (stsQueueId, &stsBuf, WD_STATUS_SIZE, 0) < 0)
                WPEProxy->logger_->Log(ERR, LOCATION);
        }
    }

    msgctl(cmdQueueId, IPC_RMID, NULL);
    msgctl(stsQueueId, IPC_RMID, NULL);

    return 0;
}

int main(int argc, char **argv) {

    bool enableConsoleLog = false;

    pthread_t commandThreadId;
    sem_init(&jsRespWait, 0, 0);
    if (argc >= 2) { //Analyse arguments
        if (std::string(argv[1]) == "verbose") {
            enableConsoleLog = true;
        }
    }
    WPEDriverProxy* WPEProxy = new WPEDriverProxy(enableConsoleLog);

    WPEProxy->WDStatus_ = WPE_WD_RUN;
    pthread_create (&commandThreadId, NULL, WPECommandDispatcherThread, WPEProxy);

    pthread_join (commandThreadId, NULL);
}
