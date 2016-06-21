#include "extension_wpe/wpe_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "wpe_view_util.h"
#include "extension_wpe/wpe_view_handle.h"
//#include "q_event_filter.h"
#include "base/string_number_conversions.h"

namespace webdriver {

WpeViewCreator::WpeViewCreator() {}

bool WpeViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className,
                                           const Point* position, const Size* size, ViewHandle** view) const {
    printf("\nInside %s:%s:%d\n", __FILE__, __func__, __LINE__);
    // view was not created
    return false;
}

bool WpeViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url,
                                          const Point* position, const Size* size, ViewHandle** view) const {
    printf("\nInside %s:%s:%d\n", __FILE__, __func__, __LINE__);

    if (!WpeViewUtil::isUrlSupported(url))
        return false;

    std::string className = WpeViewUtil::extractClassName(url);

    return CreateViewByClassName(logger, className, position, size, view);
}

} // namespace webdriver
