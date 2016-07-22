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
      '<(WPE_INC_PATH)',
      '<(INTERMEDIATE_DIR)',
    ],

    'conditions': [
      [ '<(WPE) == 1', {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L<(WPE_LIB_PATH)',
             '-lpthread',
              '-lrt',
              '-ldl',
           ],
          } ],
        ],
      }, {
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-L<(WPE_LIB_PATH)',
              '-lpthread',
              '-lrt',
              '-ldl',
            ],
          } ],
        ],
      } ],
    ], # conditions

  }, # target_defaults

  'targets': [
  {
      'target_name': 'test_WD_hybrid',
      'type': 'executable',
      
      'product_name': 'WebDriver',

      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_wpe.gyp:WebDriver_wpe_driver',
        'wd_ext_wpe.gyp:WebDriver_extension_wpe_base_shared',
      ],

      'defines': [ 'WD_TEST_ENABLE_WEB_VIEW=1' ],
      
      'sources': [
        'src/Test/main.cc',
        'src/Test/shutdown_command.cc',
      ],

    }, 
  ],
}
