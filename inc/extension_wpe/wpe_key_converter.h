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

#ifndef WEBDRIVER_WPE_KEY_CONVERTER_H_
#define WEBDRIVER_WPE_KEY_CONVERTER_H_
#include <string>
#include <vector>

#include "base/string16.h"

namespace webdriver {
using namespace std;

class Logger;

#define WD_NORMAL_KEY_IDENTIFIER 0x2000
#define WD_NORMAL_KEY_MASK       0x00FF

class KeyEvent {
public:
   typedef enum  {
        KeyRelease = 0,
        KeyPress = 1
    } Type;
    typedef enum { //TODO: Map keys to linux/input.h keys
        KEY_ESC = 0x01000000,
        KEY_TAB = 0x01000001,
        KEY_BACKSPACE = 0x01000003,
        KEY_ENTER = 0x01000004,  	     //Key_Return
        KEY_KPENTER = 0x01000005,   	     //Key_Enter
        KEY_INSERT = 0x01000006,
        KEY_DELETE = 0x01000007,
        KEY_PAUSE = 0x01000008,
        KEY_SYSREQ = 0x0100000a,
        KEY_HOME = 0x01000010,
        KEY_END = 0x01000011,
        KEY_LEFT = 0x01000012,
        KEY_UP = 0x01000013,
        KEY_RIGHT = 0x01000014,
        KEY_DOWN = 0x01000015,
        KEY_PAGEUP = 0x01000016,
        KEY_PAGEDOWN = 0x01000017,
        KEY_LEFTSHIFT = 0x01000020,
        KEY_LEFTCTRL = 0x01000021,
        KEY_LEFTMETA = 0x01000022,
        KEY_LEFTALT = 0x01000023,
        KEY_CAPSLOCK = 0x01000024,
        KEY_NUMLOCK = 0x01000025,
        KEY_SCROLLLOCK = 0x01000026,
        KEY_F1 = 0x01000030,
        KEY_F2 = 0x01000031,
        KEY_F3 = 0x01000032,
        KEY_F4 = 0x01000033,
        KEY_F5 = 0x01000034,
        KEY_F6 = 0x01000035,
        KEY_F7 = 0x01000036,
        KEY_F8 = 0x01000037,
        KEY_F9 = 0x01000038,
        KEY_F10 = 0x01000039,
        KEY_F11 = 0x0100003a,
        KEY_F12 = 0x0100003b,
        KEY_SPACE = 0x20,
        KEY_APOSTROPHE = 0x27,
        KEY_QUOTEDBL = KEY_APOSTROPHE,
        KEY_COMMA = 0x2c,
        KEY_MINUS = 0x2d,
        KEY_DOT = 0x2e,
        KEY_SLASH = 0x2f,
        KEY_0 = 0x30,
        KEY_1 = 0x31,
        KEY_2 = 0x32,
        KEY_3 = 0x33,
        KEY_4 = 0x34,
        KEY_5 = 0x35,
        KEY_6 = 0x36,
        KEY_7 = 0x37,
        KEY_8 = 0x38,
        KEY_9 = 0x39,
        KEY_EXCLAM = KEY_1,
        KEY_NUMBERSIGN = KEY_3,
        KEY_DOLLAR = KEY_4,
        KEY_PERCENT = KEY_5,
        KEY_AMPERSAND = KEY_7,
        KEY_ASTERISK = KEY_8,
        KEY_PARENLEFT = KEY_9,
        KEY_PARENRIGHT = KEY_0,
        KEY_COLON = 0x3a,
        KEY_SEMICOLON = KEY_COLON,
        KEY_LESS = KEY_COMMA,
        KEY_EQUAL = 0x3d,
        KEY_PLUS = KEY_EQUAL,
        KEY_GREATER = KEY_DOT,
        KEY_QUESTION = KEY_SLASH,
        KEY_AT = KEY_2,
        KEY_A = 0x41,
        KEY_B = 0x42,
        KEY_C = 0x43,
        KEY_D = 0x44,
        KEY_E = 0x45,
        KEY_F = 0x46,
        KEY_G = 0x47,
        KEY_H = 0x48,
        KEY_I = 0x49,
        KEY_J = 0x4a,
        KEY_K = 0x4b,
        KEY_L = 0x4c,
        KEY_M = 0x4d,
        KEY_N = 0x4e,
        KEY_O = 0x4f,
        KEY_P = 0x50,
        KEY_Q = 0x51,
        KEY_R = 0x52,
        KEY_S = 0x53,
        KEY_T = 0x54,
        KEY_U = 0x55,
        KEY_V = 0x56,
        KEY_W = 0x57,
        KEY_X = 0x58,
        KEY_Y = 0x59,
        KEY_Z = 0x5a,
        KEY_LEFTBRACE = 0x5b,
        KEY_BACKSLASH = 0x5c,
        KEY_RIGHTBRACE = 0x5d,
        Key_AsciiCircum = KEY_6,
        Key_Underscore = KEY_MINUS,
        KEY_BAR = KEY_BACKSLASH,
        KEY_GRAVE = 0x7e,
        KEY_UNKNOWN = 0x01ffffff,
        KEY_META=0x01000009
    } Key;

    typedef enum {
        NoModifier           = 0x00000000,
        ShiftModifier        = 0x02000000,
        ControlModifier      = 0x04000000,
        AltModifier          = 0x08000000,
        MetaModifier         = 0x10000000,
        KeypadModifier       = 0x20000000,
        GroupSwitchModifier  = 0x40000000,
        KeyboardModifierMask = 0xfe000000
    } KeyboardModifiers;
	
    inline int key() const { return k; }
    inline KeyboardModifiers modifiers() const { return static_cast<KeyboardModifiers>(modState); }
    inline Type type() const { return static_cast<Type>(t); }
    inline string text() const { return txt; }
    inline bool isAutoRepeat() const { return autor; }

    KeyEvent(Type type, int key, KeyboardModifiers modifiers, string text, bool autorep)
           : t(type),
             k(key),
             modState(modifiers),
             txt(text),
             autor(autorep) {}

    KeyEvent(Type type, int key, KeyboardModifiers modifiers)
           : t(type),
             k(key),
             modState(modifiers) {}

    ~KeyEvent() {}

protected:
    short t;
    int k;
    int modState;
    string txt;
    bool autor= true;
};

class KeyConverter {
public:
	/// Converts keys into appropriate |WebKeyEvent|s. This will do a best effort
	/// conversion. However, if the input is invalid it will return false and set
	/// an error message. If |release_modifiers| is true, add an implicit NULL
	/// character to the end of the input to depress all modifiers. |modifiers|
	/// acts both an input and an output, however, only when the conversion
	/// process is successful will |modifiers| be changed.
    static bool ConvertKeysToWebKeyEvents(const string16& client_keys,
                                          const Logger& logger,
                                          bool release_modifiers,
                                          int* modifiers,
                                          std::vector<KeyEvent>* client_key_events,
                                          std::string* error_msg);
private:
    KeyConverter() {};
    ~KeyConverter() {};

    struct ModifierMaskAndKeyCode {
    	int mask;
    	unsigned int key_code;
    };

    static const ModifierMaskAndKeyCode kModifiers[];
    static const KeyEvent::Key kSpecialWebDriverKeys[];

    static bool IsModifierKey(char16 key);
    static bool KeyCodeFromSpecialWebDriverKey(char16 key, KeyEvent::Key* key_code);
    static bool KeyCodeFromShorthandKey(char16 key_utf16,
                                        KeyEvent::Key*  key_code,
                                        bool* client_should_skip);

};

}  // namespace webdriver
#endif  // WEBDRIVER_WPE_KEY_CONVERTER_H_
