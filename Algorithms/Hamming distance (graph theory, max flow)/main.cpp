#include <iostream>
#include <vector>
#include <queue>

const int64_t INF = 4e18;

struct Edge {
    int from;
    int to;
    int back;
    int64_t cap;
    int64_t flow;

    Edge(int from, int to, int64_t cap):
        from(from), to(to), back(back), cap(cap), flow(0)
    {   }
};

int n;
int source;
int sink;
std::vector<std::vector<int>> g;
std::vector<Edge> edges;

std::vector<bool> result;
std::vector<bool> used;

std::vector<int> position;
std::vector<int64_t> d;

inline int back(int e) {
    return e ^ 1;
}

void add_edge(int from, int to, int64_t cap, int64_t back_cap = 0) {
    g[from].push_back(edges.size());
    g[to].push_back(edges.size() + 1);

    edges.emplace_back(from, to, cap);
    edges.emplace_back(to, from, back_cap);
}

bool bfs() {
    d.assign(n, -1);
    d[source] = 0;
    std::queue<int> q;

    q.push(source);

    while (!q.empty()) {
        int top = q.front();
        q.pop();

        for (int i: g[top]) {
            const auto& edge = edges[i];
            if (d[edge.to] == -1 && edge.flow < edge.cap) {
                d[edge.to] = d[top] + 1;
                q.push(edge.to);
            }
        }
    }

    return d[sink] != -1;
}

int64_t dfs(int v, int64_t flow) {
    if (flow == 0)
        return 0;
    if (v == sink)
        return flow;
    for (; position[v] < static_cast<int>(g[v].size()); ++position[v]) {
        int edge_n = g[v][position[v]];

        auto& edge = edges[edge_n];
        auto& back_edge = edges[back(edge_n)];

        if (d[edge.to] != d[v] + 1)
            continue;
        int64_t push = dfs(edge.to, std::min(flow, edge.cap - edge.flow));
        if (push) {
            edge.flow += push;
            back_edge.flow -= push;
            return push;
        }
    }
    return 0;
}

int64_t dinitz() {
    int64_t flow = 0;
    while (true) {
        if (!bfs())
            break;
        position.assign(n, 0);
        while (int64_t push = dfs(source, INF)) {
            flow += push;
        }
    }
    return flow;
}

void dfs_result(int v) {
    used[v] = true;
    for (int edge_n: g[v]) {
        const auto& edge = edges[edge_n];
        if (used[edge.to] || edge.flow == edge.cap)
            continue;
        result[edge.to] = false;
        dfs_result(edge.to);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);

    std::string s;
    std::string p;
    std::cin >> s >> p;

    int m = s.size();
    int k = p.size();

    n = 2 * (m + k) + 2;

    source = n - 2;
    sink = n - 1;

    g.assign(n, std::vector<int>());

    for (int i = 0; i < k; ++i) {
        for (int j = i; j + k <= i + m; ++j) {
            if (p[i] == '?' || s[j] == '?') {
                if (p[i] == '?') {
                    if (s[j] == '0')
                        add_edge(source, 2 * i, 1);
                    else if (s[j] == '1')
                        add_edge(2 * i, sink, 1);
                    else
                        add_edge(2 * i, 2 * j + 1, 1, 1);
                } else {
                    if (p[i] == '0')
                        add_edge(source, 2 * j + 1, 1);
                    else
                        add_edge(2 * j + 1, sink, 1);
                }
            } else if (p[i] != s[j]) {
                add_edge(source, sink, 1);
            }
        }
    }

    int64_t ans = dinitz();
    std::cout << ans << '\n';

    result.assign(n, true);
    used.assign(n, false);

    dfs_result(source);

    for (int i = 0; i < m; ++i) {
        std::cout << (s[i] == '?' ? result[2 * i + 1] : static_cast<int>(s[i] - '0'));
    }
    std::cout << '\n';
    for (int i = 0; i < k; ++i) {
        std::cout << (p[i] == '?' ? result[2 * i] : static_cast<int>(p[i] - '0'));
    }
    return 0;
}

