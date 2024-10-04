/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <rtthread.h>

#include <memcheck/memcheck.h>

static void *test_malloc(unsigned int size)
{
    void *ptr = malloc(size);
    if (NULL != ptr) {
        memcheck_add(ptr, size);
    }
    return ptr;
}

static void test_free(void *ptr)
{
    if (NULL == ptr) {
        return;
    }
    free(ptr);
    memcheck_del(ptr);
}

static void test_func_1(char *format, ...)
{
    va_list args;
    void *buf = test_malloc(34);

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
}

int main(void)
{
    void *ptr = NULL;

    memcheck_enable();

    ptr = test_malloc(128);

    ptr = test_malloc(256);
    test_free(ptr);

    ptr = test_malloc(512);

    test_func_1("test function 1");

    memcheck_disable();

    while (1) {
        rt_thread_mdelay(500);
    }
}
