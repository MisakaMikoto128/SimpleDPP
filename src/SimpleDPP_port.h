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
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
uint32_t SimpleDPP_getMsTick(void);
#ifdef __cplusplus
}
#endif
#endif //!SIMPLEDPP_PORT_H
