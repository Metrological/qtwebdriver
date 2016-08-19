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

#ifndef WEBDRIVER_WPE_VIEW_UTIL_H_
#define WEBDRIVER_WPE_VIEW_UTIL_H_

#include <string>

#include "webdriver_basic_types.h"
#include "webdriver_view_id.h"
#include "webdriver_error.h"

namespace webdriver {

class Session;	

class WpeViewUtil {
public:
    static void* getWpeView(Session* session, const ViewId& viewId);
    static bool isUrlSupported(void* pWpeView, const std::string& url, Error  **error);
    static bool isUrlSupported(const std::string& url, Error  **error);
    static std::string extractClassName(const std::string& url);
    static std::string makeUrlByClassName(const std::string& className);

private:
    static const char url_protocol[];
    WpeViewUtil() {}
    ~WpeViewUtil() {}
};

}  // namespace webdriver

#endif  // WEBDRIVER_WPE_VIEW_UTIL_H_
