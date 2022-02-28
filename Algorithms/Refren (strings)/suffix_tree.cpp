// Suffix tree solution
// (suffix array is used only to build the tree)

#include <iostream>
#include <vector>
#include <algorithm>

const int L = 20;

int n, m;
std::vector<int> s;
std::vector<std::vector<int>> equiv;
std::vector<int> suf_arr;
std::vector<int> neighbor_lcp;
long long ans;
int arg_max;
std::vector<int> ans_substr;
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
    neighbor_lcp[n - 1] = -1;
}

struct Node {
    int parent;
    int depth;
    int max_depth;
    int size;
    std::vector<int> children;

    Node(int parent, int depth):
        parent(parent), depth(depth), max_depth(depth), size(0), children()
    {   }
};

std::vector<Node> suffix_tree;

int add_suffix(int previous, int len, int lcp) {
    if (suffix_tree[previous].depth == 0 || suffix_tree[previous].depth == lcp) {
        suffix_tree[previous].children.push_back(static_cast<int>(suffix_tree.size()));
        suffix_tree.emplace_back(previous, len);
        return static_cast<int>(suffix_tree.size() - 1);
    } else {
        int previous_parent = suffix_tree[previous].parent;
        if (suffix_tree[previous_parent].depth < lcp) {
            suffix_tree[previous_parent].children.pop_back();
            suffix_tree[previous_parent].children.push_back(static_cast<int>(suffix_tree.size()));
            
            suffix_tree[previous].parent = static_cast<int>(suffix_tree.size());
            suffix_tree.emplace_back(previous_parent, lcp);
            suffix_tree.back().children.push_back(previous);
        }
        return add_suffix(suffix_tree[previous].parent, len, lcp);
    }
}

void build_suffix_tree() {
    suffix_tree.emplace_back(-1, 0);
    int previous = add_suffix(0, n - suf_arr[0], 0);
 

    for (int i = 1; i < n; ++i) {
        previous = add_suffix(previous, n - suf_arr[i], neighbor_lcp[i - 1]);
    }
}

void suffix_tree_count_vals(int v) {
    if (suffix_tree[v].children.empty())
        suffix_tree[v].size = 1;
    for (int to: suffix_tree[v].children) {
        suffix_tree_count_vals(to);
        suffix_tree[v].max_depth = std::max(suffix_tree[v].max_depth, suffix_tree[to].max_depth);
        suffix_tree[v].size += suffix_tree[to].size;
    }
}

void suffix_tree_count_answer(int v, int len) {
    if (v != 0) {
        int parent = suffix_tree[v].parent;
        int start = n - suffix_tree[v].max_depth + suffix_tree[parent].depth;
        int end = start + suffix_tree[v].depth - suffix_tree[parent].depth - 1;
        if (end == n - 1)
            --end;
        len += end - start + 1;
        int size = suffix_tree[v].size;

        if (1ll * len * size > ans) {
            ans = 1ll * len * size;
            arg_max = v;
        }
    }
    for (int to: suffix_tree[v].children) {
        suffix_tree_count_answer(to, len);
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
    
    ans = 0;

    s.push_back(m);
    ++n;
    build_suffix_array();
    count_neighbor_lcps();


    build_suffix_tree();
    suffix_tree_count_vals(0);
    suffix_tree_count_answer(0, 0);

    std::cout << ans << '\n';
    int cur = arg_max;
    while (cur != 0) {
        int parent = suffix_tree[cur].parent;
        int start = n - suffix_tree[cur].max_depth + suffix_tree[parent].depth;
        int end = start + suffix_tree[cur].depth - suffix_tree[parent].depth - 1;
        if (end == n - 1)
            --end;
        for (int i = end; i >= start; --i) {
            ans_substr.push_back(s[i]);
        }
        cur = parent;
    }
    std::reverse(ans_substr.begin(), ans_substr.end());
    std::cout << ans_substr.size() << '\n';
    for (int x: ans_substr) {
        std::cout << x + 1 << ' ';
    }
    std::cout << '\n';

    return 0;
}

