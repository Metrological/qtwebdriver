/****************************************************************************
**
** Copyright © 1992-2014 Cisco and/or its affiliates. All rights reserved.
** All rights reserved.
** 
** $CISCO_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $CISCO_END_LICENSE$
**
****************************************************************************/

#include "build/build_config.h"

#if defined(OS_LINUX)

#include "extension_wpe/uinput_manager.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>

static int lookup_code(int keysym);

UInputManager* UInputManager::_instance = NULL;

UInputManager* UInputManager::getInstance()
{
    if (NULL == _instance)
        _instance = new UInputManager();

    return _instance;
}

UInputManager::UInputManager()
    : _deviceDescriptor(0),
      _isReady(false)
{
    _logger = new Logger();
}

UInputManager::~UInputManager()
{
    delete _logger;
    ioctl(_deviceDescriptor, UI_DEV_DESTROY);   // try destroy device
}

bool UInputManager::registerUinputDevice()
{
    struct uinput_user_dev uidev;

    _deviceDescriptor = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT | O_NDELAY, S_IREAD | S_IWRITE);
    //_logger->Log(kInfoLogLevel, std::string("#### Device descriptor: ") + QString::number(_deviceDescriptor).toStdString());

    if (0 > _deviceDescriptor)
    {
        _logger->Log(kWarningLogLevel, "Can't open uinput device");
        return false;
    }

    // enable Key and Synchronization events
    int ret = ioctl(_deviceDescriptor, UI_SET_EVBIT, EV_KEY);
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can't register uinput key events");
        return false;
    }
    ret = ioctl(_deviceDescriptor, UI_SET_EVBIT, EV_SYN);
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can't register uinput synchronization events");
        return false;
    }

    // initialize device info
    memset(&uidev, 0, sizeof(uidev));
    uidev.id.bustype = BUS_USB;
    uidev.id.version = 0x01;
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "wd_key_input");
    ret = write(_deviceDescriptor, &uidev, sizeof(uidev));

    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can not initialize user input device");
        return false;
    }
    registerHandledKeys();

    ret = ioctl(_deviceDescriptor, UI_DEV_CREATE); // create device
    if (0 > ret)
    {
        _logger->Log(kWarningLogLevel, "Can not create user input device");
        return false;
    }

    _isReady = true;
    return true;
}

int UInputManager::injectKeyEvent(void* event)
{
    struct input_event ev;
    int res = -1;

    memset(&ev, 0, sizeof(ev));

    gettimeofday(&(ev.time), NULL);

/*    if (QKeyEvent::KeyPress == event->type())
    {
        ev.value = 1;
    }
    else if (QKeyEvent::KeyRelease == event->type())
    {
        ev.value = 0;
    }
*/
//    printf("#### Key code: %d\n", event->key());
//    printf("#### Key text: %s, modifiers: %d\n", event->text().toStdString().c_str(), (int)event->modifiers());
    int key_text;// = event->text().toStdString().c_str()[0];

    // Check keyCode for capital letters
    if ((key_text>='>' && key_text<='Z') ||       // '>','?','@'  included
            (key_text>='!' && key_text<='&') ||   // '!' - '&'
            (key_text>='(' && key_text<='+') ||    // '(' - '+'
            (key_text>='^' && key_text<='_') ||    // '^','_'
            (key_text>='{' && key_text<='}') ||    // '{' - '}'
            '<' == key_text )
    {
        ev.type = EV_KEY;
        ev.code = KEY_RIGHTSHIFT;
        res = write(_deviceDescriptor, &ev, sizeof(ev));
    }

    ev.type = EV_KEY;
    ev.code = 0;//lookup_code(event->key());

    res = write(_deviceDescriptor, &ev, sizeof(ev));
/*    _logger->Log(kInfoLogLevel, std::string("#### Write event time: ") +
                 QString::number(ev.time.tv_sec).toStdString() + std::string(".") +
                 QString::number(ev.time.tv_usec).toStdString() + std::string(", res: ") +
                 QString::number(res).toStdString() + std::string(", errno: ") +
                 QString::number(errno).toStdString() );
*/
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;

    res = write(_deviceDescriptor, &ev, sizeof(ev));

    return res;
}

void UInputManager::registerHandledKeys()
{
    // set range of keys
    for (int i=0; i<256; i++)
    {
        ioctl(_deviceDescriptor, UI_SET_KEYBIT, i);
    }
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE000U);   // POWER
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEF00U);   // MENU
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE002U);   // BACK
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE100U);   // UP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE101U);   // DOWN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE102U);   // LEFT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE103U);   // RIGHT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE001U);   // OK
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE00EU);   // INFO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE00FU);   // TEXT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE403U);   // RECOERD
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE402U);   // STOP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE301U);   // ONE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE302U);   // TWO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE303U);   // THREE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE304U);   // FOUR
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE305U);   // FIVE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE306U);   // SIX
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE307U);   // SEVEN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE308U);   // EIGHT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE309U);   // NINE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xE300U);   // ZERO
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE01U);   // COMPANION_DEVICE_KEY_LIVE_SWIPE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE02U);   // COMPANION_DEVICE_KEY_VOD_SWIPE
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE03U);   // COMPANION_DEVICE_KEY_PAD_UP
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE04U);   // COMPANION_DEVICE_KEY_PAD_DOWN
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE05U);   // COMPANION_DEVICE_KEY_PAD_LEFT
    ioctl(_deviceDescriptor, UI_SET_KEYBIT, 0xEE06U);   // COMPANION_DEVICE_KEY_PAD_RIGHT
}

bool UInputManager::isReady()
{
    return _isReady;
}

int UInputManager::injectSynEvent()
{
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&(ev.time), NULL);

    int res = write(_deviceDescriptor, &ev, sizeof(ev));

    return res;
}

static int lookup_code(int keysym) {

    switch(keysym) {
    default:		return keysym;
    }
}

#endif // OS_LINUX
