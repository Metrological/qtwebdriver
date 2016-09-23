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

#include <curl/curl.h>

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

    // Implementation of MimeType parser
    std::string mimeType;

    CURL *curl;
    CURLcode res;

    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    curl = curl_easy_init();
    if(curl) {
        char *tmpMimeType = NULL;
        FILE *f = fopen("/dev/null", "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        res = curl_easy_perform(curl);
        fclose(f);

        if(CURLE_OK == res) {
            // ask for the content-type
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &tmpMimeType);
        }
        if ( NULL != tmpMimeType ) {
             mimeType.assign(tmpMimeType);
        }
        // always cleanup
        curl_easy_cleanup(curl);
    }

    bool isUrlSupported = false;
    if ( mimeType.empty() ) {
        // Content Type is not specified, hence setting the return
        // status as supportable URL
        isUrlSupported = true;
    } else {
        ExecuteCommand(pWpeView, WPE_WD_IS_URL_SUPPORTED, (void*) mimeType.c_str(), &isUrlSupported);
    }

    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    return isUrlSupported;
}	

bool WpeViewUtil::isUrlSupported(const std::string& url, Error 	**error) {
    return true;
}

std::string WpeViewUtil::extractClassName(const std::string& url) {
    const std::string widget_prefix(url_protocol);	
    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    return url.substr(widget_prefix.length());
}

std::string WpeViewUtil::makeUrlByClassName(const std::string& className) {
    const std::string widget_prefix(url_protocol);
    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    return widget_prefix + className;	
}

void* WpeViewUtil::getWpeView(Session* session, const ViewId& viewId) {
    session->logger().Log(kInfoLogLevel, LOCATION);
    ViewHandle* viewHandle =  session->GetViewHandle(viewId);

    if (NULL == viewHandle) 
        return NULL;

    session->logger().Log(kInfoLogLevel, LOCATION);
    return viewHandle;
}

} // namespace webdriver
