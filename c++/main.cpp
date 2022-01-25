#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include "SimpleDPP.hpp"
using namespace std;

void fun(const char *c ,size_t len){


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

    sdp.send("hello",strlen("hello"));


    return 0;
}