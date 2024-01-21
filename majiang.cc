#include <iostream>
#include <stdio.h>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <algorithm>

/*
 * 编码:
 * 万  1- 9
 * 筒 11-19
 * 条 21-29
 * 东西南北中发白 31 33 35 37 39 41 43
 *
 * */

const std::vector<const char*> repr = []() {
    std::vector<const char*> r(44, nullptr);
    r[1] = "一万";
    r[2] = "二万";
    r[3] = "三万";
    r[4] = "四万";
    r[5] = "五万";
    r[6] = "六万";
    r[7] = "七万";
    r[8] = "八万";
    r[9] = "九万";

    r[11] = "一筒";
    r[12] = "二筒";
    r[13] = "三筒";
    r[14] = "四筒";
    r[15] = "五筒";
    r[16] = "六筒";
    r[17] = "七筒";
    r[18] = "八筒";
    r[19] = "九筒";

    r[21] = "一条";
    r[22] = "二条";
    r[23] = "三条";
    r[24] = "四条";
    r[25] = "五条";
    r[26] = "六条";
    r[27] = "七条";
    r[28] = "八条";
    r[29] = "九条";

    r[31] = "东风";
    r[33] = "西风";
    r[35] = "南风";
    r[37] = "北风";
    r[39] = "红中";
    r[41] = "发财";
    r[43] = "白板";

    return r;
}();

struct Majiang {
    // 本来应该是 44，为了简化 `first + 2` 的判断改成 46
    std::array<int, 46> cnt;

    Majiang(const std::array<int, 46> & c):cnt(c) {
        int total = 0;
        for (int i = 0; i < cnt.size(); ++i) {
            if (cnt[i] < 0 || cnt[i] > 4) {
                throw "damn";
            }
            total += cnt[i];
        }
        if (total != 14) {
            throw "damn";
        }
        for (int i : {0, 10, 20, 30, 32, 34, 36, 38, 40, 42, 44, 45}) {
            if (cnt[i] != 0) {
                throw "damn";
            }
        }
    }


    bool is_valid() {
        for (int i = 0; i < cnt.size(); ++i) {
            if (cnt[i] >= 2) {
                cnt[i] -= 2;
                bool res = check(4);
                cnt[i] += 2;
                if (res) {
                    return true;
                }
            }
        }
        return false;
    }

    bool check(int n) {
        if (n == 0) {
            return true;
        }

        int first = 0;
        while (first < cnt.size() && cnt[first] == 0) {
            ++first;
        }

        if (cnt[first] >= 3) {
            cnt[first] -= 3;
            bool res = check(n - 1);
            cnt[first] += 3;
            if (res) {
                return true;
            }
        }
        if (cnt[first + 1] > 0 && cnt[first + 2] > 0) {
            cnt[first] -= 1;
            cnt[first + 1] -= 1;
            cnt[first + 2] -= 1;
            bool res = check(n - 1);
            cnt[first] += 1;
            cnt[first + 1] += 1;
            cnt[first + 2] += 1;
            if (res) {
                return true;
            }
        }
        return false;
    }
};

int main() {
    std::vector<int> pool;
    for (int k = 0; k < 4; ++k) {
        for (int i = 1; i <= 9; ++i) {
            pool.push_back(i);
        }
        /* 天胡太难，简化成缺二门
        for (int i = 11; i <= 19; ++i) {
            pool.push_back(i);
        }
        for (int i = 21; i <= 29; ++i) {
            pool.push_back(i);
        }
        */
        for (int i: {31, 33, 35, 37, 39, 41, 43}) {
            pool.push_back(i);
        }
    }


    srand(time(nullptr));
    for (int k = 0; k < 10000; ++k) {
        std::random_shuffle(pool.begin(), pool.end());
        std::array<int, 46> stage{0};
        std::multiset<int> order;
        for (int i = 0; i < 14; ++i) {
            stage[pool[i]] += 1;
            order.emplace(pool[i]);
        }
        if (Majiang(stage).is_valid()) {
            for (auto i: order) {
                printf("(%s)", repr[i]);
            }
            printf("\n");
        }
    }
}










