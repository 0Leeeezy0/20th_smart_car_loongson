#include "common_system.h"
#include "common_program.h"

using namespace std;
using namespace cv;

/*
    AcrossTrack说明
    十字赛道
*/
void AcrossTrack(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    ImgProcess ImgProcess;

	// 赛道二值化图像 赛道彩色图像
	if(Data_Path_p -> InflectionPointNum[0]-1 >= 2)
	{
		// 左边线中断点补线绘制：十字四点均存在
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[0][0]),(Data_Path_p -> InflectionPointCoordinate[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(128,0,128),4);
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> InflectionPointCoordinate[0][0]),(Data_Path_p -> InflectionPointCoordinate[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(255),4);	
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(255),4);
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(128,0,128),4);
	}
	else
	{
		// 左边线中断点补线绘制：十字只存在上两点
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(255),4);
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][0]),(Data_Path_p -> SideCoordinate_Eight[0][1])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][0]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[0]-1][1])),Scalar(128,0,128),4);
	}
    if(Data_Path_p -> InflectionPointNum[1]-1 >= 2)
	{
		// 右边线中断点补线绘制：十字四点均存在
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> InflectionPointCoordinate[0][2]),(Data_Path_p -> InflectionPointCoordinate[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(255),4);
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[0][2]),(Data_Path_p -> InflectionPointCoordinate[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(128,0,128),4);
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(255),4);
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(128,0,128),4);	
	}
	else
	{
		// 右边线中断点补线绘制：十字只存在上两点
		line((Img_Store_p -> Img_OTSU),Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(255),4);
		line((Img_Store_p -> Img_Track),Point((Data_Path_p -> SideCoordinate_Eight[0][2]),(Data_Path_p -> SideCoordinate_Eight[0][3])),Point((Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][2]),(Data_Path_p -> InflectionPointCoordinate[Data_Path_p -> InflectionPointNum[1]-1][3])),Scalar(128,0,128),4);	
	}
}