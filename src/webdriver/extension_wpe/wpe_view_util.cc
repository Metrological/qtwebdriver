#include "wpe_view_util.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
//#include "extension_wpe/wpe_element_handle.h"
#include "extension_wpe/wpe_view_handle.h"


namespace webdriver {

const char WpeViewUtil::url_protocol[] = "wpewidget://";

bool WpeViewUtil::isUrlSupported(const std::string& url) {
	const std::string widget_prefix(url_protocol);
     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);

	if (url.compare(0, widget_prefix.length(), widget_prefix) == 0) {
		return true;
	}

	return false;
}	

std::string WpeViewUtil::extractClassName(const std::string& url) {
	const std::string widget_prefix(url_protocol);	

     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);
	return url.substr(widget_prefix.length());
}

std::string WpeViewUtil::makeUrlByClassName(const std::string& className) {
	const std::string widget_prefix(url_protocol);
     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);

	return widget_prefix + className;	
}

void* WpeViewUtil::getView(Session* session, const ViewId& viewId) {

   printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);

    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
    if (NULL == viewHandle) 
		return NULL;
     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);

   	return NULL;

}

} // namespace webdriver
