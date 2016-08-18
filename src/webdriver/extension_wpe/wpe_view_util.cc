#include "wpe_view_util.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

const char WpeViewUtil::url_protocol[] = "wpewidget://";

bool WpeViewUtil::isUrlSupported(void *pWpeView, const std::string& url, Error 	**error) {
    if (NULL == pWpeView) { 
        GlobalLogger::Log(kWarningLogLevel, " Invalid WpePage* ");
        return false;
    }
    std::string mimeType;
	
    //TODO: implement mimeType parser
    return true;//ExecuteCommand(pWpeView, WPE_WD_IS_URL_SUPPORTED, NULL);//WpeDriver->isUrlSupported(mimeType) ;
}	

bool WpeViewUtil::isUrlSupported(const std::string& url, Error 	**error) {
    return true;
}

std::string WpeViewUtil::extractClassName(const std::string& url) {
    const std::string widget_prefix(url_protocol);	
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return url.substr(widget_prefix.length());
}

std::string WpeViewUtil::makeUrlByClassName(const std::string& className) {
    const std::string widget_prefix(url_protocol);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return widget_prefix + className;	
}

void* WpeViewUtil::getWpeView(Session* session, const ViewId& viewId) {

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
  
    if (NULL == viewHandle) 
        return NULL;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return viewHandle;

}

} // namespace webdriver
