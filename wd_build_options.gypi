{
   # do not require cygwin
  'msvs_cygwin_shell': 0,

  'msvs_configuration_attributes': {
     'CharacterSet': '1'
  },

  'cflags': [
    '-fPIC',
    '-Wall',
    '-W',
    '-Wno-unused-parameter',
  ],

  'defines': [
    'WPE_NO_DEBUG',
    'WPE_GUI_LIB',
    'WPE_CORE_LIB',
    'WPE_SHARED',
    'WPE_NO_OPENGL',
  ],

  'conditions': [ 
    [ 'mode == "debug"', {
      'cflags': [
        '-O0',
        '-g',
      ],
    } ],

    [ 'mode == "release"', {
      'cflags': [
        '-O3',
      ],
      'defines': [
        'NDEBUG',
      ],
    } ],

    [ 'mode == "release_dbg"', {
      'cflags': [
        '-O3',
        '-g',
      ],
    } ],

    [ 'OS == "linux"', {
       'actions': [ {
         'action_name': 'input_dir',
         'inputs':      [],
         'outputs':     ['<(INTERMEDIATE_DIR)'],
         'action':      ['mkdir', '-p', '<(INTERMEDIATE_DIR)'],
       } ],
       'defines': [ 
         '__STDC_FORMAT_MACROS',
         'OS_POSIX',
       ],
    } ],

    [ '<(WD_CONFIG_PLAYER) == 1', {
     'defines': [ 'WD_ENABLE_PLAYER=1' ],
    }],

    [ '<(WD_CONFIG_ONE_KEYRELEASE) == 1', {
     'defines': [ 'WD_ENABLE_ONE_KEYRELEASE=1' ],
    }]
  ],
}
