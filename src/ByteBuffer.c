#include "ByteBuffer.h"
void byte_buffer_setmemory(pByteBuffer p,sdp_byte *data,int capacity){
    p->data = data;
    p->capacity = capacity;
    p->size = 0;
}

int byte_buffer_size(pByteBuffer p){
    return p->size;
}

int byte_buffer_capacity(pByteBuffer p){
    return p->capacity;
}

/*
Return:
    success: 0
    fail: OVER_CAPACITY_ERROR
*/
int byte_buffer_push(pByteBuffer p,sdp_byte c){
    if(p->size < p->capacity){
        p->data[p->size++] = c;
        return 0;
    }
    return OVER_CAPACITY_ERROR;
}

/*
Return:
    success: pushed data length
    fail: OVER_CAPACITY_ERROR
*/
int byte_buffer_push_str(pByteBuffer p,sdp_byte *str){
    int i;
    for(i=0;str[i]!='\0';i++){
        if(byte_buffer_push(p,str[i])==OVER_CAPACITY_ERROR){
            return OVER_CAPACITY_ERROR;
        }
    }
    return i;
}

/*
Return:
    success: pushed data length
    fail: OVER_CAPACITY_ERROR
*/
int byte_buffer_push_data(pByteBuffer p,const sdp_byte *data,int len){
    int i;
    for(i=0;i<len;i++){
        if(byte_buffer_push(p,data[i])==OVER_CAPACITY_ERROR){
            return OVER_CAPACITY_ERROR;
        }
    }
    return i;
}

void byte_buffer_clear(pByteBuffer p){
    p->size = 0;
}