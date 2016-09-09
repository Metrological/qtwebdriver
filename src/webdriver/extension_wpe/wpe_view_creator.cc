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

#include "extension_wpe/wpe_view_creator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "wpe_view_util.h"
#include "extension_wpe/wpe_view_handle.h"
#include "base/string_number_conversions.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

WpeViewCreator::WpeViewCreator()
{
}

WpeViewCreator::~WpeViewCreator()
{
}

bool WpeViewCreator::CreateViewByClassName(const Logger& logger, const std::string& className,
                                           const Point* position, const Size* size, ViewHandle** view) const {
    logger.Log(kInfoLogLevel, LOCATION);
    void *WpeHandle;

    if (className.empty() || className == "WpeWebView") {  
        int ret = CreateWpeView(&logger, &WpeHandle);
        if (0 != ret) {
            logger.Log(kInfoLogLevel, LOCATION);
            // view was not created
            return false;
        }
    
        *view = new WpeViewHandle((void*) WpeHandle);
        logger.Log(kInfoLogLevel, LOCATION);
        return true;
    }
    logger.Log(kInfoLogLevel, LOCATION);
    return false;
}

bool WpeViewCreator::CreateViewForUrl(const Logger& logger, const std::string& url,
                                          const Point* position, const Size* size, ViewHandle** view) const {
    logger.Log(kInfoLogLevel, LOCATION);

    Error* tmp_err = NULL;
    if (!WpeViewUtil::isUrlSupported(url, &tmp_err )) {
        if (tmp_err) delete tmp_err;
        return false;
    }

    std::string className = WpeViewUtil::extractClassName(url);

    return CreateViewByClassName(logger, className, position, size, view);
}

} // namespace webdriver
