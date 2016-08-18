#ifndef __WPE_DRIVER_PROXY_H__
#define __WPE_DRIVER_PROXY_H__

#include <WPE/WebKit.h>
#include <WPE/WebKit/WKCookieManagerSoup.h>
#include <WPE/WebKit/WKWebAutomation.h>

#include <string>
#include <cstdio>
#include <glib.h>
#include <initializer_list>
#include <semaphore.h>

enum WPEDriverStatus {
    WPE_WD_STOP,
    WPE_WD_RUN
};

class WPEDriverProxy {

public:
    WKViewRef        view_;
    WKPageRef        page_;
    WKContextRef     context_;
    WKPageGroupRef   pageGroup_;
    WKPreferencesRef preferences_;
    WKPageConfigurationRef pageConfiguration_;

    GMainLoop*       loop_;
    pthread_t        WpeViewThreadId_;
    WPEDriverStatus  WDStatus_;

    int              requestID_;
    std::string      browsingContext_;
    WKWebAutomationSessionRef webAutomationSession_;

    WPEDriverProxy();
    ~WPEDriverProxy();

    WDStatus CreateView();
    WDStatus LoadURL(const char* url);
    WDStatus Reload();
    WDStatus GetURL(char* command);
    WDStatus GetAttribute(const char* reqParams, char* value);
    WDStatus FindElement(bool isElements, const char* reqParams, char* element);
    bool     isUrlSupported(const std::string& mimeType);
    void     RemoveView();
    static void* RunWpeView(void*);

private:

    void CreateJSScript(const char* methodName, const char* handleStr, const char* jsScript, 
                        const char* argList, std::string& command);
    void ExecuteJSCommand(const  char* methodName, const char* handleStr, 
                          const char* jsScript, const char* argList);
    WDStatus ParseJSResponse(const char* response, char* attrib, std::string& attribValue);

    WDStatus FindElementById(const char* rootElement, const char* query, std::string& element);
    WDStatus FindElementByName(const char* rootElement, const char* query, std::string& element);
    WDStatus FindElementByCss(bool isElements, const char* rootElement, const char* query, std::string& element);
    WDStatus FindElementByXPath(bool iselements, const char* rootElement, const char* query, std::string& element);
	
    void CreateBrowsingContext();
    void CloseBrowsingContext();
};

#endif // __WPE_DRIVER_PROXY_H__ 
