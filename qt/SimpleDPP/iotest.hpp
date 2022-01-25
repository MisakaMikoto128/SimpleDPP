#ifndef IOTEST_HPP
#define IOTEST_HPP

#include <QObject>
#include "simpledpp.hpp"
#include <iostream>
using namespace std;
class iotest : public QObject
{
    Q_OBJECT
    SimpleDPP sdp;
public:
    explicit iotest(QObject *parent)
        : QObject{parent}
    {
        connect(&sdp,&SimpleDPP::SendBuffer,this,&iotest::write);
        connect(&sdp,&SimpleDPP::RecvCallback,this,&iotest::SimpleDPPRecvCallback);
        connect(&sdp,&SimpleDPP::RevErrorCallback,this,&iotest::SimpleDPPRevErrorCallback);

        sdp.send("aa",sizeof("aa")-1);
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
