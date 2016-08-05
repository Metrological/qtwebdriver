
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

using std::unique_ptr;

std::string respMsg;
sem_t  jsRespWait;

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
              : requestID_(1)
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

WPEDriverProxy::~WPEDriverProxy()
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

static void AutomationCallback(WKStringRef wkRspMsg) {
    printf("\nInside Callback %s:%s:%d \n", __FILE__, __func__, __LINE__);
    size_t bufferSize = WKStringGetMaximumUTF8CStringSize(wkRspMsg);
    std::unique_ptr<char> buffer(new char[bufferSize]);
    size_t stringLength = WKStringGetUTF8CString(wkRspMsg, buffer.get(), bufferSize);

    respMsg.assign(buffer.get());
    printf("\n Response : = %s\n", respMsg.c_str());
    sem_post(&jsRespWait);
}

WDStatus WPEDriverProxy::CreateView () {
    WDStatus ret = WD_SUCCESS;
    printf("\nInside %s:%s:%d Launching WKView\n", __FILE__, __func__, __LINE__);
    if (0 != (pthread_create(&WpeViewThreadId_, NULL, RunWpeView, this ))) {
        printf("Can't start RunWpeView Thread\n"); 
        ret = WD_FAILURE;
    }
    printf("\nInside %s:%s:%d Launching WKView\n", __FILE__, __func__, __LINE__);
    return ret; 
}

void* WPEDriverProxy::RunWpeView (void *arg){

    WPEDriverProxy *pWpeDriverProxy =  (WPEDriverProxy*) arg;

    pWpeDriverProxy->loop_ = g_main_loop_new(nullptr, FALSE);

    auto contextConfiguration = WKContextConfigurationCreate();
    auto injectedBundlePath = WKStringCreateWithUTF8CString("/usr/lib/libWebDriver_wpe_driver_injected_bundle.so");
    WKContextConfigurationSetInjectedBundlePath(contextConfiguration, injectedBundlePath);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    gchar *wpeStoragePath = g_build_filename(g_get_user_cache_dir(), "wpe", "local-storage", nullptr);
    g_mkdir_with_parents(wpeStoragePath, 0700);
    auto storageDirectory = WKStringCreateWithUTF8CString(wpeStoragePath);
    g_free(wpeStoragePath);
    WKContextConfigurationSetLocalStorageDirectory(contextConfiguration, storageDirectory);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    gchar *wpeDiskCachePath = g_build_filename(g_get_user_cache_dir(), "wpe", "disk-cache", nullptr);
    g_mkdir_with_parents(wpeDiskCachePath, 0700);
    auto diskCacheDirectory = WKStringCreateWithUTF8CString(wpeDiskCachePath);
    g_free(wpeDiskCachePath);
    WKContextConfigurationSetDiskCacheDirectory(contextConfiguration, diskCacheDirectory);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    WKRelease(injectedBundlePath);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    pWpeDriverProxy->context_ = WKContextCreateWithConfiguration(contextConfiguration);
    WKRelease(contextConfiguration);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    auto pageGroupIdentifier = WKStringCreateWithUTF8CString("WPEPageGroup");
    pWpeDriverProxy->pageGroup_ = WKPageGroupCreateWithIdentifier(pageGroupIdentifier);
    WKRelease(pageGroupIdentifier);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    pWpeDriverProxy->preferences_ = WKPreferencesCreate();
    // Allow mixed content.
    WKPreferencesSetAllowRunningOfInsecureContent(pWpeDriverProxy->preferences_, true);
    WKPreferencesSetAllowDisplayOfInsecureContent(pWpeDriverProxy->preferences_, true);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    // By default allow console log messages to system console reporting.
    if (!g_getenv("WPE_SHELL_DISABLE_CONSOLE_LOG"))
      WKPreferencesSetLogsPageMessagesToSystemConsoleEnabled(pWpeDriverProxy->preferences_, true);

    WKPageGroupSetPreferences(pWpeDriverProxy->pageGroup_, pWpeDriverProxy->preferences_);

    pWpeDriverProxy->pageConfiguration_  = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(pWpeDriverProxy->pageConfiguration_, pWpeDriverProxy->context_);
    WKPageConfigurationSetPageGroup(pWpeDriverProxy->pageConfiguration_, pWpeDriverProxy->pageGroup_);
    WKPreferencesSetFullScreenEnabled(pWpeDriverProxy->preferences_, true);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    if (!!g_getenv("WPE_SHELL_COOKIE_STORAGE")) {
      gchar *cookieDatabasePath = g_build_filename(g_get_user_cache_dir(), "cookies.db", nullptr);
      auto path = WKStringCreateWithUTF8CString(cookieDatabasePath);
      g_free(cookieDatabasePath);
      auto cookieManager = WKContextGetCookieManager(pWpeDriverProxy->context_);
      WKCookieManagerSetCookiePersistentStorage(cookieManager, path, kWKCookieStorageTypeSQLite);
    }

    pWpeDriverProxy->view_ = WKViewCreate(pWpeDriverProxy->pageConfiguration_);
    WKViewSetViewClient(pWpeDriverProxy->view_, &s_viewClient.base);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    pWpeDriverProxy->page_ = WKViewGetPage(pWpeDriverProxy->view_);
    WKPageSetPageNavigationClient(pWpeDriverProxy->page_, &s_navigationClient.base);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    auto shellURL = WKURLCreateWithUTF8CString("http://www.google.com");
    WKPageLoadURL(pWpeDriverProxy->page_, shellURL);
    WKRelease(shellURL);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    //Create Automation session
    pWpeDriverProxy->webAutomationSession_ = WKWebAutomationSessionCreate(pWpeDriverProxy->context_, pWpeDriverProxy->page_);
    //WKPageSetControlledByAutomation(pWpeDriverProxy->page_, true); 
    pWpeDriverProxy->CreateBrowsingContext();

    g_main_loop_run(pWpeDriverProxy->loop_);

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WKRelease(pWpeDriverProxy->view_);
    WKRelease(pWpeDriverProxy->pageConfiguration_);
    WKRelease(pWpeDriverProxy->pageGroup_);
    WKRelease(pWpeDriverProxy->context_);
    WKRelease(pWpeDriverProxy->preferences_);

    pWpeDriverProxy->CloseBrowsingContext();
    WKRelease(pWpeDriverProxy->webAutomationSession_);

    return 0;
}

void WPEDriverProxy::Reload() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (NULL != page_) {
        WKPageReload (page_);      
        sleep (2);
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    }
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

bool WPEDriverProxy::isUrlSupported (const std::string& mimeType) {
    
    if (NULL != page_) {
        return true;// (WKPageCanShowMIMEType(page_, mimeType)); TODO enable this once implement mimetype parsing 
                                                             // support in wpe_view_utils.cc
    }
    return false;
}

void WPEDriverProxy::CreateJSScript(const char* methodName, const char* handleStr, const char* jsScript, 
                                    const char* argList, std::string& command) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    json_object *jobj = json_object_new_object();
    json_object *jint = json_object_new_int(requestID_);
    json_object_object_add(jobj, "id", jint);
    json_object *jmethodStr = json_object_new_string(methodName);
    json_object_object_add(jobj, "method", jmethodStr);

    if (strcmp (handleStr, "")) {
        json_object *jhandleStr = json_object_new_string(browsingContext_.c_str());
        json_object_object_add(jobj, handleStr, jhandleStr);
    }

    if (strcmp (jsScript, "")) {
        json_object *jscriptStr = json_object_new_string(jsScript);
        json_object_object_add(jobj, "function", jscriptStr);
		json_object *jargStr = json_object_new_string(argList);
		json_object_object_add(jobj, "arguments", jargStr);
    }

    command.assign(json_object_to_json_string(jobj));
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WPEDriverProxy::ExecuteJSCommand(const  char* methodName, const char* handleStr, 
                                      const char* jsScript, const char* argList) {
    std::string command;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    CreateJSScript(methodName, handleStr, jsScript, argList, command);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    WKWebAutomationExecuteCommand(webAutomationSession_, WKStringCreateWithUTF8CString(command.c_str()), AutomationCallback);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WPEDriverProxy::ParseJSResponse(const char *response, char *attrib, std::string& attribStr) {
    json_object *jsObj;
    jsObj = json_tokener_parse(response);
   
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (strcmp (attrib, "")) { 
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        if(json_object_object_get_ex(jsObj, "result", &jsObj))
            if(json_object_object_get_ex(jsObj, attrib, &jsObj)) {
                printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
                const char *value = json_object_get_string(jsObj);
                printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
                attribStr.assign(value);
            }
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    }
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}
void WPEDriverProxy::CreateBrowsingContext() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    ExecuteJSCommand("Automation.createBrowsingContext", "", "", "");
    sem_wait(&jsRespWait);
    ParseJSResponse(respMsg.c_str(), "handle", browsingContext_);
    printf("Browsing Context = %s\n", browsingContext_.c_str());
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WPEDriverProxy::CloseBrowsingContext() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    ExecuteJSCommand("Automation.closeBrowsingContext", "handle", "", "");
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WPEDriverProxy::GetURL(const std::string& url) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    ExecuteJSCommand("Automation.evaluateJavaScriptFunction", 
	                 "browsingContextHandle", 
	                 "function() { return document.URL }", 
					 "");
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WPEDriverProxy::RemoveView() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
    if (loop_ != NULL) {
        g_main_loop_quit (loop_);
        g_main_loop_unref(loop_);
    }
    WDStatus_ = WPE_WD_STOP; 
    pthread_join (WpeViewThreadId_, NULL);
}

void* WPECommandDispatcherThread (void* pArgs)
{
    WDStatusBuf  stsBuf;
    WDCommandBuf cmdBuf;
 
    int cmdQueueId, stsQueueId;
    cmdQueueId = stsQueueId = 0;
    WPEDriverProxy* WPEProxy = (WPEDriverProxy*) pArgs;
   
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
    if ((cmdQueueId = msgget(WPE_WD_CMD_KEY, 0666)) < 0) {
         printf("Error in command queue creation \n");
         return 0;
    }
    if ((stsQueueId = msgget(WPE_WD_STATUS_KEY, 0666)) < 0) {
         printf("Error in status queue creation \n");
         return 0;
    }

    while (WPE_WD_RUN == WPEProxy->WDStatus_) { 
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
        if (msgrcv (cmdQueueId, &cmdBuf, WD_CMD_SIZE, 0, 0) >= 0) {
            
            printf("This is %d from %s in %s command = %d\n",__LINE__,__func__,__FILE__, cmdBuf.command); 
            stsBuf.status = WD_FAILURE;
            switch (cmdBuf.command) {
                case WD_CREATE_VIEW: {
                    stsBuf.status = WPEProxy->CreateView();
                    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
                    sleep(10);
                    break;
                }
                case WD_REMOVE_VIEW: {
                    WPEProxy->RemoveView();
                    break;
                }
                case WD_RELOAD: {
                    WPEProxy->Reload();
                    //break; fallthrough to check JS Execution Sequence
                }
                case WD_GET_URL: {
                    WPEProxy->GetURL(cmdBuf.message);
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
            if (cmdBuf.command > WD_JS_CMD_START && cmdBuf.command < WD_JS_CMD_END)
            {
                //wait for responces from webkit
                printf("\n Response : = %s\n", respMsg.c_str());
                strcpy (stsBuf.rspMsg, respMsg.c_str()); //TODO: need to check parsing also required here.
            }
            if (msgsnd (stsQueueId, &stsBuf, WD_STATUS_SIZE, 0/*IPC_NOWAIT*/) < 0)
                 printf("Error in status queue send\n");
            printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        }
    }

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
//    msgctl(cmdQueueId, IPC_RMID, NULL);
//    msgctl(stsQueueId, IPC_RMID, NULL);

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
