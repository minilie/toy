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

template<typename T, typename U, typename V = std::less<T>>
class BTree {
    using ptr = BNode<T, U> *;
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

    
    void print_node(ptr root) {
        printf("%d : ", root->n);
        for (int i = 0; i < root->n; i++) {
            printf("%d ", root->key[i].first);
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

    for (int i = 0; i < 25; i++) {
        int x = rand() % 100 + 1;
        int y = rand() % 100;
        m.insert(std::make_pair(x, y));
        printf("insert %d into the tree\n\n", x);
    }
    m.output();


    return 0;
}
