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

#include "wpe_view_util.h"
#include "webdriver_session.h"
#include "webdriver_util.h"
#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

const char WpeViewUtil::url_protocol[] = "wpewidget://";

bool WpeViewUtil::isUrlSupported(void* pWpeView, const std::string& url, Error 	**error) {
    if (NULL == pWpeView) { 
        GlobalLogger::Log(kWarningLogLevel, " Invalid WpePage* ");
        return false;
    }
    std::string mimeType;
	
    //TODO: implement mimeType parser
    return true;//ExecuteCommand(pWpeView, WPE_WD_IS_URL_SUPPORTED, NULL);//WpeDriver->isUrlSupported(mimeType) ;
}	

bool WpeViewUtil::isUrlSupported(const std::string& url, Error 	**error) {
    return true;
}

std::string WpeViewUtil::extractClassName(const std::string& url) {
    const std::string widget_prefix(url_protocol);	
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return url.substr(widget_prefix.length());
}

std::string WpeViewUtil::makeUrlByClassName(const std::string& className) {
    const std::string widget_prefix(url_protocol);
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return widget_prefix + className;	
}

void* WpeViewUtil::getWpeView(Session* session, const ViewId& viewId) {

    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);
  
    if (NULL == viewHandle) 
        return NULL;
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    return viewHandle;
}

} // namespace webdriver
