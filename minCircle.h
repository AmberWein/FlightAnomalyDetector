// 312292998	208783522

#ifndef MINCIRCLE_H
#define MINCIRCLE_H

#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "anomaly_detection_util.h"
using namespace std;

class Circle {
public:
    Point center;
    float radius;

    Circle(Point c, float r) : center(c), radius(r) {}
};


    float distance(Point &p1, Point &p2);

    float dist(Point p1, Point p2);

    Circle defineCircleByTwoPoints(Point &p1, Point &p2);

    Circle minRadiusCircle(vector<Circle *> circles);

    Circle circleBy3Points(Point &p1, Point &p2, Point &p3);

    bool isInside(Point p, Circle &c);

    Circle defineCircleByThreePoints(Point &p1, Point &p2, Point &p3);

    Circle recursiveMinCircle(Point **points, vector<Point *> pRadius, size_t size);

    Circle findMinCircle(Point **points, size_t size);

#endif //MINCIRCLE_H

