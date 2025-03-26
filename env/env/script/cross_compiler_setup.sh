#! /bin/bash
 
LOONGSON_COMPILER_DIR=$1    # 龙芯交叉编译器压缩包绝对路径

echo -e "\e[32m [CROSS COMPILER SETUP] \e[0m"

# 解压龙芯交叉编译器
echo -e "\e[32m [UNPACK] \e[0m loongson cross compiler to \e[32m [/opt] \e[0m"
cd /opt
tar -xvf $LOONGSON_COMPILER_DIR
echo -e "\e[32m [UNPACK FINSH] \e[0m" 

# 配置环境变量 
echo -e "\e[32m [CONFIGURE] \e[0m system env path of loongson cross compiler"
echo "export PATH="/opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.4/bin/:$PATH"" > /etc/profile
source /etc/profile
echo -e "\e[32m [CONFIGURE FINSH] \e[0m" 

echo -e "\e[32m [SETUP FINSH] \e[0m" 