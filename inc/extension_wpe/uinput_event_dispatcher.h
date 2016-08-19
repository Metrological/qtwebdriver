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

#ifndef UINPUT_EVENT_DISPATCHER_H
#define UINPUT_EVENT_DISPATCHER_H


#include "extension_wpe/event_dispatcher.h"
#include "extension_wpe/uinput_manager.h"

class UInputEventDispatcher : public EventDispatcher
{
public:
    /// Constructor
    /// @param manager - pointer to user events manager
    //UInputEventDispatcher(UInputManager *manager);
    UInputEventDispatcher();
    /// Destructor
    ~UInputEventDispatcher();

    /// Dispatch event to user input device
    /// @param event - pointer to event for dispatching
    /// @param consumed - flag whether event was consumed by previous dispatchers
    /// @return true, if event was consumed, else false
    virtual bool dispatch(void* event, bool consumed);
    void   registerUInputManager(UInputManager* manager);
    static UInputEventDispatcher* getInstance();

private:
    UInputManager *_eventManager;
    static UInputEventDispatcher *_instance;
};

#endif // UINPUT_EVENT_DISPATCHER_H
