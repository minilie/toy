/*************************************************************************
	> File Name: mtest.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Mon 30 Sep 2024 07:25:44 PM CST
 ************************************************************************/
#include "M_Pool.h"

int main() {
    M_pool *pool = M_pool::Creat_Pool(1024);
    char *p1 = (char *)M_pool::Pool_Malloc(pool, 2);
    fprintf(stdout, "little malloc1 : %p\n", p1);
    char *p2 = (char *)M_pool::Pool_Malloc(pool, 4);
    fprintf(stdout, "little malloc2 : %p\n", p2);
    char *p3 = (char *)M_pool::Pool_Malloc(pool, 8);
    fprintf(stdout, "little malloc3 : %p\n", p3);
    char *p4 = (char *)M_pool::Pool_Malloc(pool, 256);
    fprintf(stdout, "little malloc4 : %p\n", p4);
    char *p5 = (char *)M_pool::Pool_Malloc(pool, 512);
    fprintf(stdout, "little malloc5 : %p\n", p5);

    char *p6 = (char *)M_pool::Pool_Malloc(pool, 512);
    fprintf(stdout, "new small_block little malloc6 : %p\n", p6);

    char *p7 = (char *)M_pool::Pool_Malloc(pool, 2048);
    fprintf(stdout, "big malloc1 : %p\n", p7);

    char *p8 = (char *)M_pool::Pool_Malloc(pool, 4096);
    fprintf(stdout, "big malloc2 : %p\n", p8);

    M_pool::FreeBigBlock(pool, p8);

    char *p9 = (char *)M_pool::Pool_Malloc(pool, 2048);
    fprintf(stdout, "again malloc big malloc3 : %p\n", p9);
    M_pool::DePool(pool);
    return 0;
}
