/**
* @file cqueue.c
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-09-13
* @last modified 2023-09-13
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#include "cqueue.h"

/**
 * @brief 创建一个静态的循环数组队列。
 *
 * @param Q 队列对象的指针。
 * @param pBuf 队列数据存放内存指针。
 * @param uiQueueLength 队列中允许存放的最大元素个数。
 * @param uiItemSize 队列中每个元素的大小。
 * @return uint8_t 1成功，0失败。
 */
uint8_t cqueue_create(CQueue Q, CObject_t pBuf, uint32_t uiQueueLength, uint32_t uiItemSize)
{
    Q->Capacity = uiQueueLength;
    Q->pData = pBuf;
    Q->ItemSize = uiItemSize;

    // Make empty
    cqueue_make_empty(Q);
	return 0;
}

/**
 * @brief 队列入队一个元素。
 *
 * @param Q
 * @param obj 指向入队元素对象的指针。
 * @return uint8_t 1入队成功，0入队失败队列满了。
 */
uint8_t cqueue_enqueue(CQueue Q, const CObject_t obj)
{
    uint8_t ret = 0;
    if (!cqueue_is_full(Q))
    {
        memcpy(Q->pData + Q->Rear * Q->ItemSize, obj, Q->ItemSize);
        Q->Rear = (Q->Rear + 1) % Q->Capacity;
        ret = 1;
    }
    return ret;
}

/**
 * @brief 队列出队一个元素。
 *
 * @param Q
 * @param obj 指向出队元素对象的指针。
 * @return uint8_t 1出队成功，0出队失败队列为空。
 */
uint8_t cqueue_dequeue(CQueue Q, CObject_t obj)
{
    uint8_t ret = 0;
    if (!cqueue_is_empty(Q))
    {
        // font
        memcpy(obj, Q->pData + Q->Front * Q->ItemSize, Q->ItemSize);
        // dequeue
        Q->Front = (Q->Front + 1) % Q->Capacity;
        ret = 1;
    }

    return ret;
}

/**
 * @brief 向队列中入队一段数据。
 *
 * @param Q
 * @param pBuf 指向入队元素内存的指针。
 * @param bufSize 入队元素的个数。
 * @return uint32_t 实际入队的元素个数。
 */
uint32_t cqueue_in(CQueue Q, const CObject_t pBuf, uint32_t bufSize)
{
    uint32_t ret = 0;

    while (!cqueue_is_full(Q) && bufSize > 0)
    {
        memcpy(Q->pData + Q->Rear * Q->ItemSize, pBuf, Q->ItemSize);
        Q->Rear = (Q->Rear + 1) % Q->Capacity;
        bufSize--;
        ret++;
    }

    return ret;
}

/**
 * @brief 将队列中一部分元素出队
 *
 * @param Q
 * @param pBuf 指向出存放出队元素内存的指针。
 * @param bufSize 希望出队的元素个数。
 * @return uint32_t 实际出队的元素个数。
 */
uint32_t cqueue_out(CQueue Q, CObject_t pBuf, uint32_t bufSize)
{
    uint32_t ret = 0;

    while (!cqueue_is_empty(Q) && bufSize > 0)
    {
        // font
        memcpy(pBuf, Q->pData + Q->Front * Q->ItemSize, Q->ItemSize);
        pBuf += Q->ItemSize;
        // dequeue
        Q->Front = (Q->Front + 1) % Q->Capacity;
        bufSize--;
        ret++;
    }

    return ret;
}
