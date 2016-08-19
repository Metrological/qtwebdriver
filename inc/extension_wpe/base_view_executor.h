/*
 * Copyright (C) 2016 TATA ELXSI
 * Copyright (C) 2016 Metrological
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef WEBDRIVER_BASE_VIEW_EXECUTOR_H
#define WEBDRIVER_BASE_VIEW_EXECUTOR_H

#include <string>

#include "webdriver_view_executor.h"
#include "webdriver_error.h"

namespace webdriver {

#define NOT_SUPPORTED_IMPL      {*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}

/// base class for WPE based view's implementation
class BaseViewCmdExecutor : public ViewCmdExecutor {
public:
    BaseViewCmdExecutor(Session* session, ViewId viewId) : ViewCmdExecutor (session, viewId) {}
    virtual ~BaseViewCmdExecutor();

    virtual void GetTitle(std::string* title, Error** error);
    virtual void GetWindowName(std::string* name, Error** error);
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
    virtual void SetOrientation(const std::string& orientation, Error** error);
    virtual void GetOrientation(std::string* orientation, Error** error);

protected:
    void* getView(const ViewId& viewId, Error** error);
private:
};

}  // namespace webdriver

#endif  // WEBDRIVER_BASE_VIEW_EXECUTOR_H
