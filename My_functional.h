/*************************************************************************
	> File Name: My_functional.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Wed 24 Jul 2024 11:37:12 AM CST
 ************************************************************************/

#ifndef _MY_FUNCTIONAL_H
#define _MY_FUNCTIONAL_H

#include <utility> 
template<typename T, typename ...ARGS>
class BASE {
public :
    virtual T run(ARGS...) = 0;
    virtual BASE<T, ARGS...> *getCopy() = 0;
    virtual ~BASE() {}
};

template<typename T, typename ...ARGS>
class normal_function : public BASE<T, ARGS...> {
public :
    normal_function(T (*ptr)(ARGS...)) : func(ptr) {}
    T run(ARGS ...args) override {
        return func(std::forward<ARGS>(args)...);
    }
    BASE<T, ARGS...> *getCopy() override {
        return new normal_function(*this);
    }
private :
    T (*func)(ARGS...);
};

template<typename CLASS_T, typename T, typename ...ARGS>
class functor : public BASE<T, ARGS...> {
public :
    functor(CLASS_T obj) : obj(obj) {}
    T run(ARGS ...args) {
        return obj(std::forward<ARGS>(args)...);
    }
    BASE<T, ARGS...> *getCopy() override {
        return new functor(*this);
    }
private :
    CLASS_T obj;
};

template<typename T, typename ...ARGS> class mfunction;
template<typename T, typename ...ARGS>
class mfunction<T(ARGS...)> {
public :
    mfunction(T (*ptr)(ARGS...)) : ptr(new normal_function<T, ARGS...>(ptr)) {}
    template<typename CLASS_T>
    mfunction(CLASS_T obj) : ptr(new functor<CLASS_T, T, ARGS...>(obj)) {}
    mfunction(const mfunction &obj) : ptr(obj.ptr->getCopy()) {}
    T operator()(ARGS ...args) {
        return ptr->run(std::forward<ARGS>(args)...);
    }
    mfunction &operator=(const mfunction<T(ARGS...)> &func) {
        delete this->ptr;
        this->ptr = func.ptr->getCopy();
        return *this;
    }
    ~mfunction() {
        delete ptr;
        return ;
    }
private :
    BASE<T, ARGS...> *ptr;
};

#endif
