#include "webdriver_wrapper.h"
#include <webdriver_server.h>

#include <iostream>

using namespace std;

namespace webdriver {

WebdriverWrapper::WebdriverWrapper(){
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
}

int WebdriverWrapper::RunWebdriver(){
     CommandLine cmd_line(CommandLine::NO_PROGRAM);

    /* Parse command line */
    webdriver::Server* wd_server = webdriver::Server::GetInstance();
    if (0 != wd_server->Configure(cmd_line)) {
        cout << "Error while configuring WD server, exiting..." << std::endl;
        return 1;
    }
    printf("Got webdriver server instance...\n");

    /* Start webdriver */
    int startError = wd_server->Start();
    if (startError){
        printf("Error while starting server, errorCode\n");
        return startError;
    }
    state_ = WD_RUNNING;
    printf("webdriver server started...\n");
  
}

int WebdriverWrapper::IsWebdriverRunning()
{
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__); 
    if (state_ == WD_RUNNING) {
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        return 1;
    }
    else{
        return 0;
    }
}

} // namespace webdriver
