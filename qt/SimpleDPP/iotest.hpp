#ifndef IOTEST_HPP
#define IOTEST_HPP

#include <QObject>
#include "simpledpp.hpp"
#include <iostream>
#include <cstring>
using namespace std;
class iotest : public QObject
{
    Q_OBJECT
    //1. Init Simple DPP
    SimpleDPP sdp;
public:
    explicit iotest(QObject *parent)
        : QObject{parent}
    {
//        connect(&sdp,&SimpleDPP::SendBuffer,this,&iotest::write);
//        connect(&sdp,&SimpleDPP::RecvCallback,this,&iotest::SimpleDPPRecvCallback);
//        connect(&sdp,&SimpleDPP::RevErrorCallback,this,&iotest::SimpleDPPRevErrorCallback);
        connect(&sdp,SIGNAL(SendBuffer(const QByteArray &)),this,SLOT(write(const QByteArray &)));
        connect(&sdp,&SimpleDPP::RecvCallback,this,SLOT(SimpleDPPRecvCallback(const QByteArray&)));
        connect(&sdp,&SimpleDPP::RevErrorCallback,this,SLOT(SimpleDPPRevErrorCallback(SimpleDPPERROR)));

//        connect(&sdp,&SimpleDPP::SendBuffer,[&](const QByteArray& writedata){
//            //
//        });

        //2. send and parse one msg,msg cnn be type of char * or byte *
        const char *msg = "hello worl@\\00\r\n000d";
        if (sdp.send(msg, strlen(msg)) == SIMPLEDPP_SENDFAILED)
        {
            printf("send error\n");
        }
        
        //3. send and parse multiple msg,msg cnn be type of char * or byte *
        int send_len = sdp.send_datas("hello",strlen("hello"),"world",strlen("world"));
        if(send_len == 0)
        {
            cout << "send error" << endl;
        }
        else
        {
            cout << "send success" << send_len << endl;
        }
    }


public slots:
    void write(const QByteArray& writedata){
        cout<<writedata.data()<<endl;
        cout<<writedata.size()<<endl;
        cout<<writedata.length()<<endl;

        sdp.parse(writedata.data(),writedata.size());
    }
    void SimpleDPPRecvCallback(const QByteArray& revdata){
        cout<<revdata.data()<<endl;
    }
    void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){
        cout<<"error"<<error_code<<endl;
    }

};

#endif // IOTEST_HPP
