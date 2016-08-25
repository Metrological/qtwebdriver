{
  'includes': [
    'wd.gypi',
    'wd_common.gypi',
  ],

  'target_defaults': {
    'includes': [
      'wd_build_options.gypi',
    ],

    # TODO: review include_dirs
    'include_dirs': [
      'inc/',
      'src/',
      '<(INTERMEDIATE_DIR)',
      ' -I<(WPE_INC_PATH)',
      ' -I<(WPE_INC_PATH)/WPE',
    ],

  },

  'targets': [
    {
      'target_name': 'WebDriver_wpe_driver_injected_bundle',
      'type': 'shared_library',

      'cflags': [
        '-std=c++11',
      ],

      'sources': [
        'src/webdriver/extension_wpe/wpe_driver/injectedbundle/WpeDriverInjectedBundle.cc',
      ],
    },
    {
      'target_name': 'WPE_Proxy',
      'type': 'executable',
      'product_name': 'WPEProxy',      

      'cflags': [
        '-std=c++11',
      ],

      'dependencies': [
          'base.gyp:chromium_base_shared',
      ],
      'sources': [
        'src/webdriver/extension_wpe/wpe_driver/wpe_driver_proxy.cc',
      ],
    },
    {
      'target_name': 'WebDriver_extension_wpe_base',
      'type': 'static_library',
      'standalone_static_library': 1,
      'dependencies': [
      ],
      'sources': [
        'src/webdriver/extension_wpe/uinput_manager.cc',
        'src/webdriver/extension_wpe/uinput_event_dispatcher.cc',
        'src/webdriver/extension_wpe/wpe_key_converter.cc',
        'src/webdriver/extension_wpe/wpe_view_util.cc',
        'src/webdriver/extension_wpe/wpe_view_creator.cc',
        'src/webdriver/extension_wpe/wpe_view_handle.cc',
        'src/webdriver/extension_wpe/wpe_view_enumerator.cc',
        'src/webdriver/extension_wpe/wpe_view_executor.cc',
        'src/webdriver/extension_wpe/wpe_driver/wpe_driver.cc',
      ],
    }, 
    {
      'target_name': 'WebDriver_extension_wpe_base_shared',
      'type': 'shared_library',

      'product_name': 'WebDriver_extension_wpe_base',

      'dependencies': [
        'WebDriver_extension_wpe_base',
      ],
    } 
  ],
}
