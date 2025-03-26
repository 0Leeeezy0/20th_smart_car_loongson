# 开发环境
## 目录结构
```
env
├─docs              文档
├─script            开发环境配置脚本
├─compiler          交叉编译器
└─image             配置好的DOCKER镜像
```
## DOCKER命令
`docker run --privileged --mount type=bind,source=<1> --name="<2>" -h <3> -it loongson4smart_car`
<br><b>参数说明</b>

| 参数 | 说明 |
| :-: | :-: |
| <b><1></b> | 主机共享文件夹目录 |
| <b><2></b> | DOCKER容器名称 |
| <b><3></b> | DOCKER容器主机名 |

## 脚本说明
| 脚本名称 | 说明 | 参数 | 
| :-: | :-: | :-: |
| <b>cross_compiler_setup.sh</b> | 开发端交叉编译器设置 <br><b>e.g.`./cross_compiler_setup.sh /home/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.4.tar.xz`</b> | 龙芯交叉编译器压缩包绝对路径 |
| <b>env_setup.sh</b> | 开发端环境设置<br><b>e.g.`./env_setup.sh`</b> | 无 |
