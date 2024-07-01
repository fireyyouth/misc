/*
 * https://leetcode.cn/problems/regular-expression-matching/
 */

struct Bits {
    vector<uint64_t> data;
    Bits() {}
    Bits(int n) {
        data.resize((n + 63) / 64);
    }

    void set(int i) {
        data[i / 64] |= 1ULL<< (i % 64);
    }

    bool get(int i) const {
        return data[i / 64] & (1ULL<< (i % 64));
    }

    void fn_or(const Bits &other) {
        for (int i = 0; i < data.size(); ++i) {
            data[i] |= other.data[i];
        }
    }

    bool all_zero() const {
        for (int x: data) {
            if (x) {
                return false;
            }
        }
        return true;
    }

    void foreach(function<void(int)> call) const {
        for (int i = 0; i < data.size(); ++i) {
            auto t = data[i];
            while (t) {
                int j = __builtin_ctz(t);
                call(i * 64 + j);
                t &= ~(1ULL << j);
            }
        }
    }
};

struct NFA {
    vector<vector<Bits>> table;
    Bits init_eps;

    NFA(string &p) {
        vector<string_view> seg;
        {
            int i = 0;
            while (i < p.size()) {
                if (i + 1 < p.size() && p[i + 1] == '*') {
                    seg.emplace_back(&p[i], 2);
                    i += 2;
                } else {
                    seg.emplace_back(&p[i], 1);
                    i += 1;
                }
            }
        }

        vector<Bits> eps;
        {
            eps.assign(seg.size() + 1, Bits(seg.size() + 1));
            for (int i = 0; i < eps.size(); ++i) {
                eps[i].set(i);
            }
            for (int i = (int)seg.size() - 1; i >= 0; --i) {
                if (seg[i].size() == 2) {
                    eps[i].fn_or(eps[i + 1]);
                }
            }
        }

        table.assign(seg.size() + 1, vector<Bits>(26, Bits(seg.size() + 1)));
        for (int i = 0; i < seg.size(); ++i) {
            char ch = seg[i][0];
            bool is_star = seg[i].size() == 2;
            if (isalpha(ch)) {
                if (is_star) {
                    table[i][ch - 'a'].fn_or(eps[i]);
                } else {
                    table[i][ch - 'a'].fn_or(eps[i + 1]);
                }
            } else {
                if (is_star) {
                    for (int j = 0; j < 26; ++j) {
                        table[i][j].fn_or(eps[i]);
                    }
                } else {
                    for (int j = 0; j < 26; ++j) {
                        table[i][j].fn_or(eps[i + 1]);
                    }
                }
            }
        }

        init_eps = eps[0];
    }

    bool match(string &s) {
        auto state = init_eps;
        for (char ch: s) {
            Bits next(table.size());
            state.foreach([ch, &next, &table=this->table](int i) {
                next.fn_or(table[i][ch - 'a']);
            });
            if (next.all_zero()) {
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
