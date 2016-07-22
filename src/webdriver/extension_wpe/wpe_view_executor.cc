
#include "extension_wpe/wpe_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_writer.h"

#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "wpe_view_util.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

#define CHECK_VIEW_EXISTANCE    \
    if (NULL == view_) { \
        session_->logger().Log(kWarningLogLevel, "checkView - no such web view"); \
        *error = new Error(kNoSuchWindow); \
        return; \
    } 

WpeWidget::WpeWidget() {}
WpeWidget::~WpeWidget() {}

const ViewType WpeViewCmdExecutorCreator::WPE_VIEW_TYPE = 0x13f6;    

WpeViewCmdExecutorCreator::WpeViewCmdExecutorCreator()
	: ViewCmdExecutorCreator() { printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); }

ViewCmdExecutor* WpeViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    void* pWpeView = WpeViewUtil::getWpeView(session, viewId);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);

    if (NULL != pWpeView) {
        session->logger().Log(kFineLogLevel, "Web executor for view("+viewId.id()+")");    
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        return new WpeViewCmdExecutor(session, viewId);
    }
    return NULL;
}

bool WpeViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    void* pWpeView = WpeViewUtil::getWpeView(session, viewId);
    if (NULL != pWpeView) {
        if (NULL != viewType) *viewType = WPE_VIEW_TYPE;  
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
       return true;
    }

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return false;
}

std::string WpeViewCmdExecutorCreator::GetViewTypeName() const {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return "html";
}

WpeViewCmdExecutor::WpeViewCmdExecutor(Session* session, ViewId viewId) 
    : ViewCmdExecutor (session, viewId) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    view_ = WpeViewUtil::getWpeView(session_, viewId);
}

WpeViewCmdExecutor::~WpeViewCmdExecutor() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);}

void* WpeViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return NULL;
}

void WpeViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    CHECK_VIEW_EXISTANCE
    *can = WpeViewUtil::isUrlSupported(view_, url, error);
}

void WpeViewCmdExecutor::Reload(Error **error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetSource(std::string* source, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseDoubleClick(Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseButtonUp(Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseButtonDown(Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseWheel(const int delta, Error **error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::moveMouseInternal(void* view, int& point) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::FindElements(void* parent, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetURL(std::string* url, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

bool WpeViewCmdExecutor::FilterElement(const void* item, const std::string& locator, const std::string& query) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return true;
}

void WpeViewCmdExecutor::FindElementsByXpath(void* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::Close(Error** error) {
    CHECK_VIEW_EXISTANCE

    WpeDriver->WpeRemoveView();
    session_->logger().Log(kInfoLogLevel, "close View("+view_id_.id()+")");

    session_->RemoveView(view_id_);

//    view->close();
}
} //namespace webdriver 
