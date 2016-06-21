#ifndef WEBDRIVER_WPE_VIEW_UTIL_H_
#define WEBDRIVER_WPE_VIEW_UTIL_H_

#include <string>

//#include "common_util.h"
#include "webdriver_view_id.h"

namespace webdriver {

class Session;	

class WpeViewUtil {
public:
    static bool isUrlSupported(const std::string& url);
    static std::string extractClassName(const std::string& url);
    static std::string makeUrlByClassName(const std::string& className);
    static void* getView(Session* session, const ViewId& viewId);

private:
    static const char url_protocol[];
    WpeViewUtil() {}
    ~WpeViewUtil() {}
};


}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_UTIL_H_
