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
using namespace webdriver;

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

static WKContextInjectedBundleClientV1 _handlerInjectedBundle = {
    { 1, nullptr },
    nullptr, // didReceiveMessageFromInjectedBundle
    // didReceiveSynchronousMessageFromInjectedBundle
    nullptr,// onDidReceiveSynchronousMessageFromInjectedBundle,
    nullptr, // getInjectedBundleInitializationUserData
};

WPEDriverProxy::WPEDriverProxy()
    : WpeViewThreadId_ (0),
      requestID_(1)
{
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

WPEDriverProxy::~WPEDriverProxy()
{
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    RemoveView();
}

static void AutomationCallback(WKStringRef wkRspMsg) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    size_t bufferSize = WKStringGetMaximumUTF8CStringSize(wkRspMsg);
    std::unique_ptr<char> buffer(new char[bufferSize]);
    WKStringGetUTF8CString(wkRspMsg, buffer.get(), bufferSize);

    respMsg.assign(buffer.get());
    printf("\n Response Msg : = %s\n", respMsg.c_str());
    sem_post(&jsRespWait);
}

WDStatus WPEDriverProxy::CreateView() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    WDStatus ret = WD_SUCCESS;
    if (0 != (pthread_create(&WpeViewThreadId_, NULL, RunWpeView, this ))) {
        printf("Can't start RunWpeView Thread\n"); 
        ret = WD_FAILURE;
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return ret; 
}

void* WPEDriverProxy::RunWpeView(void* arg){

    WPEDriverProxy *pWpeDriverProxy =  (WPEDriverProxy*) arg;

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    
    //Create Automation session
    pWpeDriverProxy->webAutomationSession_ = WKWebAutomationSessionCreate(pWpeDriverProxy->context_, pWpeDriverProxy->page_);
    pWpeDriverProxy->CreateBrowsingContext();

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    g_main_loop_run(pWpeDriverProxy->loop_);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (NULL != page_) {
        auto shellURL = WKURLCreateWithUTF8CString(url);
        WKPageLoadURL (page_, shellURL);
        sleep (2);
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return WD_SUCCESS;
}

WDStatus WPEDriverProxy::Reload() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (NULL != page_) {
        WKPageReload (page_);
        sleep (2);
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return WD_SUCCESS;
}

WDStatus WPEDriverProxy::IsUrlSupported (const char* mimeType) {
    bool isUrlSupported = false;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (NULL != page_) {
        isUrlSupported = WKPageCanShowMIMEType(page_, WKStringCreateWithUTF8CString(mimeType));
        if (isUrlSupported){
            return WD_SUCCESS;
        }
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return WD_FAILURE;
}

void WPEDriverProxy::CreateJSScript(const char* methodName, const char* handleStr, const char* jsScript,
                                    const char* argList, std::string& command) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    json_object *jobj = json_object_new_object();
    json_object *jint = json_object_new_int(requestID_);
    json_object_object_add(jobj, "id", jint);
    json_object *jmethodStr = json_object_new_string(methodName);
    json_object_object_add(jobj, "method", jmethodStr);

    //Create Parameters
    {
        json_object *subObj = json_object_new_object();
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WPEDriverProxy::ExecuteJSCommand(const char* methodName, const char* handleStr,
                                      const char* jsScript, const char* argList) {
    std::string command;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CreateJSScript(methodName, handleStr, jsScript, argList, command);
    WKWebAutomationExecuteCommand(webAutomationSession_, WKStringCreateWithUTF8CString(command.c_str()), AutomationCallback);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

WDStatus WPEDriverProxy::ParseJSResponse(const char* response, char* attrib, std::string& attribValue) {
    WDStatus ret = WD_FAILURE;
    json_object *jsObj;
    jsObj = json_tokener_parse(response);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (json_object_object_get_ex(jsObj, "result", &jsObj)) {
        if (strcmp (attrib, "")) {
           json_object_object_get_ex(jsObj, attrib, &jsObj);
        } else {
            //parsing of final result will handle outside this function
        }
        if (NULL != jsObj) {
            const char *value = json_object_get_string(jsObj);
            printf("%s:%s:%d value = %s len = %d\n", __FILE__, __func__, __LINE__, value, strlen(value));

            attribValue.assign(value);
            ret = WD_SUCCESS;
        }
    }
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return ret;
}

void WPEDriverProxy::CreateBrowsingContext() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ExecuteJSCommand("Automation.createBrowsingContext", "", "", "");
    sem_wait(&jsRespWait);
    ParseJSResponse(respMsg.c_str(), "handle", browsingContext_);
    printf("Browsing Context = %s\n", browsingContext_.c_str());
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WPEDriverProxy::CloseBrowsingContext() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ExecuteJSCommand("Automation.closeBrowsingContext", "handle", "", "");
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

WDStatus WPEDriverProxy::GetURL(char *url) {
    std::string tmpResponse;
    WDStatus retStatus = WD_FAILURE;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction", 
                     "browsingContextHandle", 
                     "function() { return document.URL }", 
                     "");
    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", tmpResponse);
    strcpy(url, tmpResponse.c_str());
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

WDStatus WPEDriverProxy::GetAttribute(const char* reqParams, char* value) {
    std::string element, key;
    WDStatus retStatus = WD_FAILURE;

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    json_object *jObj = json_tokener_parse(reqParams);
    if (NULL != jObj) {
        json_object *jIdxObj;
        int elementSize = json_object_array_length(jObj);
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
        if (2 == elementSize) {
            jIdxObj = json_object_array_get_idx(jObj, 0); //Element Id
            if (jIdxObj) {
                element.assign(json_object_get_string(jIdxObj));
            }
            jIdxObj = json_object_array_get_idx(jObj, 1); //Key
            if (jIdxObj) {
                key.assign(json_object_get_string(jIdxObj));
            }
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

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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
        printf("%s:%s:%d locator:id \n", __FILE__, __func__, __LINE__);
        retStatus = FindElementById(rootElement.c_str(), query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(),"name")) {
        printf("%s:%s:%d locator:name \n", __FILE__, __func__, __LINE__);
        retStatus = FindElementByName(rootElement.c_str(), query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(), "css")) {
        printf("%s:%s:%d locator:css \n", __FILE__, __func__, __LINE__);
        retStatus = FindElementByCss(isElements, rootElement.c_str(), query.c_str(), tmpResponse);
    } else if (!strcmp(locator.c_str(), "xpath")) {
        printf("%s:%s:%d locator:xpath \n", __FILE__, __func__, __LINE__);
        retStatus = FindElementByXPath(isElements, rootElement.c_str(), query.c_str(), tmpResponse);
    } else {
       printf("\n%s:%s:%d Invalid Query\n", __FILE__, __func__, __LINE__);
    }

    strcpy(element, tmpResponse.c_str());
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementById(const char* rootElement, const char* query, std::string& element) {
    std::string script;

    WDStatus retStatus = WD_FAILURE;
    printf("%s:%s:%d query = %s \n", __FILE__, __func__, __LINE__, query);

    if (strcmp(rootElement, "")) {
        script =  base::StringPrintf("function(e) { return e.document.getElementById(\"%s\"); }", query);
    } else {
        script =  base::StringPrintf("function() { return document.getElementById(\"%s\"); }", query);
    }
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByName(const char* rootElement, const char* query, std::string& element) {
    std::string script;

    WDStatus retStatus = WD_FAILURE;
    printf("%s:%s:%d query = %s \n", __FILE__, __func__, __LINE__, query);

    if (strcmp(rootElement, "")) {
        script =  base::StringPrintf("function(e) { return e.document.getElementsByName(\"%s\"); }", query);
    } else {
        script =  base::StringPrintf("function() { return document.getElementsByName(\"%s\"); }", query);
    }

    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByXPath(bool isElements, const char* rootElement, const char* query, std::string& element) {
    std::string script, rootArg, link;
    WDStatus retStatus = WD_FAILURE;
    printf("%s:%s:%d query = %s \n", __FILE__, __func__, __LINE__, query);

    rootArg.assign(strcmp(rootElement, "")?"e":"");
    link.assign(strcmp(rootElement, "")?".":"");
    if (isElements) {
        script = base::StringPrintf(
                "function (%s) { \
                     var aResult = new Array();\
                     var a = %s%sdocument.evaluate(\"%s\", document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);\
                     for ( var i = 0 ; i < a.snapshotLength ; i++ ) {\
                          aResult.push(a.snapshotItem(i));\
                     }\
                     return aResult;\
                }",
                rootArg.c_str(), rootArg.c_str(), link.c_str(), query);
    }
    else {
        script = base::StringPrintf(
               "function (%s) {\
                   var elem = %s%sdocument.evaluate(\"%s\", document, null, XPathResult.ANY_TYPE, null ); \
                   return elem.iterateNext();\
               }",
               rootArg.c_str(), rootArg.c_str(), link.c_str(), query);
    }
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

WDStatus WPEDriverProxy::FindElementByCss(bool isElements, const char* rootElement, const char* query, std::string& element) {
    std::string script, function, rootArg, link;
    WDStatus retStatus = WD_FAILURE;
    printf("%s:%s:%d query = %s \n", __FILE__, __func__, __LINE__, query);

    function.assign(isElements? "querySelectorAll":"querySelector");
    rootArg.assign(strcmp(rootElement, "")?"e":"");
    link.assign(strcmp(rootElement, "")?".":"");

    script = base::StringPrintf("function (%s) { return %s%sdocument.%s(\"%s\") }",
                                rootArg.c_str(), rootArg.c_str(), link.c_str(), function.c_str(), query);
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction",
                     "browsingContextHandle",
                     script.c_str(),
                     rootElement);

    sem_wait(&jsRespWait);
    retStatus = ParseJSResponse(respMsg.c_str(), "result", element);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return retStatus;
}

void WPEDriverProxy::RemoveView() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (loop_ != NULL) {
        g_main_loop_quit (loop_);
        g_main_loop_unref(loop_);
    }
    WDStatus_ = WPE_WD_STOP;
    if (WpeViewThreadId_) {
        pthread_join (WpeViewThreadId_, NULL);
        WpeViewThreadId_ = 0;
    }
}

void* WPECommandDispatcherThread(void* pArgs)
{
    WDStatusBuf  stsBuf;
    WDCommandBuf cmdBuf;

    int cmdQueueId, stsQueueId;

    cmdQueueId = stsQueueId = 0;
    WPEDriverProxy* WPEProxy = (WPEDriverProxy*) pArgs;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if ((cmdQueueId = msgget(WPE_WD_CMD_KEY, 0666)) < 0) {
         printf("Error in command queue creation \n");
         return 0;
    }
    if ((stsQueueId = msgget(WPE_WD_STATUS_KEY, 0666)) < 0) {
         printf("Error in status queue creation \n");
         return 0;
    }

    while (WPE_WD_RUN == WPEProxy->WDStatus_) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        if (msgrcv (cmdQueueId, &cmdBuf, WD_CMD_SIZE, 0, 0) >= 0) {
            printf("%s:%s:%d command = %d\n", __FILE__, __func__, __LINE__, cmdBuf.command);
            stsBuf.status = WD_FAILURE;
            switch (cmdBuf.command) {
                case WD_CREATE_VIEW: {
                    stsBuf.status = WPEProxy->CreateView();
                    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                    sleep(2);
                    break;
                }
                case WD_REMOVE_VIEW: {
                    WPEProxy->RemoveView();
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
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
                    stsBuf.status = WPEProxy->IsUrlSupported(cmdBuf.message);
                    break;
                }

                case WD_GET_URL: {
                    stsBuf.status = WPEProxy->GetURL(stsBuf.rspMsg);
                    break;
                }
                case WD_FIND_ELEMENT: {
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
                    stsBuf.status = WPEProxy->FindElement(false, cmdBuf.message, stsBuf.rspMsg);
                    break;
                }
                case WD_FIND_ELEMENTS: {
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
                    stsBuf.status = WPEProxy->FindElement(true, cmdBuf.message, stsBuf.rspMsg);
                    break;
                }
                case WD_GET_ATTRIBUTE: {
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
                    stsBuf.status = WPEProxy->GetAttribute(cmdBuf.message, stsBuf.rspMsg);
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);fflush(stdout);
                    break;
                }
                case WD_JS_CMD_END:
                case WD_JS_CMD_START:
                case WD_CMD_NONE: //fall through
                    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
                default:
                    printf("%s:%s:%d Invalid command\n", __FILE__, __func__, __LINE__);
                    break;
            }
            if (cmdBuf.command > WD_JS_CMD_START && cmdBuf.command < WD_JS_CMD_END) {
                printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
            }
            if (msgsnd (stsQueueId, &stsBuf, WD_STATUS_SIZE, 0/*IPC_NOWAIT*/) < 0)
                 printf("Error in status queue send\n");
            printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
        }
    }

    printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
    msgctl(cmdQueueId, IPC_RMID, NULL);
    msgctl(stsQueueId, IPC_RMID, NULL);

    return 0;
}

int main(int argc, char **argv) {

    pthread_t commandThreadId;
    sem_init(&jsRespWait, 0, 0);

    WPEDriverProxy* WPEProxy = new WPEDriverProxy;
    WPEProxy->WDStatus_ = WPE_WD_RUN;
    pthread_create (&commandThreadId, NULL, WPECommandDispatcherThread, WPEProxy);

    pthread_join (commandThreadId, NULL);
}
