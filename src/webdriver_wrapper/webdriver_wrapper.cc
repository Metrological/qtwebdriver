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

#include <glib.h>
#include <iostream>
#include <versioninfo.h>
#include <webdriver_server.h>
#include <webdriver_switches.h>
#include <webdriver_route_patterns.h>
#include <webdriver_view_transitions.h>

#include "webdriver_wrapper.h"
#include "extension_wpe/uinput_manager.h"
#include "extension_wpe/wpe_view_creator.h"
#include "extension_wpe/wpe_view_executor.h"
#include "extension_wpe/wpe_key_converter.h"
#include "extension_wpe/wpe_view_enumerator.h"
#include "extension_wpe/uinput_event_dispatcher.h"


using namespace std;

namespace webdriver {

void InitUInputClient() {
    // start user input device
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
    UInputManager *manager = UInputManager::getInstance();
    if (!manager->isReady()) {
         printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
         manager->registerUinputDevice();

    }
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
    UInputEventDispatcher::getInstance()->registerUInputManager(manager);
}

WebdriverWrapper::WebdriverWrapper() {
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
}

int WebdriverWrapper::RunWebdriver() {

    loop_= g_main_loop_new(NULL, FALSE);
    CommandLine cmd_line(CommandLine::NO_PROGRAM);

    // Create WPE Driver instance
    // WPE Webkit View
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
    webdriver::ViewCreator* wpeViewCreator = new webdriver::WpeViewCreator();
    webdriver::ViewFactory::GetInstance()->AddViewCreator(wpeViewCreator);
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);

    webdriver::ViewEnumerator::AddViewEnumeratorImpl(new webdriver::WpeViewEnumeratorImpl());
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);

    webdriver::ViewCmdExecutorFactory::GetInstance()->AddViewCmdExecutorCreator(new webdriver::WpeViewCmdExecutorCreator());


    /* Parse command line */
    webdriver::Server* wdServer = webdriver::Server::GetInstance();
    wdServer_ = (void*) wdServer;
    if (0 != wdServer->Configure(cmd_line)) {
        cout << "Error while configuring WD server, exiting..." << std::endl;
        return 1;
    }
    printf("Got webdriver server instance...\n");

    InitUInputClient();

    /* Start Webdriver Server*/
    int startError = wdServer->Start();
    state_ = WD_RUNNING;
    if (startError) {
        printf("Error while starting server, errorCode\n");
        return startError;
    }
    printf("webdriver server started...\n");
    g_main_loop_run(loop_);
    g_main_loop_unref(loop_);

    return 0;
}

int WebdriverWrapper::IsWebdriverRunning() {
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
    if (state_ == WD_RUNNING) {
        printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
        return 1;
    }
    else{
        return 0;
    }
}

int WebdriverWrapper::StopWebDriver() {
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
    webdriver::Server* wdServer = (webdriver::Server*)wdServer_;

    int stopError = wdServer->Stop();
    state_ = WD_UNCONFIGURED;
    printf("state_ from StopWebDriver: %d\n", state_);
    if(stopError) {
        printf("Error while stoping server, errorCode\n");
        return stopError;
    }
    g_main_loop_quit(loop_);
    printf("webdriver server stopped...\n");
    return 0;
}

WebdriverWrapper::~WebdriverWrapper(){
    printf("%s:%s:%d\n",__FILE__,__func__,__LINE__);
}

}
 // namespace webdriver
