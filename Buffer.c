#include "Buffer.h"
void buffser_setmemory(pBuffer p,byte *data,int capacity){
    p->data = data;
    p->capacity = capacity;
    p->size = 0;
}
int buffer_size(pBuffer p){
    return p->size;
}
int buffer_capacity(pBuffer p){
    return p->capacity;
}

/*
Return:
    success: 0
    fail: SAMPLE_ERROR
*/
int buffer_push(pBuffer p,byte c){
    if(p->size < p->capacity){
        p->data[p->size++] = c;
        return 0;
    }
    return OVER_CAPACITY_ERROR;
}

/*
Return:
    success: pushed data length
    fail: SAMPLE_ERROR
*/
int buffer_push_str(pBuffer p,byte *str){
    int i;
    for(i=0;str[i]!='\0';i++){
        if(buffer_push(p,str[i])==OVER_CAPACITY_ERROR){
            return OVER_CAPACITY_ERROR;
        }
    }
    return i;
}

/*
Return:
    success: pushed data length
    fail: SAMPLE_ERROR
*/
int buffer_push_data(pBuffer p,byte *data,int len){
    int i;
    for(i=0;i<len;i++){
        if(buffer_push(p,data[i])==OVER_CAPACITY_ERROR){
            return OVER_CAPACITY_ERROR;
        }
    }
    return i;
}

void buffer_clear(pBuffer p){
    p->size = 0;
}