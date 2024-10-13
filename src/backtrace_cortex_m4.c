/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <memcheck/memcheck.h>

#define M4_USE_FRAME_RECORD 0 /* 1: 使用 frame record, 0: 使用其他方式 */

#if (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_CORTEX_M4))
#if M4_USE_FRAME_RECORD
typedef struct cortex_m4_frame_record {
    struct cortex_m4_frame_record *prev_frame;
    void *return_addr;
} cortex_m4_frame_record_t;

void backtrace_cortex_m4(void **ptr_array, unsigned int ptr_array_size)
{
    int i = 0;
    cortex_m4_frame_record_t *frame_record_ptr = NULL;

    if ((NULL == ptr_array) || (0 == ptr_array_size)) {
        return;
    }

    MEMCHECK_TRACE("backtrace enter.");

    /**
     * 对于 Thumb, 使用 r7 寄存器表示 fp (frame pointer)
     * 
     * 对于 RT-Thread:
     *   如果从 fp 拿到的数值是 0xdeadbeef, 说明 fp 并没有启用
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

    for (i = 0; i < ptr_array_size; i++) {
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
#define M4_DEPTH_LIMIT       0X200

#define INST16_PUSH_MASK     0xff00
#define INST16_PUSH          0xb400      /* push {...} */
#define INST16_PUSH_WITH_LR  0xb500      /* push {..., lr} */

#define INST32_STMDB_MASK    0xfffff000
#define INST32_STMDB_WITH_LR 0xe92d4000  /* stmdb sp!, {..., lr} */

#define INST16_SUB_MASK      0xff80
#define INST16_SUB           0xb080      /* sub sp, ... */

#define INST32_SUB_MASK      0xfbff8f00
#define INST32_SUB           0xf1ad0d00  /* sub.w sp, ... */

static unsigned int _m4_stack_search(unsigned int *pc, unsigned int *sp, unsigned int **new_sp)
{
    int offset = 0;

    unsigned short inst_16 = 0; /* 16bit instruction */
    unsigned int inst_32 = 0; /* 32bit instruction */

    unsigned int *lr = 0;
    unsigned int lr_value = 0;
    unsigned int pc_value = 0;
    unsigned int addr = 0;

    unsigned int reg_num = 0;
    unsigned int var_num = 0;

    unsigned int subw = 0;
    unsigned int subw_shift = 0;

    pc_value = (unsigned int)pc;

    for (offset = 0; offset < M4_DEPTH_LIMIT; offset += 2) {
        /**
         * 找到距离当前 pc 最近的 push 指令 (将 lr 寄存器压栈), 也就是说找到了所在函数的入口
         * 
         * 场景一:
         *   push	{r7, lr}
         * 
         * 场景二:
         *   push	{r0, r1, r2, r3}
         *   push	{r7, lr}
         */
        inst_16 = *((unsigned short *)(pc_value - offset));
        if (INST16_PUSH_WITH_LR == (inst_16 & INST16_PUSH_MASK)) {
            /* 计算压栈保存了多少个寄存器 (包括 lr 寄存器) */
            reg_num = __builtin_popcount(inst_16 & 0xff);
            reg_num += 1;

            inst_16 = *((unsigned short *)(pc_value - offset - 2));
            if (INST16_PUSH == (inst_16 & INST16_PUSH_MASK)) {
                reg_num += __builtin_popcount(inst_16 & 0xff);
            }
            addr = pc_value - offset;
            MEMCHECK_TRACE("push reg num: %d.", reg_num);
            MEMCHECK_TRACE("entry addr: 0x%x.", addr);
            break;
        }

        /* 如果是使用 stmdb 指令来进行压栈 */
        inst_32 = *(unsigned short *)(pc_value - offset);
        inst_32 <<= 16;
        inst_32 |= *(unsigned short *)(pc_value - offset + 2);
        if (INST32_STMDB_WITH_LR == (inst_32 & INST32_STMDB_MASK)) {
            reg_num += __builtin_popcount(inst_32 & 0xfff);
            reg_num += 1;
            addr = pc_value - offset;

            MEMCHECK_TRACE("stmdb reg num: %d.", reg_num);
            MEMCHECK_TRACE("entry addr: 0x%x.", addr);
            break;
        }
    }

    if (0 == addr) {
        return 0;
    }

    /* 到这里, 我们知道了保存了 reg_num 个寄存器, 栈空间使用了 (reg_num * 4) Byte */

    for (offset = 0; offset < M4_DEPTH_LIMIT; ) {
        if ((addr + offset) > (unsigned int)*pc) {
            break;
        }

        /**
         * 找到 push / stmdb 指令后面的 sub / sub.w 指令, 也就是说是否有预留栈空间给局部变量使用
         * 有些函数没有使用局部变量, 所以可能找不到
         */
        inst_16 = *((unsigned short *)(pc_value - offset));
        if ((inst_16 & INST16_SUB_MASK) == INST16_SUB) {
            var_num = (inst_16 & 0x7f) << 2; /* 需要 x4 */
            MEMCHECK_TRACE("sub var num: %d.", var_num);
            break;
        }

        inst_32 = *(unsigned short *)(pc_value - offset);
        inst_32 <<= 16;
        inst_32 |= *(unsigned short *)(pc_value - offset + 2);
        if ((inst_32 & INST32_SUB_MASK) == INST32_SUB) {
            subw = 128 + (inst_32 & 0x7f);
            subw_shift = (inst_32 >> 7) & 0x1;
            subw_shift += ((inst_32 >> 12) & 0x7) << 1;
            subw_shift += ((inst_32 >> 26) & 0x1) << 4;
            var_num += subw << (30 - subw_shift);
            MEMCHECK_TRACE("sub.w var num: %d.", var_num);
            break;
        }

        if ((inst_16 & 0xf800) >= 0xe800) {
            offset += 4;
        } else {
            offset += 2;
        }
    }

    lr = (unsigned int *)((unsigned int)sp + var_num + 4);
    lr_value = *lr - 1; /* Thumb bit[0] = 1 */
    MEMCHECK_TRACE("LR: %p value: 0x%x.", lr, lr_value);

    *new_sp = lr + (reg_num - 1); /* 不包含 lr */

    /* 检测 lr 地址是否合理 */
    if (MEMECHK_ADDR_VALID(lr_value)) {
        return lr_value;
    } else {
        return 0;
    }
}

void backtrace_cortex_m4(void **ptr_array, unsigned int ptr_array_size)
{
    int i = 0;
    unsigned int ret = 0;
    unsigned int *pc = NULL;
    unsigned int *sp = NULL;

    __asm__ volatile("mov %0, pc" : "=r"(pc));
    __asm__ volatile("mov %0, sp" : "=r"(sp));

    for (i = 0; i < ptr_array_size; i++) {
        ret = _m4_stack_search(pc, sp, &sp);
        if (0 == ret) {
            break;
        }
        ptr_array[i] = (void *)ret;
        pc = (unsigned int *)ret;
    }
}

#endif /* M4_USE_FRAME_RECORD */
#else
void backtrace_cortex_m4(unsigned int *ptr_array, unsigned int ptr_array_size)
{
}
#endif /* (MEMCHECK_BACKTRACE_ON && (MEMCHECK_ARCH == MEMCHECK_ARCH_CORTEX_M4)) */
