#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <typeinfo>

namespace Geometry {
    const double EPS = 1e-6;
    const double PI = M_PI;

    bool isPositive(double x) {
        return x > EPS;
    }

    bool isNegative(double x) {
        return isPositive(-x);
    }

    bool less(double x, double y) {
        return isPositive(y - x);
    }

    bool greater(double x, double y) {
        return less(y, x);
    }

    bool equal(double x, double y) {
        return !less(x, y) && !less(y, x);
    }

    bool lessOrEqual(double x, double y) {
        return !less(y, x);
    }

    bool greaterOrEqual(double x, double y) {
        return !less(x, y);
    }

    double toRadians(double angle) {
        return PI * angle / 180;
    }

    double toDegrees(double angle) {
        return 180 * angle / PI;
    }

}

struct Vector {
    double x = 0;
    double y = 0;

    Vector(double x, double y):
        x(x), y(y)
    {   }

    Vector()
    {   }

    Vector& operator+=(const Vector& another) {
        x += another.x;
        y += another.y;
        return *this;
    }

    Vector& operator-=(const Vector& another) {
        return *this += -another;
    }

    Vector& operator*=(double lambda) {
        x *= lambda;
        y *= lambda;
        return *this;
    }

    Vector& operator/=(double lambda) {
        return *this *= 1 / lambda;
    }

    Vector operator-() const {
        Vector answer = Vector();
        answer.x = -x;
        answer.y = -y;
        return answer;
    }
};

using Point = Vector;

Vector operator+(const Vector& left, const Vector& right) {
    Vector answer = left;
    answer += right;
    return answer;
}

Vector operator-(const Vector& left, const Vector& right) {
    Vector answer = left;
    answer -= right;
    return answer;
}

Vector operator*(double left, const Vector& right) {
    Vector answer = right;
    answer *= left;
    return answer;
}

Vector operator*(const Vector& left, double right) {
    Vector answer = left;
    answer *= right;
    return answer;
}

Vector operator/(const Vector& left, double right) {
    Vector answer = left;
    answer /= right;
    return answer;
}

bool operator<(const Vector& left, const Vector& right) {
    if (Geometry::equal(left.x, right.x))
        return Geometry::less(left.y, right.y);
    return Geometry::less(left.x, right.x);
}

bool operator>(const Vector& left, const Vector& right) {
    return right < left;
}

bool operator<=(const Vector& left, const Vector& right) {
    return !(right < left);
}

bool operator>=(const Vector& left, const Vector& right) {
    return !(left < right);
}

bool operator==(const Vector& left, const Vector& right) {
    return !(left < right) && !(right < left);
}

bool operator!=(const Vector& left, const Vector& right) {
    return !(left == right);
}

namespace Geometry {

    double abs(const Vector& v) {
        return sqrt(v.x * v.x + v.y * v.y);
    }

    double distance(const Point& left, const Point& right) {
        return abs(right - left);
    }

    double dotProduct(const Vector& a, const Vector& b) {
        return a.x * b.x + a.y * b.y;
    }

    double crossProduct(const Vector& a, const Vector& b) {
        return a.x * b.y - b.x * a.y;
    }

    double angleBetween(const Vector& a, const Vector& b) {
        return acos(dotProduct(a, b) / (abs(a) * abs(b)));
    }

    void rotate(Point& point, double angle) {
        angle = toRadians(angle);
        double sin = std::sin(angle);
        double cos = std::cos(angle);

        Point old = point;
        point.x = old.x * cos - old.y * sin;
        point.y = old.x * sin + old.y * cos;

    }

}

class Line {
private:
    double a = 1;
    double b = -1;
    double c = 0;

public:
    Line(Point p1, Point p2) {
        if (p2.x != p1.x) {
            a = (p2.y - p1.y) / (p2.x - p1.x);
            b = -1;
            c = p1.y - a * p1.x;
        }
        else {
            a = 1;
            b = 0;
            c = -p1.x;
        }
    }

    Line(Point p, double k0):
            Line(p, Point(p.x + 1, p.y + k0))
    {   }

    Line(double k0, double b0):
            Line(Point(0, b0), k0)
    {   }

    bool operator==(const Line& other) const {
        return Geometry::equal(a, other.a) &&
            Geometry::equal(b, other.b) &&
            Geometry::equal(c, other.c);
    }

    bool operator!=(const Line& other) const {
        return !(*this == other);
    }

    void reflexPoint(Point& p) const {
        double coefficient = (a * p.x + b * p.y + c) / (a * a + b * b);
        p.x -= 2 * a * coefficient;
        p.y -= 2 * b * coefficient;
    }

};

class Shape {
protected:
    virtual void move(const Vector& delta) = 0;
    virtual void scale(double coefficient) = 0;
    virtual void rotate(double angle) = 0;

public:
    virtual ~Shape() = default;

    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool operator==(const Shape& another) const = 0;
    bool operator!=(const Shape& another) const {
        return !(*this == another);
    }
    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point& center, double angle) {
        move(-center);
        rotate(angle);
        move(center);
    }

    virtual void scale(const Point& center, double coefficient) {
        move(-center);
        scale(coefficient);
        move(center);
    }

    virtual void reflex(const Point& center) = 0;
    virtual void reflex(const Line& axis) = 0;

};

class Polygon: public Shape {
protected:

    std::vector<Point> vertices;

    void incrementVertexCounter(size_t& i, bool reverse = false) const {
        if (reverse && i == 0)
            i = vertices.size();
        reverse ? --i : ++i;
        if (i == vertices.size())
            i = 0;
    }

    void rotate(double angle) override {
        for (auto& vertex: vertices) {
            Geometry::rotate(vertex, angle);
        }
    }

    void scale(double coefficient) override {
        for (auto& vertex: vertices) {
            vertex *= coefficient;
        }
    }

    void move(const Vector& delta) override {
        for (auto& vertex: vertices) {
            vertex += delta;
        }
    }

public:

    explicit Polygon(std::vector<Point> prototypeVertices):
        vertices(std::move(prototypeVertices))
    {   }

    Polygon(const std::initializer_list<Point>& prototypeVertices):
        vertices(prototypeVertices)
    {   }

    virtual ~Polygon() override = default;

    double perimeter() const override {
        double answer = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t j = i;
            incrementVertexCounter(j);
            answer += Geometry::distance(vertices[i], vertices[j]);
        }
        return answer;
    }

    double area() const override {
        double answer = 0;
        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t j = i;
            incrementVertexCounter(j);
            answer += vertices[i].x * vertices[j].y;
            answer -= vertices[j].x * vertices[i].y;
        }

        answer /= 2;
        answer = std::abs(answer);

        return answer;
    }

    bool operator==(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Polygon&>(another);
            if (vertices.size() != anotherCast.vertices.size())
                return false;
            size_t i = std::find(vertices.begin(), vertices.end(), anotherCast.vertices[0]) -
                    vertices.begin();
            if (i == vertices.size())
                return false;

            size_t j = i;
            bool reverse = false;

            incrementVertexCounter(j);
            if (vertices[j] != anotherCast.vertices[1])
                reverse = true;

            for (size_t anotherI = 0; anotherI < vertices.size();
                ++anotherI, incrementVertexCounter(i, reverse)) {

                if (vertices[i] != anotherCast.vertices[anotherI])
                    return false;
            }

            return true;
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    bool isScalableTo(const Polygon& another) const {
        if (vertices.size() != another.vertices.size())
            return false;

        for (size_t i = 0; i < vertices.size(); ++i) {
            Polygon copy = another;
            size_t j = i;
            incrementVertexCounter(j);

            copy.move(vertices[0] - copy.vertices[i]);

            Vector vector0j = copy.vertices[j] - vertices[0];
            Vector vector01 = vertices[1] - vertices[0];

            double angle = acos(Geometry::dotProduct(vector0j, vector01) /
                    (Geometry::abs(vector01) * Geometry::abs(vector0j)));

            for (int count = 0; count < 2; ++count) {
                Polygon newCopy = copy;

                newCopy.rotate(vertices[0], Geometry::toDegrees(angle));
                newCopy.scale(vertices[0], Geometry::abs(vector01) / Geometry::abs(vector0j));

                angle = -angle;

                if (vertices[1] != newCopy.vertices[j])
                    continue;

                if (*this == newCopy)
                    return true;

                newCopy.reflex(vertices[0] + vector01 / 2);

                if (*this == newCopy)
                    return true;
            }
        }

        return false;
    }

    bool isSimilarTo(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Polygon&>(another);
            if (vertices.size() != anotherCast.vertices.size())
                return false;

            if (isScalableTo(anotherCast))
                return true;
            Polygon copy = anotherCast;
            copy.reflex(Line(0, 0));

            return isScalableTo(copy);
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    bool isCongruentTo(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Polygon&>(another);
            return isSimilarTo(anotherCast) && Geometry::equal(perimeter(), anotherCast.perimeter());
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    void rotate(const Point& center, double angle) override {
        Shape::rotate(center, angle);
    }

    void reflex(const Point& center) override {
        for (auto& vertex: vertices) {
            vertex += 2 * (center - vertex);
        }
    }

    void reflex(const Line& axis) override {
        for (auto& vertex: vertices) {
            axis.reflexPoint(vertex);
        }
    }

    void scale(const Point& center, double coefficient) override {
        Shape::scale(center, coefficient);
    }

    size_t verticesCount() const {
        return vertices.size();
    }

    const std::vector<Point>& getVertices() const {
        return vertices;
    }

    bool isConvex() const {
        bool gotNegative = false;
        bool gotPositive = false;

        for (size_t i = 0; i < vertices.size(); ++i) {
            size_t j = i;
            const Point& a = vertices[j];
            incrementVertexCounter(j);
            const Point& b = vertices[j];
            incrementVertexCounter(j);
            const Point& c = vertices[j];

            double crossProduct = Geometry::crossProduct(a - b, c - b);

            if (Geometry::isNegative(crossProduct))
                gotNegative = true;
            if (Geometry::isPositive(crossProduct))
                gotPositive = true;

            if (gotNegative && gotPositive)
                return false;
        }

        return true;
    }

    bool containsPoint(const Point& point) const override {
        bool result = false;
        size_t j = vertices.size() - 1;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (((Geometry::less(vertices[i].y, point.y) && Geometry::greaterOrEqual(vertices[j].y, point.y)) ||
                (Geometry::less(vertices[j].y, point.y) && Geometry::greaterOrEqual(vertices[i].y, point.y))) &&
                Geometry::less(vertices[i].x + (point.y - vertices[i].y) / (vertices[j].y - vertices[i].y)
                    * (vertices[j].x - vertices[i].x), point.x))

                result = !result;
            j = i;
        }

        return result;
    }

};

class Ellipse: public Shape {
protected:
    Point f1;
    Point f2;
    double a = 1;

    double getC() const {
        return Geometry::abs(f1 - f2) / 2;
    }

    double getB() const {
        double c = getC();
        return sqrt(a * a - c * c);
    }

public:
    Ellipse(const Point& f1, const Point& f2, double dist):
        f1(f1), f2(f2), a(dist / 2)
    {   }

    virtual ~Ellipse() override = default;

    virtual double perimeter() const override {
        double b = getB();
        return Geometry::PI * (3 * (a + b) - sqrt((3 * a + b) * (a + 3 * b)));
    }

    double area() const override {
        return Geometry::PI * a * getB();
    }

    bool operator==(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Ellipse&>(another);
            return std::min(f1, f2) == std::min(anotherCast.f1, anotherCast.f2) &&
                    std::max(f1, f2) == std::max(anotherCast.f1, anotherCast.f2) &&
                    Geometry::equal(a, anotherCast.a);
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    bool isCongruentTo(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Ellipse &>(another);
            return Geometry::equal(a, anotherCast.a) &&
                Geometry::equal(getC(), anotherCast.getC());
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    bool isSimilarTo(const Shape& another) const override {
        try {
            const auto& anotherCast = dynamic_cast<const Ellipse&>(another);
            return Geometry::equal(eccentricity(), anotherCast.eccentricity());
        }
        catch (const std::bad_cast&) {
            return false;
        }
    }

    bool containsPoint(const Point& point) const override {
        return Geometry::lessOrEqual(Geometry::distance(f1, point) +
                                     Geometry::distance(f2, point), 2 * a);
    }

    void move(const Point& delta) override {
        f1 += delta;
        f2 += delta;
    }

    void scale(double coefficient) override {
        f1 *= coefficient;
        f2 *= coefficient;
        a *= coefficient;
    }

    void scale(const Point& center, double coefficient) override {
        Shape::scale(center, coefficient);
    }

    void rotate(double angle) override {
        Geometry::rotate(f1, angle);
        Geometry::rotate(f2, angle);
    }

    void rotate(const Point& center, double angle) override {
        Shape::rotate(center, angle);
    }

    void reflex(const Point& center) override {
        f1 += 2 * (center - f1);
        f2 += 2 * (center - f2);
    }

    void reflex(const Line& axis) override {
        axis.reflexPoint(f1);
        axis.reflexPoint(f2);
    }

    double eccentricity() const {
        return getC() / a;
    }

    Point center() const {
        return (f1 + f2) / 2;
    }

    std::pair<Point, Point> focuses() const {
        return std::make_pair(f1, f2);
    }

    std::pair<Line, Line> directrices() const {
        double e = eccentricity();
        double angle = Geometry::toDegrees(atan((f2.y - f1.y) / (f2.x - f1.x)));

        Point dir1p0(a / e, 0);
        Point dir2p0 = -dir1p0;
        Point dir1p1 = dir1p0 + Vector(0, 1);
        Point dir2p1 = dir2p0 + Vector(0, 1);

        Geometry::rotate(dir1p0, angle);
        Geometry::rotate(dir2p0, angle);
        Geometry::rotate(dir1p1, angle);
        Geometry::rotate(dir2p1, angle);

        Vector shift = center();

        dir1p0 += shift;
        dir1p1 += shift;
        dir2p0 += shift;
        dir2p1 += shift;

        return std::make_pair(Line(dir1p0, dir1p1), Line(dir2p0, dir2p1));
    }

};

class Circle: public Ellipse {
public:
    Circle(const Point& center, double radius):
        Ellipse(center, center, 2 * radius)
    {   }

    ~Circle() override = default;

    double perimeter() const override {
        return 2 * Geometry::PI * a;
    }

    double radius() const {
        return a;
    }

};

class Rectangle: public Polygon {
public:

    Rectangle(const Point& point1, const Point& point2, double ratio):
        Polygon(std::vector<Point>(4)) {

        if (ratio < 0)
            ratio = 1 / ratio;

        double c = Geometry::abs(point2 - point1);
        double a = sqrt((c * c) / (ratio * ratio + 1));
        double b = ratio * a;

        double angle = Geometry::toDegrees(atan(ratio));

        vertices[0] = point1;
        vertices[2] = point2;

        vertices[1] = (vertices[2] - vertices[0]) * (a / c);
        Geometry::rotate(vertices[1], angle);
        vertices[1] += vertices[0];

        vertices[3] = (vertices[2] - vertices[0]) * (b / c);
        Geometry::rotate(vertices[3], angle - 90);
        vertices[3] += vertices[0];
    }

    virtual ~Rectangle() override = default;

    Point center() const {
        return (vertices[0] + vertices[2]) / 2;
    }

    std::pair<Line, Line> diagonals() const {
        return std::make_pair(Line(vertices[0], vertices[2]),
                              Line(vertices[1], vertices[3]));
    }
};

class Square: public Rectangle {
public:
    Square(const Point& point1, const Point& point2):
        Rectangle(point1, point2, 1)
    {   }

    ~Square() override = default;

    Circle circumscribedCircle() const {
        return Circle(center(), Geometry::abs(vertices[2] - vertices[0]) / 2);
    }

    Circle inscribedCircle() const {
        return Circle(center(), Geometry::abs(vertices[1] - vertices[0]));
    }
};

class Triangle: public Polygon {
private:

    Point inscribedCircleCenter() const {
        double a = Geometry::abs(vertices[2] - vertices[1]);
        double b = Geometry::abs(vertices[2] - vertices[0]);
        double c = Geometry::abs(vertices[1] - vertices[0]);

        return Point((a * vertices[0].x + b * vertices[1].x + c * vertices[2].x) / (a + b + c),
                     (a * vertices[0].y + b * vertices[1].y + c * vertices[2].y) / (a + b + c));
    }

    Point circumscribedCircleCenter() const {

        Point A = vertices[0];
        Point B = vertices[1];
        Point C = vertices[2];

        double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));

        return Point((A.x * A.x + A.y * A.y) * (B.y - C.y) +
                     (B.x * B.x + B.y * B.y) * (C.y - A.y) +
                     (C.x * C.x + C.y * C.y) * (A.y - B.y),
                     (A.x * A.x + A.y * A.y) * (C.x - B.x) +
                     (B.x * B.x + B.y * B.y) * (A.x - C.x) +
                     (C.x * C.x + C.y * C.y) * (B.x - A.x)
        ) / D;
    }

public:

    explicit Triangle(const std::vector<Point>& prototypeVertices):
        Polygon(prototypeVertices)
    {   }

    Triangle(const Point& A, const Point& B, const Point& C):
        Triangle({A, B, C})
    {   }

    Triangle(const std::initializer_list<Point>& prototypeVertices):
        Polygon(prototypeVertices)
    {   }

    ~Triangle() override = default;

    Point centroid() const {
        return Point((vertices[0].x + vertices[1].x + vertices[2].x) / 3,
                     (vertices[0].y + vertices[1].y + vertices[2].y) / 3);
    }

    Point orthocenter() const {

        Point A = vertices[0];
        Point B = vertices[1];
        Point C = vertices[2];

        Vector AB = B - A;
        Vector BC = C - B;
        Vector CA = A - C;

        double tanAngleA = tan(Geometry::angleBetween(AB, -CA));
        double tanAngleB = tan(Geometry::angleBetween(BC, -AB));
        double tanAngleC = tan(Geometry::angleBetween(CA, -BC));

        Point answer;

        answer.x = (A.x * tanAngleA + B.x * tanAngleB + C.x * tanAngleC) /
                (tanAngleA + tanAngleB + tanAngleC);
        answer.y = (A.y * tanAngleA + B.y * tanAngleB + C.y * tanAngleC) /
                   (tanAngleA + tanAngleB + tanAngleC);

        return answer;
    }

    Circle circumscribedCircle() const {
        Point center = circumscribedCircleCenter();
        return Circle(center, Geometry::distance(center, vertices[0]));
    }

    Circle inscribedCircle() const {
        Point center = inscribedCircleCenter();
        return Circle(center, 2 * area() / perimeter());
    }

    Line EulerLine() const {
        Point orthocenterPoint = orthocenter();
        Point circumscribedCircleCenterPoint = circumscribedCircleCenter();
        Point centroidPoint = centroid();

        if (orthocenterPoint == centroidPoint && centroidPoint == circumscribedCircleCenterPoint)
            return Line(orthocenterPoint, 1);
        else if (orthocenterPoint == centroidPoint)
            return Line(centroidPoint, circumscribedCircleCenterPoint);
        else
            return Line(orthocenterPoint, centroidPoint);
    }

    Circle ninePointsCircle() const {
        Point center = (orthocenter() + circumscribedCircleCenter()) / 2;

        return Circle(center, Geometry::distance(center, (vertices[0] + vertices[1]) / 2));
    }

};

#endif //GEOMETRY_H