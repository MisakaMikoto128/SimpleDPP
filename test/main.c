#include "EasyTel.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "host_slave_model_io_if.h"
#include "SimpleDPP_port.h"

#define BUFFER_SIZE 1024
static uint8_t buffer[BUFFER_SIZE];

int com_fd = 0;

uint32_t SimpleDPP_write(const void *buf, unsigned int len)
{
    return host_write(com_fd, (uint8_t *)buf, len);
}

uint32_t SimpleDPP_read(void *buf, unsigned int len)
{
    return host_read(com_fd, buf, len);
}

void SimpleDPP_debug(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

static EasyTelPointAdapter_t adapter = {
    .write = SimpleDPP_write,
    .read = SimpleDPP_read,
    .debug = SimpleDPP_debug,
    .error = NULL,
    .lock = NULL,
    .unlock = NULL,
};

void com_test()
{

    host_open(com_fd);
    uint32_t tick = SimpleDPP_getMsTick();

    while (1)
    {
        if (SimpleDPP_getMsTick() - tick > 1000)
        {
            tick = SimpleDPP_getMsTick();
            host_write(com_fd, "hello world", 11);
            if (host_read(com_fd, buffer, 1024) > 0)
            {
                printf("recv data: %s\n", buffer);
            }
        }
        Sleep(1);
    }
}

void EasyTelRevCallback(EasyTelPoint *etp, struct EasyTelMessage *msg)
{
    printf("[ETP] recv data: %s\n", msg->data);
}

int main(char argc, char *argv[])
{
    uint16_t addr = 1;
    uint16_t dst_addr = 2;
    //-c 0 -s 0x1234 -d 0xffff 代表打开fd为0的串口，本机地址为0x1234, 目标地址为0xffff
    // .\SimpleDPP.exe -c 0 -s 1 -d 2
    //.\SimpleDPP.exe -c 1 -s 2 -d 1
    if (argc < 7)
    {
        printf("Usage: %s -c <com_fd> -s <src_addr> -d <dst_addr>\n", argv[0]);
        return -1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-c") == 0)
        {
            com_fd = strtol(argv[i + 1], NULL, 10);
        }
        else if (strcmp(argv[i], "-s") == 0)
        {
            addr = strtol(argv[i + 1], NULL, 16);
        }
        else if (strcmp(argv[i], "-d") == 0)
        {
            dst_addr = strtol(argv[i + 1], NULL, 16);
        }
    }

    printf("com_fd: %d, src_addr: %#x, dst_addr: %#x\n", com_fd, addr, dst_addr);

    char send_data[128] = {0};

    SetConsoleOutputCP(65001);
    // com_test();
    host_open(com_fd);
    EasyTelPoint *etp = EasyTel_obj_create(&adapter, addr);
    EasyTel_obj_set_rev_callback(etp, EasyTelRevCallback);
    uint32_t tick = 0;

    while (1)
    {
        if (SimpleDPP_getMsTick() - tick > 1000)
        {
            tick = SimpleDPP_getMsTick();
            snprintf(send_data, 128, "hello world %u", tick);
            EasyTel_obj_send(etp, dst_addr, send_data, strlen(send_data));
        }
        EasyTel_obj_process(etp);
        Sleep(1);
    }
    return 0;
}
