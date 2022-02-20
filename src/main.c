#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
#define SIMPLE_DPP_REV_BUFFER_SIZE 512
#define SIMPLE_DPP_SEND_BUFFER_SIZE 512
__implemented sdp_byte __send_data[SIMPLE_DPP_SEND_BUFFER_SIZE];
__implemented sdp_byte __recv_data[SIMPLE_DPP_REV_BUFFER_SIZE];
__implemented int send_capacity = SIMPLE_DPP_SEND_BUFFER_SIZE;
__implemented int recv_capacity = SIMPLE_DPP_REV_BUFFER_SIZE;

__implemented void SimpleDPPRecvCallback(const sdp_byte *data, int len)
{
    printf("SimpleDPPRecvCallback------------------> \r\n");
    // print data
    int i = 0;
    for (i; i < len; i++)
    {
        putchar(data[i]);
    }
}
__implemented sdp_byte SimpleDPP_putchar(sdp_byte c)
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
    //1.1 if you want to use custom buffers, you just need to explicitly define them.default buffer size is 1024.
    //1.2 if you want to use default buffers, you just needn't to do anything.
    SimpleDPP_init();
    
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
