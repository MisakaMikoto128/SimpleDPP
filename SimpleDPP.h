#ifndef _SIMPLE_DPP_H_
#define _SIMPLE_DPP_H_
#include "Buffer.h"

// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22

//cast char * to byte *
#define byte unsigned char
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (byte *)(ptr)

// SimpleDPP receive state machine's states
#define SIMPLEDPP_REV_WAIT_START 0
#define SIMPLEDPP_REV_WAIT_END 1
#define SIMPLEDPP_REV_WAIT_CTRL_BYTE 2

typedef int SimpleDPPERROR;
// SimpleDPP frame control byte (The frame delimiter)
#define SOH 0x01 //DEC: 1
#define EOT 0x04 //DEC: 4
#define ESC 0x18 //DEC: 27
#define containSimpleDPPCtrolByte(c) ((c) == SOH || (c) == EOT || (c) == ESC)

// A tag with an implementation function or variable
#define __unimplemented
#define __implemented

// Externally provided methods
void SimpleDPP_init(byte *send_data, int send_capacity, byte *recv_data, int recv_capacity);
int SimpleDPP_send(byte *data, int len);
void SimpleDPP_parse(byte c);
int getSimpleDPPErrorCnt();
__unimplemented void SimpleDPPRecvCallback(byte *data, int len);
__unimplemented void SimpleDPPErrorCallback(SimpleDPPERROR error_code);
__unimplemented byte SimpleDPP_putchar(byte c);

#endif // _SIMPLE_DPP_H_