#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <cstdio>
#include <functional>
#include <numeric>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>    
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/time.h>
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>  
#include <errno.h>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <sys/time.h>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <pthread.h>
#include <thread>
#include "opencv2/opencv.hpp"
#include "json.hpp"

/************************************************************* 编码器 ***************************************************************/
#define ENCODER_PPR 1024 // 编码器线数

#define PWM_BASE_ADDR 0x1611B000
#define PWM_OFFSET 0x10
#define LOW_BUFFER_OFFSET 0x4
#define FULL_BUFFER_OFFSET 0x8
#define CONTROL_REG_OFFSET 0xC

#define CNTR_ENABLE_BIT (1 << 0)       // 计数器使能
#define PULSE_OUT_ENABLE_BIT (1 << 3)  // 脉冲输出使能（低有效）
#define SINGLE_PULSE_BIT (1 << 4)      // 单脉冲控制位
#define INT_ENABLE_BIT (1 << 5)        // 中断使能
#define INT_STATUS_BIT (1 << 6)        // 中断状态
#define COUNTER_RESET_BIT (1 << 7)     // 计数器重置
#define MEASURE_PULSE_BIT (1 << 8)     // 测量脉冲使能
#define INVERT_OUTPUT_BIT (1 << 9)     // 输出翻转使能
#define DEAD_ZONE_ENABLE_BIT (1 << 10) // 防死区使能

/************************************************************* GTIM ***************************************************************/
#define GPIO_MUX_BASE_ADDR 0x16000490

#define GTIM_BASE_ADDR 0x16119000
#define GTIM_CR1_OFFSET 0x00
#define GTIM_CR2_OFFSET 0x04
#define GTIM_SMCR_OFFSET 0x08
#define GTIM_DIER_OFFSET 0x0C
#define GTIM_SR_OFFSET 0x10
#define GTIM_EGR_OFFSET 0x14
#define GTIM_CCMR1_OFFSET 0x18
#define GTIM_CCMR2_OFFSET 0x1C
#define GTIM_CCER_OFFSET 0x20
#define GTIM_CNT_OFFSET 0x24
#define GTIM_PSC_OFFSET 0x28
#define GTIM_ARR_OFFSET 0x2C
#define GTIM_CCR1_OFFSET 0x34
#define GTIM_CCR2_OFFSET 0x38
#define GTIM_CCR3_OFFSET 0x3C
#define GTIM_CCR4_OFFSET 0x40
#define GTIM_INSTA_OFFSET 0x50

/************************************************************* 循线 ***************************************************************/
#define PI 3.1415926
#define IMAGE_WIDTH 160
#define IMAGE_HEIGHT 120
#define SEARCH_START 95
#define SEARCH_END 30
#define LOONGEST_WHITE_COL_START 80
#define SIDE_END 115             // 八邻域寻到中间的线结束，中间线距离边框距离

inline int TRACK_WIDTH = 50;
inline int CONTROL_POINT = 40;
inline int INFLECTION_POINT_DISTANCE = 10;
inline int SKIP_POINT_DISTANCE = 20;
inline int INFLECTION_POINT_ANGLE_MIN = 80;
inline int INFLECTION_POINT_ANGLE_MAX = 150;
inline int BEND_POINT_ANGLE_MIN = 0;
inline int BEND_POINT_ANGLE_MAX = 170;
inline int PREDICTION_POINT = 20;

/************************************************************* PID ***************************************************************/
inline float SERVO_KP[4] = {0};
inline float SERVO_KI[4] = {0};
inline float SERVO_KD[4] = {0};
inline float MOTOR_KP[2] = {0};
inline float MOTOR_KI[2] = {0};
inline float MOTOR_KD[2] = {0};

#endif