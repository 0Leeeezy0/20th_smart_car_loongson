#include "common_system.h"
#include "libdata_store.h"

#ifndef _PATH_H_
#define _PATH_H_

/*
    寻路径线存坐标
    @参数说明
    Img_Store_p 图像存储指针
    Data_Path_p 路径数据指针
*/
void ImgPathSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


/*
    寻边线存坐标
    八临域寻线
    使用前必须使用 ImgPathSearch()
    对赛道寻边线处理以此提供寻找中断点的边线坐标
    @参数说明
    Img_Store_p 图像存储指针
    Data_Path_p 路径数据指针
*/
void ImgSideSearch(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


/*
    十字赛道
    @参数说明
    Img_Store_p 图像存储指针
    Data_Path_p 路径数据指针
*/
void AcrossTrack(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


// 圆环准备入环步骤：补线
void CircleTrack_Step_IN_Prepare(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

// 圆环准备入环步骤：补线
void CircleTrack_Step_IN_Prepare_Stright(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

// 圆环入环步骤：补线
void CircleTrack_Step_IN(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

// 圆环出环步骤：补线
void CircleTrack_Step_OUT(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

// 圆环出环后直线补线
void Circle2CommonTrack(Img_Store *Img_Store_p,Data_Path *Data_Path_p);

#endif