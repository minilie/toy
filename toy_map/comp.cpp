/*************************************************************************
	> File Name: 33.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 27 Jul 2024 03:43:46 PM CST
 ************************************************************************/

#include "Mytest.h"
#include <iostream>
#include <map>
#include <functional>
#include <chrono>

template<typename key, typename val, typename CMP>
bool testcomp(std::map<key, val> &m) {
    std::function<bool(key, key)> comp = CMP();
    key firstkey = m.begin()->first;
    long long cnt = 0;
    for (auto iter = ++m.begin(); iter != m.end(); iter++) {
        cnt += 1;
        if (comp(iter->first, firstkey)) return 0;
        firstkey = iter->first;
    }
    return 1;
}

#define T_key long long 
#define T_val int
#define T_cmp std::less<T_key>
TEST(insert, T_treetype) { // test one key insert
    std::map<T_key, T_val> m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % T_case;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    bool testanswer = testcomp<T_key, T_val, T_cmp>(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_key
#undef T_val
#undef T_cmp

#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(insert, subscript_operator) { // test variadic insert
    std::map<T_key, T_val> m;
    T_key x;
    T_val y;
    x = rand() % 10000;
    y = rand() % 1000;
    m[x] = y;             // test [] insert
    EXPECT_EQ(y, m.find(x)->second);

    x = rand() % 10000;
    y = rand() % 1000;
    std::pair<T_key, T_val> p3 = std::make_pair(x, y);
    m[x] = 5;             // test [] modify
    EXPECT_EQ(5, m.find(x)->second);

    bool testanswer = testcomp<T_key, T_val, T_cmp>(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_key
#undef T_val
#undef T_cmp

#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, key) { // test key erase
    std::map<T_key, T_val> m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % T_case;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    // m.output();
    auto iter = m.begin();
    for (int i = 0, I = m.size() / 2; i < I; i++) {
        auto q = iter++;
        m.erase(q->first);
    }
    
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    bool testanswer = testcomp<T_key, T_val, T_cmp>(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_key
#undef T_val
#undef T_cmp

#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, one_iter) { // tset one iterator erase
    std::map<T_key, T_val> m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % T_case;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    auto iter = m.begin();
    auto size = m.size();
    // m.output();
    for (int i = 0; i < size / 2; i++) {
        m.erase(iter++);
    }
    // m.output();
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    bool testanswer = testcomp<T_key, T_val, T_cmp>(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order

}
#undef T_key
#undef T_val
#undef T_cmp


#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, two_iter) { // test range iterator erase
    std::map<T_key, T_val> m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % T_case;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    auto iter = m.begin();
    auto size = m.size();
    for (int i = 0; i < size / 2; i++) iter++;
                       std::cout << "begin val : " << m.begin()->first << "   iter val : " << iter->first << std::endl;
    m.erase(m.begin(), iter);
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
    bool testanswer = testcomp<T_key, T_val, T_cmp>(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order

}
#undef T_key
#undef T_val
#undef T_cmp



// Treetype : BRTree(BLACK RED TREE) AVLTree(AVL TREE)
int main() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    unsigned seed = duration.count();
    srand(seed);
    return RUN_ALL_TEST();
}
