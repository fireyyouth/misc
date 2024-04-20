#include <atomic>
#include <chrono>
#include <vector>
#include <thread>
#include <string>
#include <iostream>

using namespace std;

struct RuleList {
    virtual ~RuleList() {}
    // manager api
    virtual int insert_after(int prev_id, string pattern) =0; // prev_id == -1 means prepend
    virtual bool erase(int id) =0;
    virtual bool update(int id, string pattern) =0;
    virtual vector<pair<int, string>> inspect() const =0;
    // worker api
    virtual int match(string s) const =0; // return -1 means no match
};

struct InplaceRuleList: public RuleList {
private:
    struct Node {
        int id;
        shared_ptr<string> pattern;
        shared_ptr<Node> next;
        Node(int i, string p):id(i), pattern(make_shared<string>(p)) {}
    };

    using Pointer = shared_ptr<Node>;

    Pointer root;

public:
    InplaceRuleList() {
        root = make_shared<Node>(-1, "");
    }


    int max_id() {
        int id = -1;
        for (auto p = root; p; p = p->next) {
            id = max(p->id, id);
        }
        return id;
    }

    Pointer find_id(int id) {
        for (auto p = root; p; p = p->next) {
            if (p->id == id) {
                return p;
            }
        }
        return {};
    }


    Pointer find_prev(int id) {
        for (auto p = root; p && p->next; p = p->next) {
            if (p->next->id == id) {
                return p;
            }
        }
        return {};
    }


    int insert_after(int prev_id, string pattern) {
        auto prev_node = find_id(prev_id);
        if (!prev_node) {
            return -1;
        }

        int id = max_id() + 1;
        auto node = make_shared<Node>(id, pattern);
        node->next = prev_node->next;
        atomic_store(&prev_node->next, node);
        return id;
    }

    bool erase(int id) {
        auto prev_node = find_prev(id);
        if (!prev_node) {
            return false;
        }
        auto p = prev_node->next->next;
        atomic_store(&prev_node->next, p);
        return true;
    }

    bool update(int id, string pattern) {
        auto node = find_id(id);
        if (!node) {
            return false;
        }
        atomic_store(&node->pattern, make_shared<string>(pattern));
        return true;
    }

    vector<pair<int, string>> inspect() const {
        vector<pair<int, string>> r;
        for (auto p = root->next; p; p = p->next) {
            r.emplace_back(p->id, *p->pattern);
        }
        return r;
    }

    int match(string s) const {
        for (auto p = atomic_load(&root->next); p; p = atomic_load(&p->next)) {
            if (s == *atomic_load(&p->pattern)) {
                return p->id;
            }
        }
        return -1;
    }

};



struct COWRuleList: public RuleList {
private:
    struct Node {
        const int id;
        const string pattern;
        const shared_ptr<Node> next;
        Node(int i, string p, shared_ptr<Node> n):id(i), pattern(p), next(n) {}
    };

    using Pointer = shared_ptr<Node>;

    Pointer root;

public:
    COWRuleList() {
        root = make_shared<Node>(-1, "", nullptr);
    }

// manager api

    int max_id() {
        int id = -1;
        for (auto p = root; p; p = p->next) {
            id = max(p->id, id);
        }
        return id;
    }

    pair<Pointer, int> insert_after(Pointer node, int prev_id, string pattern) {
        if (!node) {
            return {node, -1};
        }
        if (node->id == prev_id) {
            int id = max_id() + 1;
            auto node_new = make_shared<Node>(id, pattern, node->next);
            return {make_shared<Node>(node->id, node->pattern, node_new), id};
        }
        auto [node_next, id] = insert_after(node->next, prev_id, pattern);
        if (!node_next) {
            return {node_next, id};
        } else {
            return {make_shared<Node>(node->id, node->pattern, node_next), id};
        }
    }


    int insert_after(int prev_id, string pattern) {
        auto [node_root, id] = insert_after(root, prev_id, pattern);
        if (node_root) {
            atomic_store(&root, node_root);
        }
        return id;
    }

    Pointer erase(Pointer node, int id) {
        if (!node) {
            return {};
        }
        if (node->id == id) {
            return node->next;
        }
        auto node_next = erase(node->next, id);
        if (!node_next) {
            return node_next;
        } else {
            return make_shared<Node>(node->id, node->pattern, node_next);
        }
    }

    bool erase(int id) {
        if (id < 0) {
            return false;
        }
        auto node_root = erase(root, id);
        if (node_root) {
            atomic_store(&root, node_root);
            return true;
        } else {
            return false;
        }
    }

    Pointer update(Pointer node, int id, string pattern) {
        if (!node) {
            return {};
        }
        if (node->id == id) {
            return make_shared<Node>(node->id, pattern, node->next);
        }
        auto node_next = update(node->next, id, pattern);
        if (!node_next) {
            return node_next;
        } else {
            return make_shared<Node>(node->id, node->pattern, node_next);
        }
    }


    bool update(int id, string pattern) {
        if (id < 0) {
            return false;
        }
        auto node_root = update(root, id, pattern);
        if (node_root) {
            atomic_store(&root, node_root);
            return true;
        } else {
            return false;
        }
    }

    vector<pair<int, string>> inspect() const {
        vector<pair<int, string>> r;
        for (auto p = root->next; p; p = p->next) {
            r.emplace_back(p->id, p->pattern);
        }
        return r;
    }

    int match(string s) const {
        for (auto p = atomic_load(&root->next); p; p = p->next) {
            if (s == p->pattern) {
                return p->id;
            }
        }
        return -1;
    }

};



string rand_str() {
    string s(3, ' ');
    for (int i = 0; i < s.size(); ++i) {
        s[i] = 'a' + rand() % 26;
    }
    return s;
}

void manage(RuleList &rule_list) {
    for (int i = 0; i < 10; ++i) {
        rule_list.insert_after(-1, rand_str());
    }
    while (1) {
        auto info = rule_list.inspect();
        for (auto &[id, pattern]: info) {
            printf("(%d,'%s')", id, pattern.data());
        }
        printf("\n");
        if (!info.empty()) {
            int x = rand() % 3;
            if (x == 0) {
                int i = rand() % info.size();
                int id = rule_list.insert_after(info[i].first, rand_str());
                printf("insert %d after %d\n", id, info[i].first);
            } else if (x == 1) {
                int i = rand() % info.size();
                rule_list.erase(info[i].first);
                printf("erase %d\n", info[i].first);
            } else {
                int i = rand() % info.size();
                rule_list.update(info[i].first, rand_str());
                printf("update %d\n", info[i].first);
            }
        } else {
            int id = rule_list.insert_after(-1, rand_str());
            printf("prepend %d\n", id);
        }

        printf("-----------------------\n");
        this_thread::sleep_for(2000ms);
    }
}

void work(const RuleList &rule_list, int worker_no) {
    while (1) {
        auto s = rand_str();
        auto id = rule_list.match(s);
        if (id >= 0) {
            printf("worker[%d]: %s match %d\n", worker_no, s.data(), id);
        }
        this_thread::sleep_for(10ms);
    }
}

int main() {
    shared_ptr<string> p;
    atomic<char *>q;
    printf("%d %d\n", atomic_is_lock_free(&p), atomic_is_lock_free(&q));
    COWRuleList  rule_list;
    vector<thread> workers;
    for (int i = 0; i < 3; ++i) {
        workers.emplace_back([&rule_list, i]() {
            work(rule_list, i);
        });
    }
    manage(rule_list);
}
