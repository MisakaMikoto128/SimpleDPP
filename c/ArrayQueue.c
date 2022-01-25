#include "ArrayQueue.h"
/**
 * This queue is a circular queue.
 * 实现循环队列最关键的部分是确定队列何时为空何时满。在用数组实现的非循环队列中，队满的判断条件是rear == n，
 * 队空的判断条件是head == tail。在循环队列中，队列为空的判断条件仍然是head ==rear，
 * 但队列满的判断条件有所更改，是( rear + 1 ) % capacity == head。
 * 另外一点需要注意的是，当队列满时，rear指针指向的位置实际上是没有存储数据的，所以会浪费一个数组的存储空间。
 */

/**
* @brief: 创建队列,分配内存 Create a circular array queue,
* allocate memory by pass a array.the actual capacity of the queue is param capacity - 1.
* @param: data: the array to store the queue data.
* 
*/
void CreateArrayQueue(pElementType data, int capacity, pArrayQueue Q)
{
    Q->Data = data;
    Q->Capacity = capacity;
    Q->Front = 0;
    Q->Rear = 0;
}

/**
 * @brief Check if the circular array queue is empty.
 * 
 * @param Q Pointer of the queue.
 * @return true if the queue is empty.
 * @return false else.
 */
inline bool IsEmptyArrayQueue(pArrayQueue Q)
{
    return Q->Front == Q->Rear;
}
/**
 * @brief Make the circular array queue empty.
 * 
 * @param Q Pointer of the queue.
 */
void MakeEmptyArrayQueue(pArrayQueue Q)
{
    Q->Front = 0;
    Q->Rear = 0;
}

/**
 * @brief Check whether the circular array queue is full.(rear + 1 ) % capacity == head。
 * 
 * @param Q Pointer of the queue.
 * @return true is the queue is full.
 * @return false else.
 */
inline bool IsFullArrayQueue(pArrayQueue Q)
{
    return (Q->Rear + 1) % Q->Capacity == Q->Front;
}
/**
 * @brief Get the remaining capacity of the circular array queue.
 * 
 * @param Q Pointer of the queue.
 * @return int the remaining capacity.
 */
inline int ResCapacityArrayQueue(pArrayQueue Q)
{
    return Q->Capacity - Q->Rear + Q->Front - 1;
}
/**
 * @brief get the actural number of elements in the circular array queue bewteen front and rear.
 * 
 * @param Q Pointer of the queue.
 * @return int the actual number of elements in the queue.
 */
inline int SizeArrayQueue(pArrayQueue Q)
{
    return (Q->Rear - Q->Front + Q->Capacity) % Q->Capacity;
}

/**
 * @brief Enqueue an element to the circular array queue.If the circular array queue is full,return OVER_CAPACITY_ERROR.
 * 
 * @param X The element to be enqueued.
 * @param Q Pointer of the queue.
 * @return int success: 0
 * @return int fail: OVER_CAPACITY_ERROR
 */
int EnArrayQueue(ElementType X, pArrayQueue Q)
{
    if (IsFullArrayQueue(Q))
    {
        return OVER_CAPACITY_ERROR;
    }
    Q->Data[Q->Rear] = X;
    Q->Rear = (Q->Rear + 1) % Q->Capacity;
    return 0;
}

/**
 * @brief Dequeue an element from the circular array queue.
 * 
 * @param Q Pointer of the queue.
 * @return int success: 0
 * @return int fail: DEQUEUE_EMPTY_QUEUE
 */
int DeArrayQueue(pArrayQueue Q)
{
    if (IsEmptyArrayQueue(Q))
    {
        return DEQUEUE_EMPTY_QUEUE;
    }
    Q->Front = (Q->Front + 1) % Q->Capacity;
    return 0;
}

/**
 * @brief Accquire the front element of the queue.
 * 
 * @param Q Pointer of the queue.
 * @return ElementType The front element.
 */
inline ElementType Front(pArrayQueue Q)
{
    return Q->Data[Q->Front];
}

/**
 * @brief Accquire the front element of the queue and dequeue it.
 * 
 * @param Q Accquire the front element of the queue.
 * @return ElementType Element of the front element.
 */
ElementType FrontAndDeArrayQueue(pArrayQueue Q)
{
    ElementType X = Q->Data[Q->Front];
    Q->Front = (Q->Front + 1) % Q->Capacity;
    return X;
}

/**
 * @brief Dequeue n elements from the queue.If n is larger than the size of the queue, the queue will be empty.
 * 
 * @param Q Pointer of the queue.
 * @param n The number of elements to be dequeued.
 * @return int The actual number of elements dequeued.
 */
int nDeArrayQueue(pArrayQueue Q, int n)
{
    int size = SizeArrayQueue(Q);
    if (n > size)
    {
        MakeEmptyArrayQueue(Q);
        return size;
    }
    Q->Front = (Q->Front + n) % Q->Capacity;
    return n;
}

/**
 * @brief Enqueue n elements to the circular array queue.
 * 
 * @param X The element data array pointer.
 * @param len The length of the element data array.
 * @param Q Pointer of the circular array queue.
 * @return int The actual number of elements enqueued.
 */
int nEnArrayQueue(pElementType X, int len, pArrayQueue Q)
{
    int i;
    for (i = 0; i < len; i++)
    {
        if (EnArrayQueue(X[i], Q) == OVER_CAPACITY_ERROR)
        {
            break;
        }
    }
    return i;
}
/**
 * @brief Get the Front Ptr object
 * 
 * @param Q Pointer of the queue.
 * @return pElementType 
 */
inline pElementType getFrontPtr(pArrayQueue Q)
{
    return Q->Data + Q->Front;
}

/**
 * @brief Get the Rear Ptr object
 * 
 * @param Q Pointer of the queue.
 * @return pElementType 
 */
inline pElementType getRearPtr(pArrayQueue Q)
{
    return Q->Data + Q->Rear;
}

/*
Test the circular array queue.
*/
/*
#include <stdlib.h>
#include <stdio.h>
int main()
{
    int i;
    int capacity = 10;
    int len = 10;

    ElementType *X = (ElementType *)malloc(sizeof(ElementType) * len);
    for (i = 0; i < len; i++)
    {
        X[i] = i;
    }
    pArrayQueue Q = (pArrayQueue)malloc(sizeof(ArrayQueue));
    CreateArrayQueue(X, capacity, Q);

    for (i = 0; i < len; i++)
    {
        if (EnArrayQueue(X[i], Q) != OVER_CAPACITY_ERROR)
        {
            printf("IsEmptyArrayQueue: %d\n", IsEmptyArrayQueue(Q));
            printf("IsFullArrayQueue: %d\n", IsFullArrayQueue(Q));
            printf("ResCapacityArrayQueue: %d\n", ResCapacityArrayQueue(Q));
            printf("SizeArrayQueue: %d\n", SizeArrayQueue(Q));
            printf("\n");
        }
        else
        {
            printf("OVER_CAPACITY_ERROR\n");
        }
    }

     printf("---------------------------------------------------------------\n");
    for (i = 0; i < len; i++)
    {
        if (DeArrayQueue(Q) != DEQUEUE_EMPTY_QUEUE)
        {
            printf("IsEmptyArrayQueue: %d\n", IsEmptyArrayQueue(Q));
            printf("IsFullArrayQueue: %d\n", IsFullArrayQueue(Q));
            printf("ResCapacityArrayQueue: %d\n", ResCapacityArrayQueue(Q));
            printf("SizeArrayQueue: %d\n", SizeArrayQueue(Q));
        }
        else
        {
            printf("DEQUEUE_EMPTY_QUEUE\n");
        }
    }
    printf("---------------------------------------------------------------\n");
    return 0;
}
*/
