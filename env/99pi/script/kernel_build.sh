#! /bin/bash

KERNEL_TAR_DIR=$1   # 内核源码压缩文件路径
KERNEL_UNPACK_DIR=/home/ubuntu   # 内核解压路径
COMPILE_THREAD_NUM=$2   # 多线程编译线程数
KERNEL_CONFIG=$KERNEL_UNPACK_DIR/LS2K0300-linux-4.19/arch/loongarch/configs/loongson_2k0300_99pi_wifi_defconfig  # 默认配置
KERNEL_DTS=$KERNEL_UNPACK_DIR/LS2K0300-linux-4.19/arch/loongarch/boot/dts/loongson/loongson_2k0300_pai_99_wifi.dts # 设备树
KERNEL_CONFIG_FRAGMENT=$KERNEL_UNPACK_DIR/LS2K0300-linux-4.19/.config   # 用户配置文件路径

# 解压内核源码
echo -e "\e[32m [UNPACK] \e[0m KERNEL sources to \e[32m [$KERNEL_UNPACK_DIR] \e[0m"
mkdir $KERNEL_UNPACK_DIR
cd $KERNEL_UNPACK_DIR
tar -xvf $KERNEL_TAR_DIR
echo -e "\e[32m [UNPACK FINSH] \e[0m"

# 编译内核
echo -e "\e[32m [BUILD] \e[0m kernel"
cd $KERNEL_UNPACK_DIR/LS2K0300-linux-4.19
# 显示编译配置
echo "\e[32m====================================BUILD CONFIG====================================\e[0m"
echo "\e[32mTARGET_ARCH\e[0m                    = loongarch"
echo "\e[32mTARGET_KERNEL_CONFIG\e[0m           = $KERNEL_CONFIG"
echo "\e[32mTARGET_KERNEL_DTS\e[0m              = $KERNEL_DTS"
echo "\e[32mTARGET_KERNEL_CONFIG_FRAGMENT\e[0m  = $KERNEL_CONFIG_FRAGMENT"
echo "\e[32mTHREAD_NUM\e[0m                     = $COMPILE_THREAD_NUM"
echo "\e[32m====================================================================================\e[0m"

cp -r 
make vmlinuz ARCH=loongarch CROSS_COMPILE=loongarch64-linux-gnu- KERNEL_DTS=$KERNEL_DTS KERNEL_CFG=$KERNEL_CONFIG -j $COMPILE_THREAD_NUM
cksum vmlinuz

echo -e "\e[32m [BUILD FINSH] \e[0m" 