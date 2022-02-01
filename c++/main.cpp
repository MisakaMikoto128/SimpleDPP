#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "SimpleDPP.hpp"
using namespace std;

void fun(const char *c ,int len){


}
int main(void)
{
    SimpleDPP sdp;
    sdp.bindRecvCallback([](const std::vector<byte> &revdata)
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

    sdp.bindSendBuffer([&sdp](const std::vector<byte> &senddata)
    {
        cout << "senddata: " << endl;
        for(auto &i : senddata)
        {
            cout << (int)i << " ";
        }
        cout << endl;
        
        sdp.parse(senddata.data(),senddata.size());
    });

    // sdp.send("hello",strlen("hello"));
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