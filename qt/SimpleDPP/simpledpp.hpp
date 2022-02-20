#ifndef SIMPLEDPP_H
#define SIMPLEDPP_H

#include <QObject>

typedef int SimpleDPPERROR;

//cast char * to byte *
#ifndef sdp_byte
#define sdp_byte char
#endif
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (sdp_byte *)(ptr)

// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2  //USING,SEND ONLY USING THIS ERROR CODE
#define SIMPLEDPP_NORMAL 0
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11 //USING
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21 //USING
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22 //USING
#define SIMPLEDPP_CRC_CHECK_ERROR -23

// SimpleDPP receive state machine's states
#define SIMPLEDPP_REV_WAIT_START 0
#define SIMPLEDPP_REV_WAIT_END 1
#define SIMPLEDPP_REV_WAIT_CTRL_BYTE 2

// SimpleDPP frame control byte (The frame delimiter)
#define SOH 0x01 //DEC: 1
#define EOT 0x04 //DEC: 4
#define ESC 0x18 //DEC: 27
#define containSimpleDPPCtrolByte(c) ((c) == SOH || (c) == EOT || (c) == ESC)

class SimpleDPP : public QObject
{
    Q_OBJECT

private:
    QByteArray sendBuffer;
    QByteArray revBuffer;
    int SimpleDPPErrorCnt;
    int SimpleDPPRevState;


    constexpr static int SEND_START = 0;
    constexpr static int SENDING = 1;
    int send_stage=SEND_START; //0:start, 1:sending,only used in send_datas()
private:
    void SimpleDPPRecvInnerCallback(){
        emit RecvCallback(revBuffer);
        revBuffer.clear();
    }

    void SimpleDPPRevErrorInnerCallback(SimpleDPPERROR error_code){
        emit RevErrorCallback(error_code);
    }

    void send_buffer(){
        emit SendBuffer(sendBuffer);
    }
signals:
    void RecvCallback(const QByteArray &revdata);
    void RevErrorCallback(SimpleDPPERROR error_code);
    void SendBuffer(const QByteArray &senddata);
public:
    explicit SimpleDPP(QObject *parent = nullptr):QObject{parent}{
        SimpleDPPErrorCnt = 0;
        SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;
    }
    int getSimpleDPPErrorCnt(){return SimpleDPPErrorCnt;}

    void parse(const sdp_byte* data,int len){
        for(int i = 0; i < len;i++){
            parse(data[i]);
        }
    }

    void parse(const QByteArray & data){
        for(char c:data){
            parse(c);
        }
    }

    void parse(sdp_byte c){
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
                revBuffer.append(c);
                break;
            }
            break;
        case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
            if (containSimpleDPPCtrolByte(c))
            {
                revBuffer.append(c);
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


    int send(const sdp_byte *data, int len){
        int i;
        //1. empty buffer
        sendBuffer.clear();
        //2. push SHO
        sendBuffer.append(SOH);

        for (i = 0; i < len; i++)
        {
            //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
            if (containSimpleDPPCtrolByte(data[i]))
            {
                // escaped control byte only 2 bytes
                sendBuffer.append(ESC);
                sendBuffer.append(data[i]);
            }
            else
            {
                sendBuffer.append(data[i]);
            }
        }
        //4. push EOT
        sendBuffer.append(EOT);
        //5. send message
        send_buffer();
        return len;
    }


public:
    /**
     * @brief must be used before send_datas_add() and send_datas_end()
     */
    void send_datas_start()
    {
        // 1. empty buffer
        sendBuffer.clear();
        // 2. push SHO
        sendBuffer.append(SOH);
    }

    /**
     * @brief must be used between send_datas_start() and send_datas_add()
     */
    void send_datas_add(const sdp_byte *data, int len)
    {
        for (int i = 0; i < len; i++)
        {
            //3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
            if (containSimpleDPPCtrolByte(data[i]))
            {
                // escaped control byte only 2 bytes
                sendBuffer.append(ESC);
                sendBuffer.append(data[i]);
            }
            else
            {
                sendBuffer.append(data[i]);
            }
        }
    }

    /**
     * @brief must be used after send_datas_start() and send_datas_add()
     */
    void send_datas_end()
    {
        //4. push EOT
        sendBuffer.append(EOT);
        //5. send message
        send_buffer();
    }

public:
    /**
     * @brief send_datas : simpledpp send data
     * @param first data object will be send.
     * @param second data object byte length.
     * @param rest variable parameter
     * @return The number of bytes actually sent
     * @example send float and string:
     * float a = 1f;
     * char[] str = "simpledpp";
     * char *p = &str[0];
     * send_datas(&a,sizeof(a),str,sizeof(str),p,sizeof(str));
     */
    template <typename First, typename Second, typename... Rest>
    int send_datas(const First &first, const Second &second, const Rest &...rest)
    {
        //if args number is not even, return SIMPLEDPP_SENDFAILED
        if (sizeof...(rest) % 2 != 0)
        {
            return SIMPLEDPP_SENDFAILED;
        }
        const sdp_byte *data = (const sdp_byte *)first;
        int len = (int)second;
        switch (send_stage)
        {
        case SEND_START:
            send_datas_start();
            send_datas_add(data, len);
            send_stage = SENDING;
            break;
        case SENDING:
            send_datas_add(data, len);
            break;
        default:
            break;
        }
        return send_datas(rest...); // 将会根据语法来递归调用
    }

private:
    int send_datas(void)
    {
        send_datas_end();
        send_stage = SEND_START;
        return sendBuffer.size();
    }
};

#endif // SIMPLEDPP_H
