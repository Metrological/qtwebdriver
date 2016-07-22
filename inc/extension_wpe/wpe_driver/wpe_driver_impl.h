#ifndef __WPE_DRIVER_IMPL_H__
#define __WPE_DRIVER_IMPL_H__

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>

#include <string>
#include <cstdio>
#include <glib.h>
#include <initializer_list>
#include <semaphore.h>

enum WPEViewStatus {
    WPE_VIEW_STOP,
    WPE_VIEW_RUN
};

class WPEDriverImpl {
    public:
       sem_t            waitForViewControl_; 
       WKViewRef        view_;
       WKPageRef        page_;
       WKContextRef     context_;
       WKPageGroupRef   pageGroup_;
       WKPreferencesRef preferences_;
       WKPageConfigurationRef pageConfiguration_;
       pthread_t WpeViewThreadID_;    
       WPEViewStatus  ViewStatus_;       
       
       WPEDriverImpl();
       ~WPEDriverImpl();
       int CreateView ();
       bool isUrlSupported(const std::string& mimeType);
       static void* WpeRunView(void*);
       void RemoveView ();
};

#endif // __WPE_DRIVER_IMPL_H__ 
