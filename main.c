#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "main.h"
#include "BMP.h"
#include "quantify.h"
#include "dpcm.h"

void write_dpcm_img(FILE *fp, BMP_IMG * bmp_img)
{
    RGB_IMG * rgb_img = &bmp_img->rgb_img;
    int size = rgb_img->size;
    BYTE * rgb_buf = bmp_img->raw;
    int ch_count = bmp_img->channel_count;
    for(int ch=0;ch<ch_count;ch++) {
        BYTE * dpcm_in_buf = (BYTE *) malloc(size);
        BYTE * dpcm_out_buf = (BYTE *) malloc(size);
        for(int i=0;i<size;i++){
            dpcm_in_buf[i] = rgb_buf[i*ch_count + ch];
        } 
        dpcm(dpcm_in_buf, dpcm_out_buf, size);
        fwrite(dpcm_out_buf, size, 1, fp);
    }

    // BYTE * dpcm_in_buf = (BYTE *) malloc(size * 3);
    // BYTE * dpcm_out_buf = (BYTE *) malloc(size);
    // BYTE * dpcm_in_buf_r = dpcm_in_buf;
    // BYTE * dpcm_in_buf_g = dpcm_in_buf + size;bmp_img->raw
    // BYTE * dpcm_in_buf_b = dpcm_in_buf + size * 2;
    // printf("pixels: %d\r\n", size);
    // for(int i=0;i<size;i++){
    //     //printf("%d ", rgb_buf[i].R)
    //     dpcm_in_buf_r[i] = rgb_buf[i].R;
    //     dpcm_in_buf_g[i] = rgb_buf[i].G;
    //     dpcm_in_buf_b[i] = rgb_buf[i].B;
    // } 
    
    // dpcm(dpcm_in_buf_r, dpcm_out_buf, size);
    // fwrite(dpcm_out_buf, size, 1, fp);
    // dpcm(dpcm_in_buf_g, dpcm_out_buf, size);
    // fwrite(dpcm_out_buf, size, 1, fp);
    // dpcm(dpcm_in_buf_b, dpcm_out_buf, size);
    // fwrite(dpcm_out_buf, size, 1, fp);
}

void read_bmp_file(FILE * fp, BMP_IMG * bmp_img) {
    BMP2RGB2(fp, bmp_img);
}


int main(int argc, char const *argv[]) {
    if(argc!=4) {
        exit(1);
    }
    const char * path = argv[1]; //"./test/Noise256B.bmp";
    const char * output_path = argv[2]; //"./test/test_out/test.dpcm";
    int d;
    sscanf(argv[3], "%d", &d);
    dpcm_set_delta(d);

    FILE *fp,  *fp_out;
    fp = fopen(path, "rb");
    fp_out = fopen(output_path, "wb");

    BMP_IMG bmp_img;
    read_bmp_file(fp, &bmp_img);
    BMPWriteHeader(fp_out, &bmp_img);
    write_dpcm_img(fp_out, &bmp_img);
    fclose(fp);
    fclose(fp_out);
    return 0;
}


