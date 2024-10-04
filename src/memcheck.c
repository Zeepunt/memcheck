/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <memcheck/list.h>
#include <memcheck/backtrace.h>
#include <memcheck/memcheck.h>

typedef struct {
    struct list_head node;                   /* 链表节点 */
    void *ptr;                               /* 分配的内存指针 */
    unsigned long size;                      /* 分配的内存大小 */
#if MEMCHECK_BACKTRACE_ON
    void *backtrace[MEMCHECK_BACKTRACE_MAX]; /* 调用者追溯 */
#endif
} mem_trace_info_t;

static struct list_head _mem_list = LIST_HEAD_INIT(_mem_list);
static mem_trace_info_t _mem_info_array[MEMCHECK_TRACE_MAX];
static unsigned char _mem_flag = 0;

/**
 * 因为是记录内存分配的情况, 所以使用静态大数组来实现, 而不是使用动态分配
 * 
 * 对于 _mem_info_array, 使用时记录的情况如下: (空的表示已经被释放了)
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+
 * | x | x | x |   |   | x |   | x |   |   |   |   | x |
 * +---+---+---+---+---+---+---+---+---+---+---+---+---+
 * 
 * 对于 _mem_list, 仅记录正在使用的内存:
 * +---+    +---+    +---+    +---+    +---+
 * | x | -> | x | -> | x | -> | x | -> | x |
 * +---+    +---+    +---+    +---+    +---+
 */

void memcheck_enable(void)
{
    int i = 0;
    int j = 0;
    _mem_flag = 1;

    for (i = 0; i < MEMCHECK_TRACE_MAX; i++) {
        _mem_info_array[i].ptr = NULL;
        _mem_info_array[i].size = 0;
#if MEMCHECK_BACKTRACE_ON
        for (j = 0; j < MEMCHECK_BACKTRACE_MAX; j++) {
            _mem_info_array[i].backtrace[j] = NULL;
        }
#endif /* MEMCHECK_BACKTRACE_ON */
    }
    INIT_LIST_HEAD(&_mem_list);
}

void memcheck_disable(void)
{
    int i = 0;
    unsigned int total_size = 0;
    mem_trace_info_t *info = NULL;
    struct list_head *pos = NULL;
    struct list_head *n = NULL;

    _mem_flag = 0;

    MEMCHECK_PRINT("---------- memcheck result ----------");
    list_for_each_safe(pos, n , &_mem_list) {
        info = list_entry(pos, mem_trace_info_t, node);
        MEMCHECK_PRINT("malloc ptr: 0x%x, size: %d.", info->ptr, info->size);
        total_size += info->size;
#if MEMCHECK_BACKTRACE_ON
        for (i = 0; i < MEMCHECK_BACKTRACE_MAX; i++) {
            if (NULL != info->backtrace[i]) {
                MEMCHECK_PRINT("backtrace: %p.", info->backtrace[i]);
            }
        }
#endif /* MEMCHECK_BACKTRACE_ON */
    }
    MEMCHECK_PRINT("total_size : %d.", total_size);
    MEMCHECK_PRINT("---------- memcheck result ----------");
}

void memcheck_add(void *ptr, unsigned int size)
{
    if ((1 == _mem_flag) && (NULL != ptr)) {
        int i = 0;
        for (i = 0; i < MEMCHECK_TRACE_MAX; i++) {
            if (NULL == _mem_info_array[i].ptr) {
                break;
            }
        }

        if (i >= MEMCHECK_TRACE_MAX) {
            MEMCHECK_ERROR("[memcheck] Exceeded the MEMCHECK_TRACE_MAX limit.");
        } else {
            _mem_info_array[i].ptr = ptr;
            _mem_info_array[i].size = size;
#if MEMCHECK_BACKTRACE_ON
            BACKTRACE_GET(_mem_info_array[i].backtrace, MEMCHECK_BACKTRACE_MAX);
#endif /* MEMCHECK_BACKTRACE_ON */
            list_add(&_mem_info_array[i].node, &_mem_list);
        }
    }
}

void memcheck_del(void *ptr)
{
    if ((1 == _mem_flag) && (NULL != ptr)) {
        mem_trace_info_t *info = NULL;
        struct list_head *pos = NULL;
        struct list_head *n = NULL;

        list_for_each_safe(pos, n, &_mem_list) {
            info = list_entry(pos, mem_trace_info_t, node);
            if (info->ptr == ptr) {
                info->ptr = NULL;
                info->size = 0;
                list_del(pos);
                break;
            }
        }
    }
}