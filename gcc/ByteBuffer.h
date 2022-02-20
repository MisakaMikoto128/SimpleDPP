#ifndef _BYTE_BUFFER_H_
#define _BYTE_BUFFER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stddef.h>
#define OVER_CAPACITY_ERROR -1
#ifndef byte
#define byte unsigned char
#endif



typedef struct _ByteBuffer {
    byte *data;
    int size;
    int capacity;
} ByteBuffer,*pByteBuffer;

void byte_buffer_setmemory(pByteBuffer p,byte *data,int capacity);
int byte_buffer_size(pByteBuffer p);
int byte_buffer_capacity(pByteBuffer p);
int byte_buffer_push(pByteBuffer p,byte c);
int byte_buffer_push_str(pByteBuffer p,byte *str);
int byte_buffer_push_data(pByteBuffer p,const byte *data,int len);
void byte_buffer_clear(pByteBuffer p);
#ifdef __cplusplus
}
#endif
#endif  // _BYTE_BUFFER_H_