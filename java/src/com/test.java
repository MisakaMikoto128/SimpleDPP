package com;

import java.io.UnsupportedEncodingException;
import java.nio.charset.StandardCharsets;

public class test {
    //test SimpleDPP
    public static void main(String[] args){
        SimpleDPP simpleDPP = new SimpleDPP();
        simpleDPP.setSimpleDPPReceiveCallback(new SimpleDPP.SimpleDPPReceiveCallback() {
            @Override
            public void receiveCallback(byte[] data) {
                System.out.println("receiveCallback");
                try {
                    System.out.println(new String(data,"UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
            }
        });

        simpleDPP.setSimpleDPPRevErrorCallback(new SimpleDPP.SimpleDPPRevErrorCallback() {
            @Override
            public void errorCallback(int errorCode) {
                System.out.println("errorCallback");
                switch (errorCode){
                    case SimpleDPP.SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END :
                        System.out.println("SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END");
                    break;

                    case SimpleDPP.SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE :
                        System.out.println("SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE");
                    break;
                    default :
                        break;
                }
            }
        });

        simpleDPP.setSimpleDPPSendBytesData(new SimpleDPP.SimpleDPPSendBytesData() {
            @Override
            public void sendBytesData(byte[] data) {
                try {
                    System.out.println(new String(data,"UTF-8"));
                } catch (UnsupportedEncodingException e) {
                    e.printStackTrace();
                }
                try {
                    simpleDPP.parse(data);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });

        simpleDPP.send("Hell0/r/n#@012");
        simpleDPP.send_datas("Hell0/r/n#@012","ssss");
    } 
}
