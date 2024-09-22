/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#ifndef __MEMCHECK_H__
#define __MEMCHECK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define MEMCHECK_DEBUG(fmt, arg...) \
    do {                            \
        printf(fmt, ##arg);         \
    } while (0)

void memcheck_enable(void);
void memcheck_disable(void);

void memcheck_add(void *ptr, unsigned int size);
void memcheck_del(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* __MEMCHECK_H__ */
