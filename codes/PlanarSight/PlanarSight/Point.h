// Point.h: 定义平面上点的数据结构

#pragma once
#include <vector>
using namespace std;

#define TOLERANCE	1e-7
#define PI				3.1415926
#define DOUBLE_PI	6.28318852

class Point
{
public:
	double x;
	double y;

public:
	Point(){}
	Point(double px, double py) : x(px), y(py) {}

	void operator=(Point& p);
	Point operator+(Point& p);
	Point operator-(Point& p);
	Point operator*(double d);
	double operator*(Point& p);
	double operator^(Point& p);

	double length();
	void normalize();
};

typedef Point Vector;
typedef vector<Point> PointArray;
typedef vector<Vector> VectorArray;
typedef vector<int> IntArray;

double calAngle(Vector& v1, Vector& v2);
bool equalZero(double d);




