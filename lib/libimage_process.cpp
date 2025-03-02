#include "common_system.h"
#include "common_program.h"

using namespace std;
using namespace cv;

mutex CameraCapture_Mutex;  // 摄像头采集资源互斥锁

/*
	CameraInit说明
	摄像头初始化
*/
void CameraInit(VideoCapture& Camera,CameraKind Camera_EN,int FPS)
{
	// 相机类型设置
    switch(Camera_EN)
    {
        case DEMO_VIDEO:{ Camera.open("../img/sample.mp4",CAP_V4L2); break; }    // 演示视频
        case VIDEO_0:{ Camera.open("/dev/video0",CAP_V4L2); break; }  // 摄像头video0
        case VIDEO_1:{ Camera.open("/dev/video1",CAP_V4L2); break; }  // 摄像头video1
    }

    Camera.set(CAP_PROP_FRAME_WIDTH, 320); //帧宽
    Camera.set(CAP_PROP_FRAME_HEIGHT, 240);//帧高
    Camera.set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));  // 视频流格式
    Camera.set(CAP_PROP_FPS, FPS);    // 帧率
	// Camera.set(CAP_PROP_EXPOSURE, -14);	// 曝光度

	if (!Camera.isOpened())
    {
        cout << "<---------------------相机初始化失败--------------------->" << endl;
        abort();
    }
    else
    {
        cout << "<---------------------相机初始化成功--------------------->" << endl;
    }
}


/*
	摄像头获取图像线程
*/
void CameraImgGetThread(VideoCapture& Camera,Img_Store *Img_Store_p)
{
	Mat Img;
	while(1)
	{
		Camera >> Img;   // 将视频流转为图像流
		CameraCapture_Mutex.lock();
		if(!Img_Store_p -> Img_Capture.empty())
		{
			(Img_Store_p -> Img_Capture).pop();
		}
		(Img_Store_p -> Img_Capture).push(Img);
		CameraCapture_Mutex.unlock();
	}
}


/*
	获取图像
*/
void CameraImgGet(Img_Store *Img_Store_p)
{
	while(Img_Store_p -> Img_Capture.empty());
	CameraCapture_Mutex.lock();
	(Img_Store_p -> Img_Color) = (Img_Store_p -> Img_Capture).front().clone();
	CameraCapture_Mutex.unlock();
}



/*
	ImgPrepare说明
	图像预处理
	先二值化使赛道边缘更为清晰
	然后用sobel算子检测边缘
	最后再次二值化
*/
void ImgProcess::ImgPrepare(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p)
{
	JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    (Img_Store_p -> Img_Track) = (Img_Store_p -> Img_Color).clone();
	cvtColor((Img_Store_p -> Img_Color) , (Img_Store_p -> Img_Gray) , COLOR_BGR2GRAY);  // 彩色图像灰度化
	// blur((Img_Store_p -> Img_Gray) , (Img_Store_p -> Img_Gray) , Size(18,18) , Point(-1,-1));	// 均值滤波	
	threshold((Img_Store_p -> Img_Gray) , (Img_Store_p -> Img_OTSU) , 0 , 255 , THRESH_BINARY | THRESH_OTSU);   //灰度图像二值化
	ImgProcess::ImgSobel((Img_Store_p -> Img_OTSU));	//Sobel算子处理
	threshold((Img_Store_p -> Img_OTSU) , (Img_Store_p -> Img_OTSU) , 0 , 255 , THRESH_BINARY | THRESH_OTSU);  //灰度图像二值化

	// ImgProcess::ImgSharpen((Img_Store_p -> Img_OTSU),5);
	for(int i = 0;i <= JSON_TrackConfigData.DilateErode_Factor[0];i++)
	{
		dilate((Img_Store_p -> Img_OTSU),(Img_Store_p -> Img_OTSU),(Img_Store_p -> Dilate_Kernel));
	}
	for(int i = 0;i <= JSON_TrackConfigData.DilateErode_Factor[0];i++)
	{
		erode((Img_Store_p -> Img_OTSU),(Img_Store_p -> Img_OTSU),(Img_Store_p -> Erode_Kernel));
	}
	// 加白框防止八邻域寻线出错
	line((Img_Store_p -> Img_OTSU),Point(0,0),Point(319,0),Scalar(255),3);
	line((Img_Store_p -> Img_OTSU),Point(319,0),Point(319,239),Scalar(255),3);
	line((Img_Store_p -> Img_OTSU),Point(319,239),Point(0,239),Scalar(255),3);
	line((Img_Store_p -> Img_OTSU),Point(0,239),Point(0,0),Scalar(255),3);
}


/*
	ImgSobel说明
	Sobel算子检测边缘
	将传入的图像用Sobel算子处理
*/
void ImgProcess::ImgSobel(Mat& Img)
{
	Mat ImgX;
	Mat ImgY;

	//对X方向微分
    Sobel(Img,ImgX,CV_16S,1,0,5); 	//x方向差分阶数 y方向差分阶数 核大小  
    convertScaleAbs(ImgX,ImgX);     //可将任意类型的数据转化为CV_8UC1
	//对Y方向微分
	Sobel(Img,ImgY,CV_16S,0,1,5); 	//x方向差分阶数 y方向差分阶数 核大小  
    convertScaleAbs(ImgY,ImgY);     //将任意类型的图像转化为CV_8UC1
    addWeighted(ImgX,0.5,ImgY,0.5,0,Img);	//图像的线性混合
}


/*
	ImgScharr说明
	Scharr算子检测边缘
	将传入的逆透视边缘二值化图像用Scharr算子处理
*/
void ImgProcess::ImgScharr(Mat& Img)
{
	Mat ImgX;
	Mat ImgY;

	//对X方向微分
    Scharr(Img,ImgX,CV_16S,1,0,3); 	//x方向差分阶数 y方向差分阶数 核大小  
    convertScaleAbs(ImgX,ImgX);     //可将任意类型的数据转化为CV_8UC1
	//对Y方向微分
	Scharr(Img,ImgY,CV_16S,1,0,3); 	//x方向差分阶数 y方向差分阶数 核大小  
    convertScaleAbs(ImgY,ImgY);     //将任意类型的图像转化为CV_8UC1
    addWeighted(ImgX,0.5,ImgY,0.5,0,Img);	//图像的线性混合
}


/*
	ImgSharpen说明
	通过原图像和高斯滤波图像进行融合进行图像锐化
*/
void ImgProcess::ImgSharpen(Mat &Img,int blursize = 5)
{
	Mat Img_Gauss;
	GaussianBlur(Img,Img_Gauss,Size(blursize,blursize),3,3);
	addWeighted(Img,2,Img_Gauss,-1,0,Img);
}


/*
	ImgUnpivot说明
	逆透视
*/
void ImgProcess::ImgUnpivot(Mat Img,Mat& Img_Unpivot)
{
    Point2f SrcPoints[] = { 
		Point2f(0,240),
		Point2f(320,240),
		Point2f(120,25),
		Point2f(200,25) };
 
	Point2f DstPoints[] = {
		Point2f(80,240),
		Point2f(240,240),
		Point2f(80,0),
		Point2f(240,0) };
 
	Mat UnpivotMat = getPerspectiveTransform(SrcPoints , DstPoints);

    warpPerspective(Img , Img_Unpivot , UnpivotMat , Size(320,240) , INTER_LINEAR);
}


/*
	ImgSynthesis说明
	将多个图像合成在同一窗口
*/
void ImgProcess::ImgSynthesis(Img_Store *Img_Store_p,Function_EN *Function_EN_p)
{
	JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];

	int ImgAllWidth = (Img_Store_p -> Img_Color).cols;	//宽度
	int ImgAllHeight = (Img_Store_p -> Img_Color).rows; //高度
	Mat ImgAll = Mat(ImgAllHeight+210,ImgAllWidth*3+18,CV_8UC3,Scalar(0,0,0));	//显示全部画面的画布

	//统一图像的类型为8UC3
	cvtColor((Img_Store_p -> Img_OTSU) , (Img_Store_p -> Img_OTSU) ,COLOR_GRAY2RGB);
	// cvtColor((Img_Store_p -> Img_OTSU_Unpivot) , (Img_Store_p -> Img_OTSU_Unpivot) ,COLOR_GRAY2RGB);

	//Rect roi(ImgAllWidth*i,0,ImgAllWidth,ImgAllHeight);  
	//定义一个矩形roi
	//将img_tmp复制到img中roi指定的矩形位置
	//此处简化
    
	(Img_Store_p -> Img_Color).copyTo(ImgAll(Rect(0,0,ImgAllWidth,ImgAllHeight))); 
	(Img_Store_p -> Img_Track).copyTo(ImgAll(Rect(ImgAllWidth+6,0,ImgAllWidth,ImgAllHeight)));  
	(Img_Store_p -> Img_OTSU).copyTo(ImgAll(Rect(ImgAllWidth*2+12,0,ImgAllWidth,ImgAllHeight))); 
	(Img_Store_p -> Img_Text).copyTo(ImgAll(Rect(ImgAllWidth+6,ImgAllHeight+6,ImgAllWidth,200))); 

    (Img_Store_p -> Img_All) = ImgAll;

	if(JSON_FunctionConfigData.VideoShow_EN == true)
	{
		imshow("CAMERA",(Img_Store_p -> Img_All));
	}
}


/*
	ImgSave说明
	以Mat形式传入待存储图像
*/
void ImgProcess::ImgSave(Img_Store *Img_Store_p)
{
	string ImgWritePath = "../img/ImgAll/" + to_string(Img_Store_p -> ImgNum) + ".jpg";
	//建立一个字符串用于存储图片存储路径
	//使用字符串定义图片存储路径
	//必须要加后缀 否则编译会报错

	imwrite(ImgWritePath , (Img_Store_p -> Img_All));
	//存储图片流
}


/*
	ImgForwardLine说明
	前瞻点画线
*/
void ImgProcess::ImgForwardLine(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
	JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];
    line((Img_Store_p -> Img_Track),Point(160,300),Point((Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][0]),(Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][1])),Scalar(255,0,0),3);
	putText((Img_Store_p -> Img_Track),to_string(abs(160-(Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][0]))),Point((Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][0]),(Data_Path_p -> TrackCoordinate[(JSON_TrackConfigData.Forward)-(JSON_TrackConfigData.Path_Search_Start)][1])),FONT_HERSHEY_COMPLEX,0.6,(255,255,255),1);
}


/*
	ImgCompress说明
	图像压缩
	将图像压缩至320*240大小
*/
void ImgProcess::ImgCompress(Mat& Img,bool ImgCompress_EN)
{
	Mat ImgCompress;
	if(ImgCompress_EN == true)
	{
		Size size = Size(320,240);
		resize(Img,ImgCompress,size,0,0,INTER_AREA);
		//将图像压缩为320*240大小
		Img = ImgCompress;
	}
}


/*
	ImgText说明
	赛道类型、圆环步骤、编码器积分标志位、模型区域类型显示
*/
void ImgProcess::ImgText(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p)
{
	int ImgWidth = (Img_Store_p -> Img_Color).cols;	// 宽度
	(Img_Store_p -> Img_Text) = Mat(200,ImgWidth,CV_8UC3,Scalar(0,0,0));	// 显示文字画布

	string TextTrackKind[8] = {"STRIGHT_TRACK","BEND_TRACK","R_CIRCLE_TRACK_OUTSIDE","R_CIRCLE_TRACK_INSIDE","L_CIRCLE_TRACK_OUTSIDE","L_CIRCLE_TRACK_INSIDE","ACROSS_TRACK","MODEL_TRACK"};
	string TextCircleTrackStep[5] = {"IN_PREPARE","IN","OUT_PREPARE","OUT","INIT"};
	string TextGyroscope[2] = {"FALSE","TRUE"};
	string TextModelTrackKind[5] = {"BRIDGE_ZONE","CROSSWALK_ZONE","DANGER_ZONE","RESCUE_ZONE","CHASE_ZONE"};
	string TextControl[2] = {"FALSE","TRUE"};

	putText((Img_Store_p -> Img_Text),TextTrackKind[int(Data_Path_p -> Track_Kind)],Point(5,25),FONT_HERSHEY_COMPLEX,1,(255),2);
	putText((Img_Store_p -> Img_Text),TextCircleTrackStep[int(Data_Path_p -> Circle_Track_Step)],Point(5,65),FONT_HERSHEY_COMPLEX,1,(255),2);
	putText((Img_Store_p -> Img_Text),TextGyroscope[int(Function_EN_p -> Gyroscope_EN)],Point(5,105),FONT_HERSHEY_COMPLEX,1,(255),2);
	putText((Img_Store_p -> Img_Text),TextControl[int(Function_EN_p -> Control_EN)],Point(5,185),FONT_HERSHEY_COMPLEX,1,(255),2);
}


/*
	ImgShow说明
	图像合成显示并保存
*/
void ImgProcess::ImgShow(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p)
{
	JSON_FunctionConfigData JSON_FunctionConfigData = Function_EN_p -> JSON_FunctionConfigData_v[0];
	ImgProcess::ImgInflectionPointDraw(Img_Store_p,Data_Path_p); 
	ImgProcess::ImgBendPointDraw(Img_Store_p,Data_Path_p); 
	ImgProcess::ImgForwardLine(Img_Store_p,Data_Path_p);
	ImgProcess::ImgReferenceLine(Img_Store_p,Data_Path_p);
	ImgProcess::ImgText(Img_Store_p,Data_Path_p,Function_EN_p);
	ImgProcess::ImgSynthesis(Img_Store_p,Function_EN_p);
	if(JSON_FunctionConfigData.ImageSave_EN == true)
	{
		ImgProcess::ImgSave(Img_Store_p);
	}
}


/*
    ImgInflectionPointDraw说明
	图像边线拐点绘制
	限制绘制数目
*/
void ImgProcess::ImgInflectionPointDraw(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
	// 变量设置
	int i = 0;
	int j = 0;
	// 左边线拐点点绘制
	if((Data_Path_p -> InflectionPointNum[0]) >= 1)
	{
		for(i = 0;i <= (Data_Path_p -> InflectionPointNum[0])-1;i++)
		{
			if(i == 0)
			{
				putText((Img_Store_p -> Img_Track),to_string(Data_Path_p -> InflectionPointNum[0]),Point((Data_Path_p -> InflectionPointCoordinate[i][0]),(Data_Path_p -> InflectionPointCoordinate[i][1])),FONT_HERSHEY_COMPLEX,0.6,(255,0,255),1);
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[i][0]),(Data_Path_p -> InflectionPointCoordinate[i][1])),6,Scalar(128,128,0),2);	// 左边线断点画点：浅蓝色
			}
			else
			{
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[i][0]),(Data_Path_p -> InflectionPointCoordinate[i][1])),6,Scalar(255,255,0),2);	// 左边线断点画点：蓝色
			}
		}
	}
	// 右边线拐点点绘制
	if((Data_Path_p -> InflectionPointNum[1]) >= 1)
	{
		for(j = 0;j <= (Data_Path_p -> InflectionPointNum[1])-1;j++)
		{
			if(j == 0)
			{
				putText((Img_Store_p -> Img_Track),to_string(Data_Path_p -> InflectionPointNum[1]),Point((Data_Path_p -> InflectionPointCoordinate[j][2]),(Data_Path_p -> InflectionPointCoordinate[j][3])),FONT_HERSHEY_COMPLEX,0.6,(255,0,255),1);
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[j][2]),(Data_Path_p -> InflectionPointCoordinate[j][3])),6,Scalar(128,128,0),2);	// 右边线断点画点：浅蓝色
			}
			else
			{
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> InflectionPointCoordinate[j][2]),(Data_Path_p -> InflectionPointCoordinate[j][3])),6,Scalar(255,255,0),2);	// 右边线断点画点：蓝色
			}
		}
	}
}


/*
    ImgBendPointDraw说明
	图像边线弯点绘制
	限制绘制数目
*/
void ImgProcess::ImgBendPointDraw(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
	// 变量设置
	int i = 0;
	int j = 0;
	// 左边线弯点点绘制
	if((Data_Path_p -> BendPointNum[0]) >= 1)
	{
		for(i = 0;i <= (Data_Path_p -> BendPointNum[0])-1;i++)
		{
			if(i == 0)
			{
				putText((Img_Store_p -> Img_Track),to_string(Data_Path_p -> BendPointNum[0]),Point((Data_Path_p -> BendPointCoordinate[i][0]),(Data_Path_p -> BendPointCoordinate[i][1])),FONT_HERSHEY_COMPLEX,0.6,(255,0,255),1);
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> BendPointCoordinate[i][0]),(Data_Path_p -> BendPointCoordinate[i][1])),6,Scalar(0,128,128),2);	// 左边线断点画点：浅黄色
			}
			else
			{
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> BendPointCoordinate[i][0]),(Data_Path_p -> BendPointCoordinate[i][1])),6,Scalar(0,255,255),2);	// 左边线断点画点：黄色
			}
		}
	}
	// 右边线弯点点绘制
	if((Data_Path_p -> BendPointNum[1]) >= 1)
	{
		for(j = 0;j <= (Data_Path_p -> BendPointNum[1])-1;j++)
		{
			if(j == 0)
			{
				putText((Img_Store_p -> Img_Track),to_string(Data_Path_p -> BendPointNum[1]),Point((Data_Path_p -> BendPointCoordinate[j][2]),(Data_Path_p -> BendPointCoordinate[j][3])),FONT_HERSHEY_COMPLEX,0.6,(255,0,255),1);
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> BendPointCoordinate[j][2]),(Data_Path_p -> BendPointCoordinate[j][3])),6,Scalar(0,128,128),2);	// 右边线断点画点：浅黄色
			}
			else
			{
				circle((Img_Store_p -> Img_Track),Point((Data_Path_p -> BendPointCoordinate[j][2]),(Data_Path_p -> BendPointCoordinate[j][3])),6,Scalar(0,255,255),2);	// 右边线断点画点：黄色
			}
		}
	}
}


/*
	ImgReferenceLine说明
	图像参考线绘制
	1.边线拐点起始线
	2.边线拐点结束线
	3.中心竖线
*/
void ImgProcess::ImgReferenceLine(Img_Store *Img_Store_p,Data_Path *Data_Path_p)
{
    JSON_TrackConfigData JSON_TrackConfigData = Data_Path_p -> JSON_TrackConfigData_v[0];

	line((Img_Store_p -> Img_Track),Point(0,239-(JSON_TrackConfigData.Path_Search_Start)),Point(319,239-(JSON_TrackConfigData.Path_Search_Start)),Scalar(0,0,255),1);	// 寻路径起始线
	line((Img_Store_p -> Img_Track),Point(0,239-(JSON_TrackConfigData.Path_Search_End)),Point(319,239-(JSON_TrackConfigData.Path_Search_End)),Scalar(0,0,255),1);	// 寻路径结束线
	line((Img_Store_p -> Img_Track),Point(0,239-(JSON_TrackConfigData.Side_Search_Start)),Point(319,239-(JSON_TrackConfigData.Side_Search_Start)),Scalar(255,0,255),1);	// 寻边线起始线
	line((Img_Store_p -> Img_Track),Point(0,239-(JSON_TrackConfigData.Side_Search_End)),Point(319,239-(JSON_TrackConfigData.Side_Search_End)),Scalar(255,0,255),1);	// 寻边线结束线
	line((Img_Store_p -> Img_Track),Point(160,239-(JSON_TrackConfigData.Path_Search_Start)),Point(160,239-(JSON_TrackConfigData.Path_Search_End)),Scalar(0,0,255),1);	// 中心竖线
}


/*
	ImgChannel说明
	图像通道提取
*/
Mat ImgProcess::ImgChannel(Mat Img,RGB_Channel RGB_Channel)
{
	vector<Mat> Channel;
	Mat RGB_Img;
	split(Img,Channel);
	switch(RGB_Channel)
	{
		case R_Channel:{ Channel[0] = Scalar(0); Channel[1] = Scalar(0); break;}
		case G_Channel:{ Channel[0] = Scalar(0); Channel[2] = Scalar(0); break;}
		case B_Channel:{ Channel[1] = Scalar(0); Channel[2] = Scalar(0); break;}
	}

	merge(Channel,RGB_Img);

	return RGB_Img;
}