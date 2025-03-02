#include "common_system.h"
#include "common_program.h"
#include "libdata_store.h"

using namespace std;
using namespace cv;


/*
    TrackKind_Judge说明
    赛道循环类型决策
    1.普通赛道循环类型
    2.圆环赛道循环类型
    3.十字赛道循环类型
    4.AI赛道类型
*/
LoopKind Judge::TrackKind_Judge(Img_Store* Img_Store_p,Data_Path *Data_Path_p,Function_EN* Function_EN_p)
{
    LoopKind Loop_Kind;
    JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    static int State = 0;   // 状态记录
    static int State_Across = 0;
    static int State_Circle_IN_PREPARE = 0; // 准备入环时间
    static int State_Circle_IN = 0;    // 入环时间
    static int State_Circle_OUT_PREPARE = 0;    // 准备出环时间
    static int State_Circle_OUT = 0;    // 出环时间

    State = Img_Store_p -> ImgNum;

    if(Function_EN_p -> Control_EN == false)
    {
        Judge::InflectionPointSearch(Img_Store_p,Data_Path_p);
        Judge::BendPointSearch(Img_Store_p,Data_Path_p);
        // 十字判断
        // 若左右边线都有拐点则为十字
        if((Data_Path_p -> InflectionPointNum[0] >= 1) && (Data_Path_p -> InflectionPointNum[1] >= 1) && JSON_FunctionConfigData.AcrossIdentify_EN == true && Function_EN_p -> Gyroscope_EN == false && (Data_Path_p -> Circle_Track_Step != OUT_PREPARE || Data_Path_p -> Circle_Track_Step != OUT))
        {
            State_Across = Img_Store_p -> ImgNum;
            Loop_Kind = ACROSS_TRACK_LOOP;
            Data_Path_p -> Track_Kind = ACROSS_TRACK;
            Data_Path_p -> Circle_Track_Step = INIT;

            // 防止左右边线均寻找到同一个拐点导致误判为十字
            if(abs((Data_Path_p -> InflectionPointCoordinate[0][0])-(Data_Path_p -> InflectionPointCoordinate[0][2])) <= 30)
            {
                switch(Data_Path_p -> Previous_Circle_Kind)
                {
                    case L_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = L_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = L_CIRCLE_TRACK_OUTSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                    case R_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = R_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = R_CIRCLE_TRACK_OUTSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                }
            }
        }
        // 圆环判断
        // 若左右边线只有一边有拐点和弯点  
        //且当前图像序号和十字中存储的图像序号有间隔才为左右圆环
        else if((Data_Path_p -> InflectionPointNum[0] == 0) && (Data_Path_p -> InflectionPointNum[1] >= 1) && (Data_Path_p -> BendPointNum[0] <= 2) && (Data_Path_p -> BendPointNum[1] >= 1) && State - State_Across >= 5 && Function_EN_p -> Gyroscope_EN == false && JSON_FunctionConfigData.CircleIdentify_EN == true)
        {
            // 准备入环阶段
            // 在出环后经过固定帧数才能再次准备进环
            if(((Data_Path_p -> Circle_Track_Step) == INIT || (Data_Path_p -> Circle_Track_Step) == IN_PREPARE || (Data_Path_p -> Circle_Track_Step) == IN) && Data_Path_p -> Vector_Add_Unit_Dir[1] == 1)
            {
                Loop_Kind = R_CIRCLE_TRACK_LOOP;
                Data_Path_p -> Track_Kind = R_CIRCLE_TRACK_OUTSIDE;

                Data_Path_p -> Circle_Track_Step = IN_PREPARE;
                Data_Path_p -> Previous_Circle_Kind = R_CIRCLE_TRACK_OUTSIDE;

                // 记录准备进环时间
                State_Circle_IN_PREPARE = Img_Store_p -> ImgNum;
            }
            // 入环阶段
            else if(Data_Path_p -> Vector_Add_Unit_Dir[1] == -1 && (Data_Path_p -> Circle_Track_Step == IN_PREPARE || Data_Path_p -> Circle_Track_Step == IN))   
            {
                Data_Path_p -> Circle_Track_Step = IN;

                // 以准备入环阶段确定的圆环类型作为入环阶段的圆环类型
                switch(Data_Path_p -> Previous_Circle_Kind)
                {
                    case L_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = L_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = L_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                    case R_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = R_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = R_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                }

                // 记录进环时间
                State_Circle_IN = Img_Store_p -> ImgNum;
            }   
            // 考虑十字姿态不好，误判为圆环的情况
            else if(Data_Path_p -> Vector_Add_Unit_Dir[1] == -1 && Data_Path_p -> Circle_Track_Step == INIT)  
            {
                Loop_Kind = COMMON_TRACK_LOOP;
                Data_Path_p -> Track_Kind = BEND_TRACK;
            }
            else
            {
                Loop_Kind = COMMON_TRACK_LOOP;
                Data_Path_p -> Track_Kind = BEND_TRACK;
            }
        }
        else if((Data_Path_p -> InflectionPointNum[0] >= 1) && (Data_Path_p -> InflectionPointNum[1] == 0) && (Data_Path_p -> BendPointNum[0] >= 1) && (Data_Path_p -> BendPointNum[1] <= 2) && State - State_Across >= 5 && Function_EN_p -> Gyroscope_EN == false && JSON_FunctionConfigData.CircleIdentify_EN == true)
        {
            // 准备入环阶段
            // 在出环后经过固定帧数才能再次准备进环
            if(((Data_Path_p -> Circle_Track_Step) == INIT || (Data_Path_p -> Circle_Track_Step) == IN_PREPARE || (Data_Path_p -> Circle_Track_Step) == IN) && Data_Path_p -> Vector_Add_Unit_Dir[0] == 1)
            {
                Loop_Kind = L_CIRCLE_TRACK_LOOP;
                Data_Path_p -> Track_Kind = L_CIRCLE_TRACK_OUTSIDE;

                Data_Path_p -> Circle_Track_Step = IN_PREPARE;
                Data_Path_p -> Previous_Circle_Kind = L_CIRCLE_TRACK_OUTSIDE;

                // 记录准备进环时间
                State_Circle_IN_PREPARE = Img_Store_p -> ImgNum;
            }
            // 入环阶段
            else if(Data_Path_p -> Vector_Add_Unit_Dir[0] == -1 && (Data_Path_p -> Circle_Track_Step == IN_PREPARE || Data_Path_p -> Circle_Track_Step == IN))   
            {
                Data_Path_p -> Circle_Track_Step = IN;

                // 以准备入环阶段确定的圆环类型作为入环阶段的圆环类型
                switch(Data_Path_p -> Previous_Circle_Kind)
                {
                    case L_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = L_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = L_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                    case R_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = R_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = R_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = IN; break; }
                }
                
                // 记录进环时间
                State_Circle_IN = Img_Store_p -> ImgNum;
            }   
            // 考虑十字姿态不好，误判为圆环的情况
            else if(Data_Path_p -> Vector_Add_Unit_Dir[0] == -1 && Data_Path_p -> Circle_Track_Step == INIT)  
            {
                Loop_Kind = COMMON_TRACK_LOOP;
                Data_Path_p -> Track_Kind = BEND_TRACK;
            }
            else
            {
                Loop_Kind = COMMON_TRACK_LOOP;
                Data_Path_p -> Track_Kind = BEND_TRACK;
            }
        }
        // 出环判断
        // 若当前圆环步骤为准备出环或出环状态则在下位机陀螺仪积分达到目标值的时间区间内进行出环操作
        else if((Data_Path_p -> Circle_Track_Step == OUT_PREPARE || Data_Path_p -> Circle_Track_Step == OUT) && Function_EN_p -> Gyroscope_EN == true)
        {
            Data_Path_p -> Circle_Track_Step = OUT;

            // 以准备入环阶段确定的圆环类型作为出环阶段的圆环类型
            switch(Data_Path_p -> Previous_Circle_Kind)
            {
                case L_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = L_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = L_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = OUT; break; }
                case R_CIRCLE_TRACK_OUTSIDE:{ Loop_Kind = R_CIRCLE_TRACK_LOOP; Data_Path_p -> Track_Kind = R_CIRCLE_TRACK_INSIDE; Data_Path_p -> Circle_Track_Step = OUT; break; }
            }

            // 记录出环时间
            State_Circle_OUT = Img_Store_p -> ImgNum;
        }
        // 普通赛道判断
        else
        {
            Loop_Kind = COMMON_TRACK_LOOP;
            // 判定是弯道还是直道
            if((Data_Path_p -> BendPointNum[0] >= JSON_TrackConfigData.BendPointNum[0]) || (Data_Path_p -> BendPointNum[1] >= JSON_TrackConfigData.BendPointNum[0]))
            {
                Data_Path_p -> Track_Kind = BEND_TRACK;
            }
            else
            {
                Data_Path_p -> Track_Kind = STRIGHT_TRACK;
            }

            // 判定圆环步骤
            // 进入圆环后固定帧数进入准备出环步骤
            if(State - State_Circle_IN >= 10 && Data_Path_p -> Circle_Track_Step == IN)
            {
                Data_Path_p -> Circle_Track_Step = OUT_PREPARE;
                State_Circle_OUT_PREPARE = Img_Store_p -> ImgNum;
            }
            // 若误判为准备入环则在固定帧数之后进入占位：防止在弯道十字等位置误判导致一直补线从而影响寻线
            if(State - State_Circle_IN_PREPARE >= JSON_TrackConfigData.Circle_In_Prepare_Time && Data_Path_p -> Circle_Track_Step == IN_PREPARE)
            {
                Data_Path_p -> Circle_Track_Step = INIT;
            }
            // 出环后进入出环转直线
            if((Data_Path_p -> Circle_Track_Step) == OUT)
            {
                Data_Path_p -> Circle_Track_Step = OUT_2_STRIGHT;
                State_Circle_OUT = Img_Store_p -> ImgNum;
            }
            // 经过固定帧数后出环转直线进入占位从而可以进行准备下一次的圆环
            if((Data_Path_p -> Circle_Track_Step) == OUT_2_STRIGHT && State-State_Circle_OUT >= 60)
            {
                Data_Path_p -> Circle_Track_Step = INIT;
            }
            // 防止上次圆环未入环导致状态卡在准备出环阶段
            // 经过固定帧数后准备出环进入占位从而可以进行准备下一次的圆环
            if((Data_Path_p -> Circle_Track_Step) == OUT_PREPARE && State-State_Circle_OUT_PREPARE >= 200)
            {
                Data_Path_p -> Circle_Track_Step = INIT;
            }
        }
    }
    
    return Loop_Kind;
}

/*
    ServoDirAngle_Judge说明
    计算舵机方向和舵机角度
*/
void Judge::ServoDirAngle_Judge(Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    (Data_Path_p -> ServoAngle) = (Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][0]) - 160;   
    // 计算舵机方向和角度
    if((Data_Path_p -> ServoAngle) < 0)
    {
        (Data_Path_p -> ServoDir) = 1;  // 右转
        (Data_Path_p -> ServoAngle) = abs(Data_Path_p -> ServoAngle);
    }
    else
    {
        (Data_Path_p -> ServoDir) = -1;  // 左转
    }
}


/*
    MotorSpeed_Judge说明
    电机速度决策
*/
void Judge::MotorSpeed_Judge(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    /*
        由于圆环步骤和赛道类型是独立的，因此会出现如下情况
        1、直道但是圆环步骤不为占位，如准备入环到入环的阶段
        2、弯道但是圆环步骤不为占位，同上
        为防止速度过快时的拐点识别率下降，因此引入直道弯道时也要考虑圆环步骤防止误判
    */
    switch(Data_Path_p -> Track_Kind)
    {
        // 直道速度决策
        case STRIGHT_TRACK:
        {
            // 准备入环的直线部分的速度决策
            if(Data_Path_p -> ServoAngle > 30 || Data_Path_p -> Circle_Track_Step == IN_PREPARE)
            {
                Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[4];
            }
            // 准备出环、出环进直线的直线部分的速度决策
            else if(Data_Path_p -> ServoAngle > 30 || Data_Path_p -> Circle_Track_Step == OUT_PREPARE || Data_Path_p -> Circle_Track_Step == OUT_2_STRIGHT)
            {
                Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[5];
            }
            // 真正的直道的速度决策
            else
            {
                Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[0];
            }
            break;
        }
        case BEND_TRACK:
        {
            // 小弯道速度决策
            if(Data_Path_p -> BendPointNum[0] <= JSON_TrackConfigData.BendPointNum[1] || Data_Path_p -> BendPointNum[1] <= JSON_TrackConfigData.BendPointNum[1])
            {
                // 准备入环的小弯道部分的速度决策
                if(Data_Path_p -> Circle_Track_Step == IN_PREPARE)
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[4];
                }
                // 准备出环、出环进直线的小弯道部分的速度决策
                else if(Data_Path_p -> Circle_Track_Step == OUT_PREPARE || Data_Path_p -> Circle_Track_Step == OUT_2_STRIGHT)
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[5];
                }
                // 其他小弯道部分的速度决策
                else
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[1];
                }
            }
            // 大弯道速度决策
            else
            {
                // 准备入环的大弯道部分的速度决策
                if(Data_Path_p -> Circle_Track_Step == IN_PREPARE)
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[4];
                }
                // 准备出环、出环进直线的大弯道部分的速度决策
                else if(Data_Path_p -> Circle_Track_Step == OUT_PREPARE || Data_Path_p -> Circle_Track_Step == OUT_2_STRIGHT)
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[5];
                }
                // 其他大弯道部分的速度决策
                else
                {
                    Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[2];
                }
            }
            break;
        }
        case L_CIRCLE_TRACK_OUTSIDE:
        {
            Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[4];
            break;
        }
        case R_CIRCLE_TRACK_OUTSIDE:
        {
            Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[4];
            break;
        }
        case L_CIRCLE_TRACK_INSIDE:
        {
            Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[5];
            break;
        }
        case R_CIRCLE_TRACK_INSIDE:
        {
            Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[5];
            break;
        }
        case ACROSS_TRACK:
        {
            Data_Path_p -> MotorSpeed = JSON_TrackConfigData.CommonMotorSpeed[3];
            break;
        }
    }
}


/*
    InflectionPointSearch说明
    边线拐点寻找
*/
void Judge::InflectionPointSearch(Img_Store* Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    int i;
    int j;
    // static int Record;
    Data_Path_p -> InflectionPointNum[0] = 0;
    Data_Path_p -> InflectionPointNum[1] = 0;
    int Vector[2][4] = {0}; // 左右中断点与上下两点构成的向量坐标
    int Vector_ScalarProduct[2] = {0};  // 左右中断点向量点乘
    double Vector_Module[4] = {0};   // 左右中断点向量的模
    double AngleVector[2] = {0}; // 左右中断点向量夹角(角度制)
    // 寻拐点范围
    // 左边线拐点
    for(i = (JSON_TrackConfigData.InflectionPointVectorDistance);i <= (Data_Path_p -> NumSearch[0])-(JSON_TrackConfigData.InflectionPointVectorDistance);)
    {
        // 左边线第一个向量
        Vector[0][0] = (Data_Path_p -> SideCoordinate_Eight[i-(JSON_TrackConfigData.InflectionPointVectorDistance)][0])-(Data_Path_p -> SideCoordinate_Eight[i][0]);
        Vector[0][1] = (Data_Path_p -> SideCoordinate_Eight[i-(JSON_TrackConfigData.InflectionPointVectorDistance)][1])-(Data_Path_p -> SideCoordinate_Eight[i][1]);
        // 左边线第二个向量
        Vector[1][0] = (Data_Path_p -> SideCoordinate_Eight[i+(JSON_TrackConfigData.InflectionPointVectorDistance)][0])-(Data_Path_p -> SideCoordinate_Eight[i][0]);
        Vector[1][1] = (Data_Path_p -> SideCoordinate_Eight[i+(JSON_TrackConfigData.InflectionPointVectorDistance)][1])-(Data_Path_p -> SideCoordinate_Eight[i][1]);

        // 计算中断点向量点乘
        Vector_ScalarProduct[0] = Vector[0][0]*Vector[1][0]+Vector[0][1]*Vector[1][1];

        // 计算中断点向量的模
        Vector_Module[0] = sqrt(pow(Vector[0][0],2)+pow(Vector[0][1],2));
        Vector_Module[1] = sqrt(pow(Vector[1][0],2)+pow(Vector[1][1],2));
    
        if( Vector_Module[0]*Vector_Module[1] != 0)
        {
            AngleVector[0] = acos(Vector_ScalarProduct[0]/(Vector_Module[0]*Vector_Module[1]))*(180/PI);    // 左边线断点向量夹角
            // cout << AngleVector[0] << "  " << AngleVector[1] << endl;
        }

        // 计算拐点并存储坐标，前提：拐点坐标不再边框上
        if(abs(AngleVector[0]) > (JSON_TrackConfigData.InflectionPointIdentifyAngle[0]) && abs(AngleVector[0]) < (JSON_TrackConfigData.InflectionPointIdentifyAngle[1]) && (Data_Path_p -> SideCoordinate_Eight[i][0]) > 30 && (Vector[0][1]*Vector[1][1]) >= -40)
        {
            //  cout << abs(AngleVector[0]) << endl;
            (Data_Path_p -> InflectionPointCoordinate[(Data_Path_p -> InflectionPointNum[0])][0]) = (Data_Path_p -> SideCoordinate_Eight[i][0]);
            (Data_Path_p -> InflectionPointCoordinate[(Data_Path_p -> InflectionPointNum[0])][1]) = (Data_Path_p -> SideCoordinate_Eight[i][1]);
            if(Data_Path_p -> InflectionPointNum[0] == 0)
            {
                // 向量加和
                Data_Path_p -> Vector_Add_Unit_Dir[0] = (Vector[0][1]+Vector[1][1])/abs(Vector[0][1]+Vector[1][1]);
            }
            Data_Path_p -> InflectionPointNum[0]++;
            i = i+10;
        }
        i++;
    }
    // 右边线拐点
    for(j = (JSON_TrackConfigData.InflectionPointVectorDistance);j <= (Data_Path_p -> NumSearch[1])-(JSON_TrackConfigData.InflectionPointVectorDistance);)
    {
        // 右边线第一个向量
        Vector[0][2] = (Data_Path_p -> SideCoordinate_Eight[j-(JSON_TrackConfigData.InflectionPointVectorDistance)][2])-(Data_Path_p -> SideCoordinate_Eight[j][2]);
        Vector[0][3] = (Data_Path_p -> SideCoordinate_Eight[j-(JSON_TrackConfigData.InflectionPointVectorDistance)][3])-(Data_Path_p -> SideCoordinate_Eight[j][3]);
        // 右边线第二个向量
        Vector[1][2] = (Data_Path_p -> SideCoordinate_Eight[j+(JSON_TrackConfigData.InflectionPointVectorDistance)][2])-(Data_Path_p -> SideCoordinate_Eight[j][2]);
        Vector[1][3] = (Data_Path_p -> SideCoordinate_Eight[j+(JSON_TrackConfigData.InflectionPointVectorDistance)][3])-(Data_Path_p -> SideCoordinate_Eight[j][3]);

        // 计算拐点向量点乘
        Vector_ScalarProduct[1] = Vector[0][2]*Vector[1][2]+Vector[0][3]*Vector[1][3];

        // 计算拐点向量的模
        Vector_Module[2] = sqrt(pow(Vector[0][2],2)+pow(Vector[0][3],2));
        Vector_Module[3] = sqrt(pow(Vector[1][2],2)+pow(Vector[1][3],2));
    
        if( Vector_Module[2]*Vector_Module[3] != 0)
        {
            AngleVector[1] = acos(Vector_ScalarProduct[1]/(Vector_Module[2]*Vector_Module[3]))*(180/PI);    // 右边线断点向量夹角
            // cout << AngleVector[0] << "  " << AngleVector[1] << endl;
        }

        // 计算拐点并存储坐标，前提：拐点坐标不在边框上
        if(abs(AngleVector[1]) > (JSON_TrackConfigData.InflectionPointIdentifyAngle[0]) && abs(AngleVector[1]) < (JSON_TrackConfigData.InflectionPointIdentifyAngle[1]) && 319-(Data_Path_p -> SideCoordinate_Eight[j][2]) > 30 && (Vector[0][3]*Vector[1][3]) >= -40)
        {
            // cout << abs(AngleVector[1]) << endl;
            (Data_Path_p -> InflectionPointCoordinate[(Data_Path_p -> InflectionPointNum[1])][2]) = (Data_Path_p -> SideCoordinate_Eight[j][2]);
            (Data_Path_p -> InflectionPointCoordinate[(Data_Path_p -> InflectionPointNum[1])][3]) = (Data_Path_p -> SideCoordinate_Eight[j][3]);
            if(Data_Path_p -> InflectionPointNum[1] == 0)
            {
                // 向量加和
                Data_Path_p -> Vector_Add_Unit_Dir[1] = (Vector[0][3]+Vector[1][3])/abs(Vector[0][3]+Vector[1][3]);
            }
            Data_Path_p -> InflectionPointNum[1]++;
            j = j+10;
        }
        j++;
    }
}



/*
    BendPointSearch说明
    边线弯点寻找
*/
void Judge::BendPointSearch(Img_Store* Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

    int i;
    int j;
    // static int Record;
    Data_Path_p -> BendPointNum[0] = 0;
    Data_Path_p -> BendPointNum[1] = 0;
    int Vector[2][4] = {0}; // 左右弯点与上下两点构成的向量坐标
    int Vector_ScalarProduct[2] = {0};  // 左右弯点向量点乘
    double Vector_Module[4] = {0};   // 左右弯点向量的模
    double AngleVector[2] = {0}; // 左右弯点向量夹角(角度制)
    // 寻弯点范围
    // 左边线弯点
    for(i = (JSON_TrackConfigData.BendPointVectorDistance);i <= (Data_Path_p -> NumSearch[0])-(JSON_TrackConfigData.BendPointVectorDistance);)
    {
        // 左边线第一个向量
        Vector[0][0] = (Data_Path_p -> SideCoordinate_Eight[i-(JSON_TrackConfigData.BendPointVectorDistance)][0])-(Data_Path_p -> SideCoordinate_Eight[i][0]);
        Vector[0][1] = (Data_Path_p -> SideCoordinate_Eight[i-(JSON_TrackConfigData.BendPointVectorDistance)][1])-(Data_Path_p -> SideCoordinate_Eight[i][1]);
        // 左边线第二个向量
        Vector[1][0] = (Data_Path_p -> SideCoordinate_Eight[i+(JSON_TrackConfigData.BendPointVectorDistance)][0])-(Data_Path_p -> SideCoordinate_Eight[i][0]);
        Vector[1][1] = (Data_Path_p -> SideCoordinate_Eight[i+(JSON_TrackConfigData.BendPointVectorDistance)][1])-(Data_Path_p -> SideCoordinate_Eight[i][1]);

        // 计算弯点向量点乘
        Vector_ScalarProduct[0] = Vector[0][0]*Vector[1][0]+Vector[0][1]*Vector[1][1];

        // 计算弯点向量的模
        Vector_Module[0] = sqrt(pow(Vector[0][0],2)+pow(Vector[0][1],2));
        Vector_Module[1] = sqrt(pow(Vector[1][0],2)+pow(Vector[1][1],2));
    
        if( Vector_Module[0]*Vector_Module[1] != 0)
        {
            AngleVector[0] = acos(Vector_ScalarProduct[0]/(Vector_Module[0]*Vector_Module[1]))*(180/PI);    // 左边线弯点向量夹角
            // cout << AngleVector[0] << "  " << AngleVector[1] << endl;
        }

        // 计算弯点并存储坐标，前提：拐点坐标不再边框上
        if(abs(AngleVector[0]) > (JSON_TrackConfigData.BendPointIdentifyAngle[0]) && abs(AngleVector[0]) < (JSON_TrackConfigData.BendPointIdentifyAngle[1]) && (Data_Path_p -> SideCoordinate_Eight[i][0]) > 30)
        {
            //  cout << abs(AngleVector[0]) << endl;
            (Data_Path_p -> BendPointCoordinate[(Data_Path_p -> BendPointNum[0])][0]) = (Data_Path_p -> SideCoordinate_Eight[i][0]);
            (Data_Path_p -> BendPointCoordinate[(Data_Path_p -> BendPointNum[0])][1]) = (Data_Path_p -> SideCoordinate_Eight[i][1]);
            Data_Path_p -> BendPointNum[0]++;
            i = i+10;
        }
        i++;
    }
    // 右边线弯点
    for(j = (JSON_TrackConfigData.BendPointVectorDistance);j <= (Data_Path_p -> NumSearch[1])-(JSON_TrackConfigData.BendPointVectorDistance);)
    {
        // 右边线第一个向量
        Vector[0][2] = (Data_Path_p -> SideCoordinate_Eight[j-(JSON_TrackConfigData.BendPointVectorDistance)][2])-(Data_Path_p -> SideCoordinate_Eight[j][2]);
        Vector[0][3] = (Data_Path_p -> SideCoordinate_Eight[j-(JSON_TrackConfigData.BendPointVectorDistance)][3])-(Data_Path_p -> SideCoordinate_Eight[j][3]);
        // 右边线第二个向量
        Vector[1][2] = (Data_Path_p -> SideCoordinate_Eight[j+(JSON_TrackConfigData.BendPointVectorDistance)][2])-(Data_Path_p -> SideCoordinate_Eight[j][2]);
        Vector[1][3] = (Data_Path_p -> SideCoordinate_Eight[j+(JSON_TrackConfigData.BendPointVectorDistance)][3])-(Data_Path_p -> SideCoordinate_Eight[j][3]);

        // 计算弯点向量点乘
        Vector_ScalarProduct[1] = Vector[0][2]*Vector[1][2]+Vector[0][3]*Vector[1][3];

        // 计算弯点向量的模
        Vector_Module[2] = sqrt(pow(Vector[0][2],2)+pow(Vector[0][3],2));
        Vector_Module[3] = sqrt(pow(Vector[1][2],2)+pow(Vector[1][3],2));
    
        if( Vector_Module[2]*Vector_Module[3] != 0)
        {
            AngleVector[1] = acos(Vector_ScalarProduct[1]/(Vector_Module[2]*Vector_Module[3]))*(180/PI);    // 右边线弯点向量夹角
            // cout << AngleVector[0] << "  " << AngleVector[1] << endl;
        }

        // 计算弯点并存储坐标，前提：拐点坐标不在边框上
        if(abs(AngleVector[1]) > (JSON_TrackConfigData.BendPointIdentifyAngle[0]) && abs(AngleVector[1]) < (JSON_TrackConfigData.BendPointIdentifyAngle[1]) && 319-(Data_Path_p -> SideCoordinate_Eight[j][2]) > 30)
        {
            // cout << abs(AngleVector[1]) << endl;
            (Data_Path_p -> BendPointCoordinate[(Data_Path_p -> BendPointNum[1])][2]) = (Data_Path_p -> SideCoordinate_Eight[j][2]);
            (Data_Path_p -> BendPointCoordinate[(Data_Path_p -> BendPointNum[1])][3]) = (Data_Path_p -> SideCoordinate_Eight[j][3]);
            Data_Path_p -> BendPointNum[1]++;
            j = j+10;
        }
        j++;
    }
}


/*
    HoughCircleSearch说明
    霍夫圆环识别
*/
void Judge::HoughCircleSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    vector<Vec3f> Circle;
    HoughCircles((Img_Store_p -> Img_OTSU_Unpivot),Circle,HOUGH_GRADIENT,1,100,60,30,40,150);
    for(int i = 0;i < Circle.size();i++)
    {
        circle((Img_Store_p -> Img_Color_Unpivot),Point(Circle[i][0],Circle[i][1]),Circle[i][2],Scalar(255,0,255),2);
    }
}


/*
    Protect_Thread说明
    保护线程
    若检测到有ESC键输入则速度至0
*/
void Judge::Protect_Thread(Data_Path * Data_Path_p)
{
    bool Protect_EN = false;    // 保护使能
    int Stop = 0;
    while(Protect_EN == false)
    {
        cin >> Stop;
        if(Stop != 0)
        {
            Protect_EN = true;
        }
    }
    while(Protect_EN == true)
    {
        Data_Path_p -> MotorSpeed = 0;
    }
}


/*
    ConfigData_SYNC说明
    车辆上位机设置文件数据同步
*/
void SYNC::ConfigData_SYNC(Data_Path *Data_Path_p,Function_EN *Function_EN_p)
{
    Function_EN_p -> JSON_FunctionConfigData_v.clear();
    Data_Path_p -> JSON_TrackConfigData_v.clear();
    JSON_FunctionConfigData JSON_FunctionConfigData;
    JSON_TrackConfigData JSON_TrackConfigData;

    int JSON_FileNum;
    char* ConfigFilePath;

    cout << "<---------------------JSON文件选择--------------------->" << endl;
    cout << "0.低速参数\n1.中速参数\n2.高速参数" << endl;
    cout << "参数选择：";
    cin >> JSON_FileNum;

    switch(JSON_FileNum)
    {
        case 0:{ ConfigFilePath = "../config/config_0.json"; break; }
        case 1:{ ConfigFilePath = "../config/config_1.json"; break; }
        case 2:{ ConfigFilePath = "../config/config_2.json"; break; }
    }
    ifstream ConfigFile(ConfigFilePath);
    nlohmann::json ConfigData = nlohmann::json::parse(ConfigFile);

    JSON_FunctionConfigData.Uart_EN = ConfigData.at("UART_EN");    // 获取串口使能参数
    JSON_FunctionConfigData.ImgCompress_EN = ConfigData.at("IMG_COMPRESS_EN");  // 获取图像压缩使能参数
    JSON_FunctionConfigData.Camera_EN = CameraKind(ConfigData.at("CAMERA_EN"));   // 获取摄像头使能参数
    JSON_FunctionConfigData.ImageSave_EN = ConfigData.at("IMAGE_SAVE_EN");  // 图像存储使能
    JSON_FunctionConfigData.VideoShow_EN = ConfigData.at("VIDEO_SHOW_EN"); // 获取图像显示使能参数
    JSON_FunctionConfigData.DataPrint_EN = ConfigData.at("DATA_PRINT_EN");  // 获取数据显示使能参数
    JSON_FunctionConfigData.AcrossIdentify_EN = ConfigData.at("ACROSS_IDENTIFY_EN");   // 获取十字识别使能参数
    JSON_FunctionConfigData.CircleIdentify_EN = ConfigData.at("CIRCLE_IDENTIFY_EN");   // 获取圆环识别使能参数
    JSON_TrackConfigData.Forward = ConfigData.at("FORWARD"); // 获取前瞻点
    JSON_TrackConfigData.Default_Forward = ConfigData.at("FORWARD"); // 获取默认前瞻点
    JSON_TrackConfigData.Path_Search_Start = ConfigData.at("PATH_SEARCH_START"); // 获取路径循线起始点
    JSON_TrackConfigData.Path_Search_End = ConfigData.at("PATH_SEARCH_END"); // 获取路径循线结束点
    JSON_TrackConfigData.Side_Search_Start = ConfigData.at("SIDE_SEARCH_START");    // 获取边线循线起始点
    JSON_TrackConfigData.Side_Search_End = ConfigData.at("SIDE_SEARCH_END");    // 获取边线循线结束点
    JSON_TrackConfigData.InflectionPointVectorDistance = ConfigData.at("POINT_DISTANCE");  // 获取元素拐点角度区
    JSON_TrackConfigData.BendPointVectorDistance = ConfigData.at("POINT_DISTANCE");  // 获取边线弯点角度区
    JSON_TrackConfigData.BendPointNum[0] = ConfigData.at("LITTLE_ANGLE_BEND_POINT_NUM");    // 获取边线弯点数量
    JSON_TrackConfigData.BendPointNum[1] = ConfigData.at("BIG_ANGLE_BEND_POINT_NUM");    // 获取边线弯点数量
    JSON_TrackConfigData.InflectionPointIdentifyAngle[0] = ConfigData.at("MIN_INFLECTION_POINT_ANGLE");  // 获取元素拐点角度区间
    JSON_TrackConfigData.InflectionPointIdentifyAngle[1] = ConfigData.at("MAX_INFLECTION_POINT_ANGLE"); 
    JSON_TrackConfigData.BendPointIdentifyAngle[0] = ConfigData.at("MIN_BEND_POINT_ANGLE");  // 获取边线弯点角度区间
    JSON_TrackConfigData.BendPointIdentifyAngle[1] = ConfigData.at("MAX_BEND_POINT_ANGLE"); 
    JSON_TrackConfigData.TrackWidth = ConfigData.at("TRACK_WIDTH");   // 获取赛道宽度参数
    JSON_TrackConfigData.CircleOutWidth = ConfigData.at("CIRCLE_OUT_WIDTH");    // 获取圆环出环补线时终点离中线距离
    JSON_TrackConfigData.CommonMotorSpeed[0] = ConfigData.at("STRIGHT_TRACK_MOTOR_SPEED");  // 获取直道电机速度
    JSON_TrackConfigData.CommonMotorSpeed[1] = ConfigData.at("LITTLE_ANGLE_BEND_TRACK_MOTOR_SPEED"); // 小角度弯道电机速度
    JSON_TrackConfigData.CommonMotorSpeed[2] = ConfigData.at("BIG_ANGLE_BEND_TRACK_MOTOR_SPEED"); // 大角度弯道电机速度
    JSON_TrackConfigData.CommonMotorSpeed[3] = ConfigData.at("ACROSS_TRACK_MOTOR_SPEED"); // 十字赛道电机速度
    JSON_TrackConfigData.CommonMotorSpeed[4] = ConfigData.at("CIRCLE_TRACK_MOTOR_SPEED_OUTSIDE"); // 圆环赛道电机速度
    JSON_TrackConfigData.CommonMotorSpeed[5] = ConfigData.at("CIRCLE_TRACK_MOTOR_SPEED_INSIDE"); // 圆环赛道电机速度
    JSON_TrackConfigData.BridgeZoneMotorSpeed = ConfigData.at("BRIDGE_ZONE_MOTOR_SPEED"); // 桥梁区域电机速度
    JSON_TrackConfigData.DangerZoneMotorSpeed = ConfigData.at("DANGER_ZONE_MOTOR_SPEED");   // 危险区域电机速度
    JSON_TrackConfigData.RescueZoneMotorSpeed = ConfigData.at("RESCUE_ZONE_MOTOR_SPEED");   // 救援区域电机速度 
    JSON_TrackConfigData.CrosswalkZoneMotorSpeed = ConfigData.at("CROSSWALK_ZONE_MOTOR_SPEED_STOP_PREPARE"); // 斑马线区域准备停车电机速度
    JSON_TrackConfigData.Circle_In_Prepare_Time = ConfigData.at("CIRCLE_IN_PREPARE_TIME");  // 准备入环限定时间
    JSON_TrackConfigData.DilateErode_Factor[0] = ConfigData.at("DILATE_FACTOR");  // 获取图形学膨胀系数
    JSON_TrackConfigData.DilateErode_Factor[1] = ConfigData.at("ERODE_FACTOR");  // 获取图形学腐蚀系数
    JSON_TrackConfigData.Filter_Factor =ConfigData.at("FILTER_FACTOR"); // 获取路径线滤波强度系数
    JSON_TrackConfigData.DangerTime = ConfigData.at("DANGER_TIME");  // 获取进入危险区域的时间
    JSON_TrackConfigData.BridgeTime = ConfigData.at("BRIDGE_TIME");   // 获取进入桥梁区域的时间
    JSON_TrackConfigData.RescueTime = ConfigData.at("RESCUE_TIME"); // 获取救援区进入车库前准备时间上限
    JSON_TrackConfigData.CrosswalkStopTime = ConfigData.at("CROSSWALK_STOP_TIME");   // 获取进入斑马线区域后停车的时间
    JSON_TrackConfigData.RescueGarageTime = ConfigData.at("RESCUE_GARAGE_TIME");   // 获取救援区域过标志后与开始判断进车库时机的时间间隔 
    JSON_TrackConfigData.RescueZoneConeAvgY = ConfigData.at("RESCUE_ZONE_CONE_AVG_Y"); // 获取救援区域锥桶平均高度阈值
    JSON_TrackConfigData.Crosswalk_Y = ConfigData.at("CROSSWALK_IDENTIFY_Y");   // 获取斑马线识别纵坐标阈值
    JSON_TrackConfigData.Bomb_Y = ConfigData.at("BOMB_IDENTIFY_Y");   // 获取爆炸物识别纵坐标阈值
    JSON_TrackConfigData.Bridge_Y = ConfigData.at("BRIDGE_IDENTIFY_Y");   // 获取桥识别纵坐标阈值
    JSON_TrackConfigData.Rescue_Y = ConfigData.at("RESCUE_IDENTIFY_Y");   // 获取救援区域标识牌识别纵坐标阈值 
    JSON_TrackConfigData.Danger_Zone_Barrier_Y = ConfigData.at("DANGER_ZONE_BARRIER_Y");    // 获取危险区域障碍物准备避障高度
    JSON_TrackConfigData.Danger_Zone_Barrier_Servor_Angle_Factor[0] = ConfigData.at("DANGER_ZONE_CONE_SERVOR_ANGLE_FACTOR");    // 获取危险区域锥桶准备避障舵机角度倍率系数 
    JSON_TrackConfigData.Danger_Zone_Barrier_Servor_Angle_Factor[1] = ConfigData.at("DANGER_ZONE_BLOCK_SERVOR_ANGLE_FACTOR");    // 获取危险区域锥桶准备避障舵机角度倍率系数 
    JSON_TrackConfigData.DangerZone_Cone_Radius = ConfigData.at("DANGER_ZONE_CONE_RADIUS");   // 危险区域锥桶补线圆圈半径
    JSON_TrackConfigData.DangerZone_Block_Radius = ConfigData.at("DANGER_ZONE_BLOCK_RADIUS");   // 危险区域路障补线圆圈半径
    JSON_TrackConfigData.DangerZoneForward = ConfigData.at("DANGER_ZONE_FORWARD");  // 危险区域前瞻值获取
    JSON_TrackConfigData.BridgeZoneForward = ConfigData.at("BRIDGE_ZONE_FORWARD");  // 桥梁区域前瞻值获取

    Function_EN_p -> JSON_FunctionConfigData_v.push_back(JSON_FunctionConfigData);
    Data_Path_p -> JSON_TrackConfigData_v.push_back(JSON_TrackConfigData);

    cout << "<---------------------JSON参数获取成功--------------------->" << endl;
}


/*
    DataPrint说明
    打印数据
    程序参数：1.前瞻点 2.寻边线起始点 3.寻边线结束点 4.边线断点起始点 5.边线断点结束点 6.比赛状态
    运动参数：1.舵机方向 2.舵机角度 3.点击速度
*/
void DataPrint(Data_Path *Data_Path_p,Function_EN *Function_EN_p)
{
    JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    
    cout << "\033c";    // 每次打印前清屏

    if(JSON_FunctionConfigData.DataPrint_EN == true)
    {
        if(JSON_FunctionConfigData.Uart_EN == true)
        {
            cout << "<---------------------比赛模式--------------------->" << endl;
            cout << endl;
        }
        else
        {
            cout << "<---------------------调试模式--------------------->" << endl;
            cout << endl;
        }

        // 打印程序参数
        cout << "<---------------------程序参数--------------------->" << endl;
        cout << " 前瞻点：" << JSON_TrackConfigData.Forward << endl;
        cout << " 路径线起始点：" << JSON_TrackConfigData.Path_Search_Start << endl; 
        cout << " 路径线结束点：" << JSON_TrackConfigData.Path_Search_End << endl; 
        cout << " 边线起始点：" << JSON_TrackConfigData.Side_Search_Start << endl; 
        cout << " 边线结束点：" << JSON_TrackConfigData.Side_Search_End << endl; 
        cout << " 比赛状态：";
        switch(Function_EN_p -> Game_EN)
        {
            case true:{ cout << "开始" << endl; break; }
            case false:{ cout << "结束" << endl; break; }
        }
        cout << "<-------------------------------------------------->" << endl;
        cout << endl;

        // 打印运动参数
        cout << "<---------------------运动参数--------------------->" << endl;
        cout << "舵机方向：";
        cout << Data_Path_p -> ServoDir << endl;
        cout << "舵机角度：";
        cout << Data_Path_p -> ServoAngle << endl;
        cout <<  "电机速度：";
        cout << Data_Path_p -> MotorSpeed << endl;
        cout <<  "赛道类型：";
        switch(Data_Path_p -> Track_Kind)
        {
            case STRIGHT_TRACK:{ cout << "直赛道" << endl; break; }
            case BEND_TRACK:{ cout << "弯赛道" << endl; break; }
            case R_CIRCLE_TRACK_OUTSIDE:
            { 
                cout << "右圆环赛道：准备入环" << endl; 
                break;
            }
            case L_CIRCLE_TRACK_OUTSIDE:
            { 
                cout << "左圆环赛道：准备入环" << endl; 
                break;
            }
            case R_CIRCLE_TRACK_INSIDE:
            { 
                cout << "右圆环赛道："; 
                switch(Data_Path_p -> Circle_Track_Step)
                {
                    case IN:{ cout << "入环" << endl; break; }
                    case OUT_PREPARE:{ cout << "准备出环" << endl; break; }
                    case OUT:{ cout << "出环" << endl; break; }
                    case INIT:{ cout << "初始化" << endl; break; }
                }
                break;
            }
            case L_CIRCLE_TRACK_INSIDE:
            { 
                cout << "左圆环赛道："; 
                switch(Data_Path_p -> Circle_Track_Step)
                {
                    case IN:{ cout << "入环" << endl; break; }
                    case OUT_PREPARE:{ cout << "准备出环" << endl; break; }
                    case OUT:{ cout << "出环" << endl; break; }
                    case INIT:{ cout << "初始化" << endl; break; }
                }
                break;
            }
            case ACROSS_TRACK:{ cout << "十字赛道" << endl; break; }
        }
        cout <<  "控制使能：";
        switch(Function_EN_p -> Control_EN)
        {
            case true:{ cout << "下位机控制" << endl; break; }
            case false:{ cout << "上位机控制" << endl; break; }
        }
        cout << "<-------------------------------------------------->" << endl;
    }
}


