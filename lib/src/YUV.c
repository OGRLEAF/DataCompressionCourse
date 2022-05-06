#include <stdlib.h>
#include "YUV.h" 
int save_yuv(YUV * yuv_img, BYTE ** bufp, YUV_FORMAT format, int height, int width)
{
    int i;
    int size = height * width;
    int file_size = (format.Y + format.U + format.V) * size / 4;
    int flag = 10*(format.U) + format.V;
    int h_inter=0, v_inter=0;
    BYTE * output = (BYTE*) malloc(file_size);
    *bufp = output;
    BYTE * start_addr = output;
    for(i=0;i<size;i++){
        *output = yuv_img[i].Y;
        output++;
    }
    if(flag==44) {
        v_inter = h_inter = 1;
    }else if(flag==22){
        v_inter = 1;
        h_inter = 2;
    }else if(flag==20){
        v_inter = h_inter = 2;
    }else {
        return size;
    }
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            i = y * width + x;
            if(y%v_inter==0 && x%h_inter ==0){
                *output = yuv_img[i].U;
                output++;
            }
        }
    }
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            i = y * width + x;
            if(y%v_inter==0 && x%h_inter ==0){
                *output = yuv_img[i].V;
                output++;
            }
        }
    }
    printf("Convert size: %ld\n", output - start_addr);
    return file_size;
}

void RGB2YUV(RGB * rgb_img, YUV * yuv_img, int size) {
    int i=0;
    for(i=0;i<size;i++){
        RGB rgb = rgb_img[i];
        double dr = rgb.R, dg = rgb.G, db = rgb.B;
        double y = 0.299 * dr + .587 * dg + .114 * db;
        double u = 0.713 * (dr - y) + 128;
        double v = 0.713 * (db - y) + 128;
        yuv_img[i].Y = (BYTE) y;
        yuv_img[i].U = (BYTE) u;
        yuv_img[i].V = (BYTE) v;
    }
}

int resize(RGB ** img, int srcHeight, int srcWidth, int dstHeight, int dstWidth, const char *mode)
{
    int dst_size = dstHeight * dstWidth;
    RGB * src_img = *img;
    RGB * dst_img = (RGB *) malloc(sizeof(RGB) * dst_size);
    int pos = 0;
    if(mode[0]=='C'){ // crop mode
        for(int y=0;y<srcHeight;y++)
        {
            if(y<dstHeight){
                for(int x=0;x<srcWidth;x++)
                {
                    if(x<dstWidth){
                        dst_img[pos] = src_img[y*srcWidth + x];
                        pos++;
                    }
                }
            }
        }
    }
    else if(mode[0]=='R') { // Resize Mode
        double w_interval = srcWidth/dstHeight;
        for(int y=0;y<srcHeight;y = (int) y + w_interval)
        {
            for(int x=0;x<srcWidth;x = (int) x + w_interval){
                dst_img[pos] = src_img[y*srcWidth + x];
                pos++;
            }
        }
    }
    printf("Destination size %d\r\n", pos);
    *img = dst_img;
    return dst_size;
}