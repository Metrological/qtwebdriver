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

#include "build/build_config.h"
#include "extension_wpe/wpe_key_converter.h"
#include "extension_wpe/uinput_event_dispatcher.h"
#include "extension_wpe/wpe_driver/wpe_driver_common.h"

UInputEventDispatcher* UInputEventDispatcher::_instance = NULL;

UInputEventDispatcher::UInputEventDispatcher() {
}

UInputEventDispatcher::~UInputEventDispatcher() {
}

UInputEventDispatcher* UInputEventDispatcher::getInstance() {
    if (NULL == _instance) {
        _instance = new UInputEventDispatcher;
    }
    return _instance;
}

void UInputEventDispatcher::registerUInputManager(UInputManager *manager) {
    _eventManager = manager;
}

bool UInputEventDispatcher::dispatch(void *event, bool consumed) {
    if (consumed)
        return false;

    if(NULL != event) {
        _eventManager->injectKeyEvent(event);
        return true;
    }
    return false;
}
