#ifndef _LINE_H
#define _LINE_H

#include <cmath>
#include <cstdio>
#include <algorithm>
#include "Point.h"
#include "HalfEdge.h"
#include "DCEL.h"

class Point;
class HalfEdge;

class Line
{
public:
    double a, b, c;

public:
    Line(){}
    Line(double _a, double _b, double _c) : a(_a), b(_b), c(_c){}
    Line(double k, double bb)
    {
        a = k;
        b = -1;
        c = bb;
    }
    Line(const Point &p1, const Point &p2)
    {
        a = p1.y - p2.y;
        b = p2.x - p1.x;
        c = p1.x * p2.y - p2.x * p1.y;
    }
    ~Line(){}

    bool isParallel(const Line &line);

    double getDistance(const Point &point);

    Point getIntersectionPoint(const Line &line);

    // get the intersection point with a line this edge on
    Point getIntersection(HalfEdge* edge);

    bool isOnLine(const Point &point);

    bool isIntersection(HalfEdge* edge);

    void print();
};

#endif