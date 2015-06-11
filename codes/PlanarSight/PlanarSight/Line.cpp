#include "Line.h"

bool Line::isParallel(const Line &line)
{
    if (fabs(a * line.b - b * line.a) < eps)
        return true;
    else return false;
}

bool Line::isParallel(const Line &line, double tolerance)
{
    if (fabs(a * line.b - b * line.a) < tolerance)
        return true;
    else return false;
}

double Line::getDistance(const Point &point)
{
    double d = a * point.x + b * point.y + c;
    double m = sqrt(a * a + b * b);
    return fabs(d) / m;
}

Point Line::getIntersectionPoint(const Line &line)
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

bool Line::isOnLine(const Point &point)
{
    return getDistance(point) < eps;
}

bool Line::isOnLine(const Point &point, double tolerance)
{
    return getDistance(point) < tolerance;
}

bool Line::isIntersection(HalfEdge* edge)
{
    Point p1 = edge->origin->coordinate;
    Point p2 = edge->twin->origin->coordinate;
    Line line = Line(p1, p2);
    if (isParallel(line) == true)
        return false;

    Point po = getIntersectionPoint(line);
    double xmin = min(p1.x, p2.x);
    double xmax = max(p1.x, p2.x);
    double ymin = min(p1.y, p2.y);
    double ymax = max(p1.y, p2.y);
    return (xmin + eps< po.x && po.x < xmax - eps) || (ymin + eps < po.y && po.y < ymax - eps);
}

bool Line::isIntersection(HalfEdge* edge, double tolerance)
{
    Point p1 = edge->origin->coordinate;
    Point p2 = edge->twin->origin->coordinate;
    Line line = Line(p1, p2);
    if (isParallel(line, tolerance) == true)
        return false;

    Point po = getIntersectionPoint(line);
    double xmin = min(p1.x, p2.x);
    double xmax = max(p1.x, p2.x);
    double ymin = min(p1.y, p2.y);
    double ymax = max(p1.y, p2.y);
    return (xmin + tolerance< po.x && po.x < xmax - tolerance) || (ymin + tolerance < po.y && po.y < ymax - tolerance);
}

void Line::print()
{
    printf("Line:(%lfx + %lfy + %lf = 0\n", a, b, c);
}