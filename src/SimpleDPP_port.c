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
