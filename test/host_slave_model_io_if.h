/**
 * @file host_slave_model_io_if.h
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-03
 * 
 * @copyright Copyright (c) 2022 Liu Yuanlin Personal.
 * 
 */
#ifndef HOST_SLAVE_MODEL_IO_IF_H
#define HOST_SLAVE_MODEL_IO_IF_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief 主机IO初始化。
 *
 * @param fd
 * @return uint8_t 打开成功0，失败>0
 */
uint8_t host_open(uint32_t fd);

/**
 * @brief 关闭主机IO
 * 
 * @param fd 
 * @return uint8_t 关闭成功0，失败>0
 */
uint8_t host_close(uint32_t fd);

/**
 * @brief 主机写操作
 *
 * @param fd 文件描述符
 * @param writeBuf 存放待写数据缓存区的指针
 * @param uLen 需要写多少个字节
 * @return uint32_t >0-写出去实际字节数，0-写失败
 */
uint32_t host_write(uint32_t fd, uint8_t *writeBuf, uint32_t uLen);

/**
 * @brief 主机读操作
 *
 * @param fd 文件描述符
 * @param pBuf 存放读取数据的缓存区的指针
 * @param uiLen 本次操作最多能读取的字节数
 * @return uint32_t >0-实际读取的字节数，0-没有数据
 */
uint32_t host_read(uint32_t fd, uint8_t *pBuf, uint32_t uiLen);

/**
 * @brief 主机当前可以读取的字节数。
 * 
 * @param fd 文件描述符
 * @return uint32_t 
 */
uint32_t host_available_bytes(uint32_t fd);
#ifdef __cplusplus
}
#endif
#endif //!HOST_SLAVE_MODEL_IO_IF_H