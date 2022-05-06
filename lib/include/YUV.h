#include<stdio.h>
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;
#ifndef _YUV_H_
#define _YUV_H_

typedef struct {
    BYTE Y;
    BYTE U;
    BYTE V;
} YUV;

typedef struct {
    BYTE R;
    BYTE G;
    BYTE B;
} RGB;

typedef struct {
    int Y;
    int U;
    int V;
}YUV_FORMAT;

typedef struct {
    RGB * rgb;
    int height;
    int width;
    int size;
} RGB_IMG;

int save_yuv(YUV * yuv_img, BYTE ** bufp, YUV_FORMAT format, int height, int width);
void RGB2YUV(RGB * rgb_img, YUV * yuv_img, int size);
int resize(RGB ** img, int srcHeight, int srcWidth, int dstHeight, int dstWidth, const char *mode);
#endif