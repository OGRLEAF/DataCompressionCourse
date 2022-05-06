#include "dpcm.h"

int DELTA = 1<<2;

void dpcm_set_delta(int d) {
    DELTA = 1<<d;
}

char q_1(short src)
{
    double delta = DELTA;
    return (char) (src/delta + 0.5);
    //return Q_UNIFORM(src, 9, 8);
}

short inv_q_1(char src)
{
    double delta = DELTA;
    return ((short) src)*delta; 
    //return INV_Q_UNIFORM((short)src, 8, 9);
}

void dpcm(BYTE *buf_in, BYTE * buf_out, int size){
    short predict = buf_in[0];
    char predict_q = q_1(predict_q);
    short predict_inv = inv_q_1(predict_q);
    short rebuild = predict_inv + 0;
    buf_out[0] = predict_q;

    for(int i=1;i<size;i++){
        predict = rebuild - buf_in[i];
        predict_q = q_1(predict);
        predict_inv = inv_q_1(predict_q);
        buf_out[i] = predict_q;
        rebuild = rebuild - predict_inv;
        //printf("%d \t%d \t%d %d\r\n", predict, buf_in[i], predict_q, predict_inv);
    }
}

void dpcm_inv(BYTE * buf_in, BYTE * buf_out, int size)
{
    short start = inv_q_1((signed char)buf_in[0]); //INV_Q_UNIFORM((short) (signed char)buf_in[0], 8, 9);// (signed char)buf_in[0];//
    short rebuild = start;
    buf_out[0] = start;
    for(int i=1;i<size;i++){
        short next = inv_q_1((signed char)buf_in[i]); //(signed char)buf_in[i];//
        short rebuild_d = rebuild - next;
        rebuild = rebuild_d;//>=256?255:rebuild_d; //
        if(rebuild>=256) printf("[%d %d] ", rebuild, next);
        buf_out[i] = (BYTE) rebuild;
    }
}