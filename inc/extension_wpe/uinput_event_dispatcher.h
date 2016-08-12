#ifndef UINPUT_EVENT_DISPATCHER_H
#define UINPUT_EVENT_DISPATCHER_H

#ifdef OS_LINUX

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
    virtual bool dispatch(void *event, bool consumed);
    static UInputEventDispatcher* getInstance();
    void registerUInputManager(UInputManager *manager);

private:
    UInputManager* _eventManager;
    static UInputEventDispatcher *_instance;
};

#endif // OS_LINUX

#endif // UINPUT_EVENT_DISPATCHER_H
