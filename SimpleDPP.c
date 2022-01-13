#include "SimpleDPP.h"

static void SimpleDPP_send_buffer();
static void SimpleDPPRecvInnerCallback();
static void SimpleDPPErrorInnerCallback(SimpleDPPERROR error_code);
static Buffer send_buffer;
static Buffer recv_buffer;
static int SimpleDPPErrorCnt;
static int SimpleDPPRevState;
// #define SimpleDPP_ESCAPE_CHAR_LEN 2
// static char SimpleDPP_control_byte_buf[SimpleDPP_ESCAPE_CHAR_LEN] = {0};
void SimpleDPP_init(byte *send_data, int send_capacity, byte *recv_data, int recv_capacity)
{
    buffser_setmemory(&send_buffer, send_data, send_capacity);
    buffser_setmemory(&recv_buffer, recv_data, recv_capacity);
    SimpleDPPErrorCnt = 0;
    SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
}

static void SimpleDPP_send_buffer()
{
    int i;
    for (i = 0; i < send_buffer.size; i++)
    {
        SimpleDPP_putchar(send_buffer.data[i]);
    }
}

static void SimpleDPPRecvInnerCallback()
{
    SimpleDPPRecvCallback(recv_buffer.data, recv_buffer.size);
    buffer_clear(&recv_buffer);
}

static void SimpleDPPErrorInnerCallback(SimpleDPPERROR error_code)
{
    SimpleDPPErrorCallback(error_code);
    buffer_clear(&recv_buffer);
    SimpleDPPErrorCnt++;
}

int getSimpleDPPErrorCnt()
{
    return SimpleDPPErrorCnt;
}
/*
Return:
    success: send data length
    fail: SAMPLE_ERROR
*/
int SimpleDPP_send(byte *data, int len)
{
    int i;
    //1. empty buffer
    buffer_clear(&send_buffer);
    //2. push SHO
    buffer_push(&send_buffer, SOH);
    for (i = 0; i < len; i++)
    {
        //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
        if (containSimpleDPPCtrolByte(data[i]))
        {
            // escaped control byte only 2 bytes
            if (buffer_push(&send_buffer, ESC) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
            if (buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
        else
        {
            if (buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
    }
    //4. push EOT
    buffer_push(&send_buffer, EOT);
    //5. send message
    SimpleDPP_send_buffer();
    return len;
}
// SimpleDPP receive state machine's states
// SimpleDPP receive state machine's states
#define SIMPLEDPP_REV_WAIT_START 0
#define SIMPLEDPP_REV_WAIT_END 1
#define SIMPLEDPP_REV_WAIT_CTRL_BYTE 2

// SimpleDPP frame control byte (The frame delimiter)
#define SOH 0x01 //DEC: 1
#define EOT 0x04 //DEC: 4
#define ESC 0x18 //DEC: 27
void SimpleDPP_parse(byte c)
{
    switch (SimpleDPPRevState)
    {
    case SIMPLEDPP_REV_WAIT_START:
        if (c == SOH)
        {
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_END;
        }
        break;
    case SIMPLEDPP_REV_WAIT_END:
        // {
        //     // deprecate
        // if(c == EOT){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
        //     SimpleDPPRecvInnerCallback(recv_buffer.data, recv_buffer.size);
        // }
        // else if(c == ESC){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_CTRL_BYTE;
        // }else if(c == SOH){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
        //     SimpleDPPErrorInnerCallback();
        // }
        // else{
        //     if(buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR){
        //         SimpleDPPErrorInnerCallback();
        //     }
        // }
        // }

        switch (c)
        {
        case SOH:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
            SimpleDPPErrorInnerCallback(SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END);
            break;
        case EOT:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
            SimpleDPPRecvInnerCallback(recv_buffer.data, recv_buffer.size);
            break;
        case ESC:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_CTRL_BYTE;
            break;
        default:
            if (buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
            {
                SimpleDPPErrorInnerCallback(SIMPLEDPP_ERROR_REV_OVER_CAPACITY);
            }
            break;
        }
        break;
    case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
        if (containSimpleDPPCtrolByte(c))
        {
            if (buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
            {
                SimpleDPPErrorInnerCallback(SIMPLEDPP_ERROR_REV_OVER_CAPACITY);
            }
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_END;
        }
        else
        {
            SimpleDPPErrorInnerCallback(SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE);
        }
        break;
    default:
        break;
    }
}
