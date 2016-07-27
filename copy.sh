#!/bin/bash
output_gen=$1
platform=$2
mode=$3
wpe_dir=$4

current_dir=`pwd`
if [ -z $output_gen ];
then
  output_gen=`pwd`/out
  platform="desktop"
fi

output_gen=`readlink -m ${output_gen}`
base_output_gen=`dirname ${output_gen}`

if [ -z $platform ];
then
  platforms="desktop"
else
  platforms=$platform
fi
if [ -z $mode ];
then
  modes="release"
else
  modes=$mode
fi

OUT_STATIC_LIB_FILES="libchromium_base.a libWebDriver_core.a libWebDriver_extension_wpe_base.a"
OUT_SHARED_LIB_FILES="libchromium_base.so libWebDriver_core.so libWebDriver_extension_wpe_base.so libWebDriver_wpe_driver.so libWebDriver_wpe_driver_injected_bundle.so"
OUT_BIN_FILES="WebDriver WPEProxy WebDriver_noWebkit WebDriver_noWebkit_sharedLibs"
GYP=`which gyp`

for platform in $platforms
do
  for mode in $modes
  do
    cd $current_dir

    OUTPUT_DIR=${output_gen}/$platform/$mode
    OUTPUT_DIR_OUT=${output_gen}/$platform/$mode/Default
    OUTPUT_BIN_DIR=${output_gen}/bin/$platform/$mode

    mkdir -p ${output_gen}/bin/$platform/$mode/
    [ $? -ne 0 ] && echo "**** ERROR: Can't create $OUTPUT_BIN_DIR" && exit 1

    # copy libraries
    for file in $OUT_STATIC_LIB_FILES; do cp -f $OUTPUT_DIR_OUT/$file $OUTPUT_BIN_DIR 2>/dev/null; done
    for file in $OUT_SHARED_LIB_FILES; do cp -f $OUTPUT_DIR_OUT/lib.target/$file $OUTPUT_BIN_DIR 2>/dev/null; done

    # copy test binaries
    for file in $OUT_BIN_FILES
    do
      if [ -f $OUTPUT_DIR_OUT/$file ]
      then
        cp -f $OUTPUT_DIR_OUT/$file $OUTPUT_BIN_DIR 2>/dev/null
      fi
    done
    
  done
done
