#include "extension_wpe/wpe_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "wpe_view_util.h"
#include "extension_wpe/wpe_view_handle.h"
#include "base/string_number_conversions.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {
WPEDriver *WpeDriver;

WpeViewCreator::WpeViewCreator()
{
//    WpeDriver = new WPEDriver();
}

WpeViewCreator::~WpeViewCreator()
{
 //   delete WpeDriver;
}
bool WpeViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className,
                                           const Point* position, const Size* size, ViewHandle** view) const {
    printf("%s:%s:%d Enter : requested class name : %s\n", __FILE__, __func__, __LINE__, className.c_str());
    void *WpeHandle;

    if (className.empty() || className == "WpeWebView") {  
        int ret = WpeDriver->WpeCreateView(&WpeHandle, "http://www.google.com");
        if (0 != ret) {
            printf("%s:%s:%d  : WebKitCreateView failed \n", __FILE__, __func__, __LINE__);
            // view was not created
            return false;
        }
    
        printf("%s:%s:%d  : WebKitCreateView created \n", __FILE__, __func__, __LINE__);
        *view = new WpeViewHandle((void*) WpeHandle);
        return true;
    }
    return false;
}

bool WpeViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url,
                                          const Point* position, const Size* size, ViewHandle** view) const {
    printf("\nInside %s:%s:%d\n", __FILE__, __func__, __LINE__);

    Error* tmp_err = NULL;
    if (!WpeViewUtil::isUrlSupported(url, &tmp_err )) {
        if (tmp_err) delete tmp_err;
        return false;
    }

    std::string className = WpeViewUtil::extractClassName(url);

    return CreateViewByClassName(logger, className, position, size, view);
}

} // namespace webdriver
