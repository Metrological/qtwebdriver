{
  'variables': {
    'WPE': '1',
    'QT5%': '0',
    'WD_CONFIG_QWIDGET_BASE%': '1',
    'WD_CONFIG_WEBKIT%': '1',
    'WD_CONFIG_QUICK%': '1',
    'WD_BUILD_MONGOOSE%': '0',
    'WD_CONFIG_PLAYER%': '1',
    'WD_CONFIG_ONE_KEYRELEASE%': '0',

    'QT_BIN_PATH%': '/usr/lib/x86_64-linux-gnu/qt5/bin',
    'QT_INC_PATH%': '/usr/include/qt5',
    'QT_LIB_PATH%': '/usr/lib/x86_64-linux-gnu/',
    'WPE_BIN_PATH': '${WPE_TARGET_DIR}/usr/bin',
    'WPE_INC_PATH': '${WPE_STAGING_DIR}/usr/include/wpe-0.1',
    'WPE_LIB_PATH': '${WPE_TARGET_DIR}/usr/',
    'MONGOOSE_INC_PATH%': 'src/third_party/mongoose',

    'conditions': [

      [ 'OS == "android"', {
        'ANDROID_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)/include',
        'ANDROID_LIB_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/include',
        'ANDROID_PLATFORM_INC': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/platforms/android-9/arch-<!(/bin/echo -n $ANDROID_ARCH)/usr/include',
        'ANDROID_LIB': '<!(/bin/echo -n $ANDROID_NDK_ROOT)/sources/cxx-stl/gnu-libstdc++/4.7/libs/<!(/bin/echo -n $ANDROID_LIB_ARCH)',
      } ],
    ],
  },
}
