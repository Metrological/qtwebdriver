#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__
#include <string>

class WPEDriver {
public:
    WPEDriver(){}
    ~WPEDriver();
    int WpeCreateView ( void **handle);
    void* GetViewHandle ();
    bool isUrlSupported (const std::string& mimeType);
    void WpeReload();
    void WpeRemoveView ();
    static void* RunWpeProxy(void* arg);
};

extern WPEDriver *WpeDriver;

#endif // __WPE_DRIVER_H__
 
