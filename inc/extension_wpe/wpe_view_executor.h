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

#ifndef WEBDRIVER_WPE_VIEW_EXECUTOR_H_
#define WEBDRIVER_WPE_VIEW_EXECUTOR_H_

#include <extension_wpe/base_view_executor.h>

namespace webdriver {

class WpeViewCmdExecutorCreator : public ViewCmdExecutorCreator  {
public:
    static const ViewType WPE_VIEW_TYPE;
    
    WpeViewCmdExecutorCreator();
    virtual ~WpeViewCmdExecutorCreator(){}

    virtual ViewCmdExecutor* CreateExecutor(Session* session, ViewId viewId) const;
    virtual bool CanHandleView(Session* session, ViewId viewId, ViewType* viewType = NULL) const;
    virtual std::string GetViewTypeName() const;

private:

    DISALLOW_COPY_AND_ASSIGN(WpeViewCmdExecutorCreator);
};	

#define WPE_NOT_SUPPORTED_IMPL      {printf("%s:%d :%s :Not supported\n", __FILE__, __LINE__, __FUNCTION__);*error = new Error(kCommandNotSupported, "Current view doesnt support this command.");}

#define WPE_STUB_IMPL      {printf("%s:%d :%s :Stub\n", __FILE__, __LINE__, __FUNCTION__);}

class WpeViewCmdExecutor : public ViewCmdExecutor {
public:
    explicit WpeViewCmdExecutor(Session* session, ViewId viewId);
    virtual ~WpeViewCmdExecutor();

    virtual void CanHandleUrl(const std::string& url, bool* can, Error **error);
    virtual void GoForward(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GoBack(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void Reload(Error** error);
    virtual void GetSource(std::string* source, Error** error);
    virtual void SendKeys(const string16& keys, Error** error);
    virtual void SendKeys(const ElementId& element, const string16& keys, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetElementScreenShot(const ElementId& element, std::string* png, Error** error);
    virtual void MouseDoubleClick(Error** error);
    virtual void MouseButtonUp(Error** error);
    virtual void MouseButtonDown(Error** error);
    virtual void MouseClick(MouseButton button, Error** error);
    virtual void MouseWheel(const int delta, Error **error);
    virtual void MouseMove(const int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, int x_offset, const int y_offset, Error** error);
    virtual void MouseMove(const ElementId& element, Error** error);
    virtual void ClickElement(const ElementId& element, Error** error);
    virtual void GetAttribute(const ElementId& element, const std::string& key, base::Value** value, Error** error);
    virtual void ClearElement(const ElementId& element, Error** error);
    virtual void IsElementDisplayed(const ElementId& element, bool ignore_opacity, bool* is_displayed, Error** error);
    virtual void IsElementEnabled(const ElementId& element, bool* is_enabled, Error** error);
    virtual void ElementEquals(const ElementId& element1, const ElementId& element2, bool* is_equal, Error** error);
    virtual void GetElementLocation(const ElementId& element, Point* location, Error** error);
    virtual void GetElementLocationInView(const ElementId& element, Point* location, Error** error);
    virtual void GetElementTagName(const ElementId& element, std::string* tag_name, Error** error);
    virtual void IsOptionElementSelected(const ElementId& element, bool* is_selected, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetOptionElementSelected(const ElementId& element, bool selected, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetElementSize(const ElementId& element, Size* size, Error** error);
    virtual void ElementSubmit(const ElementId& element, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetElementText(const ElementId& element, std::string* element_text, Error** error);
    virtual void GetElementCssProperty(const ElementId& element, const std::string& property, base::Value** value, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void FindElements(const ElementId& root_element, const std::string& locator, const std::string& query, std::vector<ElementId>* elements, Error** error);
    virtual void ActiveElement(ElementId* element, Error** error);
    virtual void SwitchToFrameWithNameOrId(const std::string& name_or_id, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SwitchToFrameWithIndex(int index, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SwitchToFrameWithElement(const ElementId& element, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SwitchToTopFrame(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SwitchToTopFrameIfCurrentFrameInvalid(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void NavigateToURL(const std::string& url, bool sync, Error** error);
    virtual void GetURL(std::string* url, Error** error);
    virtual void ExecuteScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error);
    virtual void ExecuteAsyncScript(const std::string& script, const base::ListValue* const args, base::Value** value, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetAppCacheStatus(int* status, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetCookies(const std::string& url, base::ListValue** cookies, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetCookie(const std::string& url, base::DictionaryValue* cookie_dict, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void DeleteCookie(const std::string& url, const std::string& cookie_name, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetStorageKeys(StorageType type, base::ListValue** keys, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetStorageItem(StorageType type, const std::string& key, const std::string& value, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void ClearStorage(StorageType type, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void RemoveStorageItem(StorageType type, const std::string& key, std::string* value, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetStorageSize(StorageType type, int* size, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetGeoLocation(base::DictionaryValue** geolocation, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetGeoLocation(const base::DictionaryValue* geolocation, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchClick(const ElementId& element, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchDoubleClick(const ElementId& element, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchDown(const int &x, const int &y, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchUp(const int &x, const int &y, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchMove(const int &x, const int &y, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchLongClick(const ElementId& element, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchScroll(const int &xoffset, const int &yoffset, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchScroll(const ElementId &element, const int &xoffset, const int &yoffset, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchFlick(const int &xSpeed, const int &ySpeed, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchFlick(const ElementId &element, const int &xoffset, const int &yoffset, const int &speed, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetPlayerState(const ElementId& element, PlayerState*, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetPlayerState(const ElementId& element, PlayerState, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetPlayerVolume(const ElementId& element, double*, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetPlayerVolume(const ElementId& element, double, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetPlayingPosition(const ElementId& element, double*, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetPlayingPosition(const ElementId& element, double, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetMute(const ElementId& element, bool, Error**error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetMute(const ElementId& element, bool*, Error**error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetPlaybackSpeed(const ElementId& element, double, Error**error) WPE_NOT_SUPPORTED_IMPL;;
    virtual void GetPlaybackSpeed(const ElementId& element, double*, Error**error) WPE_NOT_SUPPORTED_IMPL;;
    virtual void VisualizerSource(std::string* source, Error** error);
    virtual void VisualizerShowPoint(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchPinchZoom(const ElementId &element, const double &scale, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void TouchPinchRotate(const ElementId &element, const int &angle, Error** error) WPE_NOT_SUPPORTED_IMPL;

    virtual void SetOnline(bool, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void IsOnline(bool*, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetTitle(std::string* title, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetWindowName(std::string* name, Error ** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetBounds(Rect *bounds, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetBounds(const Rect& bounds, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void Maximize(Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetScreenShot(std::string* png, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void FindElement(const ElementId& root_element, const std::string& locator, const std::string& query, ElementId* element, Error** error);
    virtual void Close(Error** error) ;
    virtual void SwitchTo(Error** error);
    virtual void GetAlertMessage(std::string* text, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetAlertPromptText(const std::string& alert_prompt_text, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void AcceptOrDismissAlert(bool accept, Error** error) WPE_NOT_SUPPORTED_IMPL;
    virtual void SetOrientation(const std::string &orientation, Error **error) WPE_NOT_SUPPORTED_IMPL;
    virtual void GetOrientation(std::string *orientation, Error **error) WPE_NOT_SUPPORTED_IMPL;
    

protected:
    void* getView(const ViewId& viewId, Error** error);

    void* getElement(const ElementId &element, Error** error);
    bool FilterElement(const void* item, const std::string& locator, const std::string& query);
    void moveMouseInternal(void* view, int& point);
    
private:
    void* view_;
    DISALLOW_COPY_AND_ASSIGN(WpeViewCmdExecutor);
};

}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_EXECUTOR_H_
