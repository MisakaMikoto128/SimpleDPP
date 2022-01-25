#ifndef _SIMPLE_DPP_H_
#define _SIMPLE_DPP_H_
#include "Buffer.h"

// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2  //USING
#define SIMPLEDPP_NORMAL 0  
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11 //USING
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12 
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21 //USING
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22 //USING
#define SIMPLEDPP_CRC_CHECK_ERROR -23 

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


//default buffer size
#define SIMPLEDDP_DEFAULT_BUFFER_SIZE 1024

__attribute__((weak)) byte __send_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];
__attribute__((weak)) byte __recv_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];

// Externally provided methods
void SimpleDPP_init(int send_capacity, int recv_capacity);
void SimpleDPP_default_init();
int SimpleDPP_send(const byte *data, int len);
void SimpleDPP_parse(byte c);
int getSimpleDPPErrorCnt();
__unimplemented void SimpleDPPRecvCallback(const byte *data, int len);
__unimplemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code);
__unimplemented byte SimpleDPP_putchar(byte c);

#endif // _SIMPLE_DPP_H_