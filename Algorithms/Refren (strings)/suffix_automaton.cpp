// Suffix automaton solution

#include <iostream>
#include <vector>
#include <cassert>

int n, m;

struct State {
    int len;
    int link;
    int count;
    int firstpos;
    std::vector<int> children;
    std::vector<int> inverted_links;

    State():
        len(0), link(-1), count(0), firstpos(-1), children(m + 1, -1)
    {   }
};

std::vector<State> automaton;
int last;

void add(int d) {
    int cur = static_cast<int>(automaton.size());
    automaton.emplace_back();

    automaton[cur].len = automaton[last].len + 1;
    automaton[cur].count = 1;
    automaton[cur].firstpos = automaton[last].len;

    int p;
    for (p = last; p != -1 && automaton[p].children[d] == -1; p = automaton[p].link) {
        automaton[p].children[d] = cur;
    }

    if (p == -1) {
        automaton[cur].link = 0;
    } else {
        int q = automaton[p].children[d];
        if (automaton[p].len + 1 == automaton[q].len) {
            automaton[cur].link = q;
        } else {
            int clone = static_cast<int>(automaton.size());
            automaton.emplace_back();

            automaton[clone].len = automaton[p].len + 1;
            automaton[clone].children = automaton[q].children;
            automaton[clone].link = automaton[q].link;
            automaton[clone].firstpos = automaton[q].firstpos;

            for (; p != -1 && automaton[p].children[d] == q; p = automaton[p].link) {
                automaton[p].children[d] = clone;
            }
            automaton[q].link = automaton[cur].link = clone;
        }
    }
    last = cur;
}

void count_set_dfs(int v) {
    for (int to: automaton[v].inverted_links) {
        count_set_dfs(to);
        automaton[v].count += automaton[to].count;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    std::cin >> n >> m;

    std::vector<int> s;

    last = 0;
    automaton.emplace_back();

    for (int i = 0; i < n; ++i) {
        int x;
        std::cin >> x;
        s.push_back(x);
        add(x);
    }

    for (int i = 0; i < static_cast<int>(automaton.size()); ++i) {
        assert(i == 0 || automaton[i].link != -1);
        if (i != 0)
            automaton[automaton[i].link].inverted_links.push_back(i);
    }

    count_set_dfs(0);

    long long ans = -1;
    int argmin = -1;
    for (int i = 0; i < static_cast<int>(automaton.size()); ++i) {
        long long temp = 1ll * automaton[i].len * automaton[i].count;
        if (temp > ans) {
            ans = temp;
            argmin = i;
        }
    }

    std::cout << ans << '\n' << automaton[argmin].len << '\n';

    for (int i = 0; i < automaton[argmin].len; ++i) {
        std::cout << s[automaton[argmin].firstpos - automaton[argmin].len + i + 1] << ' ';
    }
    std::cout << '\n';

    return 0;
}

