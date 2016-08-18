
#include "extension_wpe/wpe_key_converter.h"

#include "base/format_macros.h"
#include "base/logging.h"
#include "base/stringprintf.h"
#include "base/utf_string_conversions.h"
#include "webdriver_logging.h"

using namespace std;
namespace webdriver {

string str;

const KeyConverter::ModifierMaskAndKeyCode KeyConverter::kModifiers[] = {
    { KeyEvent::ShiftModifier,     KeyEvent::KEY_LEFTSHIFT },
    { KeyEvent::ControlModifier,   KeyEvent::KEY_LEFTCTRL },
    { KeyEvent::AltModifier,       KeyEvent::KEY_LEFTALT }
};

// Ordered list of all the key codes corresponding to special WebDriver keys.
// These WebDriver keys are defined in the Unicode Private Use Area.
// http://code.google.com/p/selenium/wiki/JsonWireProtocol#/session/:sessionId/element/:id/value
const KeyEvent::Key KeyConverter::kSpecialWebDriverKeys[] = {
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_BACKSPACE,
    KeyEvent::KEY_TAB,
    KeyEvent::KEY_ENTER,	//Key_Return
    KeyEvent::KEY_KPENTER,
    KeyEvent::KEY_LEFTSHIFT,
    KeyEvent::KEY_LEFTCTRL,
    KeyEvent::KEY_ESC,		
    KeyEvent::KEY_SPACE,
    KeyEvent::KEY_PAGEUP,      
    KeyEvent::KEY_PAGEDOWN,    
    KeyEvent::KEY_END,
    KeyEvent::KEY_HOME,
    KeyEvent::KEY_LEFT,
    KeyEvent::KEY_UP,
    KeyEvent::KEY_RIGHT,
    KeyEvent::KEY_DOWN,
    KeyEvent::KEY_INSERT,
    KeyEvent::KEY_DELETE,
    KeyEvent::KEY_SEMICOLON,   
    KeyEvent::KEY_EQUAL,       
    KeyEvent::KEY_0,
    KeyEvent::KEY_1,
    KeyEvent::KEY_2,
    KeyEvent::KEY_3,
    KeyEvent::KEY_4,
    KeyEvent::KEY_5,
    KeyEvent::KEY_6,
    KeyEvent::KEY_7,
    KeyEvent::KEY_8,
    KeyEvent::KEY_9,
    KeyEvent::KEY_8,	         //Key_Asterisk
    KeyEvent::KEY_EQUAL,	 //Key_Plus
    KeyEvent::KEY_COMMA,
    KeyEvent::KEY_MINUS,
    KeyEvent::KEY_DOT,		 //Key_Period
    KeyEvent::KEY_SLASH,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_UNKNOWN,
    KeyEvent::KEY_F1,  
    KeyEvent::KEY_F2,
    KeyEvent::KEY_F3,
    KeyEvent::KEY_F4,
    KeyEvent::KEY_F5,
    KeyEvent::KEY_F6,
    KeyEvent::KEY_F7,
    KeyEvent::KEY_F8,
    KeyEvent::KEY_F9,
    KeyEvent::KEY_F10,
    KeyEvent::KEY_F11,
    KeyEvent::KEY_F12};

const char16 kWebDriverNullKey = 0xE000U;
const char16 kWebDriverShiftKey = 0xE008U;
const char16 kWebDriverControlKey = 0xE009U;
const char16 kWebDriverAltKey = 0xE00AU;
const char16 kWebDriverCommandKey = 0xE03DU;

/// Returns whether the given key is a WebDriver key modifier.
bool KeyConverter::IsModifierKey(char16 key) {
    switch (key) {
        case kWebDriverShiftKey:
        case kWebDriverControlKey:
        case kWebDriverAltKey:
        case kWebDriverCommandKey:
            return true;
        default:
            return false;
    }
}

/// Gets the key code associated with |key|, if it is a special WebDriver key.
/// Returns whether |key| is a special WebDriver key. If true, |key_code| will
/// be set.
bool KeyConverter::KeyCodeFromSpecialWebDriverKey(char16 key, KeyEvent::Key* key_code) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    int index = static_cast<int>(key) - 0xE000U;
    bool is_special_key = index >= 0 &&
        index < static_cast<int>(arraysize(kSpecialWebDriverKeys));
    if (is_special_key){
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
       *key_code = kSpecialWebDriverKeys[index];
    }
    else {
        printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
        // Key_Escape = 0x01000000. Offset from this for undefined keys
        int Value = 0x01000000 + index; 
        *key_code =  static_cast<KeyEvent::Key>(Value);
    }
    return is_special_key;
}

/// Gets the key code associated with |key|, if it is a special shorthand key.
/// Shorthand keys are common text equivalents for keys, such as the newline

/// character, which is shorthand for the return key. Returns whether |key| is
/// a shorthand key. If true, |key_code| will be set and |client_should_skip|
/// will be set to whether the key should be skipped.
bool KeyConverter::KeyCodeFromShorthandKey(char16 key_utf16,
                            KeyEvent:: Key* key_code,
                             bool* client_should_skip) {
    string16 key_str_utf16;
    key_str_utf16.push_back(key_utf16);
    std::string key_str_utf8 = UTF16ToUTF8(key_str_utf16);
    if (key_str_utf8.length() != 1)
        return false;
    bool should_skip = false;
    char key = key_str_utf8[0];
    if (key == '\n') {
        *key_code = KeyEvent::KEY_ENTER;	//Key_Return
    } else if (key == '\t') {
        *key_code = KeyEvent::KEY_TAB;
    } else if (key == '\b') {
        *key_code = KeyEvent::KEY_BACKSPACE;
    } else if (key == ' ') {
        *key_code = KeyEvent::KEY_SPACE;
    } else if (key == '\r') {
        *key_code = KeyEvent::KEY_UNKNOWN;
        should_skip = true;
    } else {
        return false;
    }
    *client_should_skip = should_skip;
    return true;
}


bool KeyConverter::ConvertKeysToWebKeyEvents(const string16& client_keys,
                               const Logger& logger,
                               bool release_modifiers,
                               int* modifiers,
                               std::vector<KeyEvent>* client_key_events,
                               std::string* error_msg) {
    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
    std::vector<KeyEvent> key_events;

    string16 keys = client_keys;
    // Add an implicit NULL character to the end of the input to depress all
    // modifiers.
    if (release_modifiers)
        keys.push_back(kWebDriverNullKey);

   KeyEvent::KeyboardModifiers sticky_modifiers((KeyEvent::KeyboardModifiers)*modifiers);

    for (size_t i = 0, size = keys.size(); i < size; ++i) {
        char16 key = keys[i];

        if (key == kWebDriverNullKey) {
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            // Release all modifier keys and clear |stick_modifiers|.
            if (sticky_modifiers & KeyEvent::ShiftModifier)
                key_events.push_back(
                    KeyEvent(KeyEvent::KeyRelease, KeyEvent::KEY_LEFTSHIFT, KeyEvent::NoModifier));
            if (sticky_modifiers & KeyEvent::ControlModifier)
                key_events.push_back(
                    KeyEvent(KeyEvent::KeyRelease, KeyEvent::KEY_LEFTCTRL, KeyEvent::NoModifier));
            if (sticky_modifiers & KeyEvent::AltModifier)
                key_events.push_back(
                    KeyEvent(KeyEvent::KeyRelease, KeyEvent::KEY_LEFTALT, KeyEvent::NoModifier));
            if (sticky_modifiers & KeyEvent::MetaModifier)
                key_events.push_back(
                    KeyEvent(KeyEvent::KeyRelease, KeyEvent::KEY_META, KeyEvent::NoModifier));
            sticky_modifiers = KeyEvent::NoModifier;
            continue;
        }
        
        if (IsModifierKey(key)) {
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            // Press or release the modifier, and adjust |sticky_modifiers|.
            bool modifier_down = false;
            KeyEvent::Key key_code = KeyEvent::KEY_UNKNOWN;
            if (key == kWebDriverShiftKey) {
                sticky_modifiers =(KeyEvent::KeyboardModifiers)((int)sticky_modifiers ^ (int)KeyEvent::ShiftModifier);
                modifier_down = sticky_modifiers & KeyEvent::ShiftModifier;
                key_code = KeyEvent::KEY_LEFTSHIFT;
            } else if (key == kWebDriverControlKey) {
                sticky_modifiers =(KeyEvent::KeyboardModifiers)((int)sticky_modifiers ^ (int)KeyEvent::ControlModifier);
                modifier_down = sticky_modifiers & KeyEvent::ControlModifier;
                key_code = KeyEvent::KEY_LEFTCTRL;
            } else if (key == kWebDriverAltKey) {
		sticky_modifiers =(KeyEvent::KeyboardModifiers)((int)sticky_modifiers ^ (int)KeyEvent::AltModifier);
                modifier_down = sticky_modifiers & KeyEvent::AltModifier;
                key_code = KeyEvent::KEY_LEFTALT;
            } else if (key == kWebDriverCommandKey) {
		sticky_modifiers =(KeyEvent::KeyboardModifiers)((int)sticky_modifiers ^ (int)KeyEvent::MetaModifier);
                modifier_down = sticky_modifiers & KeyEvent::MetaModifier;
                key_code = KeyEvent::KEY_META;
            } else {
                NOTREACHED();
            }
            if (modifier_down)
                key_events.push_back(
                        KeyEvent(KeyEvent::KeyPress, key_code, sticky_modifiers));
            else
                key_events.push_back(
                        KeyEvent(KeyEvent::KeyRelease, key_code, sticky_modifiers));
            continue;
        }

        KeyEvent::Key key_code = KeyEvent::KEY_UNKNOWN;
        string unmodified_text, modified_text;
        KeyEvent::KeyboardModifiers all_modifiers = sticky_modifiers;

        // Get the key code, text, and modifiers for the given key.
        bool should_skip = false;
        if (KeyCodeFromSpecialWebDriverKey(key, &key_code) ||
            KeyCodeFromShorthandKey(key, &key_code, &should_skip)) {

            if (should_skip)
                continue;
            if (key_code == KeyEvent::KEY_UNKNOWN) {
                *error_msg = StringPrintf(
                    "Unknown WebDriver key(%d) at string index (%" PRIuS ")",
                    static_cast<int>(key), i);
                return false;
            }

            if (key_code == KeyEvent::KEY_ENTER || key_code == KeyEvent::KEY_SPACE ||
               key_code == KeyEvent::KEY_8 || key_code == KeyEvent::KEY_EQUAL ||
               key_code == KeyEvent::KEY_COMMA || key_code == KeyEvent::KEY_MINUS ||
               key_code == KeyEvent::KEY_DOT || key_code == KeyEvent::KEY_SLASH ||
               key_code == KeyEvent::KEY_SEMICOLON) {
                // TODO: check this
                // For some reason Chrome expects a carriage return for the return key.
                   modified_text = unmodified_text = (char)key_code;
            } else {
                // TODO: check this
                // WebDriver assumes a numpad key should translate to the number,
                // which requires NumLock to be on with some platforms. This isn't
                // formally in the spec, but is expected by their tests.
                //int webdriver_modifiers = 0;
                //if (key_code >= Qt::Key_0 && key_code <= Qt::Key_9)
                    //webdriver_modifiers = automation::kNumLockKeyMask;
                //unmodified_text = ConvertKeyCodeToText(key_code, webdriver_modifiers);
                //modified_text = ConvertKeyCodeToText(
                    //key_code,
                    //all_modifiers | webdriver_modifiers);
            }
        } else {
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            KeyEvent::KeyboardModifiers necessary_modifiers;
            all_modifiers = KeyEvent::KeyboardModifiers((int)all_modifiers | (int)necessary_modifiers);

            if (key_code != KeyEvent::KEY_UNKNOWN) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            }

            if (unmodified_text.empty() || modified_text.empty()) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                // Do a best effort and use the raw key we were given.
                logger.Log(
                    kWarningLogLevel,
                    base::StringPrintf("No translation for key code. Code point: %d",
                        static_cast<int>(key)));
                if (unmodified_text.empty())
                    unmodified_text = UTF16ToUTF8(keys.substr(i, 1));
                if (modified_text.empty())
                    modified_text = UTF16ToUTF8(keys.substr(i, 1));
            }
        }

	bool autoPress = false, autoRelease = false;

        if (i < size - 1 && key == keys[i + 1]) 
            autoRelease = true;

        if (i > 0 && key == keys[i - 1]) 
            autoPress = true;

	bool sendRelease;
#if WD_ENABLE_ONE_KEYRELEASE 
        // Send only last key release
        sendRelease = !autoRelease;
 #else
        // Send key release in all cases
        sendRelease = true;
#endif
        // Create the key events.
        bool necessary_modifiers[3];
        for (int i = 0; i < 3; ++i) {
            necessary_modifiers[i] =
                all_modifiers & kModifiers[i].mask &&
                !(sticky_modifiers & kModifiers[i].mask);
            if (necessary_modifiers[i]) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                key_events.push_back(
                    KeyEvent(KeyEvent::KeyPress, kModifiers[i].key_code, sticky_modifiers, str, autoPress));
            }

        }

      if (unmodified_text.length() || modified_text.length()) {
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            key_events.push_back(KeyEvent(KeyEvent::KeyPress, key_code, all_modifiers, unmodified_text.c_str(), autoPress));
            if (sendRelease) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                key_events.push_back(KeyEvent(KeyEvent::KeyRelease, key_code, all_modifiers, unmodified_text.c_str(), autoRelease));      
            }
        }
       else
        {
            printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
            key_events.push_back(KeyEvent(KeyEvent::KeyPress, key_code, all_modifiers, str, autoPress));
            if (sendRelease) {
                printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                key_events.push_back(KeyEvent(KeyEvent::KeyRelease, key_code, all_modifiers, str, autoRelease));
            }
        }
         
        
        if (sendRelease) {
            for (int i = 2; i > -1; --i) {
                if (necessary_modifiers[i]) {
                    printf("%s:%s:%d \n", __FILE__, __func__, __LINE__);
                    key_events.push_back( 
                        KeyEvent(KeyEvent::KeyRelease, kModifiers[i].key_code, sticky_modifiers, str, autoRelease));
        

                }
            }
        }
    }

    client_key_events->swap(key_events);
    *modifiers = sticky_modifiers;
    return true;
}

}  // namespace webdriver
