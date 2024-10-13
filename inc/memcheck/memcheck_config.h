/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#ifndef __MEMCHECK_CONFIG_H__
#define __MEMCHECK_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Debug */
#define MEMCHECK_TRACE_ON      0
#define MEMCHECK_PRINT_ON      1
#define MEMCHECK_ERROR_ON      1

/* Function */
#define MEMCHECK_BACKTRACE_ON  1

/* Function Config */
#define MEMCHECK_TRACE_MAX     256 /* Memory malloc trace max */
#define MEMCHECK_BACKTRACE_MAX 10  /* Function caller tarce max */

/* Arch */
#define MEMCHECK_ARCH_SIMULATOR 0 /* Linux simulator */
#define MEMCHECK_ARCH_CORTEX_M3 1 /* Cortex-M3 */
#define MEMCHECK_ARCH_CORTEX_M4 2 /* Cortex-M4 / Cortex-M4F */
#define MEMCHECK_ARCH           MEMCHECK_ARCH_SIMULATOR

/* Information */
#define MEMCEHCK_RAM_ADDR_START   0x20000000
#define MEMCHECK_RAM_ADDR_SIZE    0x18000
#define MEMCHECK_RAM_ADDR_END     (MEMCEHCK_RAM_ADDR_START + MEMCHECK_RAM_ADDR_SIZE)

#define MEMCEHCK_FLASH_ADDR_START 0x08000000
#define MEMCHECK_FLASH_ADDR_SIZE  0x80000
#define MEMCHECK_FLASH_ADDR_END   (MEMCEHCK_FLASH_ADDR_START + MEMCHECK_FLASH_ADDR_SIZE)

#ifdef __cplusplus
}
#endif

#endif /* __MEMCHECK_CONFIG_H__ */
