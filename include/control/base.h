#include "opencv2/opencv.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

using namespace cv;
using namespace std;



#define CAMERA_H  60
#define CAMERA_W  80

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned long uint32;
typedef signed short int int16; 
typedef long int32;

 extern int szr;

#define uchar unsigned char
#define uint unsigned int

