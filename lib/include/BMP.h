#include <stdio.h>
#include "YUV.h"

#define ROTATE 0

typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct 
{
    DWORD FileSize;
    DWORD _reserved;
    DWORD DataOffset;
} BITMAPFILEHEADER;

typedef struct
{
    DWORD Size;
    int Width;
    int Height;
    WORD Planes;
    WORD BitCount;
    DWORD Compression;
    DWORD ImageSize;
    DWORD XpixelsPerM;
    DWORD YpixelsPerM;
    DWORD ColorsUsed;
    DWORD ColorsImaportant;
} BITMAPINFOHEADER;

typedef struct
{
    BYTE Red;
    BYTE Green;
    BYTE Blue;
    BYTE _reserved;
} ColorTable;

typedef struct {
    BITMAPFILEHEADER file_header;
    BITMAPINFOHEADER info_header;
    int size;
    RGB_IMG rgb_img;
    ColorTable * color_table;
    BYTE * raw;
    int channel_count;
} BMP_IMG;

void getBMPHeader(FILE *fp, BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header);

int BMP2YUV(FILE *fp, BYTE **yuv_out);
int BMP2RGB(FILE *fp, RGB **rgb_out);
void BMP2RGB2(FILE *fp, BMP_IMG * bmp_img);
void BMPWriteHeader(FILE *fp, BMP_IMG *bmp_img);
void BMPReadHeader(FILE *fp, BMP_IMG * bmp_img);
void BMPWriteRGB(FILE *fp, BMP_IMG *bmp_img);
int BMPWriteColorTable(FILE *fp, BMP_IMG *bmp_img);
void BMPReadPixels(FILE *fp, BMP_IMG *bmp_img);
double BMPCMP_PSNR(BMP_IMG *img_a, BMP_IMG *img_b);
