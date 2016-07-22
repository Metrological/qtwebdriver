

#include "extension_wpe/wpe_driver/wpe_driver_impl.h"

WPEDriverImpl::WPEDriverImpl()
{
    sem_init(&waitForViewControl_, 0, 1);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

WPEDriverImpl::~WPEDriverImpl()
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
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


int WPEDriverImpl::CreateView () {
    int ret = 0;
    ViewStatus_ = WPE_VIEW_RUN; 
    printf("\nInside %s:%s:%d Launching WKView\n", __FILE__, __func__, __LINE__);
    ret = pthread_create(&WpeViewThreadID_, NULL, WpeRunView, this); 
    if (ret != 0)
        printf("Can't start WpeRunView Thread\n"); 
    
    printf("\nInside %s:%s:%d Launching WKView\n", __FILE__, __func__, __LINE__);
    return ret; 
}

void* WPEDriverImpl::WpeRunView (void *arg){

    WPEDriverImpl *pWpeDriverImpl =  (WPEDriverImpl*) arg;

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

    pWpeDriverImpl->context_ = WKContextCreateWithConfiguration(contextConfiguration);
    WKRelease(contextConfiguration);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    auto pageGroupIdentifier = WKStringCreateWithUTF8CString("WPEPageGroup");
    pWpeDriverImpl->pageGroup_ = WKPageGroupCreateWithIdentifier(pageGroupIdentifier);
    WKRelease(pageGroupIdentifier);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    pWpeDriverImpl->preferences_ = WKPreferencesCreate();
    // Allow mixed content.
    WKPreferencesSetAllowRunningOfInsecureContent(pWpeDriverImpl->preferences_, true);
    WKPreferencesSetAllowDisplayOfInsecureContent(pWpeDriverImpl->preferences_, true);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    // By default allow console log messages to system console reporting.
    if (!g_getenv("WPE_SHELL_DISABLE_CONSOLE_LOG"))
      WKPreferencesSetLogsPageMessagesToSystemConsoleEnabled(pWpeDriverImpl->preferences_, true);

    WKPageGroupSetPreferences(pWpeDriverImpl->pageGroup_, pWpeDriverImpl->preferences_);

    pWpeDriverImpl->pageConfiguration_  = WKPageConfigurationCreate();
    WKPageConfigurationSetContext(pWpeDriverImpl->pageConfiguration_, pWpeDriverImpl->context_);
    WKPageConfigurationSetPageGroup(pWpeDriverImpl->pageConfiguration_, pWpeDriverImpl->pageGroup_);
    WKPreferencesSetFullScreenEnabled(pWpeDriverImpl->preferences_, true);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    if (!!g_getenv("WPE_SHELL_COOKIE_STORAGE")) {
      gchar *cookieDatabasePath = g_build_filename(g_get_user_cache_dir(), "cookies.db", nullptr);
      auto path = WKStringCreateWithUTF8CString(cookieDatabasePath);
      g_free(cookieDatabasePath);
      auto cookieManager = WKContextGetCookieManager(pWpeDriverImpl->context_);
      WKCookieManagerSetCookiePersistentStorage(cookieManager, path, kWKCookieStorageTypeSQLite);
    }

    pWpeDriverImpl->view_ = WKViewCreate(pWpeDriverImpl->pageConfiguration_);
    WKViewSetViewClient(pWpeDriverImpl->view_, &s_viewClient.base);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    pWpeDriverImpl->page_ = WKViewGetPage(pWpeDriverImpl->view_);
    WKPageSetPageNavigationClient(pWpeDriverImpl->page_, &s_navigationClient.base);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    auto shellURL = WKURLCreateWithUTF8CString("www.google.com");
    WKPageLoadURL(pWpeDriverImpl->page_, shellURL);
    WKRelease(shellURL);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    while (WPE_VIEW_STOP != pWpeDriverImpl->ViewStatus_) {
        sem_wait(&pWpeDriverImpl->waitForViewControl_);
    }

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    //g_usleep(100*1000000);
    WKRelease(pWpeDriverImpl->view_);
    WKRelease(pWpeDriverImpl->pageConfiguration_);
    WKRelease(pWpeDriverImpl->pageGroup_);
    WKRelease(pWpeDriverImpl->context_);
    WKRelease(pWpeDriverImpl->preferences_); 

    return 0;
}

bool WPEDriverImpl::isUrlSupported (const std::string& mimeType) {
    
    if (NULL != page_) {
        return true;// (WKPageCanShowMIMEType(page_, mimeType)); TODO enable this once implement mimetype parsing 
                                                             // support in wpe_view_utils.cc
    }
    return false;
}

void WPEDriverImpl::RemoveView() {
   printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
   sem_post(&waitForViewControl_);
   ViewStatus_ = WPE_VIEW_STOP; 
}
