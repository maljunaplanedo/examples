#include <iostream>
#include <vector>
#include <algorithm>

bool compatible(int mask1, int mask2, int m) {
    int mask = mask1 ^ mask2;
    for (int i = 0; i < m - 1; ++i) {
        if (!(mask & (1 << i)) && !(mask & (1 << (i + 1))) && ((mask1 & (1 << i)) << 1) == (mask1 & (1 << (i + 1))))
            return false;
    }
    return true;
}

std::vector<std::vector<int>> unite(const std::vector<std::vector<int>>& d1,
                                    const std::vector<std::vector<int>>& d2, int m, int mod) {

    std::vector<std::vector<int>> ans(1 << m, std::vector<int>(1 << m, 0));

    for (int maskl = 0; maskl < (1 << m); ++maskl) {
        for (int maskr = 0; maskr < (1 << m); ++maskr) {
            for (int maskm = 0; maskm < (1 << m); ++maskm) {
                ans[maskl][maskr] = (ans[maskl][maskr] +
                                       d1[maskl][maskm] * d2[maskm][maskr]) % mod;
            }
        }
    }

    return ans;
}

std::vector<std::vector<std::vector<int>>> count(int m, int mod) {
    static const int L = 350;

    std::vector<std::vector<std::vector<int>>> dp(1, std::vector<std::vector<int>>
        (1 << m, std::vector<int>(1 << m)));

    for (int mask1 = 0; mask1 < (1 << m); ++mask1) {
        for (int mask2 = 0; mask2 < (1 << m); ++mask2) {
            dp[0][mask1][mask2] = compatible(mask1, mask2, m);
        }
    }

    for (int i = 1; i < L; ++i) {
        dp.push_back(unite(dp[i - 1], dp[i - 1], m, mod));
    }

    return dp;
}

std::vector<bool> binary(std::string p) {
    std::reverse(p.begin(), p.end());
    std::vector<bool> ans;

    while (!(p == "1")) {
        bool mem = false;
        for (int i = static_cast<int>(p.size()) - 1; i >= 0; --i) {
            int temp = p[i] - '0';
            if (mem)
                temp += 10;
            mem = temp & 1;
            temp >>= 1;
            p[i] = temp + '0';
        }
        ans.push_back(mem);

        if (p.back() == '0')
            p.pop_back();
    }
    ans.push_back(true);

    return ans;
}

int main() {
    std::ios_base::sync_with_stdio(false);

    std::string n;
    int m, mod;
    std::cin >> n >> m >> mod;

    std::vector<std::vector<std::vector<int>>> dp = count(m, mod);

    std::vector<bool> binn = binary(n);

    std::vector<std::vector<int>> result;
    for (int i = 0; i < static_cast<int>(binn.size()); ++i) {
        if (!binn[i])
            continue;

        if (result.empty())
            result = dp[i];
        else
            result = unite(result, dp[i], m, mod);
    }

    int ans = 0;
    int sup = 0b10101010101 % (1 << m);

    for (int mask = 0; mask < (1 << m); ++mask) {
        ans = (ans + result[mask][sup]) % mod;
    }

    std::cout << ans << '\n';

    return 0;
}

