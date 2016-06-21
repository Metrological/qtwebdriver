#include <stdio.h>
#include "extension_wpe/wpe_view_handle.h"

namespace webdriver {

WpeViewHandle::WpeViewHandle() 
	/*: view_(NULL)*/ { }

WpeViewHandle::WpeViewHandle(void* view) 
	/*: view_(view)*/ { }

bool WpeViewHandle::equals(const ViewHandle* other) const {
     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);
}

} // namespace webdriver
