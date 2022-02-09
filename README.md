# 简单报文封装协议SimpleDPP
Simple Datagram Pack Protocol

## 简介
SimpleDPP提供了一套完整的二进制数据封装成帧和解析数据帧方法，其特点是会在每一帧的解析结束时调用一个回调函数，以便于用户处理数据。SimpleDPP会检查收中可能出现的错误，调用相应回调函数，并且会记录错误的次数。
Note: SimpleDPP在现在的版本中处于用途简单和性能的考量未实现CRC校验，当然对于可能需要CRC才能检查出来的错误出现的概率也非常小，如果不是对安全性要求高的场合大可放心使用。

## 注意
1. 线程不安全，发送和接收处理只能在一个线程中完成


## 支持
**C++**：C++11以上标准
**GCC**: C99以上标准
**C**&ensp;&emsp;: C89以上标准


# 使用：
## C语言版本(GCC)
- Step1 包含头文件
```c
#include "SimpleDPP.h"
```
- Step2 初始化SimpleDPP

```c
SimpleDPP_init();
```

- Step3 实现发送和接收的回调函数，可以根据需要实现，例如你只要发送消息那么就只需要实现`byte SimpleDPP_putchar(byte c)`回调方法。
```c
__implemented void SimpleDPPRecvCallback(const byte *data, int len)
{
    //the data is exactly the same as the data sent by the sender

}
__implemented byte SimpleDPP_putchar(byte c)
{
    // eg. using SimpleDPP in stm32
    UartSendChar(c);
    return c;
}

__implemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code){

}
```

- Step4 发送数据
    - 发送单段数据
    ```c
    if (SimpleDPP_send("msg", strlen("msg")) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }
    ```
    - 发送多段数据,这可以用于封装自定义数据包的头部之类的场景。
    ```c
    if(SimpleDPP_send_datas("msg1", 4,"msg2",4) == SIMPLEDPP_SENDFAILED)
    {
        printf("send error\n");
    }
    ```



# 实现

## 帧填充方式
![在这里插入图片描述](https://img-blog.csdnimg.cn/23c1019c60f94904a6c1451e74ac32af.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5Za15Za16ZSk6ZSk5L2g5bCP5Y-v54ix,size_20,color_FFFFFF,t_70,g_se,x_16)

## 特性
1. 通信双方事先知道发送/接收的最大长度(不需要知道具体长度，长度可变)。
2. 每帧的最大长度没有限制。
3. 可传输任意类型的数据。
4. 能够校验数据，校验失败直接丢弃。
5. 一次数据传输过程就是一次帧传输过程。
6. 有起始标志、结束标志、转义标志，能够对数据帧中与起始标志、结束标志相同的数据进行转义。
7. 自动识别传输过程中只传输了一部分的帧并丢弃。
8. 每帧数据发送完成产生一个发送完成事件。每帧数据接收完成才产生一个接收事件，在此之前不允许产生事件。
9. 每帧间隔事件即双方处理数据的时间。通信双方保证帧发送频率小于数据处理频率。
10. 理想情况下连接设备至多造成**两帧**错误判断为一帧，造成接收错误，这会被CRC检验出来，**系统的安全性依赖于CRC校验的方式**。