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
using namespace std;

const int MAX_M = 5;

template<typename T, typename U>
struct BNode {
    BNode() : n(0), fapos(0), infa(false), father(nullptr) {}
    int n, fapos;
    bool infa; // 0 : left, 1 : right
    std::pair<T, U> key[MAX_M + 1];
    BNode<T, U> *next[MAX_M + 1], *father;
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

    static iterator get_next(iterator p) {
        if (p.node->next[p.pos + 1] != nullptr) {
            Node *temp = p.node->next[p.pos + 1];
            while (temp->next[0] != nullptr) temp = temp->next[0];
            return temp;
        }
        else if (p.pos < p.node->n - 1) {
            return iterator(p.node, p.pos + 1);
        } else {
            if (p.node->father->next[p.node->fapos + 1] != nullptr) {
                return iterator(p.node->father, p.node->fapos);
            } else {
                Node *temp = p.node;
                while (temp->father != nullptr && temp->father->next[temp->fapos + 1] == nullptr) temp = temp->father;
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
    
    ptr operator->() {
        return &((*(*this)).key[this->pos]);
    }
    ptr operator->() const {
        return &((*(*this)).key[this->pos]);
    }





private :
    Node *node;
    int pos;
};


template<typename T, typename U, typename V = std::less<T>>
class BTree {
    using ptr = BNode<T, U> *;
    using helper = BIhelper<T, U, BNode<T, U> >;
public :
    BTree() : root(MakeNode::Cnode<BNode<T, U> >(this->pool)), cmp(V()) {}
    BTree(V cmp) : root(MakeNode::Cnode<BNode<T, U> >(this->pool)), cmp(cmp) {} 
    // TODO copy constructor
    ~BTree() {
        M_pool::DePool(this->pool);
        return ;
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

    ptr insert_val(ptr root, std::pair<T, U> val) {
        if (root == nullptr) {
            root = MakeNode::Cnode<BNode<T, U> >(this->pool);
            root->key[(root->n)++] = val;
            return root;
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val.first) pos += 1;
        if (root->key[pos].first == val.first) {
            root->key[pos] = val;
            return root;
        }
        for (int i = root->n - 1; i >= pos; i--) {
            root->key[i + 1] = root->key[i];
        }
        root->key[pos] = val;
        root->n += 1;
        return root;
    }

    ptr __insert(ptr root, std::pair<T, U> val) {
        if (root == nullptr || root->next[0] == nullptr) {
            return insert_val(root, val);
        }
        int pos = 0;
        while (pos < root->n && root->key[pos].first < val.first) pos += 1;
        if (pos < root->n && root->key[pos].first == val.first) { // TODO
            root->key[pos] = val;
            return root;
        }
        __insert(root->next[pos], val);
        return insert_balance(root, root->next[pos], pos);
    }

    ptr insert(std::pair<T, U> val) {
        this->root->next[0] = __insert(this->root->next[0], val);
        if (this->root->next[0]->n == MAX_M) {
            ptr p = MakeNode::Cnode<BNode<T, U> >(this->pool); 
            p->next[0] = this->root->next[0];
            this->root->next[0] = insert_balance(p, this->root->next[0], 0);
        } // n must be < MAX_M
        return this->root->next[0];
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

    ptr erase(T val) {
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
        return ;
    }
    
    void output() {
        helper::maketail(this->root->next[0], 0, this->root);
        __output(this->root->next[0]);
        return ;
    }

private :
    M_pool *pool = M_pool::Creat_Pool(2048);
    mfunction<bool(T, U)> cmp;
    ptr root; // virtual root
};


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
    int p;
    while (~scanf("%d", &p)) {
        if (p == -1) break;
        printf("erase %d from the tree\n\n", p);
        m.erase(p);
        m.output();
    }

    return 0;
}
