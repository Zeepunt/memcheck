/*
 * Copyright (c) 2024 by Zeepunt, All Rights Reserved. 
 */
#include <memcheck/list.h>
#include <memcheck/memcheck.h>

#define MAX_TRACE_CNT   1024 /* 跟踪计数最大值 */
#define MAX_TRACE_DEPTH 20   /* 跟踪深度最大值 */

typedef struct {
    struct list_head node;            /* 链表节点 */
    void *ptr;                        /* 分配的内存指针 */
    unsigned long size;               /* 分配的内存大小 */
    void *backtrace[MAX_TRACE_DEPTH]; /* 调用者追溯 */
} mem_trace_info_t;

static struct list_head _mem_list = LIST_HEAD_INIT(_mem_list);
static mem_trace_info_t _mem_info_array[MAX_TRACE_CNT];
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
    _mem_flag = 1;

    for (unsigned int i = 0; i < MAX_TRACE_CNT; i++) {
        _mem_info_array[i].ptr = NULL;
        _mem_info_array[i].size = 0;
        for (unsigned int j = 0; j < MAX_TRACE_DEPTH; j++) {
            _mem_info_array[i].backtrace[j] = NULL;
        }
    }
    INIT_LIST_HEAD(&_mem_list);
}

void memcheck_disable(void)
{
    unsigned int total_size = 0;
    mem_trace_info_t *info = NULL;
    struct list_head *pos = NULL;
    struct list_head *n = NULL;

    _mem_flag = 0;

    MEMCHECK_DEBUG("---------- memcheck result ----------\n");
    list_for_each_safe(pos, n , &_mem_list) {
        info = list_entry(pos, mem_trace_info_t, node);
        MEMCHECK_DEBUG("malloc ptr: 0x%x, size: %d.\n", info->ptr, info->size);
        total_size += info->size;
        for (unsigned int i = 0; i < MAX_TRACE_DEPTH; i++) {
            if (NULL != info->backtrace[i]) {
                MEMCHECK_DEBUG("backtrace: 0x%x.\n", info->backtrace[i]);
            }
        }
    }
    MEMCHECK_DEBUG("total_size : %d.\n", total_size);
    MEMCHECK_DEBUG("---------- memcheck result ----------\n");
}

void memcheck_add(void *ptr, unsigned int size)
{
    if ((1 == _mem_flag) && (NULL != ptr)) {
        int i = 0;
        for (i = 0; i < MAX_TRACE_CNT; i++) {
            if (NULL == _mem_info_array[i].ptr) {
                break;
            }
        }

        if (i >= MAX_TRACE_CNT) {
            MEMCHECK_DEBUG("[memcheck] Exceeded the MAX_TRACE_CNT limit.\n");
        } else {
            _mem_info_array[i].ptr = ptr;
            _mem_info_array[i].size = size;
            // TODO: backtrace
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