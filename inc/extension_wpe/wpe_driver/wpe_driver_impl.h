#ifndef __WPE_DRIVER_IMPL_H__
#define __WPE_DRIVER_IMPL_H__

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>

#include <cstdio>
#include <glib.h>
#include <initializer_list>

class WPEDriverImpl
{
    private:
       WKViewRef        view;
       WKPageRef        page;
       WKContextRef     context;
       WKPageGroupRef   pageGroup;
       WKPreferencesRef preferences;
       WKPageConfigurationRef pageConfiguration;

    public:
       WPEDriverImpl();
       ~WPEDriverImpl();
       int CreateView ( const char* url);
};

#endif // __WPE_DRIVER_IMPL_H__ 
