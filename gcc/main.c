#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
#define SIMPLE_DPP_REV_BUFFER_SIZE 1024
#define SIMPLE_DPP_SEND_BUFFER_SIZE 1024
__implemented byte __send_data[SIMPLE_DPP_SEND_BUFFER_SIZE];
__implemented byte __recv_data[SIMPLE_DPP_REV_BUFFER_SIZE];

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

}

int main(void)
{

    //1. Init Simple DPP data buffer
    //1.1 using custom buffers, the default buffer will not compile
    SimpleDPP_init(SIMPLE_DPP_SEND_BUFFER_SIZE,SIMPLE_DPP_REV_BUFFER_SIZE);
    //1.2 or you can use SimpleDPP_default_init(),and "__send_data" and "__recv_data" don't need to be defined.
    //default buffer size is 1024.
    SimpleDPP_default_init();


    //2. send and parse one msg,msg cnn be type of char * or byte *
    char *msg = "hello worl@\\00\r\n000d";
    if (SimpleDPP_send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    //3. send and parse multiple msg,msg cnn be type of char * or byte *
    if(SimpleDPP_send_datas("hello", 5,"AA",2) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    return 0;
}
