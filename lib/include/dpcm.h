#include "quantify.h"
#include "YUV.h"

void dpcm(BYTE *buf_in, BYTE * buf_out, int size);
void dpcm_inv(BYTE * buf_in, BYTE * buf_out, int size);
void dpcm_set_delta(int d);