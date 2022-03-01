#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <queue>

#include <libutils/rasserts.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/types.hpp>

#define ll long long


struct Edge {
    int u, v; // номера вершин которые это ребро соединяет
    ll w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(int u, int v, int w) : u(u), v(v), w(w) {}
};

// Эта биективная функция по координате пикселя (строчка и столбик) + размерам картинки = выдает номер вершины
int encodeVertex(int row, int column, int nrows, int ncolumns) {
    rassert(row < nrows, 348723894723980017);
    rassert(column < ncolumns, 347823974239870018);
    int vertexId = row * ncolumns + column;
    return vertexId;
}

// Эта биективная функция по номеру вершины говорит какой пиксель этой вершине соовтетствует (эта функция должна быть симметрична предыдущей!)
cv::Point2i decodeVertex(int vertexId, int nrows, int ncolumns) {

    // TODO: придумайте как найти номер строки и столбика пикселю по номеру вершины (просто поймите предыдущую функцию и эта функция не будет казаться сложной)
    int row = vertexId / ncolumns;
    int column = vertexId % ncolumns;

    // сверим что функция симметрично сработала:
    rassert(encodeVertex(row, column, nrows, ncolumns) == vertexId, 34782974923035);

    rassert(row < nrows, 34723894720027);
    rassert(column < ncolumns, 3824598237592030);
    return {column, row};
}

ll distance(const std::vector<int>& v1, const std::vector<int>& v2){
    double res = 0;
    for (int i = 0; i < v1.size(); ++i)
    {
        res += pow(v1[i] - v2[i], 2);
    }
    return round(sqrt(res));
}

void run(int mazeNumber) {
    cv::Mat maze = cv::imread("lesson15/data/mazesImages/maze" + std::to_string(mazeNumber) + ".png");
    rassert(!maze.empty(), 324783479230019);
    rassert(maze.type() == CV_8UC3, 3447928472389020);
    std::cout << "Maze resolution: " << maze.cols << "x" << maze.rows << std::endl;

    int n = maze.cols * maze.rows;

    const int INF = std::numeric_limits<int>::max();
    std::vector<std::vector<Edge>> edges_by_vertex(n);
    for (int i = 0; i < maze.rows; ++i) {
        for (int j = 0; j < maze.cols; ++j) {
            cv::Vec3b color = maze.at<cv::Vec3b>(i,j);
            unsigned char blue = color[0];
            unsigned char green = color[1];
            unsigned char red = color[2];

            auto ai = encodeVertex(i,j,maze.rows,maze.cols);
            if(i > 0) {
                cv::Vec3b color2 = maze.at<cv::Vec3b>(i-1,j);
                edges_by_vertex[ai].emplace_back(ai, encodeVertex(i-1,j, maze.rows, maze.cols),
                                                 std::max(distance({color2[0], color2[1], color2[2]}, {red, green, blue}), 1ll));
            }
            if(i < maze.rows-1) {
                cv::Vec3b color2 = maze.at<cv::Vec3b>(i+1,j);
                edges_by_vertex[ai].emplace_back(ai, encodeVertex(i+1,j, maze.rows, maze.cols),
                                                 std::max(distance({color2[0], color2[1], color2[2]}, {red, green, blue}), 1ll));
            }
            if(j > 0) {
                cv::Vec3b color2 = maze.at<cv::Vec3b>(i,j-1);
                edges_by_vertex[ai].emplace_back(ai, encodeVertex(i,j-1, maze.rows, maze.cols),
                                                 std::max(distance({color2[0], color2[1], color2[2]}, {red, green, blue}), 1ll));
            }
            if(j < maze.cols-1) {
                cv::Vec3b color2 = maze.at<cv::Vec3b>(i,j+1);
                edges_by_vertex[ai].emplace_back(ai, encodeVertex(i,j+1, maze.rows, maze.cols),
                                                 std::max(distance({color2[0], color2[1], color2[2]}, {red, green, blue}), 1ll));
            }
        }
    }

    int start, finish;
    if (mazeNumber >= 1 && mazeNumber <= 3) { // Первые три лабиринта очень похожи но кое чем отличаются...
        start = encodeVertex(300, 300, maze.rows, maze.cols);
        finish = encodeVertex(0, 305, maze.rows, maze.cols);
    } else if (mazeNumber == 4) {
        start = encodeVertex(154, 312, maze.rows, maze.cols);
        finish = encodeVertex(477, 312, maze.rows, maze.cols);
    } else if (mazeNumber ==
               5) { // Лабиринт в большом разрешении, добровольный (на случай если вы реализовали быструю Дейкстру с приоритетной очередью)
        start = encodeVertex(1200, 1200, maze.rows, maze.cols);
        finish = encodeVertex(1200, 1200, maze.rows, maze.cols);
    } else {
        rassert(false, 324289347238920081);
    }

    cv::Mat window = maze.clone(); // на этой картинке будем визуализировать до куда сейчас дошла прокладка маршрута

    std::vector<ll> d(n, INF);
    d[start] = 0;
    std::vector<int> p(n, -1);
    std::priority_queue < std::pair<ll,int> > q;
    q.push (std::make_pair (0, start));
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
    for (int v = finish; v!=start; v=p[v])
        path.push_back (v);
    path.push_back (0);
    reverse (path.begin(), path.end());
    for (auto & pa : path)
    {
        cv::Point2i p = decodeVertex(pa, maze.rows, maze.cols);
            window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 0, 255);
            cv::Mat sh = window.clone();
            cv::resize(window, sh, cv::Size(500, 500), cv::INTER_LINEAR);
            cv::imshow("Maze", sh);
            cv::waitKey(1);
    }
    // TODO СКОПИРУЙТЕ СЮДА ДЕЙКСТРУ ИЗ ПРЕДЫДУЩЕГО ИСХОДНИКА

    // TODO в момент когда вершина становится обработанной - красьте ее на картинке window в зеленый цвет и показывайте картинку:
    //    cv::Point2i p = decodeVertex(the_chosen_one, maze.rows, maze.cols);
    //    window.at<cv::Vec3b>(p.y, p.x) = cv::Vec3b(0, 255, 0);
    //    cv::imshow("Maze", window);
    //    cv::waitKey(1);
    // TODO это может тормозить, в таком случае показывайте window только после обработки каждой сотой вершины

    // TODO обозначьте найденный маршрут красными пикселями

    // TODO сохраните картинку window на диск

    std::cout << "Finished!" << std::endl;

    // Показываем результат пока пользователь не насладиться до конца и не нажмет Escape
    cv::Mat sh = window.clone();
    cv::resize(window, sh, cv::Size(500, 500), cv::INTER_LINEAR);
    while (cv::waitKey(10) != 27) {
        cv::imshow("Maze", sh);
    }
}

int main() {
    try {
        int mazeNumber =5;
        run(mazeNumber);

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
