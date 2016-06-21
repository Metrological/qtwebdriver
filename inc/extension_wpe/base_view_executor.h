#ifndef WEBDRIVER_BASE_VIEW_EXECUTOR_H
#define WEBDRIVER_BASE_VIEW_EXECUTOR_H

#include <string>
#include <vector>
#include <map>

#include "webdriver_view_executor.h"

#include "webdriver_error.h"

namespace webdriver {

#define NOT_SUPPORTED_IMPL      {*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}

/// base class for QT based view's implementation
class BaseViewCmdExecutor : public ViewCmdExecutor {
public:
    BaseViewCmdExecutor(Session* session, ViewId viewId) : ViewCmdExecutor (session, viewId) {}
    virtual ~BaseViewCmdExecutor();

    virtual void GetTitle(std::string* title, Error **error);
    virtual void GetWindowName(std::string* name, Error ** error);
    virtual void GetBounds(Rect *bounds, Error **error);
    virtual void SetBounds(const Rect& bounds, Error** error);
    virtual void Maximize(Error** error);
    virtual void GetScreenShot(std::string* png, Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void Close(Error** error);
    virtual void SwitchTo(Error** error);
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error);
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error);
    virtual void AcceptOrDismissAlert(bool accept, Error** error);
    virtual void SetOrientation(const std::string &orientation, Error **error);
    virtual void GetOrientation(std::string *orientation, Error **error);

protected:
    void* getView(const ViewId& viewId, Error** error);
private:
//    DISALLOW_COPY_AND_ASSIGN(BaseViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_Q_VIEW_EXECUTOR_H
