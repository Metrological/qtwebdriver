#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__

class WPEDriver {
public:
    WPEDriver() {}
    ~WPEDriver() {}
    int WpeCreateView ( void **handle, const char* url);
};
#endif // __WPE_DRIVER_H__
 
