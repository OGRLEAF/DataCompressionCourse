#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include "BMP.h"
#include "dpcm.h"

void dpcm_img_2_rgb(FILE *fp, RGB * rgb, int size)
{
    char * buf = (char *) malloc(size * 3);
    char * buf_out = (char *) malloc(size);
    fread(buf, size * 3, 1, fp);
    dpcm_inv(buf, buf_out, size);
    for(int i=0;i<size;i++){
        rgb[i].R = buf_out[i];
    }
    dpcm_inv(buf + size, buf_out, size);
    for(int i=0;i<size;i++){
        rgb[i].G = buf_out[i];
    }
    dpcm_inv(buf + size*2, buf_out, size);
    for(int i=0;i<size;i++){
        rgb[i].B = buf_out[i];
    }
}

void dpcm_bmp_2_bmp(FILE *fp, BMP_IMG *bmp_img){
    BMPReadHeader(fp, bmp_img);
    int ch_count = bmp_img->channel_count;
    int size = bmp_img->size;
    BYTE * buf_out = (BYTE *) malloc(size * ch_count);
    bmp_img->raw = buf_out;
    BYTE * buf = (BYTE *) malloc(size);
    BYTE * dpcm_out_buf = (BYTE *) malloc(size);
    for(int ch=0;ch<ch_count;ch++){
        fread(buf, 1, size, fp);
        dpcm_inv(buf, dpcm_out_buf, size);
        for(int i=0;i<size;i++){
            buf_out[i*ch_count + ch] = dpcm_out_buf[i];
        }
    }
    free(buf);
    free(dpcm_out_buf);
}

int main(int argc, char const *argv[]) {
    if(argc!=4) {
        exit(1);
    }
    const char * input_path = argv[1]; //"./test/test_out/test.dpcm";
    const char * output_path = argv[2]; //"./test/test_out/test.dpcm.bmp";
    int d;
    sscanf(argv[3], "%d", &d);
    dpcm_set_delta(d);

    FILE *fp, *fp_out;
    fp = fopen(input_path, "rb");
    fp_out = fopen(output_path, "wb");

    BMP_IMG bmp_img;
    dpcm_bmp_2_bmp(fp, &bmp_img);
    BMPWriteRGB(fp_out, &bmp_img);
    fclose(fp);
    fclose(fp_out);
    return 0;
}