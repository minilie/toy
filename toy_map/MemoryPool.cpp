/*************************************************************************
	> File Name: MemoryPool.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Wed 18 Sep 2024 08:24:20 PM CST
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <stack>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <cstring>
#include "Mytest.h"
using namespace std;

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
//        //fprintf(stdout, "pool address : %p\n", pool);
        pool->small_capacity = capacity; // TODO
        pool->b_start = nullptr;
        pool->s_usable = (small_block *)(pool->s_start);

        small_block *x = (small_block *)(pool + 1);
//        //fprintf(stdout, "first small block address : %p\n", x);

        x->b_usable = (char *)(x + 1);
//        fprintf(stdout, "first small buffer address : %p\n", x->b_usable);
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
//        fprintf(stdout, "new small_block address : %p\n", x);
        x->b_usable = (char *)(x + 1);
//        fprintf(stdout, "new small_block buffer address : %p\n", x);
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

TEST(Pool, malloc) {
    for (int i = 0; i < 100000; i++) {
        M_pool *pool = M_pool::Creat_Pool(1024);
        char *p1 = (char *)M_pool::Pool_Malloc(pool, 2);
    //    fprintf(stdout, "little malloc1 : %p\n", p1);
        char *p2 = (char *)M_pool::Pool_Malloc(pool, 4);
    //    fprintf(stdout, "little malloc2 : %p\n", p2);
        char *p3 = (char *)M_pool::Pool_Malloc(pool, 8);
    //    fprintf(stdout, "little malloc3 : %p\n", p3);
        char *p4 = (char *)M_pool::Pool_Malloc(pool, 256);
    //    fprintf(stdout, "little malloc4 : %p\n", p4);
        char *p5 = (char *)M_pool::Pool_Malloc(pool, 512);
    //    fprintf(stdout, "little malloc5 : %p\n", p5);
    
        char *p6 = (char *)M_pool::Pool_Malloc(pool, 512);
    //    fprintf(stdout, "new small_block little malloc6 : %p\n", p6);
    
        char *p7 = (char *)M_pool::Pool_Malloc(pool, 2048);
    //    fprintf(stdout, "big malloc1 : %p\n", p7);
    
        char *p8 = (char *)M_pool::Pool_Malloc(pool, 4096);
    //    fprintf(stdout, "big malloc2 : %p\n", p8);
    
        M_pool::FreeBigBlock(pool, p8);
    
        char *p9 = (char *)M_pool::Pool_Malloc(pool, 2048);
    //    fprintf(stdout, "again malloc big malloc3 : %p\n", p9);
        M_pool::DePool(pool);
    }
}

TEST(lib, malloc) {
    for (int i = 0; i < 100000; i++) {
        char *p1 = (char *)malloc(sizeof(char) * 2);
    //    fprintf(stdout, "little malloc1 : %p\n", p1);
        char *p2 = (char *)malloc(sizeof(char) * 4);
    //    fprintf(stdout, "little malloc2 : %p\n", p2);
        char *p3 = (char *)malloc(sizeof(char) * 8);
    //    fprintf(stdout, "little malloc3 : %p\n", p3);
        char *p4 = (char *)malloc(sizeof(char) * 256);
    //    fprintf(stdout, "little malloc4 : %p\n", p4);
        char *p5 = (char *)malloc(sizeof(char) * 512);
    //    fprintf(stdout, "little malloc5 : %p\n", p5);
    
        char *p6 = (char *)malloc(sizeof(char) * 512);
    //    fprintf(stdout, "new small_block little malloc6 : %p\n", p6);
    
        char *p7 = (char *)malloc(sizeof(char) * 2048);
    //    fprintf(stdout, "big malloc1 : %p\n", p7);
    
        char *p8 = (char *)malloc(sizeof(char) * 4096);
    //    fprintf(stdout, "big malloc2 : %p\n", p8);
    
        char *p9 = (char *)malloc(sizeof(char) * 2048);
    //    fprintf(stdout, "again malloc big malloc3 : %p\n", p9);
    
        free(p1);
        free(p2);
        free(p3);
        free(p4);
        free(p5);
        free(p6);
        free(p7);
        free(p8);
        free(p9);
        
    }
}

int main() {
    return RUN_ALL_TEST();
}
