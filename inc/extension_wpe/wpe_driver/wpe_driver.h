#ifndef __WPE_DRIVER_H__
#define __WPE_DRIVER_H__
#include <string>
#include <pthread.h>
#include "extension_wpe/wpe_driver/wpe_driver_common.h"

enum WPEDriverCommand {
    WPE_WD_LOAD_URL,
    WPE_WD_RELOAD,
    WPE_WD_REMOVE_VIEW,
    WPE_WD_IS_URL_SUPPORTED,
    WPE_WD_GET_URL,
    WPE_WD_FIND_ELEMENT,
    WPE_WD_FIND_ELEMENTS,
    WPE_WD_GET_ATTRIBUTE
};

class WPEDriver {
public:
    int cmdQueueId;
    int stsQueueId;
    WDStatusBuf  stsBuff;
    WDCommandBuf cmdBuff;
    pthread_t    WpeDriverThreadId;
    
    WPEDriver();
    ~WPEDriver();

    int   WpeCreateView();
    void* GetViewHandle();
   
    bool isUrlSupported(const std::string& mimeType);
    int WpeLoadURL(const std::string* url);
    int WpeReload();
    int WpeGetURL(std::string* url);
    int WpeFindElement(std::string* arg, std::string* output);
    int WpeFindElements(std::string* arg, std::string* output);
    int WpeGetAttribute(std::string* arg, std::string* output);
    int WpeRemoveView();

    static void* RunWpeProxy(void* arg);
};

int CreateWpeView(void** handle);
void* GetWpeViewHandle();
int ExecuteCommand(void* handle, WPEDriverCommand command, void* arg, void* ret);

#endif // __WPE_DRIVER_H__
 
