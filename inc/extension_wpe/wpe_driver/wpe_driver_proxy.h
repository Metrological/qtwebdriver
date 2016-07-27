#ifndef __WPE_DRIVER_PROXY_H__
#define __WPE_DRIVER_PROXY_H__

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>

#include <string>
#include <cstdio>
#include <glib.h>
#include <initializer_list>
#include <semaphore.h>

enum WPEDriverStatus {
    WPE_WD_STOP,
    WPE_WD_RUN
};

class WPEDriverProxy {
    public:
       GMainLoop*       loop_;
       WKViewRef        view_;
       WKPageRef        page_;
       WKContextRef     context_;
       WKPageGroupRef   pageGroup_;
       WKPreferencesRef preferences_;
       WKPageConfigurationRef pageConfiguration_;
       pthread_t WpeViewThreadId_;    
       WPEDriverStatus  WDStatus_;       
       
       WPEDriverProxy ();
       ~WPEDriverProxy ();
       WDStatus CreateView ();
       bool isUrlSupported (const std::string& mimeType);
       static void* RunWpeView (void*);
       void Reload ();
       void RemoveView ();
};

#endif // __WPE_DRIVER_PROXY_H__ 
