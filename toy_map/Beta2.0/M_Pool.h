/*************************************************************************
	> File Name: M_Pool.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Thu 19 Sep 2024 04:23:11 PM CST
 ************************************************************************/

#ifndef _M_POOL_H
#define _M_POOL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

    static char *Creat_SmallBlock(M_pool *pool, size_t size) {
        size_t all_size = sizeof(small_block) + pool->small_capacity;
        void *temp = malloc(all_size);
        memset(temp, 0, all_size);

        small_block *x = (small_block *)temp;
        x->b_usable = (char *)(x + 1);
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

    static char *MallocBigBlock(M_pool *pool, size_t size) {
        void *temp = malloc(size);
        memset(temp, 0, size);

        big_block *x = pool->b_start;
        int i = 0;
        while (x) {
            if (x->buffer == nullptr) {
                x->buffer = (char *)temp;
                return x->buffer;
            }
            if (i > 3) break;
            x = x->next;
            i += 1;
        }

        big_block *newb = (big_block *)M_pool::Pool_Malloc(pool, sizeof(big_block));
        newb->buffer = (char *)temp;
        newb->next = pool->b_start;
        pool->b_start = newb;
        return newb->buffer;
    }

    static void *Pool_Malloc(M_pool *pool, size_t size) {
        if (size < pool->small_capacity) {
            small_block *temp = pool->s_usable;
            do {
                if (temp->b_end - temp->b_usable > size) {
                    char *res = temp->b_usable;
                    temp->b_usable += size;
                    return res;
                }
                temp = temp->next;
            } while (temp);
            return Creat_SmallBlock(pool, size);
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
