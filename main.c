#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 1024
__implemented byte send_data[BUFFER_SIZE];
__implemented byte recv_data[BUFFER_SIZE];

__implemented void SimpleDPPRecvCallback(byte *data, int len)
{
    printf("SimpleDPPRecvCallback------------------> \r\n");
    // print data
    size_t i = 0;
    for (i; i < len; i++)
    {
        putchar(data[i]);
    }
}
__implemented byte SimpleDPP_putchar(byte c)
{
    // putchar(c);
    SimpleDPP_parse(c);
}

__implemented void SimpleDPPErrorCallback(SimpleDPPERROR error_code){
    __unimplemented
}

int main(void)
{

    SimpleDPP_init(send_data, BUFFER_SIZE, recv_data, BUFFER_SIZE);
    printf("Test SimpleDPP----------->\r\n");
    char *msg = "hello worl@\\00\r\n000d";
    if (SimpleDPP_send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }
    return 0;
}