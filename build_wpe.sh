#!/bin/bash
output_gen=$1
mode=$2
wpe_dir=$3

current_dir=`pwd`
if [ -z $output_gen ];
then
  output_gen=`pwd`/out
fi

output_gen=`readlink -m ${output_gen}`
base_output_gen=`dirname ${output_gen}`

platform="wpe"

if [ -z $mode ];
then
  modes="release"
else
  modes=$mode
fi

GYP=`which gyp`

#generate wdversion.cc
python generate_wdversion.py

for mode in $modes
do
  cd $current_dir

  OUTPUT_DIR=${output_gen}/$platform/$mode
  OUTPUT_DIR_OUT=${output_gen}/$platform/$mode/Default
  OUTPUT_BIN_DIR=${output_gen}/bin/$platform/$mode

  python $GYP --depth . -G output_dir=. -D platform=$platform -D mode=$mode -D ROOT_PATH=${base_output_gen} -D WPE_DIR=${wpe_dir} --generator-output=${output_gen}/$platform/$mode wd.gyp
  [ $? -ne 0 ] && exit 1

done
exit 0
