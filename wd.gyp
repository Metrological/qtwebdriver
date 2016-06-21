{
  'includes': [
    'wd.gypi',
    'wd_common.gypi',
  ],

  # TODO: move generate_wdversion.py here from build.sh

  'targets': [
    {
      'target_name': 'All',
      'type': 'none',
      'dependencies': [
        'base.gyp:chromium_base',
        'wd_core.gyp:WebDriver_core',
        'wd_ext_wpe.gyp:WebDriver_extension_wpe_base'
      ],

      'conditions': [

        [ 'OS == "linux"', {
          'dependencies': [
            'base.gyp:chromium_base_shared',
            'wd_core.gyp:WebDriver_core_shared',
            'wd_ext_wpe.gyp:WebDriver_extension_wpe_base'
          ],
        } ],

        ['platform == "desktop"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid',
          ],
        } ],

        ['platform == "rpi"', {
          'dependencies': [
            'wd_test.gyp:test_WD_hybrid',
          ],
        } ],

      ], # conditions

    }, # target_name: All
  ],
}
