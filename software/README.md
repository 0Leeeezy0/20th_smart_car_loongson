# 极速光电龙芯-软件工程
> 本工程可作为参考工程，可实现方向闭环、速度闭环<br>提供了最长白列、八邻域等算法，作为全元素完赛的基石

# 目录结构
```
software
├─build             编译生成文件
├─configs           JSON设置文件
├─img               图像、视频
├─include           头文件
├─lib               控制、循线等源文件
└─src               用户源文件
```
# 使用方法
- 使用虚拟机、DOCKER、WSL2等方式打开工程
- 在工程目录下打开LINUX终端
- 终端输入 `./compile.sh` 进行编译
- 终端输入 `./pack.sh` 进行工程打包
- 使用<b>MobaXTerm</b>上传工程打包文件至99PI板卡
- 99PI板卡终端工程路径处输入 `tar -xvf LOONGSON.tar.gz`
- 进入 <b>build文件夹</b> ，输入 `./LOONGSON` 运行程序

# <br>☠️仅供参考，不允许商用！！！