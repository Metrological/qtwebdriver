

#include "extension_wpe/wpe_driver/wpe_driver_impl.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

WPEDriverImpl *WpeDriver;

int WPEDriver::WpeCreateView ( void **handle, const char* url)
{
    WpeDriver = new WPEDriverImpl();
    WpeDriver->CreateView(url);
    *handle = WpeDriver;
    return 0;
}

