/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <memcheck/memcheck.h>

#if (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_CORTEX_M4))
typedef struct cortex_m4_frame_record {
    struct cortex_m4_frame_record *prev_frame;
    void *return_addr;
} cortex_m4_frame_record_t;

void backtrace_cortex_m4(void **ptr_array, unsigned int ptr_array_size)
{
    cortex_m4_frame_record_t *frame_record_ptr = NULL;

    if ((NULL == ptr_array) || (0 == ptr_array_size)) {
        return;
    }

    MEMCHECK_TRACE("backtrace enter.");

    /**
     * 对于 RT-Thread:
     *   如果从 fp (r7) 拿到的数值是 0xdeadbeef, 说明 fp 并没有启用
     *   对于 GCC 编译器:
     *     1. 添加 -fno-omit-frame-pointer -mapcs-frame -funwind-tables -fno-optimize-sibling-calls
     *     2. 如果要打印详细信息, 优化等级设为 -O0
     *   对于 Arm Compiler 6 编译器: (Keil)
     *     1. Options -> C/C++ -> Misc Controls: 添加 -fno-omit-frame-pointer -funwind-tables -fno-optimize-sibling-calls
     *     2. 如果要打印详细信息, Options -> C/C++ -> Launage / Code Genration: 优化等级设为 -O0
     * 
     * 其中:
     *   1. -fno-omit-frame-pointer: 确保帧指针在函数栈帧中始终存在
     *   2. -mapcs-frame: 确保 APCS 保留 fp 寄存器
     *   3. -funwind-tables: 生成的栈展开表可以用于调用栈跟踪和调试时的栈帧恢复
     *   4. -fno-optimize-sibling-calls: 在复杂的嵌套函数调用中禁用尾调用优化, 以确保每个函数调用都保留栈帧记录
     */
    __asm volatile ("mov %0, r7" : "=r" (frame_record_ptr));

    for (int i = 0; i < ptr_array_size; i++) {
        MEMCHECK_TRACE("frame_record_ptr: %p.", frame_record_ptr);
        if ((NULL == frame_record_ptr) || (0xdeadbeef == (unsigned long)frame_record_ptr)) {
            break;
        }
        MEMCHECK_TRACE("prev frame record: %p.", frame_record_ptr->prev_frame);
        MEMCHECK_TRACE("prev function caller: %p.", frame_record_ptr->return_addr);

        ptr_array[i] = frame_record_ptr->return_addr;
        frame_record_ptr = frame_record_ptr->prev_frame;
    }

    MEMCHECK_TRACE("backtrace exit.");
}
#else
void backtrace_cortex_m4(unsigned int *ptr_array, unsigned int ptr_array_size)
{
}
#endif /* (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_CORTEX_M4)) */
