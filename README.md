# 工程介绍
> 20届极速光电龙芯组，由完全模型代码移植而来

# 工程架构
```
.
├── CMakeLists.txt CMake编译文件
├── config.json	工程参数文件
├── img	图像存储文件夹
│   ├── ImgAll	存储图像
│   └── sample.mp4	演示视频
├── include	头文件
│   ├── common_program.h	工程调用的内部头文件
│   ├── common_system.h	工程调用的系统头文件
│   ├── json.hpp	JSON库
│   ├── libdata_process.h	数据处理头文件
│   ├── libdata_store.h	全局参数定义头文件
│   ├── libimage_process.h	图像处理头文件
│   └── path.h	路径处理头文件
├── lib
│   ├── control	控制
│   ├── libdata_process.cpp	数据处理库
│   └── libimage_process.cpp	图像处理库
├── README.md	工程描述文件
├── src
│   ├── main.cpp	主程序
│   ├── path_across.cpp	十字赛道操作库
│   ├── path_circle.cpp	圆环赛道操作库
│   ├── path_model.cpp	模型赛道操作库
│   └── path_side_search.cpp	赛道边界、路径识别
└── tool
    └── compile.sh	自动编译工程脚本
```
# 配置文件config.json参数
```json
"UART_EN" : 串口使能
"IMG_COMPRESS_EN" : 图像压缩使能
"CAMERA_EN" : 摄像头使能：0.演示视频 1./dev/video0 2./dev/video1
"VIDEO_SHOW_EN" : 图像显示使能：使用ssh时务必关掉，否则延迟极大
"IMAGE_SAVE_EN" : 图像存储使能：比赛时务必关闭否则有轻微延迟
"DATA_PRINT_EN" : 数据打印使能：比赛时务必关闭否则有轻微延迟
"ACROSS_IDENTIFY_EN" : 十字赛道识别使能
"CIRCLE_IDENTIFY_EN" : 圆环赛道识别使能
"MODEL_DETECTION_EN" : 模型赛道识别使能
"DANGER_ZONE_CONE_DETECTION_EN" : 危险区域锥桶识别使能：防止比赛时危险区域放置的过于刁钻，关闭则一路将锥桶撞开以完赛

"FORWARD" : 前瞻点高度
"PATH_SEARCH_START" : 路径巡线起始点
"PATH_SEARCH_END" : 路径巡线结束点
"SIDE_SEARCH_START" : 边线巡线起始点
"SIDE_SEARCH_END" : 边线巡线结束点

"POINT_DISTANCE" : 拐点弯点向量法始终点距离
"LITTLE_ANGLE_BEND_POINT_NUM" : 小角度弯道弯点阈值
"BIG_ANGLE_BEND_POINT_NUM" : 大角度弯道弯点阈值
"MIN_INFLECTION_POINT_ANGLE" : 拐点识别最小角度
"MAX_INFLECTION_POINT_ANGLE" : 拐点识别最大角度
"MIN_BEND_POINT_ANGLE" : 弯点识别最小角度
"MAX_BEND_POINT_ANGLE" : 弯点识别最大角度

"TRACK_WIDTH" : 准备入环补线宽度：与另一侧边线距离
"CIRCLE_OUT_WIDTH" : 出环补线宽度：与中心线距离

"STRIGHT_TRACK_MOTOR_SPEED" : 直赛道电机速度
"LITTLE_ANGLE_BEND_TRACK_MOTOR_SPEED" :小角度弯道电机速度
"BIG_ANGLE_BEND_TRACK_MOTOR_SPEED" : 大角度弯道电机速度
"ACROSS_TRACK_MOTOR_SPEED" : 十字赛道电机速度
"CIRCLE_TRACK_MOTOR_SPEED_OUTSIDE" : 外圆环电机速度：准备入环
"CIRCLE_TRACK_MOTOR_SPEED_INSIDE" : 内圆环电机速度：入环、准备出环、出环
"BRIDGE_ZONE_MOTOR_SPEED" : 桥梁区域电机速度
"DANGER_ZONE_MOTOR_SPEED" : 危险区域电机速度
"RESCUE_ZONE_MOTOR_SPEED" : 救援区域电机速度
"CROSSWALK_ZONE_MOTOR_SPEED_STOP_PREPARE" : 斑马线区域准备停车阶段电机速度
"CIRCLE_IN_PREPARE_TIME" : 圆环准备入环状态清零时间

"DILATE_FACTOR" : 图形学膨胀系数
"ERODE_FACTOR" : 图形学腐蚀系数
"FILTER_FACTOR" : 路径滤波系数：越大滤波效果越弱

"DANGER_TIME" : 危险区域时间
"BRIDGE_TIME" : 桥梁区域时间
"RESCUE_TIME" : 救援区域时间：识别到牌子之后到进入车库之间的时间阈值
"RESCUE_GARAGE_TIME" : 救援区域时间：识别到牌子到开始决策入库时间的时间阈值
"CROSSWALK_STOP_TIME" : 斑马线区域停车时间：比赛时该时间应极大约300以上
"CROSSWALK_IDENTIFY_Y" : 斑马线区域标志识别高度：越小越早识别
"BOMB_IDENTIFY_Y" : 危险区域标志识别高度：越小越早识别
"BRIDGE_IDENTIFY_Y" : 桥梁区域识别高度：越小越早识别
"RESCUE_IDENTIFY_Y" : 救援区域标志识别高度:越小越早识别
"DANGER_ZONE_BARRIER_Y" : 危险区域障碍物高度阈值：当障碍物离车身远的时候舵机打角乘以倍率使得大角更大，当障碍物离车身近的时候舵机打角正常，使得避障的时候能够避开距离较近的障碍物
"DANGER_ZONE_BARRIER_SERVOR_ANGLE_FACTOR" : 危险区域障碍物舵机打角倍率
"RESCUE_ZONE_CONE_AVG_Y_MIN" : 救援区域入库时间决策高度最小值
"RESCUE_ZONE_CONE_AVG_Y_MAX" : 救援区域入库时间决策高度最大值
"DANGER_ZONE_CONE_RADIUS" : 危险区域锥桶半径
"DANGER_ZONE_BLOCK_RADIUS" : 危险区域方块半径
"DANGER_ZONE_FORWARD" : 危险区域前瞻点高度
"BRIDGE_ZONE_FORWARD" : 桥梁区域前瞻点高度
```
# 工程编写逻辑详解
## 多线程
由于串行计算会导致帧率降低，因此本工程在第四大版本进行了多线程优化
### 图像采集线程
- 采集图像并存储至`Img_Store`结构体中的`Img_Capture`

- 在主线程中使用`Img_Color`承接`Img_Capture`以此降低因图像捕捉导致的性能下降
### 串口收发线程
暂时未写
### 保护线程
使用 <b>SSH</b> 连接板卡，当车运动时，终端输入一个非零数则使能保护，车辆停止
### 主线程
#### 图像获取处理阶段
获取图像采集线程采集的图像并进行图像预处理

进行八邻域图像边线采集，获取图像边线坐标，方便状态机阶段的拐点弯点识别
#### 状态机判断阶段
1. 进行模型推理结果判断，以此判断出模型赛道的区域类型
2. 进行拐点、弯点识别结果的综合判断，以此判断出传统赛道的赛道类型
```
1.左右都有拐点：十字赛道

2.左右只有一边有拐点且只有一边有直线：圆环赛道

3.下位机陀螺仪积分达到目标值：圆环出环步骤

4.以上均不符合：普通赛道：凭弯点数量判断是直道还是弯道
```
有以下情况需注意并在代码中设置条件来防止误判

1. 斜入十字导致的先单边拐点后双边拐点：检测到十字赛道状态就把圆环赛道步骤清零，即将圆环赛道步骤置于 <b>INIT</b>

2. 弯道或十字的圆环准备入环点误判：设置时间，超出规定时间若还未从准备入环步骤进入入环步骤则将圆环赛道步骤置于 <b>INIT</b>

3. 未能成功出环导致的圆环步骤置于 <b>OUT_PREPARE</b> 使得下一次遇到圆环无法进环：设置时间，超出规定时间就把圆环赛道步骤清零，即将圆环赛道步骤置于 <b>INIT</b>

4. 因八邻域的爬线导致的左右种子均爬到同一拐点即左右边线均识别到拐点(十字赛道)：设置左右拐点横坐标间距，若左右拐点横坐标间距小于规定阈值则退出十字赛道识别，并以准备入环阶段获取的圆环类型设置入环的赛道类型(方向)
\
#### 赛道操作
1. 普通赛道
2. 十字赛道
3. 圆环赛道

- <b>圆环赛道 </b>根据状态机内判断的圆环类型和圆环步骤确定圆环赛道所需执行的动作

    - 准备入环补线
    - 入环补线
    - 出环补线
