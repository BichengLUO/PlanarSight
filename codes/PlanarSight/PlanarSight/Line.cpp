#include "Line.h"

bool Line::isParallel(Line &line)
{
    if (fabs(a * line.b - b * line.a) < eps)
        return true;
    else return false;
}

Point Line::getIntersectionPoint(Line &line)
{
    double d = a * line.b - b * line.a;
    double ex = b * line.c - c * line.b;
    double ey = c * line.a - a * line.c;
    return Point(ex / d, ey / d);
}

Point Line::getIntersection(HalfEdge* edge)
{
    Point p1 = Point(edge->origin->coordinate.x, edge->origin->coordinate.y);
    Point p2 = Point(edge->getDestination()->coordinate.x, edge->getDestination()->coordinate.y);
    Line line = Line(p1, p2);
    return Line::getIntersectionPoint(line);
}


bool Line::isOnLine(Point &point)
{
    double d = a * point.x + b * point.y + c;
    double m = sqrt(a * a + b * b);
    return fabs(d) / m < eps;
}

bool Line::isIntersection(HalfEdge* edge)
{
    Point p1 = edge->origin->coordinate;
    Point p2 = edge->twin->origin->coordinate;

    Point p0;
    if (fabs(a) < eps)
    {
        p0.x = 1;
        p0.y = -c / b;
    }
    else
    {
        p0.x = -c / a;
        p0.y = 0;
    }
    Vector vector = Vector(-b, a);
    Vector v01 = Vector(p1.x - p0.x, p1.y - p0.y);
    Vector v02 = Vector(p2.x - p0.x, p2.y - p0.y);
    double s1 = vector ^ v01;
    double s2 = vector ^ v02;
    return s1 * s2 <= 0;
}

void Line::print()
{
    printf("Line:(%lfx + %lfy + %lf = 0\n", a, b, c);
}