#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__
#include <string>

class WPEDriver {
public:
    WPEDriver(){}
    ~WPEDriver();
    int WpeCreateView ( void **handle, const char* url);
    void* GetViewHandle ();
    bool isUrlSupported (const std::string& mimeType);
};

extern WPEDriver *WpeDriver;

#endif // __WPE_DRIVER_H__
 
