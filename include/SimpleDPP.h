#ifndef _SIMPLE_DPP_H_
#define _SIMPLE_DPP_H_
#include "ByteBuffer.h"
#include "SimpleDPP_port.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/*C compiler standard support*/
// #define SIMPLEDPP_SUPPORT_C89
#define SIMPLEDPP_SUPPORT_C99
#define SIMPLEDPP_VERSION "1.1.0"

#define VAR_ARG_END ((void *)0)

// cast char * to byte *
// TODO : typedef conflict or macro pollution
#ifndef sdp_byte
#define sdp_byte unsigned char
#endif
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (sdp_byte *)(ptr)

// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2 // USING,SEND ONLY USING THIS ERROR CODE
#define SIMPLEDPP_NORMAL 0
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11 // USING
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21                // USING
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22 // USING
#define SIMPLEDPP_CRC_CHECK_ERROR -23

#define SIMPLEDPP_TIMEOUT -24 // USING

// SimpleDPP receive state machine's states
#define SIMPLEDPP_REV_WAIT_START 0
#define SIMPLEDPP_REV_WAIT_END 1
#define SIMPLEDPP_REV_WAIT_CTRL_BYTE 2

typedef int SimpleDPPERROR;
// SimpleDPP frame control byte (The frame delimiter)
#define SOH 0x01 // DEC: 1
#define EOT 0x04 // DEC: 4
#define ESC 0x18 // DEC: 27
#define containSimpleDPPCtrolByte(c) ((c) == SOH || (c) == EOT || (c) == ESC)

// A tag with an implementation function or variable
#define __unimplemented
#define __implemented

// default buffer size
#define SIMPLEDDP_DEFAULT_BUFFER_SIZE 1024
#define SIMPLEDDP_DEFAULT_FRAME_REV_TIMEOUT 500 // ms

typedef void (*SimpleDPPRecvCallback_t)(const sdp_byte *data, int len);
typedef void (*SimpleDPPRevErrorCallback_t)(SimpleDPPERROR error_code);
typedef sdp_byte (*SimpleDPP_putchar_t)(sdp_byte c);

typedef struct tagSimpleDPPAdapter
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
} SimpleDPPAdapter_t;

/* SimpleDPP Class Structure */
typedef struct SimpleDPP_
{
    ByteBuffer send_buffer;
    ByteBuffer recv_buffer;
    int SimpleDPPErrorCnt;
    int SimpleDPPRevState;
    SimpleDPPRecvCallback_t SimpleDPPRecvCallback;
    SimpleDPPRevErrorCallback_t SimpleDPPRevErrorCallback;

    uint32_t SimpleDPPFrameRevTimeout;
    uint32_t SimpleDPPFrameRevStartTick;
    const SimpleDPPAdapter_t *adapter;
} SimpleDPP, *pSimpleDPP;

// Externally provided methods
void SimpleDPP_Constructor(SimpleDPP *sdp, sdp_byte *send_buffer, int send_buffer_capacity, sdp_byte *recv_buffer, int recv_buffer_capacity, SimpleDPPRecvCallback_t SimpleDPPRecvCallback, SimpleDPPRevErrorCallback_t SimpleDPPRevErrorCallback, const SimpleDPPAdapter_t *adapter);
void SimpleDPP_Destructor(SimpleDPP *sdp);

int send_datas_start(SimpleDPP *sdp);
int send_datas_add(SimpleDPP *sdp, const sdp_byte *data, int len);
int send_datas_end(SimpleDPP *sdp);

int SimpleDPP_send(SimpleDPP *sdp, const sdp_byte *data, int len);
int __SimpleDPP_send_datas(SimpleDPP *sdp, const sdp_byte *data, int data_len, ...);
int __SimpleDPP_send_datas_n(SimpleDPP *sdp, int arg_num, const sdp_byte *data, int data_len, ...);
#ifdef SIMPLEDPP_SUPPORT_C99
#define SimpleDPP_send_datas(sdp, var_arg, ...) __SimpleDPP_send_datas(sdp, var_arg, ##__VA_ARGS__, VAR_ARG_END)
#elif defined SIMPLEDPP_SUPPORT_C89
#define SimpleDPP_send_datas __SimpleDPP_send_datas_n
#endif

void SimpleDPP_parse(SimpleDPP *sdp, sdp_byte c);
int getSimpleDPPErrorCnt(SimpleDPP *sdp);

bool SimpleDPP_isTimeout(uint32_t start, uint32_t timeout);
void SimpeDPP_poll(SimpleDPP *sdp);
static inline uint32_t SimpleDPP_getRecvBufSize(SimpleDPP *sdp)
{
    return sdp->recv_buffer.size;
}
#endif // _SIMPLE_DPP_H_