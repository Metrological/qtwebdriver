#ifndef WEBDRIVER_WPE_VIEW_ENUMERATOR_H_
#define WEBDRIVER_WPE_VIEW_ENUMERATOR_H_

#include <string>
#include <vector>
#include <set>

#include "webdriver_view_enumerator.h"


namespace webdriver {

/// class for WebView enumerator
class WpeViewEnumeratorImpl : public AbstractViewEnumeratorImpl {
public:
	virtual void EnumerateViews(Session* session, std::set<ViewId>* views) const;
};

}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_ENUMERATOR_H_
