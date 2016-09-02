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
OUT_SHARED_LIB_FILES="libchromium_base.so libWebDriver_core.so libWebDriver_extension_wpe_base.so"
OUT_BIN_FILES="WebDriver WebDriver_noWebkit WebDriver_noWebkit_sharedLibs"
GYP=`which gyp`

#generate wdversion.cc
python generate_wdversion.py

for platform in $platforms
do
  for mode in $modes
  do
    cd $current_dir

    OUTPUT_DIR=${output_gen}/$platform/$mode
    OUTPUT_DIR_OUT=${output_gen}/$platform/$mode/Default
    OUTPUT_BIN_DIR=${output_gen}/bin/$platform/$mode

    python $GYP --depth . -G output_dir=. -D platform=$platform -D mode=$mode -D ROOT_PATH=${base_output_gen} -D WPE_DIR=${wpe_dir} --generator-output=${output_gen}/$platform/$mode wd.gyp
    [ $? -ne 0 ] && exit 1

  done
done
exit 0
