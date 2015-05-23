#ifndef _CONVEX_HULL_H
#define _CONVEX_HULL_H

#include "Polygon.h"
#include <stack>

// A utility function to find next to top in a stack
int nextToTop(stack<int> &S);
// A utility function to swap two points
void swap(int &p1, int &p2);
// A utility function to return square of distance between p1 and p2
int dist(Point p1, Point p2);
// To find orientation of ordered triplet (p, q, r).
int orientation(Point p, Point q, Point r);
// A function used by library function qsort() to sort an array of
// points with respect to the first point
int compare(const void *vp1, const void *vp2);

Loop convexHull(const Loop &l);

#endif