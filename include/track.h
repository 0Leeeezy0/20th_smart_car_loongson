#ifndef _TRACK_H_
#define _TRACK_H_

void sideExtract(void);

void pathExtract(void);

void pointKindJudge(void);

void trackPoint(int X, int Y);

void trackInflectionPoint(int X, int Y);

void trackBendPoint(int X, int Y);

void trackLine(int X_0, int Y_0, int X_1, int Y_1);

#endif