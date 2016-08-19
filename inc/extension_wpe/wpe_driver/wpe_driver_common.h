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

#ifndef WPE_DRIVER_COMMON_H_
#define WPE_DRIVER_COMMON_H_

#define WD_CMD_MSG_SIZE    100
#define WD_STATUS_MSG_SIZE 2048
#define WPE_WD_CMD_KEY     1234
#define WPE_WD_STATUS_KEY  4321

enum WDStatus {
   WD_NONE,
   WD_SUCCESS,
   WD_FAILURE
};

enum WDCommand {
   WD_CMD_NONE,
   WD_CREATE_VIEW,
   WD_REMOVE_VIEW,
   WD_LOAD_URL,
   WD_RELOAD,
   WD_JS_CMD_START,
   WD_GET_URL,
   WD_FIND_ELEMENT,
   WD_FIND_ELEMENTS,
   WD_GET_ATTRIBUTE,
   WD_JS_CMD_END
};

struct WDStatusBuf {
    WDStatus status;
    char rspMsg[WD_STATUS_MSG_SIZE];
};

struct WDCommandBuf {
    WDCommand command;
    char message[WD_CMD_MSG_SIZE];
};

const int WD_CMD_SIZE = sizeof(WDCommandBuf);
const int WD_STATUS_SIZE = sizeof(WDStatusBuf);

#endif // WPE_DRIVER_COMMON_H_
