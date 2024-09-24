/*************************************************************************
	> File Name: Btree.cpp
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Sat 21 Sep 2024 07:46:35 PM CST
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <stack>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <vector>
#include "2.h"
#include "My_functional.h"
#include <unistd.h>
using namespace std;

const int MAX_M = 5;

template<typename T, typename U>
struct BNode {
    BNode() : n(0), fapos(0), father(nullptr) {}
    int n, fapos;
    std::pair<T, U> key[MAX_M + 1];
    BNode<T, U> *next[MAX_M + 1], *father;
    BNode<T, U> &operator=(U val) {
        this->key.second = val;
        return *this;
    }
};


template<typename T, typename U, bool if_const, typename Node>
class BIterator; 

template<typename T, typename U, typename Node>
class BIhelper {
    using ptr = Node *;
    using iterator = BIterator<T, U, 0, Node>;
    using const_iterator = BIterator<T, U, 1, Node>;
public :
    static void maketail(ptr p, int posfa, ptr fa) {
        if (p == nullptr) return ;
        p->fapos = posfa;
        p->father = fa;
        for (int i = 0; i <= p->n; i++) {
            maketail(p->next[i], i, p);
        }
        return ;
    }

    static iterator get_next(iterator &p) {
        if (p.node->next[p.pos + 1] != nullptr) {
            printf("-----key : %d has right child-----\n", p.node->key[p.pos].first);
            Node *temp = p.node->next[p.pos + 1];
            while (temp->next[0] != nullptr) temp = temp->next[0];
            return temp;
        }
        else if (p.pos < p.node->n - 1) {
            printf("-----key : %d has right element-----\n", p.node->key[p.pos].first);
            return iterator(p.node, p.pos + 1);
        } else {
            if (p.node->fapos < p.node->father->n) {
            // if (p.node->father->next[p.node->fapos + 1] != nullptr) {
            printf("-----key : %d in father's left-----\n", p.node->key[p.pos].first);
                return iterator(p.node->father, p.node->fapos);
            } else {
                printf("-----key : %d in father's right-----\n", p.node->key[p.pos].first);
                ptr temp = p.node;
                // while (temp->father != nullptr && temp->father->next[temp->fapos + 1] == nullptr) temp = temp->father;
                while (temp->father != nullptr && temp->fapos == temp->father->n) temp = temp->father;
                printf("=====key : %d =====\n", temp->key[0].first);
                printf("====='s father key : %d=====\n", temp->father->key[temp->fapos].first);
                return iterator(temp->father, temp->fapos);
            }
        }
    }

    static iterator get_pre(iterator &p) {
        if (p.node->next[p.pos] != nullptr) {
            printf("-----key : %d has left child-----\n", p.node->key[p.pos].first);
            Node *temp = p.node->next[p.pos];
            while (temp->next[temp->n] != nullptr) temp = temp->next[temp->n];
            return temp;
        }
        else if (p.pos > 0) {
            printf("-----key : %d has left element-----\n", p.node->key[p.pos].first);
            return iterator(p.node, p.pos + 1);
        } else {
            if (p.node->fapos > 0) {
            // if (p.node->father->next[p.node->fapos + 1] != nullptr) {
            printf("-----key : %d in father's right-----\n", p.node->key[p.pos].first);
                return iterator(p.node->father, p.node->fapos);
            } else {
                printf("-----key : %d in father's right-----\n", p.node->key[p.pos].first);
                ptr temp = p.node;
                // while (temp->father != nullptr && temp->father->next[temp->fapos + 1] == nullptr) temp = temp->father;
                while (temp->father != nullptr && temp->fapos == 0) temp = temp->father;
                printf("=====key : %d =====\n", temp->key[0].first);
                printf("====='s father key : %d=====\n", temp->father->key[temp->fapos].first);
                return iterator(temp->father, temp->fapos);
            }
        }
    }


};


template<typename T, typename U, bool if_const, typename Node>
class BIterator {
    using helper = BIhelper<T, U, Node>;
    using ref = std::conditional_t<if_const, const std::pair<T, U> &, std::pair<T, U> &>;
    using ptr = std::conditional_t<if_const, const std::pair<T, U> *, std::pair<T, U> *>;
    using self = BIterator<T, U, if_const, Node>;

public :
    BIterator(Node *node, int pos = 0) : node(node), pos(pos) {}

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
        this->node = temp.node;
        this->pos = temp.pos;
        return *this;
    }
    self operator++(int) {
        self temp = helper::get_next(*this);
        self x(*this);
        this->node = temp.node;
        this->pos = temp.pos;
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
};


template<typename T, typename U, typename V = std::less<T>>
class BTree {
    using ptr = BNode<T, U> *;
    using helper = BIhelper<T, U, BNode<T, U> >;
public :
    typedef BIterator<T, U, false, BNode<T, U> > iterator;
    typedef BIterator<T, U, true, BNode<T, U> > const_iterator;

    BTree() : root(MakeNode::Cnode<BNode<T, U> >(this->pool)), cmp(V()), node_cnt(0) {}
    BTree(V cmp) : root(MakeNode::Cnode<BNode<T, U> >(this->pool)), cmp(cmp), node_cnt(0) {} 
    BTree(const BTree &obj) : root(MakeNode::Cnode<BNode<T, U> >(this->pool)), cmp(obj.cmp), node_cnt(0) {
        for (auto iter = obj.begin(); iter != obj.end(); iter++) {
            this->insert(iter.node->key[iter.pos].first);
        }
        return ;
    }
    ~BTree() {
        M_pool::DePool(this->pool);
        return ;
    }

    BTree &operator=(const BTree &obj) {
        this->root = obj.root;
        return *this; // TODO
    }
    BNode<T, U> &operator[](T key) {
        iterator p = this->find(key);
        if (p == this->end()) {
            std::pair<T, U> k = std::make_pair(key, U());
            return *(this->insert(k).first);
        }
        return *(p);
    }

    long long size() { return this->node_cnt; }
    
    template<typename N>
    std::pair<iterator, bool> insert(N _p) {
        iterator op(nullptr);
        insertimple(_p, op);
        std::pair<iterator, bool> __p = std::make_pair(op, true);
        return __p;
    }

    ptr erase(T key) {
        return eraseimple(key);
    }
    ptr erase(iterator iter) {
        return eraseimple(iter.node->key[iter.pos]);
    }
    ptr erase(iterator itera, iterator iterb) {
        for (auto iter = itera; iter != (iterb.next());) {
            this->erase(iter++);
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
        if (this->root->next[0] == nullptr) return this->root;
        ptr p = this->root->next[0];
        while (p->next[0] != nullptr) p = p->next[0];
        return p;
    }
    const_iterator begin() const {
        if (this->root->next[0] == nullptr) return this->root;
        ptr p = this->root->next[0];
        while (p->next[0] != nullptr) p = p->next[0];
        return p;
    }

    iterator end() {
        return this->root;
    }
    const_iterator end() const {
        return this->root;
    }
   
    void output() {
        if (!this->node_cnt) return ;
        helper::maketail(this->root->next[0], 0, this->root);
        cout << "=-=-=- virtual root 's n : " << this->root->n <<  "-=-=-=" << endl;
        __output(this->root->next[0]);
        return ;
    }

private :
    iterator findimple(ptr root, T val) {
        if (root == nullptr) return iterator(nullptr);
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val) pos += 1;
        if (root->key[pos].first == val) {
            return iterator(root, pos);
        }
        return findimple(root->next[pos], val);
    }

    ptr insert_balance(ptr root, ptr child, int pos) {
        if (child->n < MAX_M) return root;
        int spos = MAX_M / 2;
        ptr node1 = MakeNode::Cnode<BNode<T, U> >(this->pool);
        ptr node2 = MakeNode::Cnode<BNode<T, U> >(this->pool);
        node1->n = spos;
        node2->n = MAX_M - 1 - spos;
        for (int i = 0; i < spos; i++) {
            node1->key[i] = child->key[i];
            node1->next[i] = child->next[i];
        }
        node1->next[spos] = child->next[spos];
        for (int i = 0; i < node2->n; i++) {
            node2->key[i] = child->key[i + spos + 1];
            node2->next[i] = child->next[i + spos + 1];
        }
        node2->next[node2->n] = child->next[child->n];
        for (int i = root->n; i >= pos; i--) {
            root->key[i + 1]  = root->key[i];
            root->next[i + 1] = root->next[i];
        } // TODO
        root->key[pos] = child->key[spos];
        root->next[pos] = node1;
        root->next[pos + 1] = node2;
        root->n += 1;
        // free(child);
        return root;
    }

    ptr insert_val(ptr root, std::pair<T, U> val, iterator &op) {
        if (root == nullptr) {
            root = MakeNode::Cnode<BNode<T, U> >(this->pool);
            root->key[(root->n)++] = val;
            op = iterator(root, root->n - 1);
            this->node_cnt += 1;
            return root;
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val.first) pos += 1;
        if (root->key[pos].first == val.first) {
            root->key[pos] = val;
            op = iterator(root, pos);
            return root;
        }
        for (int i = root->n - 1; i >= pos; i--) {
            root->key[i + 1] = root->key[i];
        }
        root->key[pos] = val;
        op = iterator(root, pos);
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
        if (pos < root->n && root->key[pos].first == val.first) { // TODO
            root->key[pos] = val;
            op = iterator(root, pos);
            // this->node_cnt += 1;
            return root;
        }
        __insert(root->next[pos], val, op);
        return insert_balance(root, root->next[pos], pos);
    }

    iterator insertimple(std::pair<T, U> val, iterator &op) {
        this->root->next[0] = __insert(this->root->next[0], val, op);
        this->root->n = 1;
        if (this->root->next[0]->n == MAX_M) {
            ptr p = MakeNode::Cnode<BNode<T, U> >(this->pool); 
            p->next[0] = this->root->next[0];
            this->root->next[0] = insert_balance(p, this->root->next[0], 0);
        } // n must be < MAX_M
        return op;
    }

    void right_rotate(ptr root, int pos) {
        for (int i = root->next[pos + 1]->n + 1; i >= 0; i--) {
            root->next[pos + 1]->key[i] = root->next[pos + 1]->key[i - 1];
            root->next[pos + 1]->next[i] = root->next[pos + 1]->next[i - 1];
        }
        root->next[pos + 1]->key[0] = root->key[pos];
        root->key[pos] = root->next[pos]->key[root->next[pos]->n - 1];
        root->next[pos + 1]->next[0] = root->next[pos]->next[root->next[pos]->n];
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
        for (int i = 0; i < root->next[pos + 1]->n; i++) {
            root->next[pos + 1]->key[i] = root->next[pos + 1]->key[i + 1];
            root->next[pos + 1]->next[i] = root->next[pos + 1]->next[i + 1];
        }
        root->next[pos + 1]->next[root->next[pos + 1]->n] = nullptr;
        root->next[pos + 1]->n -= 1;
        return ;
    }

    void merge_node(ptr root, int pos) {
        ptr node = MakeNode::Cnode<BNode<T, U> >(this->pool);
        for (int i = 0; i <= root->next[pos]->n; i++) {
            node->key[i] = root->next[pos]->key[i];
            node->next[i] = root->next[pos]->next[i];
        }
        node->n = root->next[pos]->n + 1;
        node->key[node->n - 1] = root->key[pos];
        for (int i = 0; i <= root->next[pos + 1]->n; i++) {
            node->key[node->n + i]  = root->next[pos + 1]->key[i];
            node->next[node->n + i] = root->next[pos + 1]->next[i];
        }
        node->n += root->next[pos + 1]->n;
        // free(root->next[pos]);
        // free(root->next[pos + 1]);
        for (int i = pos + 1; i <= root->n; i++) {
            root->key[i - 1] = root->key[i];
            root->next[i - 1] = root->next[i];
        }
        root->next[pos] = node;
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
        if (root == nullptr) return root;
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val) pos += 1;
        if (root->next[0] == nullptr) {
            if (root->key[pos].first == val) erase_pos(root, pos);
            return root;
        } else {
            if (pos < root->n && root->key[pos].first == val) {
                ptr temp = root->next[pos];
                while (temp->next[temp->n]) temp = temp->next[temp->n];
                swap(root->key[pos], temp->key[temp->n - 1]);
            }
            root->next[pos] = __erase(root->next[pos], val);
        }
        return erase_balance(root, pos);
    }

    ptr eraseimple(T val) {
        if (!this->node_cnt) return this->root;
        this->root->next[0] = __erase(this->root->next[0], val);
        if (this->root->next[0]->n == 0) {
            ptr p = this->root->next[0]->next[0];
            cout << "root -> n == 0" << endl;
            // free(this->root->next[0]);
            this->root->next[0] = p;
        }
        return this->root->next[0];
    }
    
    void print_node(ptr root) {
        printf("%d : \n", root->n);
        for (int i = 0; i < root->n; i++) {
            int faval = root->father->n == root->fapos ? root->father->key[root->fapos - 1].first : root->father->key[root->fapos].first;
            printf("%d(fa->val = %d, fapos = %d) \n", root->key[i].first, faval, root->fapos);
        }
        if (root->next[0] == nullptr) goto print_end;
        printf("| [ ");
        for (int i = 0; i <= root->n; i++) {
            printf("%d ", root->next[i]->key[0].first);
        }
        printf("]");
    print_end:
        printf("\n");
        return ;
    }
    
    void __output(ptr root) {
        if (root == nullptr) return ;
        print_node(root);
        for (int i = 0; i <= root->n; i++) {
            __output(root->next[i]);
        }
        cout << "the node cnt : " << this->node_cnt << endl;
        return ;
    }

    M_pool *pool = M_pool::Creat_Pool(2048);
    mfunction<bool(T, U)> cmp;
    ptr root; // virtual root
    long long node_cnt;
};

bool testcomp(BTree<int, int> &m) {
    int firstkey = m.begin()->first;
    for (auto iter = m.begin().next(); iter != m.end(); iter++) {
        if (iter->first < firstkey) return 0;
        firstkey = iter->first;
    }
    return 1;
}

int main() {
    srand(time(0));
    BTree<int, int> m;

    for (int i = 0; i < 10; i++) {
        int x = rand() % 100 + 1;
        int y = rand() % 100;
        m.insert(std::make_pair(x, y));
        printf("insert %d into the tree\n\n", x);
    }
    m.output();
    cout << endl << endl << endl;
    cout << "test ans is : " << testcomp(m) << endl << endl << endl;

    int p;
    
    while (~scanf("%d", &p)) {
        if (p == -1) break;
        printf("find %d from the tree\n\n", p);
        if (m.find(p) != m.end()) cout << "\033[32;1m" << "successfully find" << "\033[0m"<< endl;
        else cout << "\033[31;1m" << "there is no " << p << "in this tree" << "\033[0m"<< endl;
    }
    while (~scanf("%d", &p)) {
        if (p == -1) break;
        printf("erase %d from the tree\n\n", p);
        m.erase(p);
        m.output();
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            cout << "\033[31;1m" << p->first  << "\033[0m"<< endl;
        }
        if (m.begin() == m.end()) {
            cout << "\033[31;1m" << "the tree is null" << "\033[0m"<< endl;
            exit(0);
        }
    }
    for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
        cout << p->first << " ";
    }
    cout << endl;

    return 0;
}
