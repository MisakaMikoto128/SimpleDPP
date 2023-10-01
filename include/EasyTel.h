
#ifndef _EASYTEL_H_
#define _EASYTEL_H_
/* Includes */
#include "SimpleDPP.h"
#include "cqueue.h"

#define EASYTEL_VERSION "2.0.0"
#define EASYTEL_DEBUG

#ifdef EASYTEL_DEBUG
// TODO: debug method
#endif

struct EasyTelMessage
{
    uint16_t src;
    uint16_t dst;
    uint16_t seq;
    uint8_t type;
    uint8_t *data;
    uint16_t crc;
    uint16_t len;
    uint16_t capacity; // 实际申请的内存大小为capacity+1，用于实现字符串的自动补0
};

struct tagEasyTelPoint;
typedef struct tagEasyTelPoint EasyTelPoint, *pEasyTelPoint;

typedef void (*EasyTelRevCallback_t)(EasyTelPoint *etp, struct EasyTelMessage *msg);
/* Class Structure */
/**
 * EasyTelPoint name rule:
 * etp : pointer of EasyTelPoint
 * etp_o: object of EasyTelPoint
 */
struct tagEasyTelPoint
{
    uint16_t addr; // 本机地址
    SimpleDPP sdp;
    struct EasyTelMessage curr_send_msg;
    struct EasyTelMessage curr_rev_msg;
    EasyTelRevCallback_t rev_callback;
    bool is_writeable;    // 提供给外部判断是否可以发送数据
    bool is_writing;      // 内部判断是否正在发送数据
    bool have_write_task; // 模拟写任务队列
    uint32_t write_start_time;
    uint32_t write_timeout;
    uint32_t retry_times; // 重试次数，不包含第一次发送
    uint32_t retry_times_max;
    uint16_t seq;
};


typedef struct tagEasyTelPointAdapter
{
    // Lock, used in OS environment, fill in NULL if not required.
    void (*lock)(void);
    // Unlock, used in OS environment, fill in NULL if not required.
    void (*unlock)(void);
    // Data write operation (non-blocking)
    unsigned int (*write)(const void *buf, unsigned int len);
    // Data read operation (non-blocking)
    unsigned int (*read)(void *buf, unsigned int len);
    // AT error event ( if not required, fill in NULL)
    void (*error)(void *);
    // Log output interface, which can print the complete AT interaction process, fill in NULL if not required.
    void (*debug)(const char *fmt, ...);
} EasyTelPointAdapter_t;

EasyTelPoint *EasyTel_obj_create(EasyTelPointAdapter_t *adapter, uint16_t addr);
void EasyTel_obj_destroy(EasyTelPoint *etp);
void EasyTel_obj_process(EasyTelPoint *etp);

bool EasyTel_ctx_is_writeable(EasyTelPoint *etp);
bool EasyTel_obj_send(EasyTelPoint *etp, uint16_t dst, const void *data, uint32_t len);
void EasyTel_obj_set_rev_callback(EasyTelPoint *etp, EasyTelRevCallback_t callback);
#endif // _EASYTEL_H_