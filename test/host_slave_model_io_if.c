#include "host_slave_model_io_if.h"

#include <stdio.h>
#include <windows.h>
#include <string.h>

const char *com_fd_list[2] = {"COM2", "COM4"};

HANDLE hComList[2] = {INVALID_HANDLE_VALUE, INVALID_HANDLE_VALUE}; // 串口句柄

#if 1 // 开启DEBUG打印
#define LOGD(...) printf(__VA_ARGS__)
#else // 关闭DEBUG打印
#define LOGD(...)
#endif

#if 1 // 开启ERROR打印
#define LOGE(...) printf(__VA_ARGS__)
#else // 关闭ERROR打印
#define LOGE(...)
#endif

// 缓冲区大小
#define BUF_SIZE 2048
#define EXIT_STR "exit"
#define I_EXIT "I exit.\r\n"
#define I_RECEIVE "I receive.\r\n"

// 打开串口
HANDLE OpenSerial(const char *com, // 串口名称，如COM1，COM2
                  int baud,        // 波特率：常用取值：CBR_9600、CBR_19200、CBR_38400、CBR_115200、CBR_230400、CBR_460800
                  int byteSize,    // 数位大小：可取值7、8；
                  int parity,      // 校验方式：可取值NOPARITY、ODDPARITY、EVENPARITY、MARKPARITY、SPACEPARITY
                  int stopBits)    // 停止位：ONESTOPBIT、ONE5STOPBITS、TWOSTOPBITS；
{
    DCB dcb;
    BOOL b = FALSE;
    COMMTIMEOUTS CommTimeouts;
    HANDLE comHandle = INVALID_HANDLE_VALUE;

    // 打开串口
    comHandle = CreateFileA(com,                          // 串口名称
                            GENERIC_READ | GENERIC_WRITE, // 可读、可写
                            0,                            // No Sharing
                            NULL,                         // No Security
                            OPEN_EXISTING,                // Open existing port only
                            FILE_ATTRIBUTE_NORMAL,        // Non Overlapped I/O
                            NULL);                        // Null for Comm Devices

    if (INVALID_HANDLE_VALUE == comHandle)
    {
        LOGE("CreateFile fail\r\n");
        return comHandle;
    }

    // 设置读写缓存大小
    b = SetupComm(comHandle, BUF_SIZE, BUF_SIZE);
    if (!b)
    {
        LOGE("SetupComm fail\r\n");
    }

    // 设定读写超时
    CommTimeouts.ReadIntervalTimeout = MAXDWORD;   // 读间隔超时
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;   // 读时间系数
    CommTimeouts.ReadTotalTimeoutConstant = 0;     // 读时间常量
    CommTimeouts.WriteTotalTimeoutMultiplier = 1;  // 写时间系数
    CommTimeouts.WriteTotalTimeoutConstant = 1;    // 写时间常量
    b = SetCommTimeouts(comHandle, &CommTimeouts); // 设置超时
    if (!b)
    {
        LOGE("SetCommTimeouts fail\r\n");
    }

    // 设置串口状态属性
    GetCommState(comHandle, &dcb);     // 获取当前
    dcb.BaudRate = baud;               // 波特率
    dcb.ByteSize = byteSize;           // 每个字节有位数
    dcb.Parity = parity;               // 无奇偶校验位
    dcb.StopBits = stopBits;           // 一个停止位
    b = SetCommState(comHandle, &dcb); // 设置
    if (!b)
    {
        LOGE("SetCommState fail\r\n");
    }

    return comHandle;
}

/**
 * @brief 主机IO初始化。
 *
 * @param fd
 * @return uint8_t 打开成功0，失败>0
 */
uint8_t host_open(uint32_t fd)
{
    BOOL b = FALSE;
    DWORD wRLen = 0;
    DWORD wWLen = 0;
    char buf[BUF_SIZE] = {0};
    hComList[fd] = INVALID_HANDLE_VALUE; // 串口句柄

    // 打开串口
    const char *com = com_fd_list[fd];
    hComList[fd] = OpenSerial(com, CBR_115200, 8, NOPARITY, ONESTOPBIT);
    if (INVALID_HANDLE_VALUE == hComList[fd])
    {
        LOGE("OpenSerial %s fail\r\n", com);
    }
    else
    {
        LOGD("Open %s success\r\n", com);
    }

    return 0;
}

/**
 * @brief 关闭主机IO
 *
 * @param fd
 * @return uint8_t 关闭成功0，失败>0
 */
uint8_t host_close(uint32_t fd)
{

    return CloseHandle(hComList[fd]);
}

/**
 * @brief 主机写操作
 *
 * @param fd 文件描述符
 * @param writeBuf 存放待写数据缓存区的指针
 * @param uLen 需要写多少个字节
 * @return uint32_t >0-写出去实际字节数，0-写失败
 */
uint32_t host_write(uint32_t fd, uint8_t *writeBuf, uint32_t uLen)
{

    DWORD wRLen = 0;
    DWORD wWLen = 0;
    DWORD dwErrorFlags;   // 错误标志
    COMSTAT comStat;      // 通讯状态
    OVERLAPPED m_osWrite; // 异步输入输出结构体

    // 创建一个用于OVERLAPPED的事件处理，不会真正用到，但系统要求这么做
    memset(&m_osWrite, 0, sizeof(m_osWrite));
    m_osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, "WriteEvent");

    BOOL bWriteStat = WriteFile(hComList[fd], writeBuf, uLen, &wWLen, &m_osWrite);
    if (!bWriteStat)
    {
        if (GetLastError() == ERROR_IO_PENDING) // 如果串口正在写入
        {
            WaitForSingleObject(m_osWrite.hEvent, 1000); // 等待写入事件1秒钟
        }
        else
        {
            ClearCommError(hComList[fd], &dwErrorFlags, &comStat); // 清除通讯错误
            CloseHandle(m_osWrite.hEvent);                         // 关闭并释放hEvent内存
            return 0;
        }
    }
    return wWLen;
}

/**
 * @brief 主机读操作
 *
 * @param fd 文件描述符
 * @param pBuf 存放读取数据的缓存区的指针
 * @param uiLen 本次操作最多能读取的字节数
 * @return uint32_t >0-实际读取的字节数，0-没有数据
 */
uint32_t host_read(uint32_t fd, uint8_t *pBuf, uint32_t uiLen)
{
    // 异步方式
    DWORD wCount = 0;    // 成功读取的数据字节数
    DWORD dwErrorFlags;  // 错误标志
    COMSTAT comStat;     // 通讯状态
    OVERLAPPED m_osRead; // 异步输入输出结构体

    // 创建一个用于OVERLAPPED的事件处理，不会真正用到，但系统要求这么做
    memset(&m_osRead, 0, sizeof(m_osRead));
    m_osRead.hEvent = CreateEvent(NULL, TRUE, FALSE, "ReadEvent");

    ClearCommError(hComList[fd], &dwErrorFlags, &comStat); // 清除通讯错误，获得设备当前状态
    if (!comStat.cbInQue)
        return 0; // 如果输入缓冲区字节数为0，则返回false
    // std::cout << comStat.cbInQue << std::endl;
    BOOL bReadStat = ReadFile(hComList[fd], // 串口句柄
                              pBuf,         // 数据首地址
                              uiLen,        // 要读取的数据最大字节数
                              &wCount,      // DWORD*,用来接收返回成功读取的数据字节数
                              &m_osRead);   // NULL为同步发送，OVERLAPPED*为异步发送
    if (!bReadStat)
    {
        if (GetLastError() == ERROR_IO_PENDING) // 如果串口正在读取中
        {
            // GetOverlappedResult函数的最后一个参数设为TRUE
            // 函数会一直等待，直到读操作完成或由于错误而返回
            GetOverlappedResult(hComList[fd], &m_osRead, &wCount, TRUE);
        }
        else
        {
            ClearCommError(hComList[fd], &dwErrorFlags, &comStat); // 清除通讯错误
            CloseHandle(m_osRead.hEvent);                          // 关闭并释放hEvent的内存
            return 0;
        }
    }

    return wCount;
}

/**
 * @brief 主机当前可以读取的字节数。
 *
 * @param fd 文件描述符
 * @return uint32_t
 */
uint32_t host_available_bytes(uint32_t fd)
{
    // 异步方式
    DWORD wCount = 0;    // 成功读取的数据字节数
    DWORD dwErrorFlags;  // 错误标志
    COMSTAT comStat;     // 通讯状态
    OVERLAPPED m_osRead; // 异步输入输出结构体

    ClearCommError(hComList[fd], &dwErrorFlags, &comStat); // 清除通讯错误，获得设备当前状态

    return comStat.cbInQue;
}