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

#include "build/build_config.h"

#include "webdriver_session.h"
#include "extension_wpe/uinput_manager.h"
#include "extension_wpe/wpe_key_converter.h"
#include "extension_wpe/wpe_driver/wpe_driver_common.h"

#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

static int lookup_code(int keysym, bool *isShiftRequired);

UInputManager* UInputManager::instance_ = NULL;

UInputManager* UInputManager::getInstance() {
    if (NULL == instance_)
        instance_ = new UInputManager();

    return instance_;
}

UInputManager::UInputManager()
    : deviceDescriptor_(0),
      isReady_(false) {
}

UInputManager::~UInputManager() {
    ioctl(deviceDescriptor_, UI_DEV_DESTROY);   // try destroy device
}

bool UInputManager::registerUinputDevice() {
    struct uinput_user_dev uidev;

    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    deviceDescriptor_ = open("/dev/uinput", O_WRONLY | O_NONBLOCK | O_CREAT | O_NDELAY, S_IREAD | S_IWRITE);

    if (0 > deviceDescriptor_) {
        GlobalLogger::Log(kWarningLogLevel, "Can't open uinput device");
        return false;
    }

    // enable Key and Synchronization events
    int ret = ioctl(deviceDescriptor_, UI_SET_EVBIT, EV_KEY);
    if (0 > ret) {
        GlobalLogger::Log(kWarningLogLevel, "Can't register uinput key events");
        return false;
    }

    ret = ioctl(deviceDescriptor_, UI_SET_EVBIT, EV_SYN);
    if (0 > ret) {
        GlobalLogger::Log(kWarningLogLevel, "Can't register uinput synchronization events");
        return false;
    }

    // initialize device info
    memset(&uidev, 0, sizeof(uidev));
    uidev.id.bustype = BUS_USB;
    uidev.id.version = 0x01;
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "wd_key_input");
    ret = write(deviceDescriptor_, &uidev, sizeof(uidev));

    if (0 > ret) {
        GlobalLogger::Log(kWarningLogLevel, "Can not initialize user input device");
        return false;
    }
    registerHandledKeys();

    ret = ioctl(deviceDescriptor_, UI_DEV_CREATE); // create device
    if (0 > ret) {
        GlobalLogger::Log(kWarningLogLevel, "Can not create user input device");
        return false;
    }

    isReady_ = true;
    return true;
}

int UInputManager::injectKeyEvent(void* event) {
    int res = -1;
    int keyCode = 0;
    bool isShiftRequired = false;
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));

    gettimeofday(&(ev.time), NULL);

    KeyEvent *keyEvent = (KeyEvent *) event;
    int keyText = keyEvent->text().c_str()[0];

    GlobalLogger::Log(kInfoLogLevel, LOCATION);

    // Check keyCode for capital letters
    if ((keyText>='>' && keyText<='Z') ||       // '>','?','@'  included
            (keyText>='!' && keyText<='&') ||   // '!' - '&'
            (keyText>='(' && keyText<='+') ||    // '(' - '+'
            (keyText>='^' && keyText<='_') ||    // '^','_'
            (keyText>='{' && keyText<='}') ||    // '{' - '}'
            '<' == keyText ) {
        ev.type = EV_KEY;
        ev.code = KEY_RIGHTSHIFT;
        res = write(deviceDescriptor_, &ev, sizeof(ev));
    }

    ev.type = EV_KEY;
    ev.value = keyEvent->type();
    keyCode = lookup_code(keyEvent->key(), &isShiftRequired);
    if (isShiftRequired) {
        ev.code = KEY_LEFTSHIFT;
        res = write(deviceDescriptor_, &ev, sizeof(ev));
    }
    ev.code = keyCode;
    res = write(deviceDescriptor_, &ev, sizeof(ev));

    if (ev.value == KeyEvent::KeyRelease) {
        ev.type = EV_SYN;
        ev.code = SYN_REPORT;
        ev.value = 0;
        res = write(deviceDescriptor_, &ev, sizeof(ev));
    }

    GlobalLogger::Log(kInfoLogLevel, LOCATION);
    return res;
}

void UInputManager::registerHandledKeys() {
    // set range of keys
    for (int i=0; i<256; i++) {
        ioctl(deviceDescriptor_, UI_SET_KEYBIT, i);
    }
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE000U);   // POWER
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEF00U);   // MENU
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE002U);   // BACK
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE100U);   // UP
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE101U);   // DOWN
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE102U);   // LEFT
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE103U);   // RIGHT
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE001U);   // OK
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE00EU);   // INFO
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE00FU);   // TEXT
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE403U);   // RECOERD
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE402U);   // STOP
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE301U);   // ONE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE302U);   // TWO
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE303U);   // THREE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE304U);   // FOUR
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE305U);   // FIVE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE306U);   // SIX
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE307U);   // SEVEN
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE308U);   // EIGHT
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE309U);   // NINE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xE300U);   // ZERO
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE01U);   // COMPANION_DEVICE_KEY_LIVE_SWIPE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE02U);   // COMPANION_DEVICE_KEY_VOD_SWIPE
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE03U);   // COMPANION_DEVICE_KEY_PAD_UP
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE04U);   // COMPANION_DEVICE_KEY_PAD_DOWN
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE05U);   // COMPANION_DEVICE_KEY_PAD_LEFT
    ioctl(deviceDescriptor_, UI_SET_KEYBIT, 0xEE06U);   // COMPANION_DEVICE_KEY_PAD_RIGHT
}

bool UInputManager::isReady() {
    return isReady_;
}

int UInputManager::injectSynEvent() {
    struct input_event ev;

    memset(&ev, 0, sizeof(ev));

    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    gettimeofday(&(ev.time), NULL);

    int res = write(deviceDescriptor_, &ev, sizeof(ev));

    return res;
}

static int lookup_code(int keysym, bool *isShiftRequired) {

    if (keysym & WD_NORMAL_KEY_IDENTIFIER) {
        keysym = (keysym & WD_NORMAL_KEY_MASK);
    }

    if (((0x41 <= keysym) && (0x5A >= keysym)) ||  /* Identify Uppercase Letters */
        ((0x21 <= keysym) && (0x29 >= keysym)) ||  /* Identify Specical Characters */
        ((0x7B <= keysym) && (0x7E >= keysym)) ||
         (0x2A == keysym) || (0x2B == keysym) ||
         (0x3A == keysym) || (0x3C == keysym) ||
         (0x3E == keysym) || (0x3F == keysym) ||
         (0x5E == keysym) || (0x5F == keysym) ||
         (0x40 == keysym)) {
        *isShiftRequired = true;
    }

    switch (keysym) {
        /* Special Characters with shift*/
        case 0x21: return KEY_1; // ! = 1 + shiftkey
        case 0x22: return KEY_APOSTROPHE; // " = 2 + shiftkey
        case 0x23: return KEY_3; // # = 3 + shiftkey
        case 0x24: return KEY_4; // $ = 4 + shiftkey
        case 0x25: return KEY_5; // % = 5 + shiftkey
        case 0x26: return KEY_7; // & = 7 + shiftkey
        case 0x27: return KEY_GRAVE; // ~ = ` + shiftkey
        case 0x28: return KEY_9; // ( = 9 + shiftkey
        case 0x29: return KEY_0; // ) = 0 + shiftkey
        case 0x2A: return KEY_8; //* = 8 + shiftkey
        case 0x2B: return KEY_EQUAL; //+ = = + shiftkey

        case 0x3A: return KEY_SEMICOLON; //: = ; + shiftkey
        case 0x3C: return KEY_COMMA; //< = , + shiftkey
        case 0x3E: return KEY_DOT; //> = . + shiftkey
        case 0x3F: return KEY_SLASH; //? = / + shiftkey

        case 0x5E: return KEY_6; // ^ = 6 + shiftkey
        case 0x5F: return KEY_MINUS; // _ = - + shiftkey

        case 0x7B: return KEY_LEFTBRACE; // { = [ +shiftkey
        case 0x7C: return KEY_BACKSLASH; // | = \ + shiftkey
        case 0x7D: return KEY_RIGHTBRACE; // } = ] + shiftkey
        case 0x7E: return KEY_GRAVE; // ~ = ` + shiiftkey

        case 0x40: return KEY_2; //@ = 2 + shiftkey
        /*Special characters without shift*/
        case 0x2C: return KEY_COMMA; //,
        case 0x2D: return KEY_MINUS; //-
        case 0x2E: return KEY_DOT; //.
        case 0x2F: return KEY_SLASH; /* / */
        case 0x3B: return KEY_SEMICOLON; // ;
        case 0x3D: return KEY_EQUAL; // =
        case 0x5B: return KEY_LEFTBRACE; // [
        case 0x5C: return KEY_BACKSLASH; /* \ */
        case 0x5D: return KEY_RIGHTBRACE; // ]

        /*Key code mapping for space key*/
        case 0x20:  return KEY_SPACE;
        /* KeyCode mapping for numerals */
        case 0x30:  return KEY_0;
        case 0x31:  return KEY_1;
        case 0x32:  return KEY_2;
        case 0x33:  return KEY_3;
        case 0x34:  return KEY_4;
        case 0x35:  return KEY_5;
        case 0x36:  return KEY_6;
        case 0x37:  return KEY_7;
        case 0x38:  return KEY_8;
        case 0x39:  return KEY_9;

        /* KeyCode mapping for alphabets */
        case 0x41:
        case 0x61:  return KEY_A;
        case 0x42:
        case 0x62:  return KEY_B;
        case 0x43:
        case 0x63:  return KEY_C;
        case 0x44:
        case 0x64:  return KEY_D;
        case 0x45:
        case 0x65:  return KEY_E;
        case 0x46:
        case 0x66:  return KEY_F;
        case 0x47:
        case 0x67:  return KEY_G;
        case 0x48:
        case 0x68:  return KEY_H;
        case 0x49:
        case 0x69:  return KEY_I;
        case 0x4A:
        case 0x6A:  return KEY_J;
        case 0x4B:
        case 0x6B:  return KEY_K;
        case 0x4C:
        case 0x6C:  return KEY_L;
        case 0x4D:
        case 0x6D:  return KEY_M;
        case 0x4E:
        case 0x6E:  return KEY_N;
        case 0x4F:
        case 0x6F:  return KEY_O;
        case 0x50:
        case 0x70:  return KEY_P;
        case 0x51:
        case 0x71:  return KEY_Q;
        case 0x52:
        case 0x72:  return KEY_R;
        case 0x53:
        case 0x73:  return KEY_S;
        case 0x54:
        case 0x74:  return KEY_T;
        case 0x55:
        case 0x75:  return KEY_U;
        case 0x56:
        case 0x76:  return KEY_V;
        case 0x57:
        case 0x77:  return KEY_W;
        case 0x58:
        case 0x78:  return KEY_X;
        case 0x59:
        case 0x79:  return KEY_Y;
        case 0x5A:
        case 0x7A:  return KEY_Z;
        default: return keysym;
     }
}
