#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

struct SortedSet {
    std::map<std::string, double> string_to_score;
    std::map<double, std::set<std::string>> score_to_strings;
    void zadd(double score, const std::string &str) {
        if (string_to_score.find(str) != string_to_score.end()) {
            zrem(str);
        }
        string_to_score.emplace(str, score);
        auto [score_it, _] = score_to_strings.emplace(score, std::set<std::string>{});
        score_it->second.emplace(str);
    }
    void zrem(const std::string &str) {
        auto it = string_to_score.find(str);
        if (it != string_to_score.end()) {
            double score = it->second;
            auto score_it = score_to_strings.find(score);
            score_it->second.erase(str);
            if (score_it->second.empty()) {
                score_to_strings.erase(score_it);
            }
            string_to_score.erase(it);
        }
    }
    std::vector<std::pair<const std::string *, double> > zrange() const {
        std::vector<std::pair<const std::string *, double> > r;
        for (auto &p: score_to_strings) {
            for (auto &s: p.second) {
                r.emplace_back(&s, p.first);
            }
        }
        return r;
    }
};

int main() {
    SortedSet s;
    s.zadd(10, "Norem");
    s.zadd(12, "Castilla");
    s.zadd(10, "Norem");
    s.zadd(8, "Sam-Bodden");
    s.zadd(10, "Royce");
    s.zadd(6, "Ford");
    s.zadd(14, "Prickett");
       
    for (auto &p : s.zrange()) {
        std::cout << *p.first << ':' << p.second << std::endl;
    }
}