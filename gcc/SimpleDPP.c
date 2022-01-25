#include "SimpleDPP.h"
#define size_t unsigned int
static void SimpleDPP_send_buffer();
static void SimpleDPPRecvInnerCallback();
static void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code);
static Buffer send_buffer;
static Buffer recv_buffer;
static int SimpleDPPErrorCnt;
static int SimpleDPPRevState;

__attribute__((weak)) byte __send_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];
__attribute__((weak)) byte __recv_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];

// #define SimpleDPP_ESCAPE_CHAR_LEN 2
// static char SimpleDPP_control_byte_buf[SimpleDPP_ESCAPE_CHAR_LEN] = {0};
void SimpleDPP_init(int send_capacity,int recv_capacity)
{
    buffer_setmemory(&send_buffer, __send_data, send_capacity);
    buffer_setmemory(&recv_buffer, __recv_data, recv_capacity);
    SimpleDPPErrorCnt = 0;
    SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
}

void SimpleDPP_default_init()
{
    buffer_setmemory(&send_buffer, __send_data, SIMPLEDDP_DEFAULT_BUFFER_SIZE);
    buffer_setmemory(&recv_buffer, __recv_data, SIMPLEDDP_DEFAULT_BUFFER_SIZE);
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

static void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code)
{
    SimpleDPPRevErrorCallback(error_code);
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
int SimpleDPP_send(const byte *data, int len)
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
    if(buffer_push(&send_buffer, EOT) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }
    //5. send message
    SimpleDPP_send_buffer();
    return len;
}
// SimpleDPP receive state machine's states
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
        switch (c)
        {
        case SOH:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
            SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END);
            break;
        case EOT:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
            SimpleDPPRecvInnerCallback();
            break;
        case ESC:
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_CTRL_BYTE;
            break;
        default:
            if (buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
            {
                SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_OVER_CAPACITY);
            }
            break;
        }
        break;
    case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
        if (containSimpleDPPCtrolByte(c))
        {
            if (buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
            {
                SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_OVER_CAPACITY);
            }
            SimpleDPPRevState = SIMPLEDPP_REV_WAIT_END;
        }
        else
        {
            SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE);
        }
        break;
    default:
        break;
    }
}



__attribute__((weak)) void SimpleDPPRecvCallback(const byte *data, int len)
{
    __unimplemented
}
__attribute__((weak)) byte SimpleDPP_putchar(byte c)
{
   __unimplemented
    return c;
}

__attribute__((weak)) void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){
    __unimplemented
    switch (error_code)
    {
    case SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END:
        break;
    case SIMPLEDPP_ERROR_REV_OVER_CAPACITY:
        break;
    case SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE:
        break;

    default:
        break;
    }
}
