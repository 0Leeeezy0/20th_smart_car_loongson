# OPENCV
## 目录结构
```
opencv
├─docs              文档
├─install           编译完成的OpenCV库
│  └─dynamic        动态链接库（推荐）
└─script            脚本
```
## 脚本说明
| 脚本名称 | 说明 | 参数 | 
| :-: | :-: | :-: |
| <b>opencv_compile.sh</b> | 开发端交叉编译OpenCV脚本 <br><b>e.g.`./opencv_compile.sh /home/opencv-4.5.1.tar.gz 8`</b> | 1. OpenCV源码压缩包（tar）的 <b>绝对路径</b><br>2.多线程编译使用的线程数 <b>根据电脑配置决定</b><br> |
| <b>opencv_configure.sh</b> | 龙芯99Pi 安装OpenCV库脚本 <b>只有动态库需要安装</b><br><b>e.g.`./99pi_install.sh /home/shared.tar.gz`</b> | OpenCV库压缩包（tar）的 <b>绝对路径</b> |

## 链接库说明
| 链接库类型 | 说明 |
| :-: | :-: |
| <b>动态库（.so）</b> | 在可执行文件运行过程中，动态加载至程序内的库 <b>需要安装至99Pi</b> |
| <b>静态库（.a）</b> | 在编译可执行文件运行过程中，链接至程序中的库 <b>不需要安装至99Pi</b> |
> 以上两种库使用的时候选择一种即可，<b>强烈推荐使用动态库</b>

## 备注