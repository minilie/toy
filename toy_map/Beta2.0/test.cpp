/*************************************************************************
	> File Name: 33.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 27 Jul 2024 03:43:46 PM CST
 ************************************************************************/

#include "map.h"
#include "Mytest.h"
#include <chrono>

template<typename key, typename val, typename CMP, typename TreeType>
bool testcomp(map<key, val, CMP, TreeType> &m) {
    mfunction<bool(int, int)> comp = CMP();
    key firstkey = m.begin()->first;
    for (auto iter = m.begin().next(); iter != m.end(); iter++) {
        if (comp(iter->first, firstkey)) return 0;
        firstkey = iter->first;
    }
    return 1;
}

#define T_case 1000000

#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(insert, T_treetype) { // test one key insert
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % 10000;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_treetype
#undef T_key
#undef T_val
#undef T_cmp


#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(insert, va_key) { // test variadic insert
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
    T_key x;
    T_val y;
    x = rand() % 10000;
    y = rand() % 1000;
    std::pair<T_key, T_val> p1 = std::make_pair(x, y);
    x = rand() % 10000;
    y = rand() % 1000;
    std::pair<T_key, T_val> p2 = std::make_pair(x, y);
    x = rand() % 10000;
    y = rand() % 1000;
    std::pair<T_key, T_val> p3 = std::make_pair(x, y);
    x = rand() % 10000;
    y = rand() % 1000;
    std::pair<T_key, T_val> p4 = std::make_pair(x, y);
    m.insert(p1, p2, p3, p4);
    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_treetype
#undef T_key
#undef T_val
#undef T_cmp


#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(insert, subscript_operator) { // test variadic insert
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
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

    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order
}
#undef T_treetype
#undef T_key
#undef T_val
#undef T_cmp


#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, key) { // test key erase
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
    T_key x;
    T_val y;
    std::vector<T_key> keylist;
    for (int i = 0; i < T_case; i++) {
        x = rand() % 10000;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
        keylist.push_back(x);
    }
    for (int i = 0, I = keylist.size(); i < I / 2; i++) {
        m.erase(keylist[0]);
    }
    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order

}
#undef T_treetype
#undef T_key
#undef T_val
#undef T_cmp


#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, one_iter) { // tset one iterator erase
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % 10000;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    auto iter = m.begin();
    auto size = m.size();
    for (int i = 0; i < size / 2; i++) {
        m.erase(iter++);
    }
    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order

}
#undef T_treetype
#undef T_key
#undef T_val
#undef T_cmp

#define T_treetype BRTree
#define T_key long long
#define T_val int
#define T_cmp std::less<T_key>
TEST(erase, two_iter) { // test range iterator erase
    map<T_key, T_val, T_cmp, T_treetype<T_key, T_val, T_cmp > > m;
    T_key x;
    T_val y;
    for (int i = 0; i < T_case; i++) {
        x = rand() % 10000;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    auto iter = m.begin();
    auto size = m.size();
    for (int i = 0; i < size / 2; i++) iter++;
    m.erase(m.begin(), iter);
    bool testanswer = testcomp<T_key, T_val, T_cmp >(m);
    EXPECT_NE(0, testanswer); // Test if the elements are all in order

}
#undef T_treetype
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
