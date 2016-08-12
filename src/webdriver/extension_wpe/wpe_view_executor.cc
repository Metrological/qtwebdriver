
#include "extension_wpe/wpe_view_executor.h"

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_writer.h"

#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "wpe_view_util.h"
#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"
#include "extension_wpe/uinput_event_dispatcher.h"
#include "extension_wpe/wpe_key_converter.h"

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
    printf("This is %d from %s in %s pWpeView = %x\n",__LINE__,__func__,__FILE__, pWpeView);

    if (NULL != pWpeView) {
        session->logger().Log(kFineLogLevel, "Web executor for view("+viewId.id()+")");    
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        return new WpeViewCmdExecutor(session, viewId);
    }
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
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
    view_ = ((WpeViewHandle* )WpeViewUtil::getWpeView(session_, viewId))->get();
}

WpeViewCmdExecutor::~WpeViewCmdExecutor() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);}

void* WpeViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return NULL;
}

void WpeViewCmdExecutor::SwitchTo(Error** error) {
    CHECK_VIEW_EXISTANCE

    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    session_->set_current_view(view_id_);
    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");
}

void WpeViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    CHECK_VIEW_EXISTANCE
    *can = WpeViewUtil::isUrlSupported(view_, url, error);
}

void WpeViewCmdExecutor::Reload(Error **error) {
    CHECK_VIEW_EXISTANCE
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    ExecuteCommand(view_, WPE_WD_RELOAD, NULL);
}

void WpeViewCmdExecutor::GetSource(std::string* source, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

//void WpeViewCmdExecutor::SendKeys(const ElementId& element, const string16& keys, Error** error) {
void WpeViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    CHECK_VIEW_EXISTANCE
    std::string err_msg;
    std::vector<KeyEvent> key_events;
    int modifiers = session_->get_sticky_modifiers();
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (!KeyConverter::ConvertKeysToWebKeyEvents(keys,
                               session_->logger(),
                               false,
                               &modifiers,
                               &key_events,
                               &err_msg)) {
        session_->logger().Log(kSevereLogLevel, "SendKeys - cant convert keys:"+err_msg);
        *error = new Error(kUnknownError, "SendKeys - cant convert keys:"+err_msg);
        return;
    } //TODO: Test modifier keys + special keys like tab, enter, delete etc

    session_->set_sticky_modifiers(modifiers);
    std::vector<KeyEvent>::iterator it = key_events.begin();
    while (it != key_events.end()) {
       bool consumed = false;
       consumed = UInputEventDispatcher::getInstance()->dispatch(&(*it), consumed);
       printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);



        ++it;
    }
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
    CHECK_VIEW_EXISTANCE
    ExecuteCommand(view_, WPE_WD_LOAD_URL, (void*) &url);
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

void WpeViewCmdExecutor::GetURL(std::string* url, Error** error) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    CHECK_VIEW_EXISTANCE
    ExecuteCommand(view_, WPE_WD_GET_URL, url);
    printf("URL : %s\n", url->c_str());
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

    ExecuteCommand(view_, WPE_WD_REMOVE_VIEW, NULL);
    session_->logger().Log(kInfoLogLevel, "close View("+view_id_.id()+")");

    session_->RemoveView(view_id_);

//    view->close();
}
} //namespace webdriver 
