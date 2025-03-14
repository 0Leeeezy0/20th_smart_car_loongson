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
Mat imgColor;
Mat imgGray;
Mat imgOTSU;

int main(void)
{
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

    Motor motor_L("pwmchip1", 73, false);
    Motor motor_R("pwmchip2", 72, true);
    Servo servo(89);
    // Encoder encoder_L(0, 51);
    // Encoder encoder_R(3, 50);

    Config config("../configs/config.json");
    config.readJSON();

    PID PID_Servo(PID::POSITIONAL);
    PID_Servo.outputLimit = 13;
    PID_Servo.iLimit = 2;

    Size size = Size(IMAGE_WIDTH, IMAGE_HEIGHT);

    VideoWriter video;
    video.open("../img/sample.avi",VideoWriter::fourcc('M','J','P','G'),25,size,true);

    int err;
    int frameCount = 0;

    while(1)
    {
        camera>>imgColor;
        resize(imgColor, imgColor, size, 0, 0, INTER_AREA);
        cvtColor(imgColor, imgGray, COLOR_BGR2GRAY);
        threshold(imgGray, imgOTSU, 0, 255, THRESH_BINARY | THRESH_OTSU);

        cout << "a" << endl;

        line(imgColor,Point(0,SEARCH_START),Point(IMAGE_WIDTH-1,SEARCH_START),Scalar(0,0,0),1);
        line(imgColor,Point(0,SEARCH_END),Point(IMAGE_WIDTH-1,SEARCH_END),Scalar(0,0,0),1);
        line(imgColor,Point(SIDE_END,SEARCH_START),Point(SIDE_END,SEARCH_END),Scalar(0,0,0),1);
        line(imgColor,Point(IMAGE_WIDTH-SIDE_END,SEARCH_START),Point(IMAGE_WIDTH-SIDE_END,SEARCH_END),Scalar(0,0,0),1);

        sideExtract();
        pointKindJudge();
        if(L_InflectionPointNum >= 2 && R_InflectionPointNum >= 2)
        {
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], L_InflectionPoint[1][0], L_InflectionPoint[1][1]);
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], R_InflectionPoint[1][0], R_InflectionPoint[1][1]);
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], 0, IMAGE_HEIGHT-1);
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH-1, IMAGE_HEIGHT-1);
        }
        else if(L_InflectionPointNum == 1 && R_InflectionPointNum >= 2)
        {
            if(R_InflectionPoint[1][0]-TRACK_WIDTH >= 0)
            {
                trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], R_InflectionPoint[1][0]-TRACK_WIDTH, R_InflectionPoint[1][1]);
                trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], R_InflectionPoint[1][0], R_InflectionPoint[1][1]);
                trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], 0, IMAGE_HEIGHT-1);
                trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH-1, IMAGE_HEIGHT-1);
            }
        }
        else if(L_InflectionPointNum >= 2 && R_InflectionPointNum == 1)
        {
            if(L_InflectionPoint[1][0]+TRACK_WIDTH < IMAGE_WIDTH)
            {
                trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], L_InflectionPoint[1][0], L_InflectionPoint[1][1]);
                trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], L_InflectionPoint[1][0]+TRACK_WIDTH, L_InflectionPoint[1][1]);
                trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], 0, IMAGE_HEIGHT-1);
                trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH-1, IMAGE_HEIGHT-1);
            }
        }
        else if(L_InflectionPointNum >= 2 && R_InflectionPointNum == 0)
        {
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], L_InflectionPoint[1][0], L_InflectionPoint[1][1]);
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], 0, IMAGE_HEIGHT-1);
        }
        else if(L_InflectionPointNum == 0 && R_InflectionPointNum >= 2)
        {
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], R_InflectionPoint[1][0], R_InflectionPoint[1][1]);
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH-1, IMAGE_HEIGHT-1);
        }
        else if(L_InflectionPointNum == 1 && R_InflectionPointNum == 1)
        {
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], IMAGE_WIDTH/2-TRACK_WIDTH/2, 0);
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH/2+TRACK_WIDTH/2, 0);
            trackLine(L_InflectionPoint[0][0], L_InflectionPoint[0][1], 0, IMAGE_HEIGHT-1);
            trackLine(R_InflectionPoint[0][0], R_InflectionPoint[0][1], IMAGE_WIDTH-1, IMAGE_HEIGHT-1);
        }
        else if(L_InflectionPointNum == 0 && R_InflectionPointNum == 1)
        {
            /* R_CIRCLE */
        }
        else if(L_InflectionPointNum == 1 && R_InflectionPointNum == 0)
        {
            /* L_CIRCLE */
        }
        pathExtract();

        err = path[CONTROL_POINT][0] - IMAGE_WIDTH/2;

        if(abs(err) > 40)
        {
            PID_Servo.kp = SERVO_KP[3];
            PID_Servo.ki = SERVO_KI[3];
            PID_Servo.kd = SERVO_KD[3];
            motor_L.setMotorRotate(1, 170000);
            motor_R.setMotorRotate(1, 170000);
        }
        else if(abs(err) > 15 && abs(err) <= 40)
        {
            PID_Servo.kp = SERVO_KP[2];
            PID_Servo.ki = SERVO_KI[2];
            PID_Servo.kd = SERVO_KD[2];
            motor_L.setMotorRotate(1, 165000);
            motor_R.setMotorRotate(1, 165000);
        }
        else if(abs(err) > 5 && abs(err) <= 15)
        {
            PID_Servo.kp = SERVO_KP[1];
            PID_Servo.ki = SERVO_KI[1];
            PID_Servo.kd = SERVO_KD[1];
            motor_L.setMotorRotate(1, 162000);
            motor_R.setMotorRotate(1, 162000);
        }
        else
        {
            PID_Servo.kp = SERVO_KP[0];
            PID_Servo.ki = SERVO_KI[0];
            PID_Servo.kd = SERVO_KD[0];
            motor_L.setMotorRotate(1, 160000);
            motor_R.setMotorRotate(1, 160000);
        }

        float value = (int)PID_Servo.getValue(0, err);
        cout << "\033c" << endl;
        cout << err << endl;
        cout << value << endl;
        // // cout << encoder_L.pulseCounterUpdate() << endl;
        // // cout << encoder_R.pulseCounterUpdate() << endl;

        servo.setServoRotate(value);
        
        video.write(imgColor);

        frameCount++;
        if(frameCount == 100000)
            frameCount = 0;
        cout << L_InflectionPointNum << "    " << R_InflectionPointNum << endl;

        // waitKey(1);
    }

    return 0;
}

