/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <memcheck/memcheck.h>

#if (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_SIMULATOR))
void backtrace_simulator(void **ptr_array, unsigned int ptr_array_size)
{
    int i = 0;
    void *addr = NULL;

    if ((NULL == ptr_array) || (0 == ptr_array_size)) {
        return;
    }

    MEMCHECK_TRACE("backtrace enter.");

    for (i = 0; i < ptr_array_size; i++) {
        /**
         * 这里限制 __builtin_return_address 传入的数值
         * 因为当数值大于实际的函数调用深度时, 会导致 __builtin_return_address 触发段错误
         */
        switch (i) {
        case 0:
            addr = __builtin_return_address(0);
            break;

        case 1:
            addr = __builtin_return_address(1);
            break;

        case 2:
            addr = __builtin_return_address(2);
            break;

        default:
            addr = NULL;
        }

        MEMCHECK_TRACE("addr: %p.", addr);
        if (NULL == addr) {
            break;
        }
        ptr_array[i] = addr;
    }

    MEMCHECK_TRACE("backtrace exit.");
}
#else
void backtrace_simulator(unsigned int *ptr_array, unsigned int ptr_array_size)
{
}
#endif /* (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_SIMULATOR)) */
