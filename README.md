# 20届龙芯摄像头组
> 渣作，持续更新中
# 目录结构
```shell
├─build             编译生成文件文件夹
├─configs           设置文件
├─img               图像/视频
├─include           头文件
├─lib               库文件
└─src               用户源文件
```
# 工程宏简说明
| 宏名称 | 宏说明 |
| :-----: | :------: |
| <b>PI</b> | 圆周率 |
| <b>IMAGE_WIDTH</b> | 图像宽度 |
| <b>IMAGE_HEIGHT</b> | 图像高度 |
| <b>SEARCH_START</b> | 循线起始高度 |
| <b>SEARCH_END</b> | 循线结束高度 |
| <b>SIDE_END</b> | 边线搜线结束横坐标与边框距离 |

# 设置文件说明
> 使用JSON文件存储设置参数

| 参数名称 | 参数说明 |
| :-----: | :------: |
| <b>TRACK_WIDTH</b> | 赛道宽度 |
| <b>CONTROL_POINT</b> | 控制点高度，范围:0 ~ （SEARCH_START-SEARCH_END） |
| <b>INFLECTION_POINT_DISTANCE</b> | 拐点识别前后点距离 |
| <b>SKIP_POINT_DISTANCE</b> | 识别拐点后跳过点数量 |
| <b>INFLECTION_POINT_ANGLE_MIN</b> | 拐点最小角度阈值 |
| <b>INFLECTION_POINT_ANGLE_MAX</b> | 拐点最大角度阈值 |
| <b>BEND_POINT_ANGLE_MIN</b> | 弯点最小角度阈值 |
| <b>BEND_POINT_ANGLE_MAX</b> | 弯点最大角度阈值 |
| <b>PREDICTION_POINT</b> | 预测点高度 |
| <b>SERVO_KP</b> | 方向环KP |
| <b>SERVO_KI</b> | 方向环KI |
| <b>SERVO_KD</b> | 方向环KD |

# 元素识别简述
## 拐点/弯点
> 通过对<b>八邻域</b>搜线获得的边线通过使用向量叉乘进行角度判断，当角度符合设置文件中的阈值时，且不在边框上，则判定为拐点
- 使用拐点和弯点综合判断十字和圆环元素，该方法较要求摄像头高度、角度和镜头可视角度，当结构合适时，结果很准
- 通过弯点数量多少作为赛道弯曲程度的标志，从而可以更为精准地进行速度决策
## 十字
待续
## 圆环
待续