#include <unordered_set>
#include <string>
#include <assert.h>
#include <unordered_map>
#include <iostream>
#include <openssl/md5.h>

using namespace std;

struct HRW {
    unordered_set<string> node_set;

    void add(const string &node) {
        node_set.emplace(node);
    }

    void remove(const string &node) {
        node_set.erase(node);
    }
    void pop() {
        node_set.erase(node_set.begin());
    }
    const string *map(const string &key) const {
        uint64_t max_weight = 0;
        const string *r = nullptr;
        for (auto &node: node_set) {
            string md5_input = node + key;
            uint64_t md5_output[2];
            MD5((const unsigned char*)md5_input.data(), md5_input.length(), (unsigned char*)md5_output);
            if (md5_output[0] > max_weight) {
                max_weight = md5_output[0];
                r = &node;
            }
        }
        return r;
    }
};


string rand_str() {
    string r;
    int n = 16;
    r.reserve(n);
    while (n--) {
        r.push_back(rand());
    }
    return r;
}

int main(int argc, char **argv) {
    assert(argc == 3);
    int node_num = atoi(argv[1]);
    int key_num = atoi(argv[2]);

    srand(time(nullptr));

    HRW hrw;
    for (int i = 0; i < node_num; ++i) {
        hrw.add(rand_str());
    }

    unordered_map<string, string> old_map;
    for (int i = 0; i < key_num; ++i) {
        string key = rand_str();
        old_map[key] = *hrw.map(key);
    }

    auto key_new = rand_str();
    hrw.add(key_new);

    int add_miss = 0;
    for (auto &p: old_map) {
        if (*hrw.map(p.first) != p.second) {
            ++add_miss;
        }
    }
    cout << "add miss = " << add_miss << " : " << (1.0 * add_miss / key_num) << endl;


    hrw.remove(key_new);
    hrw.pop();
    int remove_miss = 0;
    for (auto &p: old_map) {
        if (*hrw.map(p.first) != p.second) {
            ++remove_miss;
        }
    }
    cout << "remove miss = " << remove_miss << " : " << (1.0 * remove_miss / key_num) << endl;
}




