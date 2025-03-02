#include "common_system.h"
#include "common_program.h"

using namespace std;
using namespace cv;

/*
    ImgPathSearch说明
    对赛道进行寻边线处理以此寻找路径线
    将边线坐标存储至 Data_Path_p -> SideCoordinate 中
    将路径线坐标存储至 Data_Path_p -> TrackCoordinate 中
*/
void ImgPathSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    // 变量设置
    //————————————————————————————————————————————————————————————————————————————————————//
    // 边线坐标
    int X[2] = {0};
    int Y = 0;

    // 设置路径起始坐标
    Data_Path_p -> TrackCoordinate[0][0] = ((Data_Path_p -> SideCoordinate_Eight[JSON_TrackConfigData.Path_Search_Start-JSON_TrackConfigData.Side_Search_Start+1][0])+(Data_Path_p -> SideCoordinate_Eight[JSON_TrackConfigData.Path_Search_Start-JSON_TrackConfigData.Side_Search_Start+1][2]))/2;
    Data_Path_p -> TrackCoordinate[0][1] = 239-(JSON_TrackConfigData.Path_Search_Start);

    int NumSearch = 0;  // 坐标数组的行序号
    //————————————————————————————————————————————————————————————————————————————————————//

    // 寻线
    //————————————————————————————————————————————————————————————————————————————————————//
    if(Img_Store_p -> ImgNum <= 5)
    {

    }
    for(Y = 239-(JSON_TrackConfigData.Path_Search_Start);Y >= 239-(JSON_TrackConfigData.Path_Search_End);Y--)
    {
        // 左边线
        for(X[0] = (Data_Path_p -> TrackCoordinate[NumSearch][0]);X[0] >= 0;X[0]--)
        {
            if((Img_Store_p -> Img_OTSU).at<uchar>(Y,X[0]) == 255)
            {
                (Data_Path_p -> SideCoordinate[NumSearch][0]) = X[0];
                (Data_Path_p -> SideCoordinate[NumSearch][1]) = Y;

                break;
            }
        }
        // 右边线
        for(X[1] = (Data_Path_p -> TrackCoordinate[NumSearch][0]);X[1] <= 319;X[1]++)
        {
            if((Img_Store_p -> Img_OTSU).at<uchar>(Y,X[1]) == 255)
            {
                (Data_Path_p -> SideCoordinate[NumSearch][2]) = X[1];
                (Data_Path_p -> SideCoordinate[NumSearch][3]) = Y;

                break;
            }
        }
        if(NumSearch != 0)
        {
            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate[NumSearch][0]),(Data_Path_p -> SideCoordinate[NumSearch][1])),1,Scalar(0,0,255),1);	// 左边线画点
            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate[NumSearch][2]),(Data_Path_p -> SideCoordinate[NumSearch][3])),1,Scalar(0,0,255),1);	// 右边线画点
        }
        else
        {
            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate[NumSearch][0]),(Data_Path_p -> SideCoordinate[NumSearch][1])),6,Scalar(0,0,255),2);	// 左边线起点画点
            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate[NumSearch][2]),(Data_Path_p -> SideCoordinate[NumSearch][3])),6,Scalar(0,0,255),2);	// 右边线起点画点
        }

        // 寻边线提前结束条件：1.左右边线间距小于20 2.左右边线位置反了
        if(abs((Data_Path_p -> SideCoordinate[NumSearch][0])-(Data_Path_p -> SideCoordinate[NumSearch][2])) <= 20 || ((Data_Path_p -> SideCoordinate[NumSearch][0]) >= (Data_Path_p -> SideCoordinate[NumSearch][2])))
        {
            NumSearch--;
            JSON_TrackConfigData.Forward = Data_Path_p -> SideCoordinate[NumSearch][1];            
            break;
        }

        (Data_Path_p -> TrackCoordinate[NumSearch][1]) = Y;
        (Data_Path_p -> TrackCoordinate[NumSearch+1][0]) = int(JSON_TrackConfigData.Filter_Factor*(((Data_Path_p -> SideCoordinate[NumSearch][0])+(Data_Path_p -> SideCoordinate[NumSearch][2]))/2)+(1-JSON_TrackConfigData.Filter_Factor)*(Data_Path_p -> TrackCoordinate[NumSearch][0]));    // 路径点预测(均值滤波)        circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> TrackCoordinate[NumSearch][0]),Y),1,Scalar(0,255,0),1);	// 路径画点
        NumSearch++;
    }
    //————————————————————————————————————————————————————————————————————————————————————//
}


/*
    ImgSideSearch说明
    八临域寻线(左右)
    使用前必须使用 ImgPathSearch()
    对赛道寻边线处理以此提供寻找中断点的边线坐标
    将边线坐标存储至 Data_Path_p -> SideCoordinate 中
*/
void ImgSideSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    // 变量设置
    //————————————————————————————————————————————————————————————————————————————————————//
    // 寻种子变量设置
    // 边线坐标
    int X = 0;
    int Y = 0;
    // 设置种子寻找起始点横坐标
    static int StartX = 160;

    // 八临域寻线变量设置
    int SeedGrow_Dir[16][4] = { {0,1,0,1} , {-1,1,1,1} , {-1,0,1,0} , {-1,-1,1,-1} , {0,-1,0,-1} , {1,-1,-1,-1} , {1,0,-1,0} , {1,1,-1,1} ,
                                {0,1,0,1} , {-1,1,1,1} , {-1,0,1,0} , {-1,-1,1,-1} , {0,-1,0,-1} , {1,-1,-1,-1} , {1,0,-1,0} , {1,1,-1,1}};    // 种子X,Y方向的生长向量：从正下方顺时针 和 从正下方逆时针 
    int Dir_Num = 0;
    int Dir_Num_Store = 0;

    int NumSearch[2] = {0};  // 坐标数组的行序号
    //————————————————————————————————————————————————————————————————————————————————————//

    // 八邻域寻边线
    //————————————————————————————————————————————————————————————————————————————————————//
    // 确定种子寻找起始点
    // 前5帧默认为160开始向左右寻找
    // 之后所有帧的起始点由上一帧的中点决定
    if(Img_Store_p -> ImgNum > 5)
    {
        StartX = ((Data_Path_p -> SideCoordinate_Eight[0][0])+(Data_Path_p -> SideCoordinate_Eight[0][2]))/2;
    }
    // 八邻域种子寻找
    if(NumSearch[0] <= 1 && NumSearch[1] <= 1)
    {
        for(Y = 239-(JSON_TrackConfigData.Side_Search_Start);Y >= 238-(JSON_TrackConfigData.Side_Search_Start);Y--)
        {
            // 左边线
            for(X = StartX;X >= 0;X--)
            {
                if((Img_Store_p -> Img_OTSU).at<uchar>(Y,X) == 255)
                {
                    // cout << "L_SIDE" << endl;
                    (Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0]) = X;
                    (Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1]) = Y;

                    break;
                }
            }
            // 右边线
            for(X = StartX;X <= 319;X++)
            {
                if((Img_Store_p -> Img_OTSU).at<uchar>(Y,X) == 255)
                {
                    // cout << "R_SIDE" << endl;
                    (Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2]) = X;
                    (Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3]) = Y;

                    break;
                }
            }

            if(NumSearch[0] == 0 && NumSearch[1] == 0)
            {
                circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1])),6,Scalar(255,0,255),2);	//左边线起点画点
                circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3])),6,Scalar(255,0,255),2);	//右边线起点画点
            }
            else
            {
                circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0]),(Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1])),1,Scalar(255,0,255),1);	//左边线画点
                circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2]),(Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3])),1,Scalar(255,0,255),1);	//右边线画点

            }
           
            NumSearch[0]++;
            NumSearch[1]++;
        }
    }

    // 八邻域寻线
    if(NumSearch[0] >= 2 && NumSearch[0] >= 2)
    {
        // 左边线寻线循环
        while(true)
        {
            // 左边线
            for(Dir_Num = Dir_Num_Store;Dir_Num <= Dir_Num_Store+7;Dir_Num++)
            {
                if((Img_Store_p -> Img_OTSU).at<uchar>((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-1][1])+SeedGrow_Dir[Dir_Num][1],(Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-1][0])+SeedGrow_Dir[Dir_Num][0]) == 0)
                {
                    if(Dir_Num-1 >= 0){Dir_Num = Dir_Num;}
                    else{Dir_Num = Dir_Num+8;}

                    Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0] = Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-1][0]+SeedGrow_Dir[Dir_Num-1][0];
                    Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1] = Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-1][1]+SeedGrow_Dir[Dir_Num-1][1];

                    // 开发调试显示
                    // cout << Data_Path_p -> SideCoordinate_Eight[NumSearch-1][0] << "  " << SeedGrow_Dir[Dir_Num-1][0] << "  " << Data_Path_p -> SideCoordinate_Eight[NumSearch-1][1] << "  " << SeedGrow_Dir[Dir_Num-1][1] << endl;
                    // cout << Data_Path_p -> SideCoordinate_Eight[NumSearch][0] << "  " << Data_Path_p -> SideCoordinate_Eight[NumSearch][1] << endl;
                    // cout << NumSearch << endl;
                    // cout << Dir_Num-1 << endl;

                    // 下次种子生长向量起始序号
                    if(Dir_Num >= 3){Dir_Num_Store = Dir_Num-3;}
                    if(Dir_Num <= 2){Dir_Num_Store = Dir_Num+5;}
                    // 防止种子生长向量越界
                    if(Dir_Num >= 11){Dir_Num_Store = Dir_Num-11;}    // *_Dir_Num-3-8
                    if(Dir_Num <= 10 && Dir_Num >= 8){Dir_Num_Store = Dir_Num-3;}   // *_Dir_Num++5-8                    

                    break;
                }
            }

            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0]),(Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1])),1,Scalar(255,0,255),1);	//左边线画点
            
            // 循环退出条件：1.寻线到寻线结束点和起始点 2.寻线折返 3.寻线到中心线 4.坐标数量大于阈值
            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1]) <= 239-(JSON_TrackConfigData.Side_Search_End) || (Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1]) >= 239-(JSON_TrackConfigData.Side_Search_Start))
            {
                break;
            } 

            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][1]) == (Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-20][1]))
            {
                if(abs((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0]) - (Data_Path_p -> SideCoordinate_Eight[NumSearch[0]-20][0])) <= 10)
                {
                    break;
                }
            }

            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[0]][0]) > 239)
            {
                break;
            } 

            NumSearch[0]++;
            Data_Path_p -> NumSearch[0] = NumSearch[0]-1;

            if(NumSearch[0] >= 500)
            {
                break;
            }
        }
        // 右边线寻线循环
        while(true)
        {
            // 左边线
            for(Dir_Num = Dir_Num_Store;Dir_Num <= Dir_Num_Store+7;Dir_Num++)
            {
                if((Img_Store_p -> Img_OTSU).at<uchar>((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-1][3])+SeedGrow_Dir[Dir_Num][3],(Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-1][2])+SeedGrow_Dir[Dir_Num][2]) == 0)
                {
                    if(Dir_Num-1 >= 0){Dir_Num = Dir_Num;}
                    else{Dir_Num = Dir_Num+8;}

                    Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2] = Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-1][2]+SeedGrow_Dir[Dir_Num-1][2];
                    Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3] = Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-1][3]+SeedGrow_Dir[Dir_Num-1][3];

                    // 开发调试显示
                    // cout << Data_Path_p -> SideCoordinate_Eight[NumSearch-1][2] << "  " << SeedGrow_Dir[Dir_Num-1][2] << "  " << Data_Path_p -> SideCoordinate_Eight[NumSearch-1][3] << "  " << SeedGrow_Dir[Dir_Num-1][3] << endl;
                    // cout << Data_Path_p -> SideCoordinate_Eight[NumSearch][2] << "  " << Data_Path_p -> SideCoordinate_Eight[NumSearch][3] << endl;
                    // cout << NumSearch << endl;
                    // cout << Dir_Num-1 << endl;

                    // 下次种子生长向量起始序号
                    if(Dir_Num >= 3){Dir_Num_Store = Dir_Num-3;}
                    if(Dir_Num <= 2){Dir_Num_Store = Dir_Num+5;}
                    // 防止种子生长向量越界
                    if(Dir_Num >= 11){Dir_Num_Store = Dir_Num-11;}    // *_Dir_Num-3-8
                    if(Dir_Num <= 10 && Dir_Num >= 8){Dir_Num_Store = Dir_Num-3;}   // *_Dir_Num++5-8                    

                    break;
                }
            }

            circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2]),(Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3])),1,Scalar(255,0,255),1);	//右边线画点
            
            // 循环退出条件：1.寻线到寻线结束点和起始点 2.寻线折返 3.寻线到中心线 4.坐标数量大于阈值
            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3]) <= 239-(JSON_TrackConfigData.Side_Search_End) || (Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3]) >= 239-(JSON_TrackConfigData.Side_Search_Start))
            {
                break;
            } 

            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][3]) == (Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-20][3]))
            {
                if(abs((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2]) - (Data_Path_p -> SideCoordinate_Eight[NumSearch[1]-20][2])) <= 10)
                {
                    break;
                }
            }

            if((Data_Path_p -> SideCoordinate_Eight[NumSearch[1]][2]) < 80)
            {
                break;
            } 

            NumSearch[1]++;
            Data_Path_p -> NumSearch[1] = NumSearch[1]-1;

            if(NumSearch[1] >= 500)
            {
                break;
            }
        }
    }
    //————————————————————————————————————————————————————————————————————————————————————//
}


