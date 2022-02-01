#ifndef _BUFFER_H_
#define _BUFFER_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stddef.h>
#define OVER_CAPACITY_ERROR -1
#define byte unsigned char

/**
 * @type type declaration
 * size,length,count -> int
 */
typedef struct _Buffer {
    byte *data;
    int size;
    int capacity;
} Buffer,*pBuffer;

void buffer_setmemory(pBuffer p,byte *data,int capacity);
int buffer_size(pBuffer p);
int buffer_capacity(pBuffer p);
int buffer_push(pBuffer p,byte c);
int buffer_push_str(pBuffer p,byte *str);
int buffer_push_data(pBuffer p,const byte *data,int len);
void buffer_clear(pBuffer p);
#ifdef __cplusplus
}
#endif
#endif  // _BUFFER_H_