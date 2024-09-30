/*************************************************************************
	> File Name: BTree.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Fri 27 Sep 2024 10:44:20 PM CST
 ************************************************************************/

#ifndef _BTREE_H
#define _BTREE_H

#include "Iterator.h"
#include "My_functional.h"
#include <vector>

const int MAX_M = 15;

template<typename T, typename U, typename V = std::less<T> >
class BTree;

template<typename T, typename U>
struct BNode {
    BNode(BTree<T, U> * tree) : n(0), fapos(0), father(nullptr), tree(tree) {}
    int n, fapos;
    std::pair<T, U> key[MAX_M + 1];
    BNode<T, U> *next[MAX_M + 1], *father;
    BTree<T, U> *tree;
};

template<typename T, typename U, bool if_const, typename Node>
class BIterator;

template<typename T, typename U, typename Node>
class BIhelper {
    using ptr = Node *;
    using iterator = BIterator<T, U, 0, Node>;
    using const_iterator = BIterator<T, U, 1, Node>;
public :

    static iterator __get_next(iterator &p) {
        if (p.node == p.node->tree->root) {
            return p;
        }
        if (p.node->next[p.pos + 1] == nullptr) {

            if (p.pos < p.node->n - 1) {
                return iterator(p.node, p.node->key[p.pos + 1].first);
            }
            if (p.node->fapos < p.node->father->n) {
                return iterator(p.node->father, p.node->father->key[p.node->fapos].first);
            } else {
                ptr temp = p.node;
                while (temp->father != nullptr && temp->fapos == temp->father->n) temp = temp->father;
                return iterator(temp->father, temp->father->key[temp->fapos].first);
            }
        } 
        else {
            ptr temp = p.node->next[p.pos + 1];
            while (temp != nullptr && temp->next[0] != nullptr) temp = temp->next[0];
            return iterator(temp, temp->key[0].first);
        }
    }
    static iterator get_next(iterator &p) {
        p = p.node->tree->find(p.val);
        return __get_next(p);
    }

    static iterator __get_pre(iterator &p) {
        if (p.node == p.node->tree->root) {
            return p;
        }
        if (p.node->next[0] == nullptr) {

            if (p.pos > 0) {
                return iterator(p.node, p.node->key[p.pos - 1].first);
            }
            if (p.node->fapos > 0) {
                return iterator(p.node->father, p.node->father->key[p.node->fapos].first);
            } else {
                ptr temp = p.node;
                while (temp->father != nullptr && temp->fapos == 0) temp = temp->father;
                return iterator(temp->father, temp->father->key[temp->fapos].first);
            }
        } 
        else {
            ptr temp = p.node->next[p.pos - 1];
            while (temp != nullptr && temp->next[temp->n] != nullptr) temp = temp->next[temp->n];
            return iterator(temp, temp->key[temp->n - 1].first);
        }
    }
    static iterator get_pre(iterator &p) {
        p = p.node->tree->find(p.val);
        return __get_pre(p);
    }

};

template<typename T, typename U, bool if_const, typename Node>
class BIterator {
    using helper = BIhelper<T, U, Node>;
    using ref = std::conditional_t<if_const, const std::pair<T, U> &, std::pair<T, U> &>;
    using ptr = std::conditional_t<if_const, const std::pair<T, U> *, std::pair<T, U> *>;
    using self = BIterator<T, U, if_const, Node>;

public :
    BIterator(Node *node = nullptr, T val = 0, int pos = 0) : node(node), val(val) {
        if (this->node == nullptr) return ;
            for (int i = 0; i < this->node->n; i++) {
                if (this->node->key[i].first != this->val) continue;
                this->pos = i;
                break;
            }
        return ;
    }
    BIterator(const BIterator &obj) : node(obj.node), val(obj.val), pos(obj.pos) {}

    Node &operator*() const {
        return *static_cast<Node *>(this->node);
    }

    ptr operator->() {
        return &((*(*this)).key[this->pos]);
    }
    ptr operator->() const {
        return &((*(*this)).key[this->pos]);
    }

    self operator++() {
        self temp = helper::get_next(*this);
        *this = temp;
        return *this;
    }
    self operator++(int) {
        self temp = helper::get_next(*this);
        self x(*this);
        *this = temp;
        return x;
    }

    self operator--() {
        self temp = helper::get_pre(*this);
        this->node = temp.node;
        this->pos = temp.pos;
        return *this;
    }
    self operator--(int) {
        self temp = helper::get_next(*this);
        self iter(*this);
        this->node = temp->node;
        this->pos = temp->pos;
        return iter;
    }

    self next() {
        return self(helper::get_next(*this));
    }
    self next() const {
        return self(helper::get_next(*this));
    }

    self pre() {
        return self(helper::get_pre(*this));
    }
    self pre() const {
        return self(helper::get_pre(*this));
    }

    self &operator=(const U &val) {
        if (this->node == nullptr) return *this;
        this->node->key[pos].second = val;
        return *this;
    }

    bool operator!=(const self &obj) {
        return this->node != obj.node;
    }
    bool operator==(const self &obj) {
        return this->node == obj.node;
    }

    Node *node;
    int pos;
    T val;
};


template<typename T, typename U, typename V>
class BTree {
    using ptr = BNode<T, U> *;
    using helper = BIhelper<T, U, BNode<T, U> >;
public :
    typedef BIterator<T, U, false, BNode<T, U> > iterator;
    typedef BIterator<T, U, true, BNode<T, U> > const_iterator;

    BTree() : root(MakeNode::Cnode<BNode<T, U> >(this->pool, this)), cmp(V()), node_cnt(0) {
        this->root->father = nullptr;
        return ;
    }
    BTree(V cmp) : root(MakeNode::Cnode<BNode<T, U> >(this->pool, this)), cmp(cmp), node_cnt(0) {
        this->root->father = nullptr;
        return ;
    }
    BTree(const BTree &obj) : root(MakeNode::Cnode<BNode<T, U> >(this->pool, this)), cmp(obj.cmp), node_cnt(0) {
        for (auto iter = obj.begin(); iter != obj.end(); iter++) {
            this->insert(iter.node->key[iter.pos].first);
        }
        this->root->father = nullptr;
        return ;
    }
    ~BTree() {
        M_pool::DePool(this->pool);
        return ;
    }

    BTree &operator=(const BTree &obj) {
        this->root = obj.root;
        return *this; 
    }
    U &operator[](T key) {
        iterator p = this->find(key);
        if (p == this->end()) {
            std::pair<T, U> k = std::make_pair(key, 233);
            this->insert(k).first->second;
            return this->find(key)->second;
        }
        return p->second;
    }

    long long size() { return this->node_cnt; }

    template<typename N>
    std::pair<iterator, bool> insert(N _p) {
        iterator op(nullptr);
        insertimple(_p, op);
        std::pair<iterator, bool> __p = std::make_pair(op, true);
        this->root->father = nullptr;
        return __p;
    }

    ptr erase(T key) {
        return this->eraseimple(key);
    }
    ptr erase(iterator &iter) {
        T key = iter.node->key[iter.pos].first;
        return this->erase(key);
    }
    ptr erase(iterator &itera, iterator &iterb) {
        T key = iterb.node->key[iterb.pos].first;
        auto p = itera;
        T val;
        while (p.node->key[p.pos].first != key) {
            auto q = p;
            p++;
            val = p.node->key[p.pos].first;
            this->erase(q);
            p = this->find(val);
        }
        return this->root->next[0];
    }

    iterator find(const T &val) {
        iterator p = findimple(this->root->next[0], val);
        if (p.node == nullptr) return this->end();
        return p;
    }
    const_iterator find(const T &val) const {
        iterator p = findimple(this->root->next[0], val);
        if (p.node == nullptr) return this->end();
        return p;
    }

    iterator begin() {
        if (!this->node_cnt) return this->root;
        ptr p = this->root->next[0];
        while (p->next[0] != nullptr) p = p->next[0];
        return iterator(p, p->key[0].first);
    }
    const_iterator begin() const {
        if (!this->node_cnt) return this->root;
        ptr p = this->root->next[0];
        while (p->next[0] != nullptr) p = p->next[0];
        return iterator(p, p->key[0].first);
    }

    iterator end() {
        return this->root;
    }
    const_iterator end() const {
        return this->root;
    }

    void output() {
        if (!this->node_cnt) return ;
        __output(this->root->next[0]);
        return ;
    }

private :
    iterator findimple(ptr root, T val) {
        if (root == nullptr) return iterator(nullptr);
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val) pos += 1;
        if (root->key[pos].first == val) {
            return iterator(root, val, pos);
        }
        return findimple(root->next[pos], val);
    }

    ptr insert_balance(ptr root, ptr child, int pos) {
        if (child->n < MAX_M) {
            // child->fapos = pos;
            // child->father = root;
            return root;
        }
        int spos = MAX_M / 2;
        ptr node1 = MakeNode::Cnode<BNode<T, U> >(this->pool, this);
        ptr node2 = MakeNode::Cnode<BNode<T, U> >(this->pool, this);
        node1->n = spos;
        node2->n = MAX_M - 1 - spos;
        for (int i = 0; i < spos; i++) {
            node1->key[i] = child->key[i];
            node1->next[i] = child->next[i];
            if (node1->next[i] != nullptr) {
                node1->next[i]->fapos = i;
                node1->next[i]->father = node1;
            }
        }
        node1->next[spos] = child->next[spos];
        if (node1->next[spos] != nullptr) node1->next[spos]->father = node1;
        for (int i = 0; i < node2->n; i++) {
            node2->key[i] = child->key[i + spos + 1];
            node2->next[i] = child->next[i + spos + 1];
            if (node2->next[i] != nullptr) {
                node2->next[i]->fapos = i;
                node2->next[i]->father = node2;
            }
        }
        node2->next[node2->n] = child->next[child->n];
        if (node2->next[node2->n] != nullptr) {
            node2->next[node2->n]->fapos = node2->n;
            node2->next[node2->n]->father = node2;
        }
        for (int i = root->n; i >= pos; i--) {
            root->key[i + 1]  = root->key[i];
            root->next[i + 1] = root->next[i];
            if (root->next[i + 1] != nullptr) {
                root->next[i + 1]->fapos = i + 1;
                root->next[i + 1]->father = root; 
            }
        } 
        root->key[pos] = child->key[spos];
        root->next[pos] = node1;
        root->next[pos + 1] = node2;
        root->n += 1;
        // make tail
        node1->fapos = pos;
        node2->fapos = pos + 1;
        node1->father = node2->father = root;
        child = nullptr;
        return root;
    }

    ptr insert_val(ptr root, std::pair<T, U> val, iterator &op) {
        if (root == nullptr) {
            root = MakeNode::Cnode<BNode<T, U> >(this->pool, this);
            root->key[(root->n)++] = val;
            op = iterator(root, val.first);
            this->node_cnt += 1;
            return root;
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val.first) pos += 1;
        if (root->key[pos].first == val.first) {
            root->key[pos] = val;
            op = iterator(root, val.first);
            return root;
        }
        for (int i = root->n - 1; i >= pos; i--) {
            root->key[i + 1] = root->key[i];
        }
        root->key[pos] = val;
        op = iterator(root, val.first);
        root->n += 1;
        this->node_cnt += 1;
        return root;
    }

    ptr __insert(ptr root, std::pair<T, U> val, iterator &op) {
        if (root == nullptr || root->next[0] == nullptr) {
            return insert_val(root, val, op);
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val.first) pos += 1;
        if (pos < root->n && root->key[pos].first == val.first) { 
            root->key[pos] = val;
            op = iterator(root, val.first);
            return root;
        }
        ptr temp = __insert(root->next[pos], val, op);
        // make tail
        temp->fapos = pos;
        temp->father = root;
        return insert_balance(root, root->next[pos], pos);
    }

    iterator insertimple(std::pair<T, U> val, iterator &op) {
        this->root->next[0] = __insert(this->root->next[0], val, op);
        this->root->n = 1;
        if (this->root->next[0]->n == MAX_M) {
            ptr p = MakeNode::Cnode<BNode<T, U> >(this->pool, this);
            p->next[0] = this->root->next[0];
            this->root->next[0] = insert_balance(p, this->root->next[0], 0);
        } // n must be < MAX_M

        // make tail
        this->root->next[0]->fapos = 0;
        this->root->next[0]->father = this->root;

        return op;
    }

    void right_rotate(ptr root, int pos) {
        for (int i = root->next[pos + 1]->n + 1; i >= 0; i--) {
            root->next[pos + 1]->key[i] = root->next[pos + 1]->key[i - 1];
            root->next[pos + 1]->next[i] = root->next[pos + 1]->next[i - 1];
            if (root->next[pos + 1]->next[i] != nullptr) {
                root->next[pos + 1]->next[i]->fapos = i;
                root->next[pos + 1]->next[i]->father = root->next[pos + 1];
            }
        }
        root->next[pos + 1]->key[0] = root->key[pos];
        root->key[pos] = root->next[pos]->key[root->next[pos]->n - 1];
        root->next[pos + 1]->next[0] = root->next[pos]->next[root->next[pos]->n];
        
        if (root->next[pos + 1]->next[0] != nullptr) {
            root->next[pos + 1]->next[0]->fapos = 0;
            root->next[pos + 1]->next[0]->father = root->next[pos + 1];
        }
        root->next[pos]->next[root->next[pos]->n] = nullptr;
        root->next[pos + 1]->n += 1;
        root->next[pos]->n -= 1;
        return ;
    }

    void left_rotate(ptr root, int pos) {
        root->next[pos]->key[root->next[pos]->n] = root->key[pos];
        root->next[pos]->n += 1;
        root->key[pos] = root->next[pos + 1]->key[0];
        root->next[pos]->next[root->next[pos]->n] = root->next[pos + 1]->next[0];
        
        if (root->next[pos]->next[root->next[pos]->n] != nullptr) {
            root->next[pos]->next[root->next[pos]->n]->fapos = root->next[pos]->n;
            root->next[pos]->next[root->next[pos]->n]->father= root->next[pos];
        }

        for (int i = 0; i < root->next[pos + 1]->n; i++) {
            root->next[pos + 1]->key[i] = root->next[pos + 1]->key[i + 1];
            root->next[pos + 1]->next[i] = root->next[pos + 1]->next[i + 1];
            if (root->next[pos + 1]->next[i] != nullptr) {
                root->next[pos + 1]->next[i]->fapos = i;
                root->next[pos + 1]->next[i]->father = root->next[pos + 1];
            }
        }
        root->next[pos + 1]->next[root->next[pos + 1]->n] = nullptr;
        root->next[pos + 1]->n -= 1;
        return ;
    }

    void merge_node(ptr root, int pos) {
        ptr node = MakeNode::Cnode<BNode<T, U> >(this->pool, this);
        for (int i = 0; i <= root->next[pos]->n; i++) {
            node->key[i] = root->next[pos]->key[i];
            node->next[i] = root->next[pos]->next[i];
            if (node->next[i] != nullptr) {
                node->next[i]->fapos = i;
                node->next[i]->father = node;
            }
        }
        node->n = root->next[pos]->n + 1;
        node->key[node->n - 1] = root->key[pos];
        for (int i = 0; i <= root->next[pos + 1]->n; i++) {
            node->key[node->n + i]  = root->next[pos + 1]->key[i];
            node->next[node->n + i] = root->next[pos + 1]->next[i];
            if (node->next[node->n + i] != nullptr) {
                node->next[node->n + i]->fapos = node->n + i;
                node->next[node->n + i]->father = node;
            }
        }
        node->n += root->next[pos + 1]->n;
        root->next[pos] = nullptr;
        root->next[pos + 1] = nullptr;
        for (int i = pos + 1; i <= root->n; i++) {
            root->key[i - 1] = root->key[i];
            root->next[i - 1] = root->next[i];
            if (root->next[i - 1] != nullptr) {
                root->next[i - 1]->fapos = i - 1;
                root->next[i - 1]->father = root;
            }
        }
        root->next[pos] = node;

        // make tail
        node->fapos = pos;
        node->father = root;
        root->n -= 1;
        return ;
    }

    ptr erase_balance(ptr root, int pos) {
        int lower_bound = (MAX_M + 1) / 2 - 1;
        if (root->next[pos]->n >= lower_bound) return root;
        if (pos > 0 && root->next[pos - 1]->n > lower_bound) {
            right_rotate(root, pos - 1);
        } else if (pos < root->n && root->next[pos + 1]->n > lower_bound) {
            left_rotate(root, pos);
        } else {
            if (pos > 0) merge_node(root, pos - 1); //
            else merge_node(root, pos);
        }
        return root;
    }

    void erase_pos(ptr root, int pos) {
        for (int i = pos + 1; i < root->n; i++) {
            root->key[i - 1] = root->key[i];
        }
        root->n -= 1;
        this->node_cnt -= 1;
        return ;
    }

    ptr __erase(ptr root, T val) {
        if (root == nullptr) {
            std::cout << " there is no elements like this " << std::endl;
            return root;
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val) pos += 1;
        if (root->next[0] == nullptr) {
            if (root->key[pos].first == val) erase_pos(root, pos);
            return root;
        } else {
            if (pos < root->n && root->key[pos].first == val) {
                ptr temp = root->next[pos];
                while (temp->next[temp->n] != nullptr) temp = temp->next[temp->n];
                swap(root->key[pos], temp->key[temp->n - 1]);
            }
            root->next[pos] = __erase(root->next[pos], val);
            // make tail
            if (root->next[pos] != nullptr) {
                root->next[pos]->fapos = pos;
                root->next[pos]->father = root;
            }
        }
        return erase_balance(root, pos);
    }

    ptr eraseimple(T val) {
        this->root->next[0] = __erase(this->root->next[0], val);
        if (!this->node_cnt) {
            this->root->n = 0;
            return this->root;
        }
        if (this->root->next[0]->n == 0) {
            ptr p = this->root->next[0]->next[0];
            this->root->next[0] = nullptr;
            this->root->next[0] = p;
        }

        // make tail
        if (this->root->next[0] == nullptr) {
            this->root->n = 0;
            return this->root;
        }
        this->root->next[0]->fapos = 0;
        this->root->next[0]->father = this->root;

        return this->root->next[0];
    }

    void print_node(ptr root) {
        std::cout << root->n << " : " << std::endl;
        for (int i = 0; i < root->n; i++) {
            int faval = root->father->n == root->fapos ? root->father->key[root->fapos - 1].first : root->father->key[root->fapos].first;
            std::cout << root->key[i].first << "(fa->val = " << faval << ", fapos = " << root->fapos << ") " << std::endl;
        }
        if (root->next[0] == nullptr) goto print_end;
        std::cout << "| [ ";
        for (int i = 0; i <= root->n; i++) {
            std::cout << root->next[i]->key[0].first << " ";
        }
        std::cout << "]";
    print_end:
        std::cout << std::endl;
        return ;
    }

    void __output(ptr root) {
        if (root == nullptr) return ;
        print_node(root);
        for (int i = 0; i <= root->n; i++) {
            __output(root->next[i]);
        }
        return ;
    }

    M_pool *pool = M_pool::Creat_Pool(2048);
    mfunction<bool(T, U)> cmp;
    long long node_cnt;
public :
    ptr root; // virtual root
};


#endif
