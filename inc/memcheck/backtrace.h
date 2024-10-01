/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#ifndef __MEMCHECK_BACKTRACE_H__
#define __MEMCHECK_BACKTRACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <memcheck/memcheck_config.h>

extern void backtrace_simulator(void **ptr_array, unsigned int ptr_array_size);
extern void backtrace_cortex_m4(void **ptr_array, unsigned int ptr_array_size);

#if (MEMCHECK_ARCH == MEMCHECK_ARCH_SIMULATOR)
    #define BACKTRACE_GET(p, s) backtrace_simulator(p, s)
#elif (MEMCHECK_ARCH == MEMCHECK_ARCH_CORTEX_M4)
    #define BACKTRACE_GET(p, s) backtrace_cortex_m4(p, s)
#else
    #define BACKTRACE_GET(p, s)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MEMCHECK_BACKTRACE_H__ */
