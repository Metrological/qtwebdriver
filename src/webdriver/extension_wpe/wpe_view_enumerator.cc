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

#include "extension_wpe/wpe_view_enumerator.h"

#include "webdriver_session.h"
#include "webdriver_logging.h"

#include "extension_wpe/wpe_view_handle.h"
#include "extension_wpe/wpe_driver/wpe_driver.h"

namespace webdriver {

void WpeViewEnumeratorImpl::EnumerateViews(Session* session, std::set<ViewId>* views) const {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    ViewHandlePtr handle(new WpeViewHandle(GetWpeViewHandle()));
    if (handle != NULL) {
        ViewId viewId = session->GetViewForHandle(handle);
        if (!viewId.is_valid()) {
            if (session->AddNewView(handle, &viewId))  {
                session->logger().Log(kInfoLogLevel,
                                      "WpeViewEnumerator found new view("+viewId.id()+")");
            }
        }
        if (viewId.is_valid()) {
            views->insert(viewId);
        }
    }
}

} // namespace webdriver
