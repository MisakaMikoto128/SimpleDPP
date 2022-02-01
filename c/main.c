#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 1024
__implemented byte send_data[BUFFER_SIZE];
__implemented byte recv_data[BUFFER_SIZE];

__implemented void SimpleDPPRecvCallback(const byte *data, int len)
{
    printf("SimpleDPPRecvCallback------------------> \r\n");
    // print data
    int i = 0;
    for (i; i < len; i++)
    {
        putchar(data[i]);
    }
}
__implemented byte SimpleDPP_putchar(byte c)
{
    // putchar(c);
    SimpleDPP_parse(c);
    return c;
}

__implemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){
    __unimplemented
}

int main(void)
{

    //1. Init Simple DPP data buffer
    SimpleDPP_init(send_data, BUFFER_SIZE, recv_data, BUFFER_SIZE);

    //2. send and parse one msg,msg cnn be type of char * or byte *
    char *msg = "hello worl@\\00\r\n000d";
    if (SimpleDPP_send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    //3. send and parse multiple msg,msg cnn be type of char * or byte *
    if(SimpleDPP_send_datas(2, "hello", 5,"AA",2) == SIMPLEDPP_SENDFAILED){
        printf("send error\n");
    }

    return 0;
}