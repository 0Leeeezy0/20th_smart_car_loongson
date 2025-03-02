#include "libdata_store.h"

#ifndef _LIBDATA_PROCESS_H_
#define _LIBDATA_PROCESS_H_


class Judge
{
    public:

        /*
            赛道循环类型决策
            1.普通赛道类型
            2.圆环赛道类型
            3.十字赛道类型
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
            Function_EN_p 函数使能相关指针
            @返回值说明
            返回赛道循环类型
        */
        LoopKind TrackKind_Judge(Img_Store* Img_Store_p,Data_Path *Data_Path_p,Function_EN* Function_EN_p);


        /*
            计算舵机方向和角度
            @参数说明
            Data_Path_p 路径相关数据指针
        */
        void ServoDirAngle_Judge(Data_Path *Data_Path_p);


        /*
            电机速度决策
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void MotorSpeed_Judge(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

        
        /*
            保护线程
            @参数说明
            Data_Path_p 路径相关数据指针
        */
        void Protect_Thread(Data_Path * Data_Path_p);
    
    private:
        /*
            边线拐点寻找
            @ 参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void InflectionPointSearch(Img_Store* Img_Store_p,Data_Path *Data_Path_p);


        /*
            边线弯点寻找
            @ 参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void BendPointSearch(Img_Store* Img_Store_p,Data_Path *Data_Path_p);


        /*
            霍夫圆环识别
            @ 参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void HoughCircleSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p);
};


class SYNC
{
    public:
        /*
            车辆上位机设置文件数据同步
            @参数说明
            Function_EN_p 函数使能指针
            Data_Path_p 路径相关数据指针
        */
        void ConfigData_SYNC(Data_Path *Data_Path_p,Function_EN *Function_EN_p);
};


/*
    打印数据
    程序参数：1.前瞻点 2.寻边线起始点 3.寻边线结束点 4.边线断点起始点 5.边线断点结束点 6.比赛状态
    运动参数：1.舵机方向 2.舵机角度 3.点击速度
    @参数说明
    Data_Path_p 路径相关数据指针
    Function_EN_p 函数使能指针
    @注意
    使用前必须使用 UartReceive_Change_To_Program_SYNC() 同步数据 和 ServoDirAngle_Judge() 计算运动参数
*/
void DataPrint(Data_Path *Data_Path_p,Function_EN *Function_EN_p);

#endif

