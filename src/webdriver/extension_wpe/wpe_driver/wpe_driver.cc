

#include "extension_wpe/wpe_driver/wpe_driver_impl.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

WPEDriverImpl *WpeDriverImpl;
void* WpeHandle = NULL;

WPEDriver::~WPEDriver() {
    WpeHandle = NULL;
    delete WpeDriverImpl;
}

int WPEDriver::WpeCreateView ( void **handle, const char* url) {
    WpeDriverImpl = new WPEDriverImpl();
    WpeDriverImpl->CreateView(url);
    WpeHandle = *handle = WpeDriverImpl;
    return 0;
}

void* WPEDriver::GetViewHandle () {
   return WpeHandle;
}

bool WPEDriver::isUrlSupported (const std::string& mimeType) {
    return WpeDriverImpl->isUrlSupported(mimeType);
}
