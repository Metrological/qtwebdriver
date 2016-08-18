
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
    if (NULL == _instance) {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
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

#endif // OS_LINUX
