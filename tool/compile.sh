#! /bin/bash

PROJECT_ROOT_DIR=$1
COMPILE_THREAD_NUM=$2

cd $PROJECT_ROOT_DIR/build/
rm -r $PROJECT_ROOT_DIR/build/*
cmake ..
make -j$PROJECT_ROOT_DIR