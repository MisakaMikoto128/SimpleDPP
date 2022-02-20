#ifndef _SIMPLE_DPP_H_
#define _SIMPLE_DPP_H_
#include "ByteBuffer.h"
#include <stdarg.h>
#include <stddef.h>
/*
support Arm Compiler 6/5,gcc/clang
*/
#define VAR_ARG_END ((void *)0)

//cast char * to byte *
//TODO : typedef conflict or macro pollution
#ifndef sdp_byte
#define sdp_byte unsigned char
#endif
#define CAST_CHAR_PTR_TO_BYTE_PTR(ptr) (sdp_byte *)(ptr)

// define SimpleDPP receive error code
// level 0:
#define SIMPLEDPP_RECEIVE_ERROR -1
#define SIMPLEDPP_SENDFAILED -2  // USING,SEND ONLY USING THIS ERROR CODE
#define SIMPLEDPP_NORMAL 0  
// level 1:
#define SIMPLEDPP_ERROR_REV_OVER_CAPACITY -11 //USING
#define SIMPLEDPP_ERROR_SEND_OVER_CAPACITY -12 
// level 2:
#define SIMPLEDPP_ERROR_REV_SOH_WHEN_WAIT_END -21 //USING
#define SIMPLEDPP_ERROR_REV_NONCTRL_BYTE_WHEN_WAIT_CTRL_BYTE -22 //USING
#define SIMPLEDPP_CRC_CHECK_ERROR -23 

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

// extern __attribute__((weak)) sdp_byte __send_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];
// extern __attribute__((weak)) sdp_byte __recv_data[SIMPLEDDP_DEFAULT_BUFFER_SIZE];

// Externally provided methods
void SimpleDPP_init(void);

int SimpleDPP_send(const sdp_byte *data, int len);
int __SimpleDPP_send_datas(const sdp_byte *data,int data_len,...);

int send_datas_start();
int send_datas_add(const sdp_byte *data, int len);
int send_datas_end();

// Only works in C99    
#define SimpleDPP_send_datas(van_arg,...) __SimpleDPP_send_datas(van_arg,##__VA_ARGS__,VAR_ARG_END)
void SimpleDPP_parse(sdp_byte c);
int getSimpleDPPErrorCnt();
__unimplemented void SimpleDPPRecvCallback(const sdp_byte *data, int len);
__unimplemented void SimpleDPPRevErrorCallback(SimpleDPPERROR error_code);
__unimplemented sdp_byte SimpleDPP_putchar(sdp_byte c);

#endif // _SIMPLE_DPP_H_