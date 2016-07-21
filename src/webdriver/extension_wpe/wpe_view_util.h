#ifndef WEBDRIVER_WPE_VIEW_UTIL_H_
#define WEBDRIVER_WPE_VIEW_UTIL_H_

#include <string>

#include "webdriver_basic_types.h"
#include "webdriver_view_id.h"
#include "webdriver_error.h"

namespace webdriver {

class Session;	

class WpeViewUtil {
public:
    static void* getWpeView(Session* session, const ViewId& viewId);
    static bool isUrlSupported(void* pWpeView, const std::string& url, Error  **error);
    static bool isUrlSupported(const std::string& url, Error  **error);
    static std::string extractClassName(const std::string& url);
    static std::string makeUrlByClassName(const std::string& className);

private:
    static const char url_protocol[];
    WpeViewUtil() {}
    ~WpeViewUtil() {}
};


}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_UTIL_H_
