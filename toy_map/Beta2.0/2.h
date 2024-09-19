/*************************************************************************
	> File Name: 2.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Thu 19 Sep 2024 07:59:22 PM CST
 ************************************************************************/

#ifndef _2_H
#define _2_H

#include <iostream>
#include <memory>
#include "M_Pool.h"

#define NIL (__NIL<T, U>)
#define RED 0
#define BLACK 1
#define DBLACK 2

template<typename T, typename U>
struct TreeNode;

class MakeNode;

template<typename T, typename U>
static TreeNode<T, U> *__NIL = new TreeNode<T, U>(T(), U(), BLACK);

template<typename T, typename U>
struct TreeNode {
    TreeNode(T key, U val, int color = RED, TreeNode<T, U> *lchild = NIL,
         TreeNode<T, U> *rchild = NIL) :
    key(key, val), color(color), lchild(lchild), rchild(rchild), father(NIL), height(1) {}
    TreeNode<T, U> &operator=(U val) {
        this->key.second = val;
        return *this;
    }
    std::pair<T, U> key;
    TreeNode<T, U> *father;
    TreeNode<T, U> *lchild, *rchild;
    int color;
    int height;
};

class MakeNode {
public :
    template<typename Node, typename T, typename U, typename ...ARGS>
    static Node *Cnode(M_pool *pool, T key, U val, ARGS ...args) {
        void *mem = M_pool::Pool_Malloc(pool, sizeof(Node));
        return new(mem) Node(key, val, std::forward<ARGS>(args)...);
    }
};


template<typename T, typename U, typename Node>
class IteratorHelper {
    using ptr = Node *;
public :
    static ptr get_next(ptr p) {
        if (p->rchild != NIL) {
            p = p->rchild;
            while (p->lchild != NIL) p = p->lchild;
            return p;
        }
        ptr parent = p->father;
        while (parent != NIL && parent->rchild == p) {
            p = p->father;
            parent = p->father;
        }
        return parent;
    }
    static ptr get_predecessor(ptr root) {
        ptr temp = root->lchild;
        while (temp->rchild != NIL) temp = temp->rchild;
        return temp;
    }
};

template<typename T, typename U, bool if_const, typename Node>
class Iterator {
    using helper = IteratorHelper<T, U, Node>;
    using ref = std::conditional_t<if_const, const std::pair<T, U> &, std::pair<T, U> &>;
    using ptr = std::conditional_t<if_const, const std::pair<T, U> *, std::pair<T, U> *>;
    using self = Iterator<T, U, if_const, Node>;

public :
    Iterator(Node *node) : node(node) {}
    Node &operator*() const {
        return *static_cast<Node *>(node);
    }

    ptr operator->() {
        return &((*(*this)).key);
    }
    ptr operator->() const {
        return &((*(*this)).key);
    }

    ref operator++() {
        this->node = helper::get_next(this->node);
        return *this;
    }
    self operator++(int) {
        self iter(*this);
        node = helper::get_next(this->node);
        return iter;
    }

    ref operator--() {
        this->node = helper::get_predecessor(this->node);
        return *this;
    }
    self operator--(int) {
        self iter(*this);
        node = helper::get_predecessor(this->node);
        return iter;
    }

    self next() {
        return self(helper::get_next(node));
    }
    self next() const {
        return self(helper::get_next(node));
    }

    self predecessor() {
        return self(helper::get_predecessor(node));
    }
    self predecessor() const {
        return self(helper::get_predecessor(node));
    }

    self &operator=(const U &val) {
        if (this->node == NIL) return *this;
        this->node->key.second = val;
        return *this;
    }
    bool operator==(const self &iter) {
        return this->node == iter.node;
    }
    bool operator!=(const self &iter) {
        return this->node != iter.node;
    }

private :
    Node *node;
};

#endif
