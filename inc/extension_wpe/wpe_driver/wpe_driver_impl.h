#ifndef __WPE_DRIVER_IMPL_H__
#define __WPE_DRIVER_IMPL_H__

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>

#include <string>
#include <cstdio>
#include <glib.h>
#include <initializer_list>

class WPEDriverImpl
{
    private:
       WKViewRef        view_;
       WKPageRef        page_;
       WKContextRef     context_;
       WKPageGroupRef   pageGroup_;
       WKPreferencesRef preferences_;
       WKPageConfigurationRef pageConfiguration_;

    public:
       WPEDriverImpl();
       ~WPEDriverImpl();
       int CreateView ( const char* url);
       bool isUrlSupported(const std::string& mimeType);
};

#endif // __WPE_DRIVER_IMPL_H__ 
