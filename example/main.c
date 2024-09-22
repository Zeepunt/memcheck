#include <stdio.h>
#include <stdlib.h>
#include <memcheck/memcheck.h>

void *test_malloc(size_t size)
{
    void *ptr = malloc(size);
    memcheck_add(ptr, size);
    return ptr;
}

void test_free(void *ptr)
{
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