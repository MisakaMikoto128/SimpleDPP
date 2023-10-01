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

    return host_write(com_fd,(uint8_t*)buf, len);
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

int main(void)
{
    SetConsoleOutputCP(65001);
    // com_test();
    host_open(com_fd);
    EasyTelPoint *etp = EasyTel_obj_create(&adapter, 0x1234);
    EasyTel_obj_set_rev_callback(etp, EasyTelRevCallback);
    uint32_t tick = 0;

    while (1)
    {
        if (SimpleDPP_getMsTick() - tick > 1000)
        {
            EasyTel_obj_send(etp, 0x02, "heaaaa", 11);
        }
        EasyTel_obj_process(etp);
        Sleep(1);
    }
    return 0;
}
