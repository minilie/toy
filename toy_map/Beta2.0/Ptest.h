/*************************************************************************
	> File Name: Ptest.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Thu 19 Sep 2024 09:13:26 PM CST
 ************************************************************************/

#ifndef _PTEST_H
#define _PTEST_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define ALIGN_UP(p, alignment) ((char *)(((uintptr_t)(p) + (alignment - 1)) & ~(alignment - 1)))

class small_block {
public :
    char *b_usable;
    char *b_end;
    small_block *next;
    int times;
};

class big_block {
public :
    char *buffer;
    big_block *next;
};

class M_pool {
public :
    static M_pool *Creat_Pool(size_t capacity) {
        size_t all_size = sizeof(M_pool) + sizeof(small_block) + capacity;
        void *temp = malloc(all_size);
        memset(temp, 0, all_size);

        M_pool *pool = (M_pool *)temp;
        pool->small_capacity = capacity; // TODO
        pool->b_start = nullptr;
        pool->s_usable = (small_block *)(pool->s_start);

        small_block *x = (small_block *)(pool + 1);

        x->b_usable = (char *)(x + 1);
        x->b_end = x->b_usable + capacity;
        x->next = nullptr;
        x->times = 0;
        return pool;
    }

static char *Creat_SmallBlock(M_pool *pool, size_t size, size_t alignment = alignof(max_align_t)) {
    size_t all_size = sizeof(small_block) + pool->small_capacity + alignment - 1;
    void *temp = malloc(all_size);
    memset(temp, 0, all_size);

    small_block *x = (small_block *)temp;
    x->b_usable = ALIGN_UP((char *)(x + 1), alignment);  // 确保 b_usable 对齐
    x->b_end = (char *)temp + all_size;
    x->next = nullptr;
    x->times = 0;

    char *res = x->b_usable;
    x->b_usable += size;

    small_block *p = pool->s_usable;
    while (p->next) {
        if (p->times > 4) pool->s_usable = p->next;
        p->times += 1;
        p = p->next;
    }
    p->next = x;
    if (pool->s_usable == nullptr) pool->s_usable = x;
    return res;
}


    static char *MallocBigBlock(M_pool *pool, size_t size, size_t alignment = alignof(max_align_t)) {
    size_t all_size = size + alignment - 1;
    void *temp = malloc(all_size);
    memset(temp, 0, all_size);

    char *aligned_buffer = ALIGN_UP((char *)temp, alignment);  // 对齐大块内存

    big_block *x = pool->b_start;
    int i = 0;
    while (x) {
        if (x->buffer == nullptr) {
            x->buffer = aligned_buffer;
            return x->buffer;
        }
        if (i > 3) break;
        x = x->next;
        i += 1;
    }

    big_block *newb = (big_block *)M_pool::Pool_Malloc(pool, sizeof(big_block));
    newb->buffer = aligned_buffer;
    newb->next = pool->b_start;
    pool->b_start = newb;
    return newb->buffer;
}


static void *Pool_Malloc(M_pool *pool, size_t size, size_t alignment = alignof(max_align_t)) {
    if (size < pool->small_capacity) {
        small_block *temp = pool->s_usable;
        do {
            // 将当前 b_usable 向上对齐
            char *aligned_usable = ALIGN_UP(temp->b_usable, alignment);
            if (temp->b_end - aligned_usable > size) {
                char *res = aligned_usable;
                temp->b_usable = aligned_usable + size;
                return res;
            }
            temp = temp->next;
        } while (temp);
        return Creat_SmallBlock(pool, size, alignment);
    }
    return MallocBigBlock(pool, size);
}


    static void FreeBigBlock(M_pool *pool, char *ptr) {
        big_block *x = pool->b_start;
        while (x) {
            if (x->buffer == ptr) {
                free(x->buffer);
                x->buffer = nullptr;
            }
            x = x->next;
        }
        return ;
    }

    static void DePool(M_pool *pool) {
        big_block *p = pool->b_start;
        while (p) {
            if (p->buffer) {
                free(p->buffer);
                p->buffer = nullptr;
            }
            p = p->next;
        }

        small_block *q = pool->s_start->next;
        while (q) {
            small_block *temp = q->next;
            free(q);
            q = temp;
        }
        free(pool);
        return ;
    }

    size_t small_capacity;
    small_block *s_usable;
    big_block *b_start;
    small_block s_start[];
};

#endif
