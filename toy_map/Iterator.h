/*************************************************************************
	> File Name: Iterator.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 27 Jul 2024 03:29:52 PM CST
 ************************************************************************/

#ifndef _ITERATOR_H
#define _ITERATOR_H

#include <iostream>
#include <memory>
#include <shared_mutex>

#define NIL (__NIL<T, U>)
#define RED 0
#define BLACK 1
#define DBLACK 2

template<typename T, typename U>
struct TreeNode;

template<typename T, typename U>
static std::shared_ptr<TreeNode<T, U> > __NIL = std::make_shared<TreeNode<T, U> >(T(), U(), BLACK);

template<typename T, typename U>
struct TreeNode {
    TreeNode(T key, U val, int color = RED, std::shared_ptr<TreeNode<T, U> >lchild = NIL,
         std::shared_ptr<TreeNode<T, U> >rchild = NIL) :
    key(key, val), color(color), lchild(lchild), rchild(rchild), father(NIL), height(1) {}
    TreeNode<T, U> &operator=(U val) {
        this->key.second = val;
        return *this;
    }
    std::pair<T, U> key;
    std::weak_ptr<TreeNode<T, U> > father;
    std::shared_ptr<TreeNode<T, U> > lchild, rchild;
    int color;
    int height;
    mutable std::shared_mutex node_mtx;
};


template<typename T, typename U, typename Node>
class IteratorHelper {
    using ptr = std::shared_ptr<Node>;
public :
    static ptr get_next(ptr p) {
        if (p->rchild != NIL) {
            p = p->rchild;
            while (p->lchild != NIL) p = p->lchild;
            return p;
        }
        ptr parent = p->father.lock();
        while (parent != NIL && parent->rchild == p) {
            p = p->father.lock();
            parent = p->father.lock();
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
    Iterator(std::shared_ptr<Node> node) : node(node) {}
    Node &operator*() const {
        return *static_cast<std::shared_ptr<Node> >(node);
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
    std::shared_ptr<Node> node;
};
#endif
