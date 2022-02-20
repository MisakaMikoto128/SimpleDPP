#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "SimpleDPP.hpp"
using namespace std;

int main(void)
{
    //1. Init Simple DPP
    SimpleDPP sdp;
    sdp.bindRecvCallback([](const std::vector<sdp_byte> &revdata)
    {
        cout << "revdata: " << endl;
        for(auto &i : revdata)
        {
            cout << i << " ";
        }
        cout << endl;
    });

    sdp.bindRevErrorCallback([](SimpleDPPERROR error_code)
    {
        cout << "error_code: " << error_code << endl;
    });

    sdp.bindSendBuffer([&sdp](const std::vector<sdp_byte> &senddata)
    {
        cout << "senddata: " << endl;
        for(auto &i : senddata)
        {
            cout << (int)i << " ";
        }
        cout << endl;
        
        sdp.parse(senddata.data(),senddata.size());
    });


    //2. send and parse one msg,msg cnn be type of char * or sdp_byte *
    char *msg = "hello worl@\\00\r\n000d";
    if (sdp.send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }
    
    //3. send and parse multiple msg,msg cnn be type of char * or sdp_byte *
    int send_len = sdp.send_datas("hello",strlen("hello"),"world",strlen("world"));
    if(send_len == 0)
    {
        cout << "send error" << endl;
    }
    else
    {
        cout << "send success" << send_len << endl;
    }
    return 0;
}