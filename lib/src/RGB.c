#include <stdio.h>
#include "RGB.h"


int RGB2RGB(FILE *fp, RGB**rgb_out_bp)
{
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int pixels = size/3; // caculating file size
    RGB * rgbs = (RGB *) malloc(size);
    fread(rgbs, 1, size, fp);
    *rgb_out_bp = rgbs;
    return pixels;
}