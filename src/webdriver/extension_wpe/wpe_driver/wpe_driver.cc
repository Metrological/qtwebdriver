

#include "extension_wpe/wpe_driver/wpe_driver_impl.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

WPEDriverImpl *WpeDriverImpl;

WPEDriver::~WPEDriver()
{
    delete WpeDriverImpl;
}

int WPEDriver::WpeCreateView ( void **handle, const char* url)
{
    WpeDriverImpl = new WPEDriverImpl();
    WpeDriverImpl->CreateView(url);
    handle_ = *handle = WpeDriverImpl;
    return 0;
}

void* WPEDriver::GetViewHandle ()
{
   return handle_;
}

