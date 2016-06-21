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
      ' -I<(WPE_INC_PATH)',
      '<(INTERMEDIATE_DIR)',
    ],

  },

  'targets': [
    {
      'target_name': 'WebDriver_extension_wpe_base',
      'type': 'static_library',
      'standalone_static_library': 1,

      'sources': [
        'src/webdriver/extension_wpe/uinput_manager.cc',
        'src/webdriver/extension_wpe/uinput_event_dispatcher.cc',
        'src/webdriver/extension_wpe/wpe_view_util.cc',
        'src/webdriver/extension_wpe/wpe_view_creator.cc',
        'src/webdriver/extension_wpe/wpe_view_handle.cc',
        'src/webdriver/extension_wpe/wpe_view_enumerator.cc',
        'src/webdriver/extension_wpe/wpe_view_executor.cc',
      ],
    }
  ],
}
