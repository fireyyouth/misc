/*
 * PPF: pre-post-fix
 * PPF of s[:i] must be extension of PPF of s[:i - 1]
 * In order to compute longest PPF of s[:i], just try all PPFs of s[:i-1] in reverse length order until find extendable
 * Next longest PPF of s[:i] is just longest PPF of longest PPF of s[:i]
 * */

class Solution {
public:
    string longestPrefix(string s) {
        vector<int> dp(s.size(), 0);
        for (int i = 1; i < s.size(); ++i) {
            int q = dp[i - 1];
            while (q > 0 && s[q] != s[i]) {
                q = dp[q - 1];
            }
            if (s[q] == s[i]) {
                dp[i] = q + 1;
            }
        }
        return s.substr(0, dp.back());
    }
};
