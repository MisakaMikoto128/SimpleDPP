#include "SimpleDPP.h"


static void SimpleDPP_send_buffer();
static void SimpleDPPRecvInnerCallback();
static void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code);
static Buffer send_buffer;
static Buffer recv_buffer;
static int SimpleDPPErrorCnt;
static int SimpleDPPRevState;
// #define SimpleDPP_ESCAPE_CHAR_LEN 2
// static char SimpleDPP_control_byte_buf[SimpleDPP_ESCAPE_CHAR_LEN] = {0};
void SimpleDPP_init(byte *send_data, int send_capacity, byte *recv_data, int recv_capacity)
{
    buffer_setmemory(&send_buffer, send_data, send_capacity);
    buffer_setmemory(&recv_buffer, recv_data, recv_capacity);
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
/**
 * @Return:
 *   success: The number of bytes actually sent
 * fail: SAMPLE_ERROR
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
    return buffer_size(&send_buffer);
}


/**
 * @brief simple dpp send datas,the input datas will be treated as one data.The last parameter should be VAR_ARG_END.
 * @return success: The number of bytes actually sent
 * fail: SAMPLE_ERROR
 * @example SimpleDPP_send_datas(3,"data1",len1,"data2",len2,"data3",len3);
 */
int SimpleDPP_send_datas(size_t data_num,const byte *data, size_t data_len,...)
{
    va_list args;
    int i;
    //1. empty buffer
    buffer_clear(&send_buffer);
    //2. push SHO
    buffer_push(&send_buffer, SOH);
    //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
    va_start(args, data_len);
    while (true)
    {
        for (i = 0; i < data_len; i++)
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
        if(--data_num == 0)
        {
            break;
        }
        data = va_arg(args, const byte *);
        data_len = va_arg(args, size_t);
    }
    va_end(args);
    //4. push EOT
    if (buffer_push(&send_buffer, EOT) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }
    //5. send message
    SimpleDPP_send_buffer();
    return buffer_size(&send_buffer);
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
        // {
        //     // deprecate
        // if(c == EOT){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
        //     SimpleDPPRecvInnerCallback();
        // }
        // else if(c == ESC){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_CTRL_BYTE;
        // }else if(c == SOH){
        //     SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
        //     SimpleDPPRevErrorInnerCallback();
        // }
        // else{
        //     if(buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR){
        //         SimpleDPPRevErrorInnerCallback();
        //     }
        // }
        // }

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