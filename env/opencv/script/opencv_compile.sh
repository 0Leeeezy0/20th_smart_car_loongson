#! /bin/bash

OPENCV_TAR_DIR=$1
COMPILE_THREAD_NUM=$2
OPENCV_UNPACK_DIR=/home/ubuntu
BUILD_SHARED_LIB=ON

echo -e "\e[32m [OPENCV COMPILE]"

# 解压OpenCV源码
echo -e "\e[32m [UNPACK] \e[0m OpenCV sources to \e[32m [$OPENCV_UNPACK_DIR] \e[0m"
mkdir $OPENCV_UNPACK_DIR
cd $OPENCV_UNPACK_DIR
tar -xvf $OPENCV_TAR_DIR
echo -e "\e[32m [UNPACK FINSH] \e[0m"

# 交叉编译OpenCV源码
echo -e "\e[32m [BUILD] \e[0m OpenCV sources \e[0m"
cd $OPENCV_UNPACK_DIR/opencv-4.5.1
mkdir build install
cd build
cmake -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_SYSTEM_PROCESSOR=loongson -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_C_COMPILER=loongarch64-linux-gnu-gcc -DCMAKE_CXX_COMPILER=loongarch64-linux-gnu-g++ -DBUILD_SHARED_LIBS=$BUILD_SHARED_LIB ..
make -j$COMPILE_THREAD_NUM
echo -e "\e[32m [BUILD FINSH] \e[0m"

# 安装OpenCV源码
echo -e "\e[32m [INSTALL] \e[0m OpenCV libs to \e[32m [install] \e[0m"
make install
echo -e "\e[32m [INSTALL FINSH] \e[0m"

echo -e "\e[32m [OPENCV COMPILE FINSH] \e[0m" 
