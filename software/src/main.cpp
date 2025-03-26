#include "opencv2/opencv.hpp"
#include <iostream>
#include "common.h"
#include "control.h"
#include "track.h"
#include "config.h"

using namespace std;
using namespace cv;
using namespace LOONGSON;

extern int path[SEARCH_START-SEARCH_END+1][2];
extern int L_Side[IMAGE_HEIGHT*4][2];
extern int R_Side[IMAGE_HEIGHT*4][2];
extern int L_SidePointNum;
extern int R_SidePointNum;
extern int L_InflectionPoint[IMAGE_HEIGHT*2][2];
extern int R_InflectionPoint[IMAGE_HEIGHT*2][2];
extern int L_InflectionPointNum;
extern int R_InflectionPointNum;
extern int L_BendPoint[IMAGE_HEIGHT*2][2];
extern int R_BendPoint[IMAGE_HEIGHT*2][2];
extern int L_BendPointNum;
extern int R_BendPointNum;
extern int longestWhiteColX;
Mat imgColor;
Mat imgGray;
Mat imgOTSU;

int main(void)
{
    /************************************** 摄像头 **************************************/
    VideoCapture camera;
    camera.open("/dev/video0", CAP_V4L2);
    if(!camera.isOpened())
        camera.open("/dev/video1", CAP_V4L2);
    if(!camera.isOpened())
        camera.open("/dev/video2", CAP_V4L2);
    if(!camera.isOpened())
        camera.open("/dev/video3", CAP_V4L2);
    if(!camera.isOpened())
        camera.open("/dev/video4", CAP_V4L2);
    camera.set(CAP_PROP_FRAME_WIDTH, 320);
    camera.set(CAP_PROP_FRAME_HEIGHT, 240);
    camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    camera.set(CAP_PROP_FPS, 90);
    /************************************************************************************/

    /************************************** 电机、舵机、编码器 **************************************/
    Motor motor_L("pwmchip0", 45, true);
    Motor motor_R("pwmchip3", 44, false);
    Servo servo(88);
    Encoder encoder_L(2, 72);
    Encoder encoder_R(1, 73);
    /***********************************************************************************************/

    /************************************** JSON配置文件参数读取 **************************************/
    Config config("../configs/config.json");
    config.readJSON();
    /*************************************************************************************************/

    /************************************** PID初始化 **************************************/
    PID PID_Servo(PID::POSITIONAL);     // 位置式
    PID_Servo.outputLimit = 13;
    PID_Servo.iLimit = 2;
    PID PID_Motor_L(PID::POSITIONAL);   // 位置式
    PID_Motor_L.outputLimit = 190000;
    PID_Motor_L.iLimit = 130000;
    PID PID_Motor_R(PID::POSITIONAL);   // 位置式
    PID_Motor_R.outputLimit = 190000;
    PID_Motor_R.iLimit = 130000;
    /***************************************************************************************/

    Size size = Size(IMAGE_WIDTH, IMAGE_HEIGHT);

    /************************************** 录像 **************************************/
    VideoWriter video;
    video.open("../img/sample.avi",VideoWriter::fourcc('M','J','P','G'),25,size,true);
    /**********************************************************************************/

    int err;
    int frameCount = 0;
    int LmotorValue = 0;
    int RmotorValue = 0;

    PID_Motor_R.kp = MOTOR_KP[0];
    PID_Motor_R.ki = MOTOR_KI[0];
    PID_Motor_R.kd = MOTOR_KD[0];
    PID_Motor_L.kp = MOTOR_KP[1];
    PID_Motor_L.ki = MOTOR_KI[1];
    PID_Motor_L.kd = MOTOR_KD[1];

    while(1)
    {
        /************************************** 图像预处理 **************************************/
        camera>>imgColor;
        resize(imgColor, imgColor, size, 0, 0, INTER_AREA);
        cvtColor(imgColor, imgGray, COLOR_BGR2GRAY);
        threshold(imgGray, imgOTSU, 0, 255, THRESH_BINARY | THRESH_OTSU);
        /************************************************ **************************************/

        /************************************** 参考线 **************************************/
        line(imgColor,Point(0,SEARCH_START),Point(IMAGE_WIDTH-1,SEARCH_START),Scalar(0,0,0),1);
        line(imgColor,Point(0,SEARCH_END),Point(IMAGE_WIDTH-1,SEARCH_END),Scalar(0,0,0),1);
        line(imgColor,Point(SIDE_END,SEARCH_START),Point(SIDE_END,SEARCH_END),Scalar(0,0,0),1);
        line(imgColor,Point(IMAGE_WIDTH-SIDE_END,SEARCH_START),Point(IMAGE_WIDTH-SIDE_END,SEARCH_END),Scalar(0,0,0),1);
        line(imgColor,Point(0,SEARCH_START-CONTROL_POINT),Point(IMAGE_WIDTH-1,SEARCH_START-CONTROL_POINT),Scalar(0,0,255),1);
        /***********************************************************************************/
        
        longestWhiteCol();
        line(imgColor,Point(longestWhiteColX,SEARCH_START-CONTROL_POINT),Point(longestWhiteColX,SEARCH_START),Scalar(0,255,0),1);

        err = longestWhiteColX - IMAGE_WIDTH/2;

        if(abs(err) > 40)
        {
            PID_Servo.kp = SERVO_KP[3];
            PID_Servo.ki = SERVO_KI[3];
            PID_Servo.kd = SERVO_KD[3];
            LmotorValue = (int)PID_Motor_L.getValue(10, (float)encoder_L.pulseCounterUpdate());
            RmotorValue = (int)PID_Motor_R.getValue(10, -(float)encoder_R.pulseCounterUpdate());
        }
        else if(abs(err) > 15 && abs(err) <= 40)
        {
            PID_Servo.kp = SERVO_KP[2];
            PID_Servo.ki = SERVO_KI[2];
            PID_Servo.kd = SERVO_KD[2];
            LmotorValue = (int)PID_Motor_L.getValue(13, (float)encoder_L.pulseCounterUpdate());
            RmotorValue = (int)PID_Motor_R.getValue(13, -(float)encoder_R.pulseCounterUpdate());
        }
        else if(abs(err) > 5 && abs(err) <= 15)
        {
            PID_Servo.kp = SERVO_KP[1];
            PID_Servo.ki = SERVO_KI[1];
            PID_Servo.kd = SERVO_KD[1];
            LmotorValue = (int)PID_Motor_L.getValue(16, (float)encoder_L.pulseCounterUpdate());
            RmotorValue = (int)PID_Motor_R.getValue(16, -(float)encoder_R.pulseCounterUpdate());
        }
        else
        {
            PID_Servo.kp = SERVO_KP[0];
            PID_Servo.ki = SERVO_KI[0];
            PID_Servo.kd = SERVO_KD[0];
            LmotorValue = (int)PID_Motor_L.getValue(21, (float)encoder_L.pulseCounterUpdate());
            RmotorValue = (int)PID_Motor_R.getValue(21, -(float)encoder_R.pulseCounterUpdate());
        }

        float servovValue = PID_Servo.getValue(0, err);
        cout << "\033c" << endl;
        cout << err << endl;
        // cout << value << endl;
        cout << encoder_L.pulseCounterUpdate() << endl;
        cout << encoder_R.pulseCounterUpdate() << endl;

        servo.setServoRotate(servovValue);
        // motor_L.setMotorRotate(LmotorValue/abs(LmotorValue), 200000-LmotorValue);
        // motor_R.setMotorRotate(RmotorValue/abs(RmotorValue), 200000-RmotorValue);
        motor_L.setMotorRotate(1, 160000);
        motor_R.setMotorRotate(1, 160000);
        
        video.write(imgColor);

        frameCount++;
        if(frameCount == 100000)
            frameCount = 0;
        cout << L_InflectionPointNum << "    " << R_InflectionPointNum << endl;

        // waitKey(1);
    }

    return 0;
}

