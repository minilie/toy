/*************************************************************************
	> File Name: BTree.h
	> Author:Xin 
	> Mail:2923262959@qq.com 
	> Created Time: Tue 24 Sep 2024 11:25:23 AM CST
 ************************************************************************/

#include "2.h"
#include "My_functional.h"
#include <vector>

const int MAX_M = 5;

template<typename T, typename U>
struct BNode {
    BNode() : n(0), fapos(0), father(nullptr) {}
    int n, fapos;
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

    static iterator get_next(iterator &p) {
        if (p.node->father == nullptr) return p;
        if (p.node->next[p.pos + 1] != nullptr) {
            Node *temp = p.node->next[p.pos + 1];
            while (temp->next[0] != nullptr) temp = temp->next[0];
            return temp;
        }
        else if (p.pos < p.node->n - 1) {
            return iterator(p.node, p.pos + 1);
        } else {
            // std::cout << "======= p->key : " << p.node->key[p.pos].first << "===========" << std::endl;
            // std::cout << "======= p->father->key : " << p.node->father->key[p.node->fapos].first << "===========" << std::endl;
            if (p.node->fapos < p.node->father->n) {
            // if (p.node->father->next[p.node->fapos + 1] != nullptr) {
                return iterator(p.node->father, p.node->fapos);
            } else {
                ptr temp = p.node;
                // while (temp->father != nullptr && temp->father->next[temp->fapos + 1] == nullptr) temp = temp->father;
                while (temp->father != nullptr && temp->fapos == temp->father->n) temp = temp->father;
                return iterator(temp->father, temp->fapos);
            }
        }
    }

    static iterator get_pre(iterator &p) {
        if (p.node->next[p.pos] != nullptr) {
            Node *temp = p.node->next[p.pos];
            while (temp->next[temp->n] != nullptr) temp = temp->next[temp->n];
            return temp;
        }
        else if (p.pos > 0) {
            return iterator(p.node, p.pos + 1);
        } else {
            if (p.node->fapos > 0) {
            // if (p.node->father->next[p.node->fapos + 1] != nullptr) {
                return iterator(p.node->father, p.node->fapos);
            } else {
                ptr temp = p.node;
                // while (temp->father != nullptr && temp->father->next[temp->fapos + 1] == nullptr) temp = temp->father;
                while (temp->father != nullptr && temp->fapos == 0) temp = temp->father;
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
    BIterator(Node *node = nullptr, int pos = 0) : node(node), pos(pos) {}

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
        return erase(iter.node->key[iter.pos].first);
    }
    ptr erase(iterator itera, iterator iterb) {
        int cnt = 0;
        iterb++;
        for (auto iter = itera; iter != (iterb);) {
            this->erase(iter++);
            cnt += 1;
        }
        std::cout << "==-==-=-=- the cnt is : " << cnt << "==-=--=-" << std::endl;
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
        return p;
    }
    const_iterator begin() const {
        if (!this->node_cnt) return this->root;
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
        if (child->n < MAX_M) {
            // child->fapos = pos;
            // child->father = root;
            return root;
        }
        int spos = MAX_M / 2;
        ptr node1 = MakeNode::Cnode<BNode<T, U> >(this->pool);
        ptr node2 = MakeNode::Cnode<BNode<T, U> >(this->pool);
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
            root->next[i + 1] = root->next[i]; // TODO
            if (root->next[i + 1] != nullptr) {
                root->next[i + 1]->fapos = i + 1;
                root->next[i + 1]->father = root; // can be erase ?
            }
        } // TODO
        root->key[pos] = child->key[spos];
        root->next[pos] = node1;
        root->next[pos + 1] = node2;
        root->n += 1;
        // make tail
        node1->fapos = pos;
        std::cout << "keyyyy " << node1->key[0].first << "ppp os : " << pos << std::endl;
        node2->fapos = pos + 1;
        std::cout << "keyyyy " << node2->key[0].first << "ppp os : " << pos + 1 << std::endl;
        node1->father = node2->father = root;
        child = nullptr;
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
            return root;
        }
        ptr temp = __insert(root->next[pos], val, op);
        // make tail
        temp->fapos = pos;
        std::cout << "ppppos = " << pos << std::endl << std::endl;
        temp->father = root;
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
        //
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
        //
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
        ptr node = MakeNode::Cnode<BNode<T, U> >(this->pool);
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
        // free(root->next[pos]);
        // free(root->next[pos + 1]);
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
        if (root == nullptr) return root;
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
            std::cout << "fully null" << std::endl << std::endl;
            return this->root;
        }
        if (this->root->next[0]->n == 0) {
            ptr p = this->root->next[0]->next[0];
            // free(this->root->next[0]);
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

    M_pool *pool = M_pool::Creat_Pool(2048);
    mfunction<bool(T, U)> cmp;
    ptr root; // virtual root
    long long node_cnt;
};


namespace test1 {
int main() {
    srand(time(0));
    
#define T_key int
#define T_val int
    BTree<int, int> m;
    T_key x;
    T_val y;
    for (int i = 0; i < 100; i++) {
        x = rand() % 10000 + 1;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    m.output();
    auto iter = m.begin();
    auto size = m.size();
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            std::cout << "\033[31;1m" << p->first  << "\033[0m"<< std::endl;
        }
    for (int i = 0; i < size / 2; i++) iter++;
    std::cout << "----------------iter : " << iter.node->key[0].first << "---------------" << std::endl;
    m.erase(m.begin(), iter);
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            std::cout << "\033[31;1m" << p->first  << "\033[0m"<< std::endl;
        }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
#undef T_key
#undef T_val
#undef T_cmp
    return 0;
}
}

namespace test2 {
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
    std::cout << std::endl << std::endl << std::endl;

    int p;
    
    while (~scanf("%d", &p)) {
        if (p == -1) break;
        printf("find %d from the tree\n\n", p);
        if (m.find(p) != m.end()) std::cout << "\033[32;1m" << "successfully find" << "\033[0m"<< std::endl;
        else std::cout << "\033[31;1m" << "there is no " << p << "in this tree" << "\033[0m"<< std::endl;
    }
    while (~scanf("%d", &p)) {
        if (p == -1) break;
        printf("erase %d from the tree\n\n", p);
        m.erase(p);
        m.output();
        std::cout << "========the node cnt is " << m.size() << std::endl << std::endl;
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            std::cout << "\033[31;1m" << p->first  << "\033[0m"<< std::endl;
        }
        if (m.begin() == m.end()) {
            std::cout << "\033[31;1m" << "the tree is null" << "\033[0m"<< std::endl;
            exit(0);
        }
    }
    for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
        std::cout << p->first << " ";
    }
    std::cout << std::endl;

    return 0;
}
}

namespace test3 {
    
#define T_key int
#define T_val int
#define T_cmp std::less<T_key>
int main() {
    srand(time(0));
    BTree<int, int> m;
    T_key x;
    T_val y;
    std::vector<T_key> keylist;
    for (int i = 0; i < 100; i++) {
        x = rand() % 10000;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
        keylist.push_back(x);
    }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            std::cout << "\033[31;1m" << p->first  << "\033[0m"<< std::endl;
        }
    for (int i = 0, I = keylist.size(); i < I / 2; i++) {
        m.erase(keylist[i]);
    }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            std::cout << "\033[31;1m" << p->first  << "\033[0m"<< std::endl;
        }
    return 0;
}
#undef T_key
#undef T_val
#undef T_cmp

}

namespace test4 {
    int main() {

#define T_key int
#define T_val int
#define T_cmp std::less<T_key>
    srand(time(0));
    BTree<int, int> m;
    T_key x;
    T_val y;
    for (int i = 0; i < 100; i++) {
        x = rand() % 10000 + 1;
        y = rand() % 100;
        std::pair<T_key, T_val> p =  std::make_pair(x, y);
        m.insert(p);
    }
    auto iter = m.begin();
    auto size = m.size();
        for (BIterator<int, int, false, BNode<int, int> > p = m.begin(); p != m.end(); p++) {
            m.erase(p);
        }
    std::cout << "----------------m.size = " << m.size() << "---------------" << std::endl;

#undef T_key
#undef T_val
#undef T_cmp
        return 0;
    }
}

int main() {
    test4::main();
    return 0;
}
