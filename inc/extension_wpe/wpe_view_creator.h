#ifndef WEBDRIVER_WPE_VIEW_CREATOR_H_
#define WEBDRIVER_WPE_VIEW_CREATOR_H_

#include <string>

#include "webdriver_view_factory.h"
#include "webdriver_logging.h"

namespace webdriver {

/// base class for custom view's creators
class WpeViewCreator : public ViewCreator {
public:
    WpeViewCreator();
    virtual ~WpeViewCreator();

    virtual bool CreateViewByClassName(const Logger& logger, const std::string& className,
                                       const Point* position, const Size* size, ViewHandle** view) const;

    virtual bool CreateViewForUrl(const Logger& logger, const std::string& url,
                                  const Point* position, const Size* size, ViewHandle** view) const;

private:
    DISALLOW_COPY_AND_ASSIGN(WpeViewCreator);
};


}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_CREATOR_H_
