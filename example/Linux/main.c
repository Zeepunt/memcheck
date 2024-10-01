/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <memcheck/memcheck.h>

static void *test_malloc(size_t size)
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

int main(int argc, char *argv[])
{
    void *ptr = NULL;

    memcheck_enable();

    ptr = test_malloc(128);
    
    ptr = test_malloc(64);
    test_free(ptr);

    ptr = test_malloc(16);

    ptr = test_malloc(8);
    test_free(ptr);

    memcheck_disable();

    return 0;
}