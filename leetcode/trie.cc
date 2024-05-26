struct Trie {
    struct Node {
        int flag;
        int child[26];
    };
    struct Free {
        void operator()(void *p) {
            free(p);
        }
    };
    Node *pool;
    int size;

    vector<string> dict;

    Trie(vector<string> &d) {
        swap(dict, d);
        pool = nullptr;
        size = 0;
    }

    ~Trie() {
        if (pool) {
            free(pool);
        }
    }

    // [i, j], k is first level
    int node_num(int i, int j, int k) {
        int r = 1;
        while (i <= j && k >= dict[i].size()) {
            ++i;
        }
        if (i > j) {
            return r;
        }
        int start = i;
        for (int p = i + 1; p <= j; ++p) {
            if (dict[p - 1][k] != dict[p][k]) {
                r += node_num(start, p - 1, k + 1);
                start = p;
            }
        }
        r += node_num(start, j, k + 1);
        return r;
    }

    void build() {
        sort(dict.begin(), dict.end());
        int cap = node_num(0, dict.size() - 1, 0);
        pool = (Node *)malloc(sizeof(Node) * cap);
        memset(&pool[0], 0, sizeof(Node));
        size = 1;
        for (auto &s: dict) {
            insert(s);
        }
    }

    void insert(string &s) {
        int node = 0;
        for (char ch: s) {
            if (!pool[node].child[ch - 'a']) {
                memset(&pool[size], 0, sizeof(Node));
                pool[node].child[ch - 'a'] = size++;
            }
            node = pool[node].child[ch - 'a'];
        }
        pool[node].flag = 1;
    }
    int prefix_size(string &s) {
        int node = 0;
        int r = 0;
        for (char ch: s) {
            if (!pool[node].child[ch - 'a']) {
                return 0;
            }
            node = pool[node].child[ch - 'a'];
            ++r;
            if (pool[node].flag) {
                return r;
            }
        }
        return 0;
    }

    int node_count(int x) {
        int r = 1;
        for (int i = 0; i < 26; ++i) {
            if (pool[x].child[i]) {
                r += node_count(pool[x].child[i]);
            }
        }
        return r;
    }
};
