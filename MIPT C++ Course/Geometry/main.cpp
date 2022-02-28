#include <iostream>

#include "geometry.h"

int main() {

    /* std::vector<Point> pa, pb;
    for (int i = 0; i < 3; ++i) {
        double x, y;
        std::cin >> x >> y;
        pa.emplace_back(x, y);
    }

    for (int i = 0; i < 3; ++i) {
        double x, y;
        std::cin >> x >> y;
        pb.emplace_back(x, y);
    }

    Polygon a(pa), b(pb);

    std::cout << (a.isSimilarTo(b) ? "YES" : "NO") << '\n'; */

    /* std::vector<Point> pa;
    for (int i = 0; i < 3; ++i) {
        double x, y;
        std::cin >> x >> y;
        pa.emplace_back(x, y);
    }

    Line l({1, 1}, {1, 2});
    Polygon a(pa);

    a.reflex(l);

    for (auto x: a.getVertices())
        std::cout << x.x << ' ' << x.y << '\n'; */

    /* Point x, a, b, c;
    std::cin >> x.x >> x.y;
    std::cin >> a.x >> a.y;
    std::cin >> b.x >> b.y;
    std::cin >> c.x >> c.y;

    Polygon tri({a, b, c});

    std::cout << tri.containsPoint(x); */

    /* Polygon a({{-2, 2},
    {1, 2},
    {6, 1},
    {3, -1},
    {1, -1},
    {-1, -2}});

    Polygon b({{9.27204, 15.7172},
    {-2.6679, 6.15486},
    {-8.45299, -0.739541},
    {2.6679, -6.15486},
     {4.2265, 0.369771},
    {8.08322, 4.96604}});

    std::cout << a.isSimilarTo(b); */

    /*
    Ellipse el({5.8, 1.5}, {8.3, 8.5}, 2 * 4);
    auto d = el.directrices();

    std::cout << d.first.a << ' ' << d.first.b << ' ' << d.first.c << '\n';
    std::cout << d.second.a << ' ' << d.second.b << ' ' << d.second.c << '\n';

     *//* Triangle({{1, 2}, {2, 3}, {3, 4}});

    Rectangle a({-1, -2}, {0, 3}, 1.5);
    for (auto p: a.getVertices())
        std::cout << p.x << ' ' << p.y << '\n'; */

    /*Triangle t({0, 0}, {2, 0}, {0, 2});
     auto p = t.circumscribedCircleCenter();
     std::cout << p.x << ' ' << p.y; */


    Triangle x({-1, -2}, {1, 2}, {-2, 2});
    Circle ins = x.inscribedCircle();

    std::cout << ins.radius() << '\n';
    std::cout << ins.center().x << ' ' << ins.center().y << '\n';

    return 0;
}


/*
 *
  -5000 0
  -5000 1
   5000 0

   -5000 5000
   5000 5000
   5000 4999
 *
 *
 *
assert(pa[0].x == -5000);
assert(pa[0].y == 0);
assert(pa[1].x == -5000);
assert(pa[1].y == 1);
assert(pa[2].x == 5000);
assert(pa[2].y == 0);

assert(pb[0].x == -5000);
assert(pb[0].y == 5000);
assert(pb[1].x == 5000);
assert(pb[1].y == 5000);
assert(pb[2].x == 5000);
assert(pb[2].y == 4999);

 */

/*
3
0 0
1 2
2 0

3 2
1 3
1 1

*/

