#include <stdio.h>
#include "extension_wpe/wpe_view_handle.h"

namespace webdriver {

WpeViewHandle::WpeViewHandle() :
	webkit_view(NULL) { }

WpeViewHandle::WpeViewHandle( void* view) 
	: webkit_view(view) { }

bool WpeViewHandle::equals(const ViewHandle* other) const {
    const WpeViewHandle* toCompare = dynamic_cast<const WpeViewHandle*>(other);
   
     printf("\n:%s:%s:%d %x = %x\n", __FILE__, __func__, __LINE__, webkit_view, toCompare->webkit_view);
     return ( webkit_view == toCompare->webkit_view);
}

} // namespace webdriver
