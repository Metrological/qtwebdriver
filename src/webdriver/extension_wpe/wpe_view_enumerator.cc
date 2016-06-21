#include "extension_wpe/wpe_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_wpe/wpe_view_handle.h"

namespace webdriver {

void WpeViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

} // namespace webdriver
