/*
 * https://leetcode.cn/problems/regular-expression-matching/
 */

struct Bits {
    vector<uint64_t> data;
    Bits(int n) {
        data.resize((n + 63) / 64);
    }
    
    void set(int i) {
        data[i / 64] |= 1 << (i % 64);
    }

    bool get(int i) const {
        return data[i / 64] & (1 << (i % 64));
    }

    void fn_or(const Bits &other) {
        for (int i = 0; i < data.size(); ++i) {
            data[i] |= other.data[i];
        }
    }
};

struct NFA {
    vector<vector<vector<int>>> table;
    vector<Bits> eps;

    NFA(string &p) {
        vector<int> eps_state;
        table.emplace_back(vector<vector<int>>(26)); // init state
        int i = 0;
        while (i < p.size()) {
            table.emplace_back(vector<vector<int>>(26));
            if (isalpha(p[i])) {
                if (i + 1 < p.size() && p[i + 1] == '*') {
                    table[table.size() - 2][p[i] - 'a'].push_back(table.size() - 2);
                    eps_state.push_back(table.size() - 2);
                    i += 2;
                } else {
                    table[table.size() - 2][p[i] - 'a'].push_back(table.size() - 1);
                    i += 1;
                }
            } else {
                if (i + 1 < p.size() && p[i + 1] == '*') {
                    for (int j = 0; j < 26; ++j) {
                        table[table.size() - 2][j].push_back(table.size() - 2);
                    }
                    eps_state.push_back(table.size() - 2);
                    i += 2;
                } else {
                    for (int j = 0; j < 26; ++j) {
                        table[table.size() - 2][j].push_back(table.size() - 1);
                    }
                    i += 1;
                }
            }
            eps.assign(table.size(), Bits(table.size()));
            for (int i = 0; i < table.size(); ++i) {
                eps[i].set(i);
            }
            for (int i = (int)eps_state.size() - 1; i >= 0; --i) {
                eps[eps_state[i]].fn_or(eps[eps_state[i] + 1]);
            }
        }
    }

    bool match(string &s) {
        Bits state(table.size());
        state.fn_or(eps[0]);
        for (char ch: s) {
            Bits next(table.size());
            bool valid = false;
            for (int i = 0; i < table.size(); ++i) {
                if (state.get(i)) {
                    for (int x: table[i][ch - 'a']) {
                        next.fn_or(eps[x]);
                        valid = true;
                    }
                }
            }
            if (!valid) {
                return false;
            }
            swap(next, state);
        }
        return state.get(table.size() - 1);
    }
};

class Solution {
public:
    bool isMatch(string s, string p) {
        NFA nfa(p);
        return nfa.match(s);
    }
};
