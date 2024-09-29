/*************************************************************************
	> File Name: map.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 27 Jul 2024 03:37:29 PM CST
 ************************************************************************/

#ifndef _MAP_H
#define _MAP_H

#include "1.h"
#include "3.h"
#include "BTree.h"

template<typename T, typename U, typename COMP = std::less<T>, class TreeType = BRTree<T, U, COMP> >
class map {
    using Tree = TreeType;
    using iterator = typename Tree::iterator;
    using const_iterator = typename Tree::const_iterator;
private :
    Tree tree;
public :
    map() : tree() {}
    map(COMP cmp) : tree(cmp) {}
    map(const map &obj) : tree(obj.tree) {}

    map &operator=(const map &obj) {
        this->tree = obj.tree;
        return *this;
    }

    template<typename N, typename ...ARGS>
    std::pair<iterator, bool> insert(N _p, ARGS ...args) {
        tree.insert(_p);
        return insert(args...);
    }
    template<typename N>
    std::pair<iterator, bool> insert(N _p) {
        std::pair<iterator, bool> __p = tree.insert(_p);
        return __p;
    }

    iterator erase(T key) {
        return tree.erase(key);
    }
    iterator erase(iterator itera) {
        return tree.erase(itera);
    }
    iterator erase(iterator itera, iterator iterb) {
        return tree.erase(itera, iterb);
    }

    iterator find(const T &key) {
        return tree.find(key);
    }
    const_iterator find(const T &key) const {
        return tree.find(key);
    }

    iterator begin() {
        return tree.begin();
    }
    const_iterator begin() const {
        return tree.begin();
    }

    iterator end() {
        return tree.end();
    }
    const_iterator end() const {
        return tree.end();
    }

    auto operator[](U key) -> decltype(tree[key]) {
        return tree[key];
    }

    long long size() { return tree.size(); }
    long long size() const { return tree.size(); }

    void output() {
        tree.output();
        return ;
    }
    void output() const {
        tree.output();
        return ;
    }
};

#endif

