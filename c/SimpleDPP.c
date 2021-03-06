#include "SimpleDPP.h"

static void SimpleDPP_send_buffer();
static void SimpleDPPRecvInnerCallback();
static void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code);
static ByteBuffer send_buffer;
static ByteBuffer recv_buffer;
static int SimpleDPPErrorCnt;
static int SimpleDPPRevState;
// #define SimpleDPP_ESCAPE_CHAR_LEN 2
// static char SimpleDPP_control_byte_buf[SimpleDPP_ESCAPE_CHAR_LEN] = {0};
void SimpleDPP_init(sdp_byte *send_data, int send_capacity, sdp_byte *recv_data, int recv_capacity)
{
    byte_buffer_setmemory(&send_buffer, send_data, send_capacity);
    byte_buffer_setmemory(&recv_buffer, recv_data, recv_capacity);
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
    byte_buffer_clear(&recv_buffer);
}

static void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code)
{
    SimpleDPPRevErrorCallback(error_code);
    byte_buffer_clear(&recv_buffer);
    SimpleDPPErrorCnt++;
}

int getSimpleDPPErrorCnt()
{
    return SimpleDPPErrorCnt;
}
/**
 * @Return:
 *   success: The number of bytes actually sent
 * fail: SIMPLEDPP_SENDFAILED
 */
int SimpleDPP_send(const sdp_byte *data, int len)
{
    int i;
    //1. empty buffer
    byte_buffer_clear(&send_buffer);
    //2. push SHO
    byte_buffer_push(&send_buffer, SOH);
    for (i = 0; i < len; i++)
    {
        //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
        if (containSimpleDPPCtrolByte(data[i]))
        {
            // escaped control byte only 2 bytes
            if (byte_buffer_push(&send_buffer, ESC) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
            if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
        else
        {
            if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
    }
    //4. push EOT
    if (byte_buffer_push(&send_buffer, EOT) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }

    //5. send message
    SimpleDPP_send_buffer();
    return byte_buffer_size(&send_buffer);
}

/**
 * @brief must be used before send_datas_add() and send_datas_end()
 * @return success : the number of bytes in the current buffer
 * fail : SIMPLEDPP_SENDFAILED
 */
int send_datas_start()
{
    //1. empty buffer
    byte_buffer_clear(&send_buffer);
    //2. push SHO
    if (byte_buffer_push(&send_buffer, SOH) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }
    return byte_buffer_size(&send_buffer);
}

/**
 * @brief must be used between send_datas_start() and send_datas_add()
 * @return success : the number of bytes in the current buffer
 * fail : SIMPLEDPP_SENDFAILED
 */
int send_datas_add(const sdp_byte *data, int len)
{
    for (int i = 0; i < len; i++)
    {
        //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
        if (containSimpleDPPCtrolByte(data[i]))
        {
            // escaped control byte only 2 bytes
            if (byte_buffer_push(&send_buffer, ESC) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
            if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
        else
        {
            if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
            {
                return SIMPLEDPP_SENDFAILED;
            }
        }
    }
    return byte_buffer_size(&send_buffer);
}

/**
 * @brief must be used after send_datas_start() and send_datas_add()
 * @return success : the number of bytes in the current buffer
 * fail : SIMPLEDPP_SENDFAILED
 */
int send_datas_end()
{
    //4. push EOT
    if (byte_buffer_push(&send_buffer, EOT) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }
    //5. send message
    SimpleDPP_send_buffer();
    return byte_buffer_size(&send_buffer);
}

/**
 * @brief simple dpp send datas,the input datas will be treated as one data.The last parameter should be VAR_ARG_END.
 * @return success: The number of bytes actually sent
 * fail: SIMPLEDPP_SENDFAILED
 * @example SimpleDPP_send_datas(3,"data1",len1,"data2",len2,"data3",len3);
 */
int SimpleDPP_send_datas(int data_num, const sdp_byte *data, int data_len, ...)
{
    va_list args;
    int i;
    //1. empty buffer
    byte_buffer_clear(&send_buffer);
    //2. push SHO
    byte_buffer_push(&send_buffer, SOH);
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
                if (byte_buffer_push(&send_buffer, ESC) == OVER_CAPACITY_ERROR)
                {
                    return SIMPLEDPP_SENDFAILED;
                }
                if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
                {
                    return SIMPLEDPP_SENDFAILED;
                }
            }
            else
            {
                if (byte_buffer_push(&send_buffer, data[i]) == OVER_CAPACITY_ERROR)
                {
                    return SIMPLEDPP_SENDFAILED;
                }
            }
        }
        if (--data_num == 0)
        {
            break;
        }
        data = va_arg(args, const sdp_byte *);
        data_len = va_arg(args, int);
    }
    va_end(args);
    //4. push EOT
    if (byte_buffer_push(&send_buffer, EOT) == OVER_CAPACITY_ERROR)
    {
        return SIMPLEDPP_SENDFAILED;
    }
    //5. send message
    SimpleDPP_send_buffer();
    return byte_buffer_size(&send_buffer);
}

// SimpleDPP receive state machine's states
void SimpleDPP_parse(sdp_byte c)
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
            if (byte_buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
            {
                SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_OVER_CAPACITY);
            }
            break;
        }
        break;
    case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
        if (containSimpleDPPCtrolByte(c))
        {
            if (byte_buffer_push(&recv_buffer, c) == OVER_CAPACITY_ERROR)
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
