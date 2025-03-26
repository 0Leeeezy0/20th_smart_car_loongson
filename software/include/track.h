#ifndef _TRACK_H_
#define _TRACK_H_

/* 边线提取 */
void sideExtract(void);

/* 路径线提取 */
void pathExtract(void);

/* 最长白列 */
void longestWhiteCol(void);

/* 点类型判断 */
void pointKindJudge(void);

void trackPoint(int X, int Y);
void trackInflectionPoint(int X, int Y);
void trackBendPoint(int X, int Y);
void trackLine(int X_0, int Y_0, int X_1, int Y_1);

#endif