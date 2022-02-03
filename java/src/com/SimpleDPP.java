package com;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class SimpleDPP {
    // define SimpleDPP receive error code
    // level 0:
    private static final int SIMPLEDPP_RECEIVE_ERROR = -1;
    private static final int SIMPLEDPP_SENDFAILED = -2;//USING,SEND ONLY USING THIS ERROR CODE
    private static final int SIMPLEDPP_NORMAL = 0;
    // level 1:
    private static final int SIMPLEDPP_ERROR_REV_OVER_CAPACITY = -11;
    private static final int SIMPLEDPP_ERROR_SEND_OVER_CAPACITY = -12;
    // level 2:
    public static final int SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END = -21;
    public static final int SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE = -22;
    public static final int SIMPLEDPP_CRC_CHECK_ERROR = -23;
    // SimpleDPP receive state machine's states
    private static final int SIMPLEDPP_REV_WAIT_START = 0;
    private static final int SIMPLEDPP_REV_WAIT_END = 1;
    private static final int SIMPLEDPP_REV_WAIT_CTRL_BYTE = 2;

    // SimpleDPP frame control byte (The frame delimiter)
    private static final byte SOH = 0x01; // DEC: 1
    private static final byte EOT = 0x04; // DEC: 4
    private static final byte ESC = 0x18;// DEC: 27

    private List<Byte> sendBuffer = new ArrayList<Byte>();
    private List<Byte> revBuffer = new ArrayList<Byte>();
    // private ByteBuffer sendBuffer
    // private ByteBuffer revBuffer;
    private SimpleDPPSendBytesData simpleDPPSendBytesData;
    private SimpleDPPReceiveCallback simpleDPPReceiveCallback;
    private SimpleDPPRevErrorCallback SimpleDPPRevErrorCallback;

    private int SimpleDPPErrorCnt = 0;
    private int SimpleDPPRevState = SIMPLEDPP_REV_WAIT_START;

    private static boolean containSimpleDPPCtrolByte(byte c) {
        return ((c) == SOH || (c) == EOT || (c) == ESC);
    }

    public SimpleDPP() {
    }

    /**
     * <p>
     * Simple DPP send msg.
     * </p>
     *
     * @param data byte data you will be sent.
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @see #send(String str_data)
     */
    public int send(byte[] data) {

        send_datas_start();
        send_datas_add(data);
        send_datas_end();
        return sendBuffer.size();
    }

    /**
     * <p>
     * Simple DPP send string msg.
     * </p>
     *
     * @param data string data you will be sent.
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @see #send(byte[] data)
     */
    public int send(Charset charset,String data) {
        return send(data.getBytes(charset));
    }

    /**
     * <p>
     * Simple DPP send string msg,default charset is UTF-8.
     * </p>
     *
     * @param data send string data with the default encoding UTF-8
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @see #send(byte[] data)
     */
    public int send(String data) {
        return send(data.getBytes(StandardCharsets.UTF_8));
    }


    /**
     * <p>
     * must be used before send_datas_add() and send_datas_end()
     * </p>
     */
    public void send_datas_start() {
        // 1. empty buffer
        sendBuffer.clear();
        // 2. push SHO
        sendBuffer.add(SOH);
    }

    
    /**
     * <p>
     * must be used between send_datas_start() and send_datas_add()
     * </p>
     */
    public void send_datas_add(byte[] data) {
        for (byte datum : data) {
            // 3. push message body,when encounter SOH,EOT or ESC,using ESC escape it.
            if (containSimpleDPPCtrolByte(datum)) {
                sendBuffer.add(ESC);
                sendBuffer.add(datum);
            } else {
                sendBuffer.add(datum);
            }
        }
    }

    /**
     * <p>
     * must be used after send_datas_start() and send_datas_add()
     * </p>
     */
    public void send_datas_end() {
        // 4. push EOT
        sendBuffer.add(EOT);
        /*
         * 5. send message
         * TODO: Here the sendBuffer allocate a new memory and copy all data from
         * sendBuffer to new Byte array,need to optimize.
         */
        simpleDPPSendBytesData.sendBytesData(ByteListToArray(sendBuffer));
    }

    /**
     * <p>
     * Simple DPP send multiple messages.
     * </p>
     *
     * @param datas... byte[].. datas you will be sent.
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @examples SimpleDPP.send_datas(bytes1,bytes2,bytes3,...);
     */
    public int send_datas(byte[]... datas) {
        send_datas_start();
        for (byte[] data : datas) {
            send_datas_add(data);
        }
        send_datas_end();
        return sendBuffer.size();
    }


    
    /**
     * <p>
     * Simple DPP send multiple messages.
     * </p>
     *
     * @param str_datas... string datas you will be sent.
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @examples SimpleDPP.send_datas(StandardCharsets.UTF_8,"Hello","World!",...);
     */
    public int send_datas(Charset charset,String... str_datas) {
        send_datas_start();
        for (String str : str_datas) {
            
            send_datas_add(str.getBytes(charset));
        }
        send_datas_end();
        return sendBuffer.size();
    }

        /**
     * <p>
     * Simple DPP send multiple messages,using default charset is UTF-8.
     * </p>
     *
     * @param str_datas... string datas you will be sent.
     * @return success: send data bytes length
     *         fail: SIMPLEDPP_SENDFAILED
     * @examples SimpleDPP.send_datas("Hello","World!",...);
     */
    public int send_datas(String... str_datas) {
        send_datas_start();
        for (String str : str_datas) {
            send_datas_add(str.getBytes(Charset.forName("UTF-8")));
        }
        send_datas_end();
        return sendBuffer.size();
    }



    /**
     * <p>
     * SimpleDPP receive state machine's states
     * </p>
     *
     * @param c byte data you received.
     * @see #parse(byte[] data)
     */
    public void parse(byte c) throws Exception {
        switch (SimpleDPPRevState) {
            case SIMPLEDPP_REV_WAIT_START:
                if (c == SOH) {
                    SimpleDPPRevState = SIMPLEDPP_REV_WAIT_END;
                }
                break;
            case SIMPLEDPP_REV_WAIT_END:
                switch (c) {
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
                        revBuffer.add(c);
                        break;
                }
                break;
            case SIMPLEDPP_REV_WAIT_CTRL_BYTE:
                if (containSimpleDPPCtrolByte(c)) {
                    revBuffer.add(c);
                    SimpleDPPRevState = SIMPLEDPP_REV_WAIT_END;
                } else {
                    SimpleDPPRevErrorInnerCallback(SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE);
                }
                break;
            default:
                break;
        }
    }

    /**
     * <p>
     * SimpleDPP receive state machine's states,parse byte array.
     * </p>
     *
     * @param data bytes array data you received.
     * @see #parse(byte c)
     */
    public void parse(byte[] data) throws Exception {
        for (byte datum : data) {
            parse(datum);
        }
    }

    /**
     * <p>
     * convert List<Byte> to byte[].
     * </p>
     *
     * @param list List<Byte>
     * @return byte[]
     */
    private byte[] ByteListToArray(List<Byte> list) {
        if (list == null || list.size() == 0)
            return null;
        byte[] bytes = new byte[list.size()];
        int i = 0;
        for (Byte aByte : list) {
            bytes[i] = aByte;
            i++;
        }
        return bytes;
    }

    private void SimpleDPPRecvInnerCallback() throws Exception {
        // TODO: Here the revBuffer allocate a new memory and copy all data from
        // revBuffer to new Byte array,need to optimize.
        try {
            simpleDPPReceiveCallback.receiveCallback(ByteListToArray(revBuffer));
        } catch (Exception e) {

            e.printStackTrace();
            throw e;
        } finally {
            revBuffer.clear();
        }

        revBuffer.clear();
    }

    private void SimpleDPPRevErrorInnerCallback(int error_code) throws Exception {
        try {
            SimpleDPPRevErrorCallback.errorCallback(error_code);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            revBuffer.clear();
            SimpleDPPErrorCnt++;
        }

    }

    int getSimpleDPPErrorCnt() {
        return SimpleDPPErrorCnt;
    }

    public interface SimpleDPPSendBytesData {
        public void sendBytesData(byte[] data);
    }

    public interface SimpleDPPReceiveCallback {
        public void receiveCallback(byte[] data);
    }

    public interface SimpleDPPRevErrorCallback {
        public void errorCallback(int errorCode);
    }

    //
    public void setSimpleDPPRevErrorCallback(SimpleDPPRevErrorCallback SimpleDPPRevErrorCallback) {
        this.SimpleDPPRevErrorCallback = SimpleDPPRevErrorCallback;
    }

    public void setSimpleDPPSendBytesData(SimpleDPPSendBytesData simpleDPPSendBytesData) {
        this.simpleDPPSendBytesData = simpleDPPSendBytesData;
    }

    public void setSimpleDPPReceiveCallback(SimpleDPPReceiveCallback simpleDPPReceiveCallback) {
        this.simpleDPPReceiveCallback = simpleDPPReceiveCallback;
    }
}
