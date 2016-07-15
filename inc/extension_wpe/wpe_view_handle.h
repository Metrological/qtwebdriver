#ifndef WEBDRIVER_WPE_VIEW_H_
#define WEBDRIVER_WPE_VIEW_H_

#include <string>

#include "webdriver_view_id.h"


namespace webdriver {

class WpeViewHandle : public ViewHandle {
public:
    WpeViewHandle();
    WpeViewHandle( void* view);
    void *webkit_view;

    virtual bool is_valid() const { return NULL;/* !view_.isNull();*/ };
    virtual bool equals(const ViewHandle* other) const;
    void* get() { return NULL; /*view_.data();*/ };
    
protected:
   // QPointer<QWidget> view_;
//private:
    virtual ~WpeViewHandle() {};
};

}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_H_
