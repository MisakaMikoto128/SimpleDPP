#ifndef _Array_Queue_h
#define _Array_Queue_h
#include <stdbool.h>

/*
                     ________________
        ----->Next	|				|   		----->Next
Font				|	   Queue    |      Rear
        Prev<-----	|_______________|			Prev<-----

*/
#define OVER_CAPACITY_ERROR -1
#define DEQUEUE_EMPTY_QUEUE -1
typedef unsigned char ElementType;
typedef ElementType *pElementType;
typedef struct _ArrayQueue
{
    int Capacity;
    int Front;
    int Rear;
    pElementType Data;
} ArrayQueue, *pArrayQueue;

void CreateArrayQueue(pElementType data, int capacity, pArrayQueue Q);
bool IsEmptyArrayQueue(pArrayQueue Q);
void MakeEmptyArrayQueue(pArrayQueue Q);
bool IsFullArrayQueue(pArrayQueue Q);
int ResCapacityArrayQueue(pArrayQueue Q);
int EnArrayQueue(ElementType X, pArrayQueue Q);
int DeArrayQueue(pArrayQueue Q);
ElementType Front(pArrayQueue Q);
ElementType FrontAndDeArrayQueue(pArrayQueue Q);
int nDeArrayQueue(pArrayQueue Q, int n);

int nEnArrayQueue(pElementType X, int len, pArrayQueue Q);
int SizeArrayQueue(pArrayQueue Q);
pElementType getFrontPtr(pArrayQueue Q);
pElementType getRearPtr(pArrayQueue Q);
#endif /* _Array_Queue_h */
