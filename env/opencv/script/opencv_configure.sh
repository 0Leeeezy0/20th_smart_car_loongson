#! /bin/bash

OPENCV_TAR_DIR=$1   # OPENCV库压缩文件路径

echo -e "\e[32m [OPENCV CONFIGURE]"

# 解压OPENCV动态库
echo -e "\e[32m [UNPACK] \e[0m OPENCV lib"
tar -xvf $OPENCV_TAR_DIR
echo -e "\e[32m [UNPACK FINSH] \e[0m" 

# 配置路径
echo -e "\e[32m [CONFIGURE] \e[0m OPENCV lib path"
cd shared/
cd lib
cp -r * /usr/lib64/
cd ../include
cp -r * /usr/include/
echo -e "\e[32m [CONFIGURE FINSH] \e[0m" 

echo -e "\e[32m [OPENCV CONFIGURE FINSH] \e[0m" 
