/*************************************************************************
	> File Name: Mytest.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Fri 12 Jul 2024 07:02:14 PM CST
 ************************************************************************/

#ifndef _MYTEST_H
#define _MYTEST_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>

#define COLOR(msg, CODE) "\033[" #CODE ";1" "m" msg "\033[0m"
#define CB(msg) COLOR(msg, 30)
#define CR(msg) COLOR(msg, 31)
#define GREEN(msg) COLOR(msg, 32)
#define YELLOW(msg) COLOR(msg, 33)
#define BLUE(msg) COLOR(msg, 36)
#define WHITE(msg) COLOR(msg, 37)

#define EXPECT(a, b, comp) { \
    auto __a = (a); \
    auto __b = (b); \
    if (!(__a comp __b)) { \
        printf("%s:%d: Failure\n", __FILE__, __LINE__); \
        printf("Expected: (%s) %s (%s), actual: %s vs %s\n", #a, #comp, #b, #a, #b); \
        RUN_FLAG = true; \
    } \
}
#define EXPECT_EQ(a, b) EXPECT(a, b, ==)
#define EXPECT_NE(a, b) EXPECT(a, b, !=)
#define EXPECT_LT(a, b) EXPECT(a, b, <)
#define EXPECT_LE(a, b) EXPECT(a, b, <=)
#define EXPECT_GT(a, b) EXPECT(a, b, >)
#define EXPECT_GE(a, b) EXPECT(a, b, >=)

bool RUN_FLAG = false;

#define TEST(a, b) \
void MY_TEST_##a##_##b(); \
static TestRegister register_##a##_##b(MY_TEST_##a##_##b, #a, #b, #a "." #b); \
void MY_TEST_##a##_##b()

struct FUNC_NODE {
    FUNC_NODE(
        void (*func)(), const char *__a, const char *__b, const char *func_name) :
    func(func), __a(__a), __b(__b), func_name(func_name) {}
    void (*func)();
    const char *func_name;
    const char *__a;
    const char *__b;
};
std::vector<FUNC_NODE> FUNCLIST, P_FUNC, F_FUNC;

void ADD_TEST_CASE_FUNCTION(void (*func)(), const char *a, const char *b, const char *func_name) {
    FUNCLIST.push_back(FUNC_NODE(func, a, b, func_name));
    return ;
}

class TestRegister {
public :
    TestRegister(void (*func)(), const char *a, const char *b, const char *func_name) {
        ADD_TEST_CASE_FUNCTION(func, a, b, func_name);
    }
};

int RUN_ALL_TEST() {
    printf(BLUE("[==========] ") "Running %d tests from %d test suites.\n",
        (int)FUNCLIST.size(), (int)FUNCLIST.size()
    );
    printf(GREEN("[----------] ") "My test environment set-up\n");
    long long b = clock();

    for (auto &x : FUNCLIST) {
        RUN_FLAG = false;
        printf(GREEN("[----------] ") "1 test from %s\n", x.__a);
        long long c = clock();
        printf(GREEN("[ RUN      ] ") "%s\n", x.func_name);
        x.func();
        long long d = clock();
        if (RUN_FLAG) {
            printf(CR("[  FAILED  ] ") "%s (%lld ms)\n", x.func_name, 1000 * (d - c) / CLOCKS_PER_SEC);
            F_FUNC.push_back(x);
        } else {
            printf(GREEN("[       OK ] ") "%s (%lld ms)\n", x.func_name, 1000 * (d - c) / CLOCKS_PER_SEC);
            P_FUNC.push_back(x);
        }
        printf(GREEN("[----------] ") "1 test from %s (%lld ms total)\n\n", x.__a, 1000 * (d - c) / CLOCKS_PER_SEC);
    }

    long long e = clock();
    printf(BLUE("[==========] ") "%d tests from %d test suites ran. (%lld ms total)\n",
        (int)FUNCLIST.size(),
        (int)FUNCLIST.size(),
        1000 * (e - b) / CLOCKS_PER_SEC
    );

    printf(GREEN("[  PASSED  ] ") "%d tests.\n", (int)P_FUNC.size());
    if (F_FUNC.size()) {
        printf(CR("[  FAILED  ] ") "%d tests, listed below:\n", (int)F_FUNC.size());
        for (auto &x : F_FUNC) {
            printf(CR("[  FAILED  ] ") "%s\n", x.func_name);
        }
        printf("\n%d FAILED TEST\n", (int)F_FUNC.size());
    }
    return 0;
}
#endif
