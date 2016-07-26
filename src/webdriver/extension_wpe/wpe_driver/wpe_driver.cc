

#include "extension_wpe/wpe_driver/wpe_driver_impl.h"
#include "extension_wpe/wpe_driver/wpe_driver_impl.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

WPEDriverImpl *WpeDriverImpl;
void* WpeHandle = NULL;

WPEDriver::~WPEDriver() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeHandle = NULL;
    delete WpeDriverImpl;
}

int WPEDriver::WpeCreateView ( void **handle) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeDriverImpl = new WPEDriverImpl();
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeDriverImpl->CreateView();
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    WpeHandle = *handle = WpeDriverImpl;
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return 0;
}

void* WPEDriver::GetViewHandle () {
   printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
   if (WpeDriverImpl)
       return WpeHandle;
   else
       return NULL;
}

bool WPEDriver::isUrlSupported (const std::string& mimeType) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeDriverImpl)
        return WpeDriverImpl->isUrlSupported(mimeType);
    else
        return false;
}

void WPEDriver::WpeReload () {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeDriverImpl)
        WpeDriverImpl->Reload();
    else
        printf("View doesn't exisit\n");

    return;
}

void WPEDriver::WpeRemoveView () {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (WpeDriverImpl)
        WpeDriverImpl->RemoveView();
    else
        printf("View already removed\n");

    return;
}
