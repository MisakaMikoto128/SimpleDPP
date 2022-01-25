#ifndef SIMPLEDPP_H
#define SIMPLEDPP_H

#include <vector>

#include <functional>
// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2 //USING
#define SIMPLEDPP_NORMAL 0
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11 //USING
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21                //USING
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22 //USING
#define SIMPLEDPP_CRC_CHECK_ERROR -23

//cast char * to byte *
#define byte char
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (byte *)(ptr)

// SimpleDPP receive state machine's states
#define SIMPLEDPP_REV_WAIT_START 0
#define SIMPLEDPP_REV_WAIT_END 1
#define SIMPLEDPP_REV_WAIT_CTRL_BYTE 2

typedef int SimpleDPPERROR;
// SimpleDPP frame control byte (The frame delimiter)
#define SOH 0x01 //DEC: 1
#define EOT 0x04 //DEC: 4
#define ESC 0x18 //DEC: 27
#define containSimpleDPPCtrolByte(c) ((c) == SOH || (c) == EOT || (c) == ESC)

class SimpleDPP
{
private:
    std::vector<byte> sendBuffer;
    std::vector<byte> revBuffer;
    size_t SimpleDPPErrorCnt;
    int SimpleDPPRevState;

private:
    void SimpleDPPRecvInnerCallback()
    {
        if(RecvCallback!=nullptr)
        {
            RecvCallback(revBuffer);
        }
        revBuffer.clear();
    }

    void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code)
    {
        if(RevErrorCallback!=nullptr)
        {
            RevErrorCallback(error_code);
        }
    }

    void send_buffer()
    {
        if(SendBuffer!=nullptr)
        {
            SendBuffer(sendBuffer);
        }
    }

    std::function<void(const std::vector<byte> &revdata)> RecvCallback = nullptr;
    std::function<void(SimpleDPPERROR error_code)> RevErrorCallback = nullptr;
    std::function<void(const std::vector<byte> &senddata)> SendBuffer = nullptr;

public:
    template <class T>
    void bindRecvCallback(const T &obj, void (T::*func)(const std::vector<byte> &revdata))
    {
        RecvCallback = std::bind(func, obj, std::placeholders::_1);
    }

    template <class T>
    void bindRevErrorCallback(const T &obj, void (T::*func)(SimpleDPPERROR error_code))
    {
        RevErrorCallback = std::bind(func, obj, std::placeholders::_1);
    }

    template <class T>
    void bindSendBuffer(const T &obj, void (T::*func)(const std::vector<byte> &senddata))
    {
        SendBuffer = std::bind(func, obj, std::placeholders::_1);
    }



    void bindRecvCallback(std::function<void(const std::vector<byte> &revdata)> RecvCallback)
    {
        this->RecvCallback = RecvCallback;
    }

    void bindRevErrorCallback(std::function<void(SimpleDPPERROR error_code)> RevErrorCallback)
    {
        this->RevErrorCallback = RevErrorCallback;
    }

    void bindSendBuffer(std::function<void(const std::vector<byte> &senddata)> SendBuffer)
    {
        this->SendBuffer = SendBuffer;
    }


public:
    explicit SimpleDPP()
    {
        SimpleDPPErrorCnt = 0;
        SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
    }

    ~SimpleDPP() {}

    void parse(const byte *data, size_t len)
    {
        for (int i = 0; i < len; i++)
        {
            parse(data[i]);
        }
    }
    size_t getSimpleDPPErrorCnt() { return SimpleDPPErrorCnt; }

    size_t send(const byte *data, size_t len)
    {
        int i;
        //1. empty buffer
        sendBuffer.clear();
        //2. push SHO
        sendBuffer.push_back(SOH);

        for (i = 0; i < len; i++)
        {
            //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
            if (containSimpleDPPCtrolByte(data[i]))
            {
                // escaped control byte only 2 bytes
                sendBuffer.push_back(ESC);
                sendBuffer.push_back(data[i]);
            }
            else
            {
                sendBuffer.push_back(data[i]);
            }
        }
        //4. push EOT
        sendBuffer.push_back(EOT);
        //5. send message
        send_buffer();
        return len;
    }
    void parse(byte c)
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
                revBuffer.push_back(c);
                break;
            }
            break;
        case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
            if (containSimpleDPPCtrolByte(c))
            {
                revBuffer.push_back(c);
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
};

#endif // SIMPLEDPP_H
