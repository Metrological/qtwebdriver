
#include <json-c/json.h>
#include <json-c/json_object.h>

#include "base/stringprintf.h"
#include "base/string_number_conversions.h"
#include "base/json/json_writer.h"
#include "webdriver_session.h"
#include "webdriver_view_factory.h"
#include "wpe_view_util.h"
#include "extension_wpe/wpe_view_executor.h"
#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"
#include "extension_wpe/uinput_event_dispatcher.h"
#include "extension_wpe/wpe_key_converter.h"
using namespace std;

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
    : ViewCmdExecutorCreator() { printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); }

ViewCmdExecutor* WpeViewCmdExecutorCreator::CreateExecutor(Session* session, ViewId viewId) const {
    void* pWpeView = WpeViewUtil::getWpeView(session, viewId);
    if (NULL != pWpeView) {
        session->logger().Log(kFineLogLevel, "Web executor for view("+viewId.id()+")");    
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        return new WpeViewCmdExecutor(session, viewId);
    }
    return NULL;
}

bool WpeViewCmdExecutorCreator::CanHandleView(Session* session, ViewId viewId, ViewType* viewType) const {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    void* pWpeView = WpeViewUtil::getWpeView(session, viewId);
    if (NULL != pWpeView) {
        if (NULL != viewType) *viewType = WPE_VIEW_TYPE;  
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
       return true;
    }

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return false;
}

std::string WpeViewCmdExecutorCreator::GetViewTypeName() const {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return "html";
}

WpeViewCmdExecutor::WpeViewCmdExecutor(Session* session, ViewId viewId) 
    : ViewCmdExecutor (session, viewId) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    view_ = ((WpeViewHandle* )WpeViewUtil::getWpeView(session_, viewId))->get();
}

WpeViewCmdExecutor::~WpeViewCmdExecutor() {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void* WpeViewCmdExecutor::getElement(const ElementId &element, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return NULL;
}

void WpeViewCmdExecutor::SwitchTo(Error** error) {
    CHECK_VIEW_EXISTANCE

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    session_->set_current_view(view_id_);
    session_->logger().Log(kInfoLogLevel, "SwitchTo - set current view ("+view_id_.id()+")");
}

void WpeViewCmdExecutor::CanHandleUrl(const std::string& url, bool* can, Error **error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CHECK_VIEW_EXISTANCE
    *can = WpeViewUtil::isUrlSupported(view_, url, error);
}

void WpeViewCmdExecutor::Reload(Error **error) {
    CHECK_VIEW_EXISTANCE
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    if (!ExecuteCommand(view_, WPE_WD_RELOAD, NULL, NULL)) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    }
    else if(NULL == *error)
        *error = new Error(kUnknownCommand);
}

void WpeViewCmdExecutor::GetSource(std::string* source, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::SendKeys(const string16& keys, Error** error) {
    CHECK_VIEW_EXISTANCE
    std::string err_msg;
    std::vector<KeyEvent> key_events;
    int modifiers = session_->get_sticky_modifiers();
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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
       printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
       ++it;
    }
}

void WpeViewCmdExecutor::GetElementScreenShot(const ElementId& element, std::string* png, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseDoubleClick(Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseButtonUp(Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseButtonDown(Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseClick(MouseButton button, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseWheel(const int delta, Error **error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseMove(const int x_offset, const int y_offset, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::MouseMove(const ElementId& element, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::moveMouseInternal(void* view, int& point) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::ClickElement(const ElementId& element, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error) {
    int retStatus = 0;
    Value *retValue;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CHECK_VIEW_EXISTANCE
    std::string arg, ret;

    json_object *jobj = json_object_new_object();
    json_object *jelement = json_object_new_string((element.id()).c_str());
    json_object_object_add(jobj, "element", jelement);
    json_object *jkey =  json_object_new_string(key.c_str());
    json_object_object_add(jobj, "key", jkey);
    arg.assign(json_object_to_json_string(jobj));
    retStatus = ExecuteCommand(view_, WPE_WD_GET_ATTRIBUTE, (void*)&arg,  (void*) &ret);
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
    if (!retStatus) {
        retValue =  Value::CreateStringValue(ret.c_str());
    }
    else if(NULL == *error)
        *error = new Error(kNoSuchElement);

    if (NULL == value) {
        retValue = Value::CreateNullValue();
    }

    scoped_ptr<Value> ret_value(retValue);
    *value = static_cast<Value*>(ret_value.release());

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
}

void WpeViewCmdExecutor::ClearElement(const ElementId& element, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetElementLocation(const ElementId& element, Point* location, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetElementLocationInView(const ElementId& element, Point* location, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetElementTagName(const ElementId& element, std::string* tag_name, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetElementSize(const ElementId& element, Size* size, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetElementText(const ElementId& element, std::string* element_text, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

#define CREATE_FIND_ELEMENT_ARGS(rootElement, locator, query, arg)  \
do {                                                     \
    json_object *jObj = json_object_new_object();        \
    if (strcmp((root_element.id()).c_str(), "")) {       \
        json_object *jRootElement = json_object_new_string((root_element.id()).c_str()); \
        json_object_object_add(jObj, "rootElement", jRootElement);   \
    }                                                                \
    json_object *jLocator = json_object_new_string(locator.c_str()); \
    json_object_object_add(jObj, "locator", jLocator);               \
    json_object *jQuery =  json_object_new_string(query.c_str());    \
    json_object_object_add(jObj, "query", jQuery);                   \
    arg.assign(json_object_to_json_string(jObj));                    \
}while (0)

void WpeViewCmdExecutor::FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    int retStatus = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);

    CHECK(root_element.is_valid());
    CHECK_VIEW_EXISTANCE
    std::string arg, ret;
    CREATE_FIND_ELEMENT_ARGS(rootElement, locator, query, arg);

    retStatus = ExecuteCommand(view_, WPE_WD_FIND_ELEMENTS, (void*)&arg,  (void*) &ret);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
    if (!retStatus) {
        json_object *jElement, *jIdxObj;
        json_object *jObj = json_tokener_parse(ret.c_str());
        if (NULL != jObj) {
            int elementSize = json_object_array_length(jObj);
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
            for (int i = 0; i < elementSize; ++i) {
                jIdxObj = json_object_array_get_idx(jObj, i);
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
                if (json_object_object_get_ex(jIdxObj, "session-node-Untitled Session", &jElement)) {
                    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                    ElementId tmpElement(json_object_get_string(jElement));
                    (*elements).push_back(tmpElement);
                }
            }
        }
    } else if(NULL == *error)
        *error = new Error(kNoSuchElement);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
}

void WpeViewCmdExecutor::FindElements(void* parent, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error) {
    int retStatus = 0;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CHECK(root_element.is_valid());
    CHECK_VIEW_EXISTANCE
    std::string arg, ret;
    CREATE_FIND_ELEMENT_ARGS(rootElement, locator, query, arg);

    retStatus = ExecuteCommand(view_, WPE_WD_FIND_ELEMENT, (void*)&arg,  (void*) &ret);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
    if (!retStatus) {
        json_object *jElement;
        json_object *jObj = json_tokener_parse(ret.c_str());
        if (NULL != jObj) {
            if (json_object_object_get_ex(jObj, "session-node-Untitled Session", &jElement)) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                ElementId elementId(json_object_get_string(jElement));
                *element = elementId;
            }
        } else {
            ElementId tmpElement;
            *element = tmpElement;
        }
    } else if(NULL == *error)
        *error = new Error(kNoSuchElement);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__); fflush(stdout);
}

void WpeViewCmdExecutor::ActiveElement(ElementId* element, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::NavigateToURL(const std::string& url, bool sync, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CHECK_VIEW_EXISTANCE
    if (!ExecuteCommand(view_, WPE_WD_LOAD_URL, (void*) &url, NULL)) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    }
    else if(NULL == *error)
        *error = new Error(kUnknownCommand);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::GetURL(std::string* url, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    CHECK_VIEW_EXISTANCE
    if (!ExecuteCommand(view_, WPE_WD_GET_URL, NULL, url)) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        printf("URL : %s\n", url->c_str());
    }
    else if(NULL == *error)
        *error = new Error(kUnknownCommand);

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::VisualizerSource(std::string* source, Error** error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

bool WpeViewCmdExecutor::FilterElement(const void* item, const std::string& locator, const std::string& query) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return true;
}

void WpeViewCmdExecutor::FindElementsByXpath(void* parent, const std::string &query, std::vector<ElementId>* elements, Error **error) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
}

void WpeViewCmdExecutor::Close(Error** error) {
    CHECK_VIEW_EXISTANCE

    ExecuteCommand(view_, WPE_WD_REMOVE_VIEW, NULL, NULL);
    session_->logger().Log(kInfoLogLevel, "close View("+view_id_.id()+")");

    session_->RemoveView(view_id_);

//    view->close();
}
} //namespace webdriver 
