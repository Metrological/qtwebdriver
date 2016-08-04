#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__
#include <string>
#include <pthread.h>
#include "extension_wpe/wpe_driver/wpe_driver_common.h"

enum WPEDriverCommand {
    WPE_WD_RELOAD,
    WPE_WD_REMOVE_VIEW,
    WPE_WD_IS_URL_SUPPORTED,
    WPE_WD_GET_URL
};

class WPEDriver {
public:
    int cmdQueueId;
    int stsQueueId;
    WDCommandBuf cmdBuff;
    WDStatusBuf stsBuff;
    pthread_t   WpeDriverThreadId;
    
    WPEDriver();
    ~WPEDriver();
    int WpeCreateView ();
    void* GetViewHandle ();
    bool isUrlSupported (const std::string& mimeType);
    void WpeGetURL(std::string* url);
    void WpeReload();
    void WpeRemoveView ();

    void ParseJSResponse(char *rspMsg, std::string* response);
    static void* RunWpeProxy(void* arg);
};

int CreateWpeView ( void **handle);
void* GetWpeViewHandle ();
int ExecuteCommand (void *handle, WPEDriverCommand command, void* arg) ;

#endif // __WPE_DRIVER_H__
 
