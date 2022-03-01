#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <queue>

int debugPoint(int line) {
    if (line < 0)
        return 0;

    // You can put breakpoint at the following line to catch any rassert failure:
    return line;
}

#define rassert(condition, message) if (!(condition)) { std::stringstream ss; (ss << "Assertion \"" << message << "\" failed at line " << debugPoint(__LINE__) << "!"); throw std::runtime_error(ss.str()); }


struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w) {}
};

void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.
    std::cin.tie(nullptr);
    std::ios ::sync_with_stdio(false);
    int n, medges;
    std::cin >> n;
    std::cin >> medges;
    std::vector<std::vector<Edge>> edges_by_vertex(n);
    for (int i = 0; i < medges; ++i) {
        int ai, bi, w;
        std::cin >> ai >> bi >> w;
        rassert(ai >= 1 && ai <= n, 23472894792020);
        rassert(bi >= 1 && bi <= n, 23472894792021);

        ai -= 1;
        bi -= 1;
        rassert(ai >= 0 && ai < n, 3472897424024);
        rassert(bi >= 0 && bi < n, 3472897424025);

        Edge edgeAB(ai, bi, w);
        edges_by_vertex[ai].push_back(edgeAB);

        edges_by_vertex[bi].emplace_back(bi, ai,
                                         w); // а тут - обратное ребро, можно конструировать объект прямо в той же строчке где он и потребовался
    }


    const int INF = std::numeric_limits<int>::max();

    std::vector<int> d(n, INF);
    d[0] = 0;
    std::vector<int> p(n, -1);
    std::priority_queue < std::pair<int,int> > q;
    q.push (std::make_pair (0, 0));
    while (!q.empty()) {
        int v = q.top().second,  cur_d = -q.top().first;
        q.pop();
        if (cur_d > d[v])  continue;

        for (auto & edge : edges_by_vertex[v]) {
            int to = edge.v,
                    len = edge.w;
            if (d[v] + len < d[to]) {
                d[to] = d[v] + len;
                p[to] = v;
                q.push (std::make_pair (-d[to], to));
            }
        }
    }

    if (d[n-1] == INF) {
        std::cout << -1;
        return;
    }
    std::vector<int> path;
    for (int v= n - 1; v!=0; v=p[v])
	    path.push_back (v);
    path.push_back (0);
    reverse (path.begin(), path.end());
    for (auto & i : path)
        std:: cout << i + 1 << " ";
}

int main() {
    try {
        run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
