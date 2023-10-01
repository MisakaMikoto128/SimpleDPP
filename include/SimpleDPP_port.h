/**
 * @file SimpleDPP_port.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-28
 * @last modified 2023-09-28
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#ifndef SIMPLEDPP_PORT_H
#define SIMPLEDPP_PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
    uint32_t SimpleDPP_getMsTick(void);
    void *sdp_malloc(unsigned int nbytes);

    void sdp_free(void *ptr);

/**
 *@brief Maximum memory usage limit (Valid when AT_MEM_WATCH_EN is enabled)
 */
#define SDP_MEM_LIMIT_SIZE (1 * 1024)
/**
 *@brief Enable memory watcher.
 */
#define SDP_MEM_WATCH_EN 1u

#if SDP_MEM_WATCH_EN
    void *sdp_core_malloc(unsigned int nbytes);
    void sdp_core_free(void *ptr);
    unsigned int sdp_max_used_memory(void);
    unsigned int sdp_cur_used_memory(void);
#else
void *sdp_core_malloc(unsigned int nbytes);
void sdp_core_free(void *ptr);
#endif

#define ETP_SEND_MSG_CAPACITY 128
#define ETP_RECV_MSG_CAPACITY 128
#define ETP_ADDR 0x00
#define ETP_WRITE_TIMEOUT 1000
#define ETP_RETRY_TIMES_MAX 3

#define ETP_MSG_TYPE_DATA 0x00
#define ETP_MSG_TYPE_ACK 0x06

#ifdef __cplusplus
}
#endif
#endif //! SIMPLEDPP_PORT_H
