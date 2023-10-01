/**
* @file cqueue.h
* @author Liu Yuanlin (liuyuanlins@outlook.com)
* @brief
* @version 0.1
* @date 2023-09-13
* @last modified 2023-09-13
*
* @copyright Copyright (c) 2023 Liu Yuanlin Personal.
*
*/
#ifndef CQUEUE_H
#define CQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "cobject.h"
    /*
    --> rear | | | | | | | font-->
    */

    typedef struct
    {
        uint32_t Capacity;
        uint32_t Front;
        uint32_t Rear;
        uint32_t ItemSize;
        CObject_t *pData;
    } CQueue_t;

    typedef CQueue_t *CQueue;

    uint8_t cqueue_create(CQueue Q, CObject_t pBuf, uint32_t uiQueueLength, uint32_t uiItemSize);
    uint8_t cqueue_enqueue(CQueue Q, const CObject_t pX);
    uint8_t cqueue_dequeue(CQueue Q, CObject_t pX);
    uint32_t cqueue_in(CQueue Q, const CObject_t pBuf, uint32_t bufSize);
    uint32_t cqueue_out(CQueue Q, CObject_t pBuf, uint32_t bufSize);

// 返回队列容量：元素个数
#define cqueue_capacity(Q) ((Q)->Capacity)
/**
 * @brief 队列中元素个数。
 *
 * @param Q
 * @return uint32_t 队列中数据的大小。
 */
#define cqueue_size(Q) (((Q)->Rear - (Q)->Front + (Q)->Capacity) % (Q)->Capacity)
// 返回队列剩余容量：元素个数
#define cqueue_residual_capacity(Q) ((Q)->Capacity - cqueue_size(Q))
// 判断队列是否为空，true 为空，false 非空
#define cqueue_is_empty(Q) ((Q)->Front == (Q)->Rear)
/**
 * @brief 队列是否满了。
 *
 * @param Q
 * @return uint8_t 1为满，0不满。
 */
#define cqueue_is_full(Q) (((Q)->Rear + 1) % (Q)->Capacity == (Q)->Front)

/**
 * @brief 清空队列。
 *
 * @param Q
 */
#define cqueue_make_empty(Q) ((Q)->Front = (Q)->Rear = 0)
#ifdef __cplusplus
}
#endif
#endif //!CQUEUE_H
