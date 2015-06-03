// Point.h: 定义平面上点的数据结构

#pragma once
#include <vector>
#include <stdlib.h>
#include <time.h>
using namespace std;

#define TOLERANCE	1e-7
#define PI				3.1415926
#define DOUBLE_PI	6.2831852
#define HALF_PI		1.5707963

class Point
{
public:
	double x;
	double y;

public:
	Point(){}
	Point(double px, double py) : x(px), y(py) {}

	//void operator=(Point& p);
	void operator=(const Point& p);
	Point operator+(const Point& p);
	void operator+=(const Point& p);
	Point operator-(const Point& p);
	Point operator*(double d);
	double operator*(const Point& p);
	double operator^(const Point& p);

	double length();
	void normalize();
};

struct Polar
{
	int id;
	double value;
};

typedef Point Vector;
typedef vector<Point> PointArray;
typedef vector<Vector> VectorArray;
typedef vector<int> IntArray;
typedef vector<double> DoubleArray;

typedef vector<Polar> PolarArray;

double calAngle(Vector& v1, Vector& v2);
bool equalZero(double d);
double randomDouble();
double calPolar(Point& o, Point& p);

bool polarSortLess(Polar& p1, Polar& p2);




