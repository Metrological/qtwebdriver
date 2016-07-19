#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__

class WPEDriver {
private:
    void *handle_;
public:
    WPEDriver():handle_(NULL) {}
    ~WPEDriver();
    int WpeCreateView ( void **handle, const char* url);
    void* GetViewHandle ();
};

extern WPEDriver *WpeDriver;

#endif // __WPE_DRIVER_H__
 
