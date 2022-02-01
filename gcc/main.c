#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 1024
__implemented byte __send_data[BUFFER_SIZE];
__implemented byte __recv_data[BUFFER_SIZE];

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

    SimpleDPP_init(BUFFER_SIZE,BUFFER_SIZE);
    // or you can use SimpleDPP_default_init(),and "__send_data" and "__recv_data" don't need to be defined.
    // The default buffer size is 1024.
    printf("Test SimpleDPP----------->\r\n");
    char *msg = "hello worl@\\00\r\n000d";
    int len1 = strlen(msg);
    if (SimpleDPP_send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }


    char *msg2 = "AAAAAAAAAA worl@\\00\r\n000d\r\n";
    int len2 = strlen(msg2);
    //test int SimpleDPP_send_datas(const byte *data, ...)
    if(SimpleDPP_send_datas(msg,len1,msg2,len2) == SIMPLEDPP_SENDFAILED){
        printf("send error\n");
    }
    return 0;
}
