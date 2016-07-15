#include <stdio.h>
#include "extension_wpe/wpe_view_handle.h"

namespace webdriver {

WpeViewHandle::WpeViewHandle() :
	webkit_view(NULL) { }

WpeViewHandle::WpeViewHandle( void* view) 
	: webkit_view(view) { }

bool WpeViewHandle::equals(const ViewHandle* other) const {
     printf("\n:%s:%s:%d\n", __FILE__, __func__, __LINE__);
     return ( this == other);
}

} // namespace webdriver
