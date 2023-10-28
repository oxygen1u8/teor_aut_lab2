#!/usr/bin/env bash

root_dir=`dirname $0`
cd $root_dir
root_dir=$(pwd)

src_dir="$root_dir/src"
lib_dir="$root_dir/teor_aut_lab1/src/build"
build_dir="$root_dir/build"

rm -rf "${lib_dir}"
mkdir -p "${lib_dir}"
cd "${lib_dir}"

cmake -G "Unix Makefiles" ../
make

rm -rf "${build_dir}"
mkdir -p "${build_dir}"
cd "${build_dir}"

cmake -G "Unix Makefiles" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
    ../
make
