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

#ifndef UINPUT_MANAGER_H
#define UINPUT_MANAGER_H

#include "webdriver_logging.h"

using namespace webdriver;

/**
  Implements functionality for using user input device
 */
class UInputManager
{
public:

    /**
      Returns pointer to UInputManager. Will create new instance, if it not exist
      @return pointer to @sa UInputManager
     */
    static UInputManager* getInstance();

    /**
      Destructor
     */
    ~UInputManager();

    /**
      Register user input devices in OS
      @return true, if device registered successfully, else - false
     */
    bool registerUinputDevice();

    /**
      Send key event to device
      @param event - pointer to event data
      @return keycode value
     */
    int injectKeyEvent(void* event);

    /**
      Check wether uinput device initialized
      @return true, if device initialized, false if not
     */
    bool isReady();

private:
    UInputManager();
    void registerHandledKeys();
    int  injectSynEvent();

private:
    int    _deviceDescriptor;
    Logger *_logger;
    bool   _isReady;

    static UInputManager* _instance;
};

#endif // UINPUT_MANAGER_H
