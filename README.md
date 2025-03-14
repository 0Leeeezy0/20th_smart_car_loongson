# 20届极速光电龙芯组
## 目录结构
```shell 
├─build             编译生成文件目录
├─configs           设置文件目录
├─img               图像/视频目录
├─include           头文件目录
├─lib               库文件目录
└─src               用户源文件目录
```
## 工程宏说明
| 宏名称 | 宏说明 |
| :-: | :-: |
| <b>PI</b> | 圆周率 |
| <b>IMAGE_WIDTH</b> | 图像宽度 |
| <b>IMAGE_HEIGHT</b> | 图像高度 |
| <b>SEARCH_START</b> | 搜线起始高度 |
| <b>SEARCH_END</b> | 搜线结束高度 |
| <b>SIDE_END</b> | 边线搜线结束横坐标与边框的距离 |

## 设置文件说明
| 参数名称 | 参数说明 |
| :-: | :-: |
| <b>TRACK_WIDTH</b> | 赛道宽度 |
| <b>CONTROL_POINT</b> | 控制点高度 |
| <b>INFLECTION_POINT_DISTANCE</b> | 拐点采样三点间距 |
| <b>SKIP_POINT_DISTANCE</b> | 识别到拐点后跳过的点数 |
| <b>INFLECTION_POINT_ANGLE_MIN</b> | 拐点最小角度阈值 |
| <b>BEND_POINT_ANGLE_MIN</b> | 拐点最大角度阈值 |
| <b>BEND_POINT_ANGLE_MAX</b> | 弯点最小角度阈值 |
| <b>PREDICTION_POINT</b> | 弯点最大角度阈值 |
| <b>SERVO_KP</b> | 方向环KP |
| <b>SERVO_KI</b> | 方向环KI |
| <b>SERVO_KD</b> | 方向环KD |

## 元素说明
### 拐点/弯点
### 十字
### 圆环