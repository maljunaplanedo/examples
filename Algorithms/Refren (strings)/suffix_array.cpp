// Suffix array solution

#include <iostream>
#include <vector>
#include <stack>

const int L = 20;

int n, m;
std::vector<int> s;
std::vector<std::vector<int>> equiv;
std::vector<int> suf_arr;
std::vector<int> neighbor_lcp;
long long ans;
int max_from;
int max_len;
int logn = 0;

void build_suffix_array() {
    std::vector<int> count(m + 1, 0);
    suf_arr.assign(n, 0);

    for (int i = 0; i < n; ++i) {
        ++count[s[i]];
    }
    for (int i = 1; i <= m; ++i) {
        count[i] += count[i - 1];
    }
    for (int i = 0; i < n; ++i) {
        suf_arr[--count[s[i]]] = i;
    }
    equiv.assign(L, std::vector<int>(n, -1));
    equiv[0][suf_arr[0]] = 0;
    int equiv_count = 1;
    for (int i = 1; i < n; ++i) {
        if (s[suf_arr[i]] != s[suf_arr[i - 1]])
            ++equiv_count;
        equiv[0][suf_arr[i]] = equiv_count - 1;
    }

    std::vector<int> suf_arr_helper(n, 0);
    for (int h = 0; (1 << h) < n; ++h) {
        logn = h + 1;

        for (int i = 0; i < n; ++i) {
            suf_arr_helper[i] = suf_arr[i] - (1 << h);
            if (suf_arr_helper[i] < 0)
                suf_arr_helper[i] += n;
        }
        count.assign(equiv_count, 0);
        for (int i = 0; i < n; ++i) {
            ++count[equiv[h][suf_arr_helper[i]]];
        }
        for (int i = 1; i < equiv_count; ++i) {
            count[i] += count[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            suf_arr[--count[equiv[h][suf_arr_helper[i]]]] = suf_arr_helper[i];
        }
        equiv[h + 1][suf_arr[0]] = 0;

        equiv_count = 1;
        for (int i = 1; i < n; ++i) {
            int mid1 = (suf_arr[i] + (1 << h)) % n;
            int mid2 = (suf_arr[i - 1] + (1 << h)) % n;

            if (equiv[h][suf_arr[i]] != equiv[h][suf_arr[i - 1]] || equiv[h][mid1] != equiv[h][mid2])
                ++equiv_count;
            equiv[h + 1][suf_arr[i]] = equiv_count - 1;
        }
    }
}

void count_neighbor_lcps() {
    neighbor_lcp.assign(n, 0);
    for (int i = 0; i < n - 1; ++i) {
        int first = suf_arr[i], second = suf_arr[i + 1];
        for (int j = logn; j >= 0; --j) {
            if (equiv[j][first] == equiv[j][second]) {
                neighbor_lcp[i] += 1 << j;
                first += 1 << j;
                second += 1 << j;
                if (first >= n || second >= n)
                    break;
            }
        }
        neighbor_lcp[i] = std::min(neighbor_lcp[i], std::min(n - suf_arr[i], n - suf_arr[i + 1]));
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cin >> n >> m;
    for (int i = 0; i < n; ++i) {
        int x;
        std::cin >> x;
        s.push_back(x - 1);
    }

    ans = n;
    max_from = 0;
    max_len = n;

    s.push_back(m);
    ++n;
    build_suffix_array();
    s.pop_back();
    --n;

    count_neighbor_lcps();

    std::stack<std::pair<int, int>> stack;
    for (int i = 0; i < n; ++i) {
        int from = i;
        while (!stack.empty() && stack.top().first >= neighbor_lcp[i]) {
            auto top = stack.top();
            from = top.second;

            int len = i - top.second + 1;
            if (1ll * len * top.first > ans) {
                ans = 1ll * len * top.first;
                max_len = top.first;
                max_from = suf_arr[top.second];
            }
            stack.pop();
        }
        stack.push({neighbor_lcp[i], from});
    }

    std::cout << ans << '\n' << max_len << '\n';
    for (int i = 0; i < max_len; ++i) {
        std::cout << s[max_from + i] + 1 << ' ';
    }
    std::cout << '\n';

    return 0;
}

