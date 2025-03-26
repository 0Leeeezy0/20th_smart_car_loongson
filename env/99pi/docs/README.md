# 99PI配置
## 目录结构
```
99PI
├─build             99PI配置完成的内核镜像
├─docs              文档
└─script            99PI板卡脚本
```
## 板卡连接
- 下载 <b>MobaXTerm</b> 👉[click](https://mobaxterm.mobatek.net/download-home-edition.html)
- 使用 <b>USB-TTL</b> 串口连接板卡 <b>注意接线要 RX<->TX  TX<->RX</b>

| 板卡接口 | 序号 |
| :-: | :-: |
| <b>GND</b> | 1 |
| <b>UART0_TXD</b> | 4 |
| <b>UART0_RXD</b> | 6 |

- 配置电脑以太网共享
- 更改电脑以太网 <b>网段IP</b>
- 使用 <b>OpenSSH</b> 进行连接  <b>板卡默认以太网IP：192.168.2.10</b>
### 参考视频
[龙芯2k0300配置有线网络并SSH登录](https://www.bilibili.com/video/BV14D421u7Gw?vd_source=f713de5a5745113c4c71ea4aa269d9e6&spm_id_from=333.788.videopod.sections)

## 脚本说明
| 脚本名称 | 说明 | 参数 | 
| :-: | :-: | :-: |
| <b>kernel_build.sh</b> | 99PI内核编译脚本 <br><b>e.g.`./kernel_build.sh /home/LS2K0300-linux-4.19.tar.gz 8`</b> | 1. 内核源码压缩文件（tar） <b>绝对路径</b><br>2.多线程编译使用的线程数 <b>根据电脑配置决定</b><br> |
### 备注 
- 编译完成的 <b>vmlinuz</b> 在 `/home/ubuntu/LS2K0300-linux-4.19/` 目录下
- 将 <b>vmlinuz</b> 通过 `SSH` 上传至板卡，终端输入 `cp -r <vmlinuz路径> /boot/` 然后按下板卡的 <b>RESET复位按键</b> 重启
