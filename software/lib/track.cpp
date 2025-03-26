#include "opencv2/opencv.hpp"
#include <cmath>
#include <iostream>
#include "common.h"
#include "track.h"

using namespace cv;
using namespace std;

extern Mat imgColor;
extern Mat imgGray;
extern Mat imgOTSU;

/* X Y */
/* 边线 */ 
int L_Side[IMAGE_HEIGHT*4][2] = {0};
int R_Side[IMAGE_HEIGHT*4][2] = {0};
int L_SidePointNum = 0;
int R_SidePointNum = 0;
/* 路径线 */
int path[SEARCH_START-SEARCH_END+1][2];
/* 拐点 */
int L_InflectionPoint[IMAGE_HEIGHT*2][2] = {0};
int R_InflectionPoint[IMAGE_HEIGHT*2][2] = {0};
int L_InflectionPointNum = 0;
int R_InflectionPointNum = 0;
/* 弯点 */
int L_BendPoint[IMAGE_HEIGHT*2][2] = {0};
int R_BendPoint[IMAGE_HEIGHT*2][2] = {0};
int L_BendPointNum = 0;
int R_BendPointNum = 0;
/* 最长白列X */
int longestWhiteColX = 0;

extern Mat imgColor;
extern Mat imgGray;
extern Mat imgOTSU;

/* 边线提取 */
void sideExtract(void)
{
    // 图像中点
    static int midX = IMAGE_WIDTH/2;
    // 八临域寻线变量设置
    int seedGrowDir[8][4] = {{0,1,0,1},{1,1,-1,1},{1,0,-1,0},{1,-1,-1,-1},{0,-1,0,-1},{-1,-1,1,-1},{-1,0,1,0},{-1,1,1,1}};    // 种子X,Y方向的生长向量：从正下方逆时针 和 从正下方顺时针 
    int growDirIdx = 0;
    // 初始化
    L_SidePointNum = 0;
    R_SidePointNum = 0;
    memset(L_Side, 0, sizeof(L_Side));
    memset(R_Side, 0, sizeof(R_Side));

    // 寻找种子起点
    // 左边线种子
    for(int X = midX;X >= 0;X--)
    {
        if(imgOTSU.at<uchar>(SEARCH_START-1,X) == 0)    // 黑色
        {
            // 存储白色点
            L_Side[0][0] = X+1;
            L_Side[0][1] = SEARCH_START-1;
            trackPoint(L_Side[0][0], L_Side[0][1]);
            break;
        }
        if(X == 0)
        {
            // 存储白色点
            L_Side[0][0] = 0;
            L_Side[0][1] = SEARCH_START-1;
            trackPoint(L_Side[0][0], L_Side[0][1]);
            break;
        }
    }
    // 右边线种子
    for(int X = midX;X <= IMAGE_WIDTH-1;X++)
    {
        if(imgOTSU.at<uchar>(SEARCH_START-1,X) == 0)    // 黑色
        {
            // 存储白色点
            R_Side[0][0] = X-1;
            R_Side[0][1] = SEARCH_START-1;
            trackPoint(R_Side[0][0], R_Side[0][1]);
            break;
        }
        if(X == IMAGE_WIDTH-1)
        {
            // 存储白色点
            R_Side[0][0] = IMAGE_WIDTH-1;
            R_Side[0][1] = SEARCH_START-1;
            trackPoint(R_Side[0][0], R_Side[0][1]);
            break;
        }
    }

    // 更新midX
    midX = (L_Side[0][0] + R_Side[0][0])/2;

    // 爬线
    // 左边线
    while(true)
    {
        // growDirIdx = 0;
        // 点在边线内
        if(L_Side[L_SidePointNum][0] < IMAGE_WIDTH-1 && L_Side[L_SidePointNum][0] > 0)
        {   
            // 是否越界
            if(L_SidePointNum >= IMAGE_HEIGHT*4-2)
                break;
            // 下一个点为黑色，这个点为白色
            if(imgOTSU.at<uchar>(L_Side[L_SidePointNum][1]+seedGrowDir[(growDirIdx+1)%8][1],L_Side[L_SidePointNum][0]+seedGrowDir[(growDirIdx+1)%8][0]) == 0 && 
                imgOTSU.at<uchar>(L_Side[L_SidePointNum][1]+seedGrowDir[growDirIdx][1],L_Side[L_SidePointNum][0]+seedGrowDir[growDirIdx][0]) == 255)
            {
                // 下一个点
                L_Side[L_SidePointNum+1][0] = L_Side[L_SidePointNum][0]+seedGrowDir[growDirIdx][0];
                L_Side[L_SidePointNum+1][1] = L_Side[L_SidePointNum][1]+seedGrowDir[growDirIdx][1];
                trackPoint(L_Side[L_SidePointNum+1][0], L_Side[L_SidePointNum+1][1]);
                L_SidePointNum+=1;
                growDirIdx = 0;
            }
            growDirIdx++;
        }
        // 点在边线上
        else if(growDirIdx <= 4)
        {
            // 是否越界
            if(L_SidePointNum >= IMAGE_HEIGHT*4-2)
                break;
            // 下一个点为黑色，这个点为白色
            if( (growDirIdx <= 3 && imgOTSU.at<uchar>(L_Side[L_SidePointNum][1]+seedGrowDir[(growDirIdx+1)%8][1],L_Side[L_SidePointNum][0]+seedGrowDir[(growDirIdx+1)%8][0]) == 0 && 
                imgOTSU.at<uchar>(L_Side[L_SidePointNum][1]+seedGrowDir[growDirIdx%8][1],L_Side[L_SidePointNum][0]+seedGrowDir[growDirIdx%8][0]) == 255) || 
                (growDirIdx == 4 && imgOTSU.at<uchar>(L_Side[L_SidePointNum][1]+seedGrowDir[growDirIdx%8][1],L_Side[L_SidePointNum][0]+seedGrowDir[growDirIdx%8][0]) == 255))
            {
                // 下一个点
                L_Side[L_SidePointNum+1][0] = L_Side[L_SidePointNum][0]+seedGrowDir[growDirIdx][0];
                L_Side[L_SidePointNum+1][1] = L_Side[L_SidePointNum][1]+seedGrowDir[growDirIdx][1];
                trackPoint(L_Side[L_SidePointNum+1][0], L_Side[L_SidePointNum+1][1]);
                L_SidePointNum+=1;
                growDirIdx = 0;
            }
            growDirIdx++;
        }
        else
            break;
        // 索引是否越界
        if(growDirIdx >= 8)
            break;
        if(L_Side[L_SidePointNum][1] < SEARCH_END)
            break;
        if(L_Side[L_SidePointNum][0] > SIDE_END)
            break;
    }
    growDirIdx = 0;
    // 右边线
    while(true)
    {
        // growDirIdx = 0;
        // 点在边线内
        if(R_Side[R_SidePointNum][0] < IMAGE_WIDTH-1 && R_Side[R_SidePointNum][0] > 0)
        {   
            // 是否越界
            if(R_SidePointNum >= IMAGE_HEIGHT*4-2)
                break;
            // 下一个点为黑色，这个点为白色
            if(imgOTSU.at<uchar>(R_Side[R_SidePointNum][1]+seedGrowDir[(growDirIdx+1)%8][3],R_Side[R_SidePointNum][0]+seedGrowDir[(growDirIdx+1)%8][2]) == 0 && 
                imgOTSU.at<uchar>(R_Side[R_SidePointNum][1]+seedGrowDir[growDirIdx%8][3],R_Side[R_SidePointNum][0]+seedGrowDir[growDirIdx%8][2]) == 255)
            {
                // 下一个点
                R_Side[R_SidePointNum+1][0] = R_Side[R_SidePointNum][0]+seedGrowDir[growDirIdx][2];
                R_Side[R_SidePointNum+1][1] = R_Side[R_SidePointNum][1]+seedGrowDir[growDirIdx][3];
                trackPoint(R_Side[R_SidePointNum+1][0], R_Side[R_SidePointNum+1][1]);
                R_SidePointNum+=1;
                growDirIdx = 0;
            }
            growDirIdx++;
        }
        // 点在边线上
        else if(growDirIdx <= 4)
        {
            // 是否越界
            if(R_SidePointNum >= IMAGE_HEIGHT*4-2)
                break;
            // 下一个点为黑色，这个点为白色
            if( (growDirIdx <= 3 && imgOTSU.at<uchar>(R_Side[R_SidePointNum][1]+seedGrowDir[(growDirIdx+1)%8][3],R_Side[R_SidePointNum][0]+seedGrowDir[(growDirIdx+1)%8][2]) == 0 && 
                imgOTSU.at<uchar>(R_Side[R_SidePointNum][1]+seedGrowDir[growDirIdx%8][3],R_Side[R_SidePointNum][0]+seedGrowDir[growDirIdx%8][2]) == 255) || 
                (growDirIdx == 4 && imgOTSU.at<uchar>(R_Side[R_SidePointNum][1]+seedGrowDir[growDirIdx%8][3],R_Side[R_SidePointNum][0]+seedGrowDir[growDirIdx%8][2]) == 255))
            {
                // 下一个点
                R_Side[R_SidePointNum+1][0] = R_Side[R_SidePointNum][0]+seedGrowDir[growDirIdx][2];
                R_Side[R_SidePointNum+1][1] = R_Side[R_SidePointNum][1]+seedGrowDir[growDirIdx][3];
                trackPoint(R_Side[R_SidePointNum+1][0], R_Side[R_SidePointNum+1][1]);
                R_SidePointNum+=1;
                growDirIdx = 0;
            }
            growDirIdx++;
        }
        else
            break;
        // 索引是否越界
        if(growDirIdx >= 8)
            break;
        if(R_Side[R_SidePointNum][1] < SEARCH_END)
            break;
        if(R_Side[R_SidePointNum][0] < IMAGE_WIDTH-SIDE_END)
            break;
    }
}   

/* 最长白列 */
void longestWhiteCol(void)
{
	int L_side[2] = {0};
	int R_side[2] = {0};
    static int midX = 0;
	
	int maxWhiteNum = 0;				// 最长白列白色像素数量
	int longestWhiteX = 0;			// 最长白列横坐标
	int longestWhiteX_cache = 0;		// 最长白列横坐标缓存
	int maxWhiteColNum = 0;			// 最长白列数量
	int maxWhiteColNumCache = 0;	// 最长白列数量缓存
	
	for(int X = midX;X >= 0;X--)
    {
        if(imgOTSU.at<uchar>(LOONGEST_WHITE_COL_START-1,X) == 0)    // 黑色
        {
            // 存储白色点
            L_side[0] = X+1;
            L_side[1] = LOONGEST_WHITE_COL_START-1;
            break;
        }
        if(X == 0)
        {
            // 存储白色点
            L_side[0] = 0;
            L_side[1] = LOONGEST_WHITE_COL_START-1;
            break;
        }
    }
    // 右边线种子
    for(int X = midX;X <= IMAGE_WIDTH-1;X++)
    {
        if(imgOTSU.at<uchar>(LOONGEST_WHITE_COL_START-1,X) == 0)    // 黑色
        {
            // 存储白色点
            R_side[0] = X-1;
            R_side[1] = LOONGEST_WHITE_COL_START-1;
            break;
        }
        if(X == IMAGE_WIDTH-1)
        {
            // 存储白色点
            R_side[0] = IMAGE_WIDTH-1;
            R_side[1] = LOONGEST_WHITE_COL_START-1;
            break;
        }
    }
	
	// 更新mid_x
    midX = (L_side[0] + R_side[0])/2;
	
	for(int X = L_side[0];X <= R_side[0];X++)
	{
		for(int Y = LOONGEST_WHITE_COL_START;Y >= LOONGEST_WHITE_COL_START-CONTROL_POINT;Y--)
		{
			if(imgOTSU.at<uchar>(Y,X) == 0 || Y == LOONGEST_WHITE_COL_START-CONTROL_POINT)
			{
				if(LOONGEST_WHITE_COL_START-Y > maxWhiteNum)
				{
					maxWhiteNum = LOONGEST_WHITE_COL_START-Y;
					maxWhiteColNumCache = 0;
					maxWhiteColNumCache = 0;
					longestWhiteX_cache +=  X;
					longestWhiteX_cache++;
				}
				else if(LOONGEST_WHITE_COL_START-Y == maxWhiteColNum)
				{
					maxWhiteNum = LOONGEST_WHITE_COL_START-Y;
					longestWhiteX_cache += X;
					longestWhiteX_cache++;
				}
				else
				{
					if(longestWhiteX_cache > maxWhiteColNum)
					{
						maxWhiteColNum = longestWhiteX_cache;
						longestWhiteX = longestWhiteX_cache;
						maxWhiteColNumCache = 0;
						longestWhiteX_cache = 0;
					}	
				}
				break;
			}
		}
	}
	if(longestWhiteX_cache > maxWhiteColNum)
	{
		maxWhiteColNum = maxWhiteColNumCache;
		longestWhiteX = longestWhiteX_cache;
	}	
	if(maxWhiteColNum != 0)
        longestWhiteColX = longestWhiteX/maxWhiteColNum;
	if(longestWhiteColX <= 0)
        longestWhiteColX = 0;
	else if(longestWhiteColX >= IMAGE_WIDTH-1)
        longestWhiteColX = IMAGE_WIDTH-1;
}

/* 路径线提取 */
void pathExtract(void)
{
    int x,y;
	static int num = 0;
	if(num < 10)
	{
		path[0][0] = IMAGE_WIDTH/2;
		num++;
	}
	else
	{
		path[0][0] = path[PREDICTION_POINT][0];
	}
	path[0][1] = SEARCH_START;
	for(y = SEARCH_START-1;y >= SEARCH_END;y--)
	{
		for(x = path[SEARCH_START-y-1][0];x < IMAGE_WIDTH;x++)
		{
			if(imgOTSU.at<uchar>(y,x) == 0)
			{
				path[SEARCH_START-y][0] = x;
				break;
			}
			if(x == IMAGE_WIDTH-1)
			{
				path[SEARCH_START-y][0] = x;
			}
		}
		for(x = path[SEARCH_START-y-1][0];x >= 0;x--)
		{
			if(imgOTSU.at<uchar>(y,x) == 0)
			{
				path[SEARCH_START-y][0] += x;
				break;
			}
			if(x == 0)
			{
				path[SEARCH_START-y][0] += x;
			}
		}
		path[SEARCH_START-y][0] = path[SEARCH_START-y][0]/2;
		path[SEARCH_START-y][1] = y;
        trackPoint(path[SEARCH_START-y][0], path[SEARCH_START-y][1]);
	}
}

/* 点类型判断 */
void pointKindJudge(void)
{
    int i;
    int vector[2][4] = {0}; // 左右拐点与上下两点构成的向量坐标
    int vectorScalarProduct[2] = {0};  // 左右拐点向量点乘
    double vectorModule[4] = {0};   // 左右拐点向量的模
    double vectorAngle[2] = {0}; // 左右拐点向量夹角(角度制)

    // 初始化
    L_InflectionPointNum = 0;
    R_InflectionPointNum = 0;
    memset(L_InflectionPoint, 0, sizeof(L_InflectionPoint));
    memset(R_InflectionPoint, 0, sizeof(R_InflectionPoint));
    L_BendPointNum = 0;
    R_BendPointNum = 0;
    memset(L_BendPoint, 0, sizeof(L_BendPoint));
    memset(R_BendPoint, 0, sizeof(R_BendPoint));

    // 寻拐点范围
    // 左边线拐点
    for(i = INFLECTION_POINT_DISTANCE;i <= L_SidePointNum-1-INFLECTION_POINT_DISTANCE;)
    {
        // 左边线第一个向量
        vector[0][0] = L_Side[i-INFLECTION_POINT_DISTANCE][0]-L_Side[i][0];
        vector[0][1] = L_Side[i-INFLECTION_POINT_DISTANCE][1]-L_Side[i][1];
        // 左边线第二个向量
        vector[1][0] = L_Side[i+INFLECTION_POINT_DISTANCE][0]-L_Side[i][0];
        vector[1][1] = L_Side[i+INFLECTION_POINT_DISTANCE][1]-L_Side[i][1];

        // 计算中断点向量点乘
        vectorScalarProduct[0] = vector[0][0]*vector[1][0]+vector[0][1]*vector[1][1];

        // 计算拐点向量的模
        vectorModule[0] = sqrt(pow(vector[0][0],2)+pow(vector[0][1],2));
        vectorModule[1] = sqrt(pow(vector[1][0],2)+pow(vector[1][1],2));
    
        if( vectorModule[0]*vectorModule[1] != 0)
        {
            vectorAngle[0] = acos(vectorScalarProduct[0]/(vectorModule[0]*vectorModule[1]))*(180/PI);    // 左边线断点向量夹角
        }

        // 计算拐点并存储坐标，前提：拐点坐标不再边框上
        if(abs(vectorAngle[0]) > INFLECTION_POINT_ANGLE_MIN && abs(vectorAngle[0]) < INFLECTION_POINT_ANGLE_MAX && L_Side[i][0] > 11 && (vector[0][0]+vector[1][0]) < 5)
        {
            //  cout << abs(AngleVector[0]) << endl;
            L_InflectionPoint[L_InflectionPointNum][0] = L_Side[i][0];
            L_InflectionPoint[L_InflectionPointNum][1] = L_Side[i][1];
            trackInflectionPoint(L_Side[i][0], L_Side[i][1]);
            if(L_InflectionPointNum < IMAGE_HEIGHT*2-1)
                L_InflectionPointNum++;
            else
                break;
            if(i < L_SidePointNum-2-INFLECTION_POINT_DISTANCE-SKIP_POINT_DISTANCE)
                i = i+SKIP_POINT_DISTANCE;
            else
                break;
        }
        else if(abs(vectorAngle[0]) > BEND_POINT_ANGLE_MIN && abs(vectorAngle[0]) < BEND_POINT_ANGLE_MAX)
        {
            //  cout << abs(AngleVector[0]) << endl;
            L_BendPoint[L_BendPointNum][0] = L_Side[i][0];
            L_BendPoint[L_BendPointNum][1] = L_Side[i][1];
            trackBendPoint(L_Side[i][0], L_Side[i][1]);
            if(L_BendPointNum < IMAGE_HEIGHT*2-1)
                L_BendPointNum++;
            else
                break;
        }
        i++;
    }
    // 右边线拐点
    for(i = INFLECTION_POINT_DISTANCE;i <= R_SidePointNum-1-INFLECTION_POINT_DISTANCE;)
    {
        // 左边线第一个向量
        vector[0][2] = R_Side[i-INFLECTION_POINT_DISTANCE][0]-R_Side[i][0];
        vector[0][3] = R_Side[i-INFLECTION_POINT_DISTANCE][1]-R_Side[i][1];
        // 左边线第二个向量
        vector[1][2] = R_Side[i+INFLECTION_POINT_DISTANCE][0]-R_Side[i][0];
        vector[1][3] = R_Side[i+INFLECTION_POINT_DISTANCE][1]-R_Side[i][1];

        // 计算中断点向量点乘
        vectorScalarProduct[1] = vector[0][2]*vector[1][2]+vector[0][3]*vector[1][3];

        // 计算拐点向量的模
        vectorModule[2] = sqrt(pow(vector[0][2],2)+pow(vector[0][3],2));
        vectorModule[3] = sqrt(pow(vector[1][2],2)+pow(vector[1][3],2));
    
        if( vectorModule[2]*vectorModule[3] != 0)
        {
            vectorAngle[1] = acos(vectorScalarProduct[1]/(vectorModule[2]*vectorModule[3]))*(180/PI);    // 左边线断点向量夹角
        }

        // 计算拐点并存储坐标，前提：拐点坐标不再边框上
        if(abs(vectorAngle[1]) > INFLECTION_POINT_ANGLE_MIN && abs(vectorAngle[1]) < INFLECTION_POINT_ANGLE_MAX && R_Side[i][0] < IMAGE_WIDTH-11 && (vector[0][2]+vector[1][2]) > 0)
        {
            //  cout << abs(AngleVector[0]) << endl;
            R_InflectionPoint[R_InflectionPointNum][0] = R_Side[i][0];
            R_InflectionPoint[R_InflectionPointNum][1] = R_Side[i][1];
            trackInflectionPoint(R_Side[i][0], R_Side[i][1]);
            if(R_InflectionPointNum < IMAGE_HEIGHT*2-1)
                R_InflectionPointNum++;
            else
                break;
            if(i < R_SidePointNum-2-INFLECTION_POINT_DISTANCE-SKIP_POINT_DISTANCE)
                i = i+SKIP_POINT_DISTANCE;
            else
                break;
        }
        else if(abs(vectorAngle[1]) > BEND_POINT_ANGLE_MIN && abs(vectorAngle[1]) < BEND_POINT_ANGLE_MAX)
        {
            //  cout << abs(AngleVector[0]) << endl;
            R_BendPoint[R_BendPointNum][0] = R_Side[i][0];
            R_BendPoint[R_BendPointNum][1] = R_Side[i][1];
            trackBendPoint(R_Side[i][0], R_Side[i][1]);
            if(R_BendPointNum < IMAGE_HEIGHT*2-1)
                R_BendPointNum++;
            else
                break;
        }
        i++;
    }
}

void trackPoint(int X, int Y)
{
    circle(imgColor,Point(X,Y),1,Scalar(0,0,255),1);	// 边线画点
}

void trackInflectionPoint(int X, int Y)
{
    circle(imgColor,Point(X,Y),1,Scalar(0,255,255),5);	// 边线画点
}

void trackBendPoint(int X, int Y)
{
    circle(imgColor,Point(X,Y),1,Scalar(255,0,0),1);	// 边线画点
}

void trackLine(int X_0, int Y_0, int X_1, int Y_1)
{
    line(imgColor,Point(X_0,Y_0),Point(X_1,Y_1),Scalar(0,255,255),2);
    line(imgOTSU,Point(X_0,Y_0),Point(X_1,Y_1),Scalar(0),2);
}    