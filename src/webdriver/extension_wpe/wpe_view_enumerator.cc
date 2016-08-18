#include "extension_wpe/wpe_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

void WpeViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ViewHandlePtr handle(new WpeViewHandle(GetWpeViewHandle()));
    if (handle != NULL) {
        ViewId viewId = session->GetViewForHandle(handle);
        if (!viewId.is_valid()) {
                if (session->AddNewView(handle, &viewId))  {
                    session->logger().Log(kInfoLogLevel,
                        "WpeViewEnumerator found new view("+viewId.id()+")");
                }
            }
            if (viewId.is_valid()) {
                views->insert(viewId);
            }
        }

}

} // namespace webdriver
