
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include "extension_wpe/wpe_driver/wpe_driver_common.h"
#include "extension_wpe/wpe_driver/wpe_driver_proxy.h"


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
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

WPEDriverProxy::~WPEDriverProxy()
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
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
    auto injectedBundlePath = WKStringCreateWithUTF8CString("/usr/llib/libWebDriver_wpe_driver_injected_bundle.so");
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

    g_main_loop_run(pWpeDriverProxy->loop_);

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WKRelease(pWpeDriverProxy->view_);
    WKRelease(pWpeDriverProxy->pageConfiguration_);
    WKRelease(pWpeDriverProxy->pageGroup_);
    WKRelease(pWpeDriverProxy->context_);
    WKRelease(pWpeDriverProxy->preferences_); 

    return 0;
}

void WPEDriverProxy::Reload() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (NULL != page_) {
        WKPageReload (page_);      
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
                    sleep(5);
                    break;
                }
                case WD_REMOVE_VIEW: {
                    WPEProxy->RemoveView();
                    break;
                }
                case WD_RELOAD: {
                    WPEProxy->Reload();
                    break;
                }
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

    WPEDriverProxy* WPEProxy = new WPEDriverProxy;
    WPEProxy->WDStatus_ = WPE_WD_RUN; 
    pthread_create (&commandThreadId, NULL, WPECommandDispatcherThread, WPEProxy);
    
    pthread_join (commandThreadId, NULL);
}
