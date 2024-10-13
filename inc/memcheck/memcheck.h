/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#ifndef __MEMCHECK_H__
#define __MEMCHECK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <memcheck/memcheck_config.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#if MEMCHECK_TRACE_ON
#define MEMCHECK_TRACE(fmt, ...)                      \
    do {                                              \
        printf("[trace] " fmt "\r\n", ##__VA_ARGS__); \
    } while (0)
#else
#define MEMCHECK_TRACE(fmt, ...)
#endif /* MEMCHECK_TRACE_ON */

#if MEMCHECK_PRINT_ON
#define MEMCHECK_PRINT(fmt, ...)           \
    do {                                   \
        printf(fmt "\r\n", ##__VA_ARGS__); \
    } while (0)
#else
#define MEMCHECK_PRINT(fmt, ...)
#endif /* MEMCHECK_PRINT_ON */

#if MEMCHECK_ERROR_ON
#define MEMCHECK_ERROR(fmt, ...)                      \
    do {                                              \
        printf("[error] " fmt "\r\n", ##__VA_ARGS__); \
    } while (0)
#else
#define MEMCHECK_PRINT(fmt, ...)
#endif /* MEMCHECK_ERROR_ON */

#define MEMECHK_ADDR_VALID(addr)                                                                         \
    ((((unsigned long)addr >= MEMCEHCK_RAM_ADDR_START) && ((unsigned long)addr < MEMCHECK_RAM_ADDR_END)) \
     || (((unsigned long)addr >= MEMCEHCK_FLASH_ADDR_START) && ((unsigned long)addr < MEMCHECK_FLASH_ADDR_END)))

void memcheck_enable(void);
void memcheck_disable(void);

void memcheck_add(void *ptr, unsigned int size);
void memcheck_del(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCHECK_H__ */
