/*************************************************************************
	> File Name: PrimeTest.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sun 28 Jul 2024 06:33:43 PM CST
 ************************************************************************/
#include "ThreadPool.h"
using namespace toy;

bool is_prime(int x) {
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return false;
    }
    return true;
}

int prime_count(int l, int r) {
    int ans = 0;
    for (int i = l; i <= r; i++) {
        ans += is_prime(i);
    }
    return ans;
}

void worker(int l, int r, int &ret) {
    TLOG_Info << std::this_thread::get_id() << " start ";
    ret = prime_count(l, r);
    TLOG_Debug << std::this_thread::get_id() << " done ";
    return ;
}

#define batch 5000000
int main() {
    ThreadPool tp(10);
    int ret[10];
    for (int i = 0, j = 1; i < 10; i++, j += batch) {
        tp.add_task(worker, j, j + batch - 1, std::ref(ret[i]));
    }
    tp.stop();
    int ans = 0;
    for (auto x : ret) ans += x;
    std::cout << ans << std::endl;
    return 0;
}
