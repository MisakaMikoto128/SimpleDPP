#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <stdbool.h>
#define OVER_CAPACITY_ERROR -1
#define byte unsigned char
typedef struct _Buffer {
    byte *data;
    int size;
    int capacity;
} Buffer,*pBuffer;

void buffser_setmemory(pBuffer p,byte *data,int capacity);
int buffer_size(pBuffer p);
int buffer_capacity(pBuffer p);
int buffer_push(pBuffer p,byte c);
int buffer_push_str(pBuffer p,byte *str);
int buffer_push_data(pBuffer p,byte *data,int len);
void buffer_clear(pBuffer p);
#endif  // _BUFFER_H_