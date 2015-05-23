#include "ConvexHull.h"

// A globle point needed for  sorting points with reference to the first point
// Used in compare function of qsort()
Point p0;
Loop loop;

// A utility function to find next to top in a stack
int nextToTop(stack<int> &S)
{
	int p = S.top();
	S.pop();
	int res = S.top();
	S.push(p);
	return res;
}

// A utility function to swap two points
void swap(int &p1, int &p2)
{
	int temp = p1;
	p1 = p2;
	p2 = temp;
}

// A utility function to return square of distance between p1 and p2
int dist(Point p1, Point p2)
{
	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(Point p, Point q, Point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear
	return (val > 0) ? 1 : 2; // clock or counterclock wise
}

// A function used by library function qsort() to sort an array of
// points with respect to the first point
int compare(const void *vp1, const void *vp2)
{
	Point *p1 = &loop.polygon->pointArray[*(int *)vp1];
	Point *p2 = &loop.polygon->pointArray[*(int *)vp2];

	// Find orientation
	int o = orientation(p0, *p1, *p2);
	if (o == 0)
		return (dist(p0, *p2) >= dist(p0, *p1)) ? -1 : 1;

	return (o == 2) ? -1 : 1;
}

Loop convexHull(const Loop &l)
{
	loop = l;
	int n = loop.pointIDArray.size();
	// Find the bottommost point
	int ymin = loop.polygon->pointArray[loop.pointIDArray[0]].y, min = 0;
	for (int i = 1; i < n; i++)
	{
		int y = loop.polygon->pointArray[loop.pointIDArray[i]].y;

		// Pick the bottom-most or chose the left most point in case of tie
		if ((y < ymin) || (ymin == y
			&& loop.polygon->pointArray[loop.pointIDArray[i]].x < loop.polygon->pointArray[loop.pointIDArray[min]].x))
			ymin = loop.polygon->pointArray[loop.pointIDArray[i]].y, min = i;
	}

	// Place the bottom-most point at first position
	swap(loop.pointIDArray[0], loop.pointIDArray[min]);

	// Sort n-1 points with respect to the first point.  A point p1 comes
	// before p2 in sorted ouput if p2 has larger polar angle (in 
	// counterclockwise direction) than p1
	p0 = loop.polygon->pointArray[loop.pointIDArray[0]];
	qsort(&loop.pointIDArray[1], n - 1, sizeof(int), compare);

	// Create an empty stack and push first three points to it.
	stack<int> S;
	S.push(loop.pointIDArray[0]);
	S.push(loop.pointIDArray[1]);
	S.push(loop.pointIDArray[2]);

	// Process remaining n-3 points
	for (int i = 3; i < n; i++)
	{
		// Keep removing top while the angle formed by points next-to-top, 
		// top, and points[i] makes a non-left turn
		while (orientation(
			loop.polygon->pointArray[nextToTop(S)],
			loop.polygon->pointArray[S.top()],
			loop.polygon->pointArray[loop.pointIDArray[i]]) != 2)
			S.pop();
		S.push(loop.pointIDArray[i]);
	}

	Loop ch;
	ch.polygon = loop.polygon;
	while (!S.empty())
	{
		int p = S.top();
		ch.pointIDArray.push_back(p);
		S.pop();
	}
	std::reverse(ch.pointIDArray.begin(), ch.pointIDArray.end());
	return ch;
}