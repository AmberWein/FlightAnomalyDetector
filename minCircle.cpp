// 312292998	208783522

#include "minCircle.h"
using namespace std;

/*
* function name: distance
* input: Point&, Point&
* output: float
* operation: function returns the distance between 2 given points
*/
float distance(Point &p1, Point &p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

/*
* function name: dist
* input: Point, Point
* output: float
* operation: function returns the distance between 2 given points
*/
float dist(Point p1, Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

/*
* function name: defineCircleByTwoPoints
* input: Point&, Point&
* output: Circle
* operation: function returns a Circle defined by the 2 given points.
*           the distance between the points is the diameter, and the middle point between them will be the center.
*/
Circle defineCircleByTwoPoints(Point &p1, Point &p2) {
    float diameter = distance(p1, p2);
    float radius = diameter / 2;
    Point center = Point((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);
    return {center, radius};
}

/*
* function name: minRadiusCircle
* input: vector<Circle*>, int
* output: Circle
* operation: function returns the Circle from the given vector which has the smallest radius.
*/
Circle minRadiusCircle(vector<Circle *> circles) {
    float min = std::numeric_limits<double>::infinity();
    // define a default circle
    Circle *minCircle = circles[0];
    for (Circle *c: circles) {
        if (c->radius < min) {
            min = c->radius;
            minCircle = c;
        }
    }
    return *minCircle;
}

/*
* function name: circleBy3Points
* input: Point&, Point&, Point&
* output: Circle
* operation: function returns a Circle defined by the 3 given points.
*           using (x-a)^2+(x-b)^2=r^2, and extracting a,b,r by setting x,y from the given points.
*           (a,b) is center point of circle, and r is the radius.
*/
Circle circleBy3Points(Point &p1, Point &p2, Point &p3) {
    float a, b, r, a1, a2, b1, b2, x, y;
    a1 = (p2.x * p2.x + p2.y * p2.y - p1.x * p1.x - p1.y * p1.y) / (2 * (p2.x - p1.x));
    a2 = (p1.y - p2.y) / (p2.x - p1.x);
    b1 = (p3.x * p3.x + p3.y * p3.y - p1.x * p1.x - p1.y * p1.y) / (2 * (p3.x - p1.x));
    b2 = (p1.y - p3.y) / (p3.x - p1.x);
    b = (b1 - a1) / (a2 - b2);
    a = a1 + a2 * b;
    x = p1.x - a;
    y = p1.y - b;
    r = sqrt(x * x + y * y);
    return {Point(a, b), r};
}

/*
* function name: isInside
* input: Point, Circle&
* output: bool
* operation: function returns true if the given point is inside the given circle, and false otherwise.
*           if the distance of the point from the center is smaller or equal to radius, the point is inside the circle.
*/
bool isInside(Point p, Circle &c) {
    float dist = distance(p, c.center);
    if (dist <= c.radius) {
        return true;
    }
    return false;
}

/*
* function name: defineCircleByThreePoints
* input: Point&, Point&, Point&
* output: Circle
* operation: function returns a Circle defined by the 3 given points.
*           if one of the points is inside the circle defined by the other two, then we define a circle by the two points.
*           we use the one that has the smallest radius.
*           if no circle defined by two points contains the 3'rd, define a circle by all 3 points, using circleBy3Points function.
*/
Circle defineCircleByThreePoints(Point &p1, Point &p2, Point &p3) {
    Circle c1 = defineCircleByTwoPoints(p1, p2);
    Circle c2 = defineCircleByTwoPoints(p2, p3);
    Circle c3 = defineCircleByTwoPoints(p1, p3);
    vector<Circle *> circles;
    // push into vector, any circle that contains the 3'rd point, that is not defining it.
    if (isInside(p3, c1))
        circles.push_back(&c1);
    if (isInside(p2, c3))
        circles.push_back(&c3);
    if (isInside(p1, c2))
        circles.push_back(&c2);
    //check if no circle contained the 3'rd point - define the circle by 3 points.
    if (circles.empty())
        return circleBy3Points(p1, p2, p3);
    // there is at least one circle that contains all 3 points, return the one with the smallest radius.
    return minRadiusCircle(circles);
}

/*
* function name: recursiveMinCircle
* input: Point**, vector<Point*>, size_t
* output: Circle
* operation: function returns a Circle that contains all the points given.
*           by recursively defining a circle by more points
*/
Circle recursiveMinCircle(Point **points, vector<Point *> pRadius, size_t size) {
    int pRadiusSize = pRadius.size();
    // base case, no points to define the circle, ot there are already 3 point on the perimeter, so they define the circle.
    if (size == 0 || pRadiusSize == 3) {
        if (pRadiusSize == 0) {
            return {Point(0, 0), 0};
        }
        if (pRadiusSize == 1) {
            return {*pRadius[0], 0};
        }
        if (pRadiusSize == 2) {
            return defineCircleByTwoPoints(*pRadius[0], *pRadius[1]);
        }
        if (pRadiusSize == 3) {
            return defineCircleByThreePoints(*pRadius[0], *pRadius[1], *pRadius[2]);
        }
    }

    // save a reference to the last point given
    Point *p = points[size - 1];
    // define a circle without Point p
    Circle c = recursiveMinCircle(points, pRadius, size - 1);
    // if p inside c return circle c
    if (isInside(*p, c)) {
        return c;
    }
    // else - put p in pRadius, and find circle for points-{p}, pRadius+{p}
    pRadius.push_back(p);
    return recursiveMinCircle(points, pRadius, size - 1);
}

/*
* function name: findMinCircle
* input: Point**, size_t
* output: Circle
* operation: function returns the minimum Circle that contains all the given points, by using recursiveMinCircle function.
*           note - the given points are randomly organized.
*/
Circle findMinCircle(Point **points, size_t size) {
    vector<Point *> onPerimeter;
    return recursiveMinCircle(points, onPerimeter, size);
}
