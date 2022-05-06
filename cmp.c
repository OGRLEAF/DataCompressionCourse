#include "BMP.h"
#include <stdlib.h>


double cmp(const char *path_a, const char * path_b)
{
    FILE *fp_a;
    FILE *fp_b;
    fp_a = fopen(path_a, "rb");
    fp_b = fopen(path_b, "rb");
    printf("input: %s, %s\r\n", path_a, path_b);
    BMP_IMG bmp_a;
    BMP_IMG bmp_b;
    BMPReadHeader(fp_a, &bmp_a);
    BMPReadHeader(fp_b, &bmp_b);
    BMPReadPixels(fp_a, &bmp_a);
    BMPReadPixels(fp_b, &bmp_b);
    return BMPCMP_PSNR(&bmp_a, &bmp_b);
}

int main(int argc, char const *argv[])
{
    if(argc!=3) exit(1);
    cmp(argv[1], argv[2]);
    return 0;
}

