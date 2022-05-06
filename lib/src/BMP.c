#include<math.h>
#include "BMP.h"
// #include "YUV.h"
#include "stdlib.h"
#include "string.h"
#include "utils.h"

void printBMPFileHeader(BITMAPFILEHEADER *fhp)
{
    printf("Signature: %.2s\r\n"
           "FileSize: %d\r\n"
           "DataOffset: %d\r\n",
           "BM", fhp->FileSize, fhp->DataOffset);
}

void printBMPINFOHeader(BITMAPINFOHEADER *bih)
{
    printf("Size: %d\r\n"
           "Width: %d\r\n"
           "Height: %d\r\n"
           "Planes: %d\r\n"
           "BitCount: %d\r\n"
           "ColorsUsed: %d\r\n",
           bih->Size, bih->Width, bih->Height, bih->Planes, bih->BitCount, bih->ColorsUsed);
}

void getBMPHeader(FILE *fp, BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header)
{
    fread(file_header, 2, 1, fp);
    fread(file_header, sizeof(BITMAPFILEHEADER), 1, fp);
    printBMPFileHeader(file_header);
    fread(info_header, sizeof(BITMAPINFOHEADER), 1, fp);
    printBMPINFOHeader(info_header);
    // validing header info
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    printf("FS:%d == HD:%d\r\n", size, file_header->FileSize);
    fseek(fp, 0, SEEK_SET);
}

int BMP2YUV(FILE *fp, BYTE **yuv_out)
{
    return 0;
}


int BMP2RGB(FILE *fp, RGB **rgb_out)
{
    fseek(fp, 2, SEEK_SET); // Rollback to start, jump off the signature
    BITMAPFILEHEADER file_header; // bitmap file header
    BITMAPINFOHEADER info_header; // bitmap info header
    fread(&file_header, sizeof(BITMAPFILEHEADER), 1, fp); // read file header
    fread(&info_header, sizeof(BITMAPINFOHEADER), 1, fp); // read info header
    DWORD data_offset = file_header.DataOffset;
    fseek(fp, data_offset, SEEK_SET); // Seek to data

    int height = info_header.Height;
    int width = info_header.Width;
    int pixels = height * width;
    RGB * rgbs = (RGB *)malloc(pixels * sizeof(RGB));
    * rgb_out = rgbs;
    fread(rgbs, sizeof(RGB), pixels, fp);
    if(info_header.Height>0) { 
        // bitmap is bottom-up DIB(Start at lower left) when height>0. rotate it.
        for(int i=0;i<info_header.Height/2;i++){
            for(int j=0;j<info_header.Width;j++){
                RGB temp = rgbs[info_header.Width* (info_header.Height - 1 - i) + j];
                rgbs[info_header.Width* (info_header.Height - 1 - i) + j] = rgbs[i * info_header.Width + j];
                rgbs[i * info_header.Width + j] = temp;
            }
        }
    }
    return pixels;
}

void BMPReadHeader(FILE *fp, BMP_IMG * bmp_img)
{
    fseek(fp, 2, SEEK_SET); // Rollback to start, jump off the signature
    fread(&bmp_img->file_header, sizeof(BITMAPFILEHEADER), 1, fp); // read file header
    fread(&bmp_img->info_header, sizeof(BITMAPINFOHEADER), 1, fp); // read info header
    BITMAPFILEHEADER * file_header = &bmp_img->file_header;
    BITMAPINFOHEADER * info_header = &bmp_img->info_header;
    //printBMPINFOHeader(info_header);
    RGB_IMG *rgb_img = &bmp_img->rgb_img;
    int height = info_header->Height;
    int width = info_header->Width;
    int pixels = height * width;
    bmp_img->size = pixels;
    int colors_used = info_header->ColorsUsed;
    DWORD data_offset = file_header->DataOffset;
    if(colors_used>=16) {
        //printf("Color table is used.\r\n");
        int color_table_size = colors_used * sizeof(ColorTable);
        int bit_count = info_header->BitCount;
        
        ColorTable * color_table = (ColorTable *) malloc(color_table_size);
        fseek(fp, 54, SEEK_SET);
        fread(color_table, sizeof(ColorTable), colors_used, fp);
        bmp_img->color_table = color_table;
        ASSERT_EQUAL(ftell(fp), data_offset, "data_offset not euqal!")
        bmp_img->channel_count = 1;
    } else if(colors_used==0){
        bmp_img->channel_count = 3;
        bmp_img->color_table = NULL;
    }
    rgb_img->height = height;
    rgb_img->width = width;
    rgb_img->size = pixels;
}

void BMPReadPixels(FILE *fp, BMP_IMG *bmp_img)
{
    BITMAPFILEHEADER * file_header = &bmp_img->file_header;
    BITMAPINFOHEADER * info_header = &bmp_img->info_header;
    RGB_IMG *rgb_img = &bmp_img->rgb_img;
    int pixels = bmp_img->size;
    int colors_used = info_header->ColorsUsed;
    DWORD data_offset = file_header->DataOffset;
    RGB * rgbs = (RGB *)malloc(pixels * sizeof(RGB));
    rgb_img->rgb = rgbs;
    if(colors_used>=16) {
        ColorTable * color_table = bmp_img->color_table;
        BYTE * pixel_buf = (BYTE *) malloc(pixels);
        ASSERT_EQUAL(ftell(fp), data_offset, "data_offset not euqal!")
        fread(pixel_buf, info_header->BitCount, pixels, fp);
        for(int i=0;i<pixels;i++){
            ColorTable * color = color_table + pixel_buf[i];
            rgbs[i].R = color->Red;
            rgbs[i].G = color->Green;
            rgbs[i].B = color->Blue;
        }
        bmp_img->color_table = color_table;
        bmp_img->raw = pixel_buf;
    }else if(colors_used==0) {
        fseek(fp, data_offset, SEEK_SET); // Seek to data
        fread(rgbs, sizeof(RGB), pixels, fp);
        bmp_img->raw = (BYTE *) rgbs;
    }

    // #if ROTATE==1
    //     if(info_header->Height>0) { 
    //         // bitmap is bottom-up DIB(Start at lower left) when height>0. rotate it.
    //         for(int i=0;i<info_header->Height/2;i++){
    //             for(int j=0;j<info_header->Width;j++){
    //                 RGB temp = rgbs[info_header->Width* (info_header->Height - 1 - i) + j];
    //                 rgbs[info_header->Width* (info_header->Height - 1 - i) + j] = rgbs[i * info_header->Width + j];
    //                 rgbs[i * info_header->Width + j] = temp;
    //             }
    //         }
    //     }
    // #endif
}

void BMP2RGB2(FILE *fp, BMP_IMG * bmp_img)
{
    RGB_IMG *rgb_img = &bmp_img->rgb_img;
    RGB ** rgb_out = &rgb_img->rgb;
    BITMAPFILEHEADER * file_header = &bmp_img->file_header;
    BITMAPINFOHEADER * info_header = &bmp_img->info_header;
    BMPReadHeader(fp, bmp_img);
    //getBMPHeader(fp, file_header, info_header);
    BMPReadPixels(fp, bmp_img);
}

void BMPWriteHeader(FILE *fp, BMP_IMG *bmp_img) {
    int size = 0;
    size += fwrite("BM", 1, 2, fp);
    size += fwrite(&bmp_img->file_header, 1, sizeof(BITMAPFILEHEADER), fp);
    size += fwrite(&bmp_img->info_header, 1, sizeof(BITMAPINFOHEADER), fp);
    if(bmp_img->color_table) size += BMPWriteColorTable(fp, bmp_img);
    printf("Write %d bytes.\r\n", size);
}

int BMPWriteColorTable(FILE *fp, BMP_IMG *bmp_img)
{
    ASSERT(bmp_img->info_header.ColorsUsed>0, "Color table not suited!");
    return fwrite(bmp_img->color_table, sizeof(ColorTable), bmp_img->info_header.ColorsUsed, fp);
}

void BMPWriteRGB_24b(FILE *fp, BMP_IMG *bmp_img){
    int size = bmp_img->rgb_img.size;
    BITMAPINFOHEADER * info_header = &bmp_img->info_header;
    RGB * rgbs = (RGB *) malloc(size * sizeof(RGB));
    bmp_img->rgb_img.rgb = (RGB *) bmp_img->raw;
    memcpy(rgbs, bmp_img->raw, size * sizeof(RGB));
    // if(info_header->Height>0) { 
    //     // bitmap is bottom-up DIB(Start at lower left) when height>0. rotate it.
    //     for(int i=0;i<info_header->Height/2;i++){
    //         for(int j=0;j<info_header->Width;j++){
    //             RGB temp = rgbs[info_header->Width* (info_header->Height - 1 - i) + j];
    //             rgbs[info_header->Width* (info_header->Height - 1 - i) + j] = rgbs[i * info_header->Width + j];
    //             rgbs[i * info_header->Width + j] = temp;
    //         }
    //     }
    // }
    int rsize = fwrite(rgbs, sizeof(RGB), size, fp);
    printf("Write %d bytes.\r\n", rsize);
    free(rgbs);
}

void BMPWriteRGB_8b(FILE *fp, BMP_IMG *bmp_img)
{
    int size = bmp_img->size;
    BITMAPINFOHEADER * info_header = &bmp_img->info_header;
    BYTE * rgbs = (BYTE *) malloc(size);
    BYTE * raw = bmp_img->raw;
    memcpy(rgbs, raw, size);
    // if(info_header->Height>0) { 
    //     // bitmap is bottom-up DIB(Start at lower left) when height>0. rotate it.
    //     for(int i=0;i<info_header->Height/2;i++){
    //         for(int j=0;j<info_header->Width;j++){
    //             BYTE temp = rgbs[info_header->Width* (info_header->Height - 1 - i) + j];
    //             rgbs[info_header->Width* (info_header->Height - 1 - i) + j] = rgbs[i * info_header->Width + j];
    //             rgbs[i * info_header->Width + j] = temp;
    //         }
    //     }
    // }
    int rsize = fwrite(rgbs, 1, size, fp);
    printf("Write %d bytes.\r\n", rsize);
    free(rgbs);
}

void BMPWriteRGB(FILE *fp, BMP_IMG *bmp_img)
{
    BMPWriteHeader(fp, bmp_img);
    switch (bmp_img->info_header.BitCount)
    {
    case 24:
        BMPWriteRGB_24b(fp, bmp_img);
        break;
    case 8:
        BMPWriteRGB_8b(fp, bmp_img);
        break;
    default:
        break;
    }
}

double BMPCMP_PSNR(BMP_IMG *img_a, BMP_IMG *img_b)
{
    // ensure two img is comparable
    ASSERT_EQUAL(img_a->size, img_b->size, "Not comparable");
    ASSERT_EQUAL(img_a->channel_count, img_b->channel_count, "Not comparable");
    int size = img_a->size * img_a->channel_count;
    double MSE = 0;
    double PSNR = 0;
    for(int i=0;i<size;i++){
        double pixel_a = (char) img_a->raw[i];
        double pixel_b = (char) img_b->raw[i];
        //printf("cmp: %lf %lf\r\n", pixel_a, pixel_b);
        MSE += (pixel_a - pixel_b)*(pixel_a - pixel_b);
    }
    MSE = MSE/size;

    PSNR = 10.0 * log10((255)/MSE);
    printf("MSE: %lf  PSNR: %lf\r\n", MSE, PSNR);
    return PSNR;
}