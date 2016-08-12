
#include "build/build_config.h"
#include "extension_wpe/wpe_key_converter.h"
#include "extension_wpe/uinput_event_dispatcher.h"

#if defined(OS_LINUX)

UInputEventDispatcher* UInputEventDispatcher::_instance = NULL;

UInputEventDispatcher::UInputEventDispatcher() {
}

UInputEventDispatcher::~UInputEventDispatcher() {
}

UInputEventDispatcher* UInputEventDispatcher::getInstance() {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (NULL == _instance) {
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        _instance = new UInputEventDispatcher;
    }
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    return _instance;
}

void UInputEventDispatcher::registerUInputManager(UInputManager *manager) {
    _eventManager = manager;
}

bool UInputEventDispatcher::dispatch(void *event, bool consumed) {
    printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
    if (consumed)
        return false;

    if(NULL != event) {
        printf("This is %d from %s in %s\n",__LINE__,__func__,__FILE__);
        _eventManager->injectKeyEvent(event);

        return true;
    }
    return false;
}

#endif // OS_LINUX
