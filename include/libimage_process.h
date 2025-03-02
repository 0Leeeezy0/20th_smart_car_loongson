#include "common_system.h"
#include "common_program.h"

#ifndef _LIBIMAGE_PROCESS_H_
#define _LIBIMAGE_PROCESS_H_

/*
    CameraInit说明
    摄像头初始化
    @参数说明
    Camera 传入VideoCapture类
    Camera_EN 相机使能
    FPS 摄像头帧率
*/
void CameraInit(cv::VideoCapture& Camera,CameraKind Camera_EN,int FPS);


/*
    摄像头图像采集(多线程)
    @参数说明
    Camera 传入VideoCapture类
    Img_Store_p 图像存储结构体指针
*/
void CameraImgGetThread(cv::VideoCapture& Camera,Img_Store *Img_Store_p);


/*
	获取图像
    @参数说明
    Img_Store_p 图像存储结构体指针
*/
void CameraImgGet(Img_Store *Img_Store_p);


class ImgProcess
{
    public:

        /*
            图像预处理
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void ImgPrepare(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p);


        /*
            图像压缩
            将图像压缩至320*240大小
            @参数说明
            Img 引用待压缩图像
            ImgCompress_EN 图像压缩使能
        */
        void ImgCompress(cv::Mat& Img,bool ImgCompress_EN);


        /*
            图像合成显示并保存
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
            Function_EN_p 函数使能指针
        */
        void ImgShow(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p);
    

    private:

        /*
            图像边线拐点绘制
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关指针
        */
        void ImgInflectionPointDraw(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


        /*
            图像边线弯点绘制
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关指针
        */
        void ImgBendPointDraw(Img_Store *Img_Store_p,Data_Path *Data_Path_p);
        

        /*
            通过原图像和高斯滤波图像进行融合进行图像锐化
            @参数说明
            Img 引用待锐化图像
            blursize 传入高斯滤波内核大小
        */
        void ImgSharpen(cv::Mat &Img,int blursize);


        /*
            赛道类型、圆环步骤显示
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
            Function_EN_p 函数使能指针
        */
        void ImgText(Img_Store *Img_Store_p,Data_Path *Data_Path_p,Function_EN *Function_EN_p);


        /*
            前瞻点画线
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
            @注意
            该函数使用前必须使用 UartSendReceiveSync() 函数同步串口接收发送协议
        */
        void ImgForwardLine(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


        /*
            将多个图像合成在同一窗口
            @参数说明
            Img_Store_p 图像存储指针
            Function_EN_p 函数使能指针
        */
        void ImgSynthesis(Img_Store *Img_Store_p,Function_EN *Function_EN_p);


        /*
            存储图像
            @参数说明
            Img_Store_p 图像存储指针
        */
        void ImgSave(Img_Store *Img_Store_p);


        /*
            Sobel算子检测边缘
            @参数说明
            Img 引用待检测图像
        */
        void ImgSobel(cv::Mat& Img);


        /*
            ImgScharr算子检测边缘
            @参数说明
            Img 引用待检测图像
        */
        void ImgScharr(cv::Mat& Img);


        /*
            逆透视
            @参数说明
            Img 原图像
            Img_Unpivot 引用逆透视图像
        */
        void ImgUnpivot(cv::Mat Img,cv::Mat& Img_Unpivot);


        /*
            图像参考线绘制
            1.边线断点起始线
            2.边线断点结束线
            3.中心竖线
            @参数说明
            Img_Store_p 图像存储指针
            Data_Path_p 路径相关数据指针
        */
        void ImgReferenceLine(Img_Store *Img_Store_p,Data_Path *Data_Path_p);


        /*
            图像通道提取
            @参数说明
            Img 传入需提取通道的图像
            RGB_Channel 传入需提取的通道
        */
        cv::Mat ImgChannel(cv::Mat Img,RGB_Channel RGB_Channel);
};

#endif
