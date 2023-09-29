#include "SimpleDPP.h"
#include <stdio.h>
#include <string.h>

#define SIMPLE_DPP_REV_BUFFER_SIZE 512
#define SIMPLE_DPP_SEND_BUFFER_SIZE 512
__implemented sdp_byte __send_data[SIMPLE_DPP_SEND_BUFFER_SIZE];
__implemented sdp_byte __recv_data[SIMPLE_DPP_REV_BUFFER_SIZE];

__implemented void SimpleDPPRecvCallback(const sdp_byte *data, int len)
{
    printf("recv frame data:%s\n", data);
}

__implemented uint32_t SimpleDPP_write(const void *buf, unsigned int len)
{

    return 0;
}

__implemented uint32_t SimpleDPP_read(void *buf, unsigned int len)
{

    return 0;
}

__implemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code)
{
}

SimpleDPP sdp;
static SimpleDPPAdapter_t adapter = {
    .write = SimpleDPP_write,
    .read = SimpleDPP_read,
    .debug = NULL,
    .error = NULL,
    .lock = NULL,
    .unlock = NULL,
};

int main(void)
{

    // 1. Init Simple DPP data buffer and callback,if you don't want to use callback,you can set callback to NULL.
    SimpleDPP_Constructor(&sdp,
                          __send_data, SIMPLE_DPP_SEND_BUFFER_SIZE,
                          __recv_data, SIMPLE_DPP_REV_BUFFER_SIZE,
                          SimpleDPPRecvCallback,
                          SimpleDPPRevErrorCallback,
                          &adapter);

    // 2. send and parse one msg,msg cnn be type of char * or byte *
    char *msg = "hello worl@\\00\r\n000d";
    if (SimpleDPP_send(&sdp, msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    // 3. send and parse multiple msg,msg cnn be type of char * or byte *
    if (SimpleDPP_send_datas(&sdp, "hello", 5, "AA", 2) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }

    return 0;
}
