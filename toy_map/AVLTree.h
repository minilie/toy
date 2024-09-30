/*************************************************************************
	> File Name: AVLTree.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 27 Jul 2024 03:35:21 PM CST
 ************************************************************************/

#ifndef _AVLTREE_H
#define _AVLTREE_H

#include "My_functional.h"
#include "Iterator.h"

template<typename T, typename U, typename V = std::less<T>>
class AVLTree {
    using helper = IteratorHelper<T, U, TreeNode<T, U>>;
    using ptr = TreeNode<T, U> *; 
public :
    typedef Iterator<T, U, false, TreeNode<T, U> > iterator;
    typedef Iterator<T, U, true, TreeNode<T, U> > const_iterator;

    AVLTree() : root(MakeNode::Cnode<TreeNode<T, U>, T, U >(this->pool, T(), U())), cmp(V()), node_cnt(0) {}
    AVLTree(V cmp) : root(MakeNode::Cnode<TreeNode<T, U>, T, U>(this->pool, T(), U())), cmp(cmp), node_cnt(0) {}
    AVLTree(const AVLTree &obj) : root(MakeNode::Cnode<TreeNode<T, U>, T, U>(this->pool, T(), U(), BLACK)), cmp(obj.cmp), node_cnt(0) {
        for (auto iter = obj.begin(); iter != obj.end(); iter++) {
            this->insertinter(std::make_pair(iter->first, iter->second));
        }
        return ;
    }
    ~AVLTree() {
        M_pool::DePool(this->pool);
        return ;
    }

    AVLTree &operator=(const AVLTree &obj) {
        this->root = obj.root;
        return *this;
    }
    U &operator[](T key) {
        iterator p = this->find(key);
        if (p == this->end()) {
            std::pair<T, U> k = std::make_pair(key, U());
            return this->insert(k).first->second;
        }
        return p->second;
    }

    long long size() { return this->node_cnt; }
    
    template<typename N>
    std::pair<iterator, bool> insert(N _p) {
        std::pair<iterator, bool> __p = insertinter(_p);
        return __p;
    }

    ptr erase(T key) {
        this->root->lchild = eraseimple(this->root->lchild, key);
        return root->lchild;
    }
    ptr erase(iterator iter) {
        this->root->lchild = eraseimple(this->root->lchild, iter->first);
        return root->lchild;
    }
    ptr erase(iterator itera, iterator iterb) {
        for (auto iter = itera; iter != (iterb.next());) {
            this->erase(iter++);
        }
        return root->lchild;
    }

    iterator find(const T &key) {
        ptr _p = findimple(this->root->lchild, key);
        if (_p == NIL) return this->end();
        return iterator(_p);
    }
    const_iterator find(const T &key) const {
        ptr _p = findimple(this->root->lchild, key);
        if (_p == NIL) return this->end();
        return const_iterator(_p);
    }


    iterator begin() {
        if (this->root->lchild == NIL) return this->root;
        ptr p = this->root->lchild;
        if (this->root->lchild == NIL) return this->root->lchild;
        while (p->lchild != NIL) p = p->lchild;
        return iterator(p);
    }
    const_iterator begin() const {
        if (this->root->lchild == NIL) return this->root;
        ptr p = this->root->lchild;
        if (this->root->lchild == NIL) return this->root->lchild;
        while (p->lchild != NIL) p = p->lchild;
        return const_iterator(p);
    }

    iterator end() {
        return iterator(this->root);
    }
    const_iterator end() const {
        return const_iterator(this->root);
    }

    void output() {
        outputimple(this->root->lchild);
        std::cout << std::endl;
        return ;
    }

private :

    std::pair<iterator, bool> insertinter(std::pair<T, U> p) {
        bool flag = 1;
        iterator insert_succeed(NIL);
        this->root->lchild = insertimple(this->root->lchild, p.first, p.second, insert_succeed, flag);
        this->root->lchild->father = this->root;
        return std::make_pair(insert_succeed, flag);
    }

    void outputimple(ptr root) {
        if (root == NIL) return ;
        std::cout << "(" << root->height << ") | (" << root->father->key.first << ") "
        << root->key.first << "(" << root->key.second << ") " << "["
        << root->lchild->key.first << ", " << root->rchild->key.first << "]" << std::endl;
        outputimple(root->lchild);
        outputimple(root->rchild);
        return ;
    }

    ptr left_rotate(ptr root) {
        ptr new_root = root->rchild;
        if (root->father->lchild == root) root->father->lchild = new_root;
        else if (root->father->rchild == root) root->father->rchild = new_root;
        new_root->father = root->father;
        root->rchild = new_root->lchild;
        new_root->lchild->father = root;
        new_root->lchild = root;
        root->father = new_root;
        update_height(root);
        update_height(new_root);
        return new_root;
    }
    ptr right_rotate(ptr root) {
        ptr new_root = root->lchild;
        if (root->father->lchild == root) root->father->lchild = new_root;
        else if (root->father->rchild == root) root->father->rchild = new_root;
        new_root->father = root->father;
        root->lchild = new_root->rchild;
        new_root->rchild->father = root;
        new_root->rchild = root;
        root->father = new_root;
        update_height(root);
        update_height(new_root);
        return new_root;
    }

    void update_height(ptr root) {
        root->height = (root->lchild->height > root->rchild->height ? root->lchild->height : root->rchild->height) + 1;
        return ;
    }

    ptr balance(ptr root) {
        if (abs(root->lchild->height - root->rchild->height) <= 1) return root;
        if (root->lchild->height > root->rchild->height) {
            if (root->lchild->rchild->height > root->lchild->lchild->height) {
                root->lchild = left_rotate(root->lchild);
            }
            root = right_rotate(root);
        } else {
            if (root->rchild->lchild->height > root->rchild->rchild->height) {
                root->rchild = right_rotate(root->rchild);
            }
            root = left_rotate(root);
        }
        return root;
    }

    ptr insertimple(ptr root, T key, U val, iterator &succeed, bool &flag) {
        if (root == NIL) {
            ptr p = MakeNode::Cnode<TreeNode<T, U>, T, U>(this->pool, key, val); 
            this->node_cnt += 1;
            succeed = p;
            flag = true;
            return p;
        }
         if (root->key.first == key) {
            succeed = root;
            flag = false;
            return root;
        }
        if (cmp(key, root->key.first)) {
            root->lchild = insertimple(root->lchild, key, val, succeed, flag);
            root->lchild->father = root;
        }
        else {
            root->rchild = insertimple(root->rchild, key, val, succeed, flag);
            root->rchild->father = root;
        }
        update_height(root);
        return balance(root);
    }

    ptr eraseimple(ptr root, T key) {
        if (root == NIL) {
            return NIL;
        }
        if (cmp(key, root->key.first)) {
            root->lchild = eraseimple(root->lchild, key);
        }
        else if (cmp(root->key.first, key)) {
            root->rchild = eraseimple(root->rchild, key);
        }
        else {
            if (root->lchild == NIL || root->rchild == NIL) {
                ptr temp = (root->lchild == NIL ? root->rchild : root->lchild);
                if (temp != NIL) temp->father = root->father;
                root = nullptr;
                this->node_cnt -= 1;
                return temp;
            }
            ptr temp = helper::get_predecessor(root);
            root->key = temp->key;
            root->lchild = eraseimple(root->lchild, temp->key.first);
        }
        update_height(root);
        return balance(root);
    }

    ptr findimple(ptr root, T key) {
        if (root == NIL) {
            return NIL;
        }
        if (root->key.first == key) return root;
        if (cmp(key, root->key.first)) return findimple(root->lchild, key);
        else return findimple(root->rchild, key);
    }

    M_pool *pool = M_pool::Creat_Pool(2048);
    ptr root;
    mfunction<bool(int, int)> cmp;
    long long node_cnt;
};
#endif
