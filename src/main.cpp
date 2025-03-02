#include "common_system.h"
#include "common_program.h"
#include "base.h"
#include "motor.h"
#include "steer.h"
#include "register.h"

using namespace std;
using namespace cv;

int main()
{
    // 类定义
    ImgProcess ImgProcess;
    Judge Judge;
    SYNC SYNC;

    // 数据结构体指针定义
    Img_Store Img_Store_c; 
    Img_Store *Img_Store_p = &Img_Store_c;
    Function_EN Function_EN_c;
    Function_EN *Function_EN_p = &Function_EN_c;
    Data_Path Data_Path_c;
    Data_Path *Data_Path_p = &Data_Path_c;

    // 参数获取
    SYNC.ConfigData_SYNC(Data_Path_p,Function_EN_p);
    JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    // PWM初始化
    init_pwm();
    PWM_GTIM L_motor(88, 0b11, 2, 10000, 0); 
    L_motor.enable();
    PWM_GTIM R_motor(89, 0b11, 3, 10000, 0);
    R_motor.enable();
    init_gpio_motor_dir();//电机方向脚初始化
    L_motor.setPeriod(10000);  
    R_motor.setPeriod(10000);

    // 摄像头初始化
    VideoCapture Camera; // 定义相机
    CameraInit(Camera,JSON_FunctionConfigData.Camera_EN,120);
    // 开启摄像头图像获取线程
    thread CameraImgCapture (CameraImgGetThread,ref(Camera),ref(Img_Store_p));
    CameraImgCapture.detach();
    
    Function_EN_p -> Game_EN = true;
    Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP;

    sleep(1);   // 延时1S等待第一帧模型推理完成

    while(Function_EN_p -> Game_EN == true)
    {   
        // 图像主循环
        while( Function_EN_p -> Loop_Kind_EN == CAMERA_CATCH_LOOP)
        {
            // 前瞻点初始化
            Data_Path_p -> JSON_TrackConfigData_v[0].Forward = Data_Path_p -> JSON_TrackConfigData_v[0].Default_Forward;

            CameraImgGet(Img_Store_p);
            // (Img_Store_p -> Img_Color) = (Img_Store_p -> Img_Capture).clone();
            ImgProcess.ImgCompress(Img_Store_p -> Img_Color,JSON_FunctionConfigData.ImgCompress_EN);
            ImgProcess.ImgPrepare(Img_Store_p,Data_Path_p,Function_EN_p); // 图像预处理

            ImgSideSearch(Img_Store_p,Data_Path_p);   // 边线八邻域寻线

            Img_Store_p -> ImgNum++;
            Function_EN_p -> Loop_Kind_EN = JUDGE_LOOP;
            cout << "CAMERA_CATCH_LOOP" << endl;
            break;
        }

        // 赛道状态机决策循环
        while( Function_EN_p -> Loop_Kind_EN == JUDGE_LOOP )
        {
            Function_EN_p -> Loop_Kind_EN = Judge.TrackKind_Judge(Img_Store_p,Data_Path_p,Function_EN_p);  // 切换至赛道循环
            cout << "JUDGE_LOOP" << endl;
        }

        // 普通赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == COMMON_TRACK_LOOP )
        {
            if(Data_Path_p -> Circle_Track_Step == IN_PREPARE)
            {
                CircleTrack_Step_IN_Prepare_Stright(Img_Store_p,Data_Path_p);   // 准备入环补线
            }
            if(Data_Path_p -> Circle_Track_Step == OUT_2_STRIGHT)
            {
                Circle2CommonTrack(Img_Store_p,Data_Path_p);    // 出环转直线补线
            }
            ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            Judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算
            Judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环

            // cout << "COMMON_TRACK_LOOP" << endl;
        }

        // 左右圆环赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == L_CIRCLE_TRACK_LOOP || Function_EN_p -> Loop_Kind_EN == R_CIRCLE_TRACK_LOOP )
        {
            switch(Data_Path_p -> Circle_Track_Step)
            {
                case IN_PREPARE:
                {
                    CircleTrack_Step_IN_Prepare(Img_Store_p,Data_Path_p);   // 准备入环补线
                    break;
                }
                case IN:
                {
                    CircleTrack_Step_IN(Img_Store_p,Data_Path_p);   // 入环补线
                    break;
                }
                case OUT:
                {
                    CircleTrack_Step_OUT(Img_Store_p,Data_Path_p);   // 出环补线
                    break;
                }
            }
            ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            Judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算
            Judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环
            // cout << "CIRCLE_TRACK_L
        
        }

        // 十字赛道主循环
        while( Function_EN_p -> Loop_Kind_EN == ACROSS_TRACK_LOOP )
        {
            AcrossTrack(Img_Store_p,Data_Path_p);   // 十字赛道补线
            ImgPathSearch(Img_Store_p,Data_Path_p); // 赛道路径线寻线
            Judge.ServoDirAngle_Judge(Data_Path_p); // 舵机角度计算
            Judge.MotorSpeed_Judge(Img_Store_p,Data_Path_p);    // 电机速度决策
            Function_EN_p -> Loop_Kind_EN = CAMERA_CATCH_LOOP; // 切换至串口发送循环
            // cout << "ACROSS_TRACK_LOOP" << endl;
        }

        Motor_Control(L_motor,R_motor,1500,1500);
        SteerControl(Data_Path_p->ServoDir,Data_Path_p->ServoAngle);
        cout << Data_Path_p->ServoDir << endl;
        cout << Data_Path_p->ServoAngle << endl;
    }

    return 0;
}
