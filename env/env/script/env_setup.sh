#！ bin/bash

APT_SOURCES_DIR="/etc/apt/sources.list.d/ubuntu.sources"

echo -e "\e[32m [ENV SETUP]"

# 备份系统APT源
echo -e "\e[32m [BACKUP] \e[0m system apt sources as [xxx.backup]"
mv $APT_SOURCES_DIR /etc/apt/ubuntu.sources.backup
echo -e "\e[32m [BACKUP FINSH] \e[0m" 

# 更改系统APT源
echo -e "\e[32m [CHANGE] \e[0m system apt sources [TUNA sources]"
touch $APT_SOURCES_DIR

echo "Types: deb
URIs: http://mirrors.tuna.tsinghua.edu.cn/ubuntu
Suites: noble noble-updates noble-backports
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg

Types: deb
URIs: http://security.ubuntu.com/ubuntu/
Suites: noble-security
Components: main restricted universe multiverse
Signed-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg" > $APT_SOURCES_DIR
echo -e "\e[32m [CHANGE FINSH] \e[0m" 

# 更新系统APT源
echo -e "\e[32m [UPDATE] \e[0m apt sources"
apt update
echo -e "\e[32m [UPDATE FINSH] \e[0m" 

# 安装必要软件包
echo -e "\e[32m [INSTALL] \e[0m necessary packages"
apt -y install vim gcc g++ neofetch htop cmake make wget unzip git libncurses-dev bison flex bc libssl-dev
echo -e "\e[32m [INSTALL FINSH] \e[0m" 

# 安装OPENCV交叉编译依赖库
echo -e "\e[32m [INSTALL] \e[0m OpenCV cross compile needed libs"
apt -y install build-essential libgtk2.0-dev libgtk-3-dev libavcodec-dev libavformat-dev libjpeg-dev libswscale-dev libtiff5-dev
echo -e "\e[32m [INSTALL FINSH] \e[0m" 

echo -e "\e[32m [SETUP FINSH] \e[0m" 