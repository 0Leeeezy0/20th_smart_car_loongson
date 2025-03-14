#! /bin/bash

cd build
rm -r *
cmake ..
make -j
cd ../
