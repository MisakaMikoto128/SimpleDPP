/**
 * @file SimpleDPP_port.c
 * @author Liu Yuanlin (liuyuanlins@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-28
 * @last modified 2023-09-28
 *
 * @copyright Copyright (c) 2023 Liu Yuanlin Personal.
 *
 */
#include "SimpleDPP_port.h"

#if defined(_WIN32) || defined(_WIN64)
// define something for Windows (64-bit only)
#include <windows.h>
#include <time.h>
#include <stdint.h>
uint64_t getCurrentSecTimestamp()
{
    time_t t;
    time(&t);
    return t;
}

uint64_t getCurrentMilliSecTimestamp()
{
    FILETIME file_time;
    GetSystemTimeAsFileTime(&file_time);
    uint64_t time = ((uint64_t)file_time.dwLowDateTime) + ((uint64_t)file_time.dwHighDateTime << 32);

    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    return (uint64_t)((time - EPOCH) / 10000LL);
}
#elif __APPLE__
#if TARGET_IPHONE_SIMULATOR
// iOS Simulator
#elif TARGET_OS_IPHONE
// iOS device
#elif TARGET_OS_MAC
// Other kinds of Mac OS
#endif
#elif __ANDROID__
// android
#elif __linux__
// linux
#include <sys/time.h>
#include <unistd.h>
#include <stdint.h>
uint64_t g getCurrentSecTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

uint64_t getCurrentMilliSecTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

#elif __unix__ // all unices not caught above
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown"
#endif

uint32_t SimpleDPP_getMsTick(void)
{
    // TODO:
    uint32_t msTimestamp = 0;
    msTimestamp = (uint32_t)getCurrentMilliSecTimestamp();
    return msTimestamp;
}

void *sdp_malloc(unsigned int nbytes)
{
    return malloc(nbytes);
}

void sdp_free(void *ptr)
{
    free(ptr);
}

#if SDP_MEM_WATCH_EN
static unsigned int sdp_max_mem; /* Maximum memory used*/
static unsigned int sdp_cur_mem; /* Currently used memory*/
#endif

#if SDP_MEM_WATCH_EN
void *sdp_core_malloc(unsigned int nbytes)
{
    if (nbytes + sdp_cur_mem > SDP_MEM_LIMIT_SIZE)
    { // The maximum memory limit has been exceeded.
        return NULL;
    }
    unsigned long *mem_info = (unsigned long *)sdp_malloc(nbytes + sizeof(unsigned long));
    *mem_info = nbytes;
    sdp_cur_mem += nbytes;         // Statistics of current memory usage.
    if (sdp_cur_mem > sdp_max_mem) // Record maximum memory usage.
        sdp_max_mem = sdp_cur_mem;
    return mem_info + 1;
}

void sdp_core_free(void *ptr)
{
    unsigned long *mem_info = (unsigned long *)ptr;
    unsigned long nbyte;
    if (ptr != NULL)
    {
        mem_info--;
        nbyte = *mem_info;
        sdp_cur_mem -= nbyte;
        sdp_free(mem_info);
    }
}

/**
 * @brief Get the maximum memory usage.
 */
unsigned int sdp_max_used_memory(void)
{
    return sdp_max_mem;
}

/**
 * @brief Get current memory usage.
 */
unsigned int sdp_cur_used_memory(void)
{
    return sdp_cur_mem;
}

#else

void *sdp_core_malloc(unsigned int nbytes)
{
    return sdp_malloc(nbytes);
}

void sdp_core_free(void *ptr)
{
    sdp_free(ptr);
}

#endif