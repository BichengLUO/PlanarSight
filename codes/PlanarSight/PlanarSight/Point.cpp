#include "Point.h"

//void Point::operator=(Point& p)
//{
//	x = p.x;
//	y = p.y;
//}

void Point::operator=(const Point& p)
{
	x = p.x;
	y = p.y;

}

Point Point::operator+(const Point& p)
{
	Point pp;
	pp.x = x + p.x;
	pp.y = y + p.y;
	return pp;
}

void Point::operator+=(const Point& p)
{
	x += p.x;
	y += p.y;
}

Point Point::operator-(const Point& p)
{
	Point pp;
	pp.x = x - p.x;
	pp.y = y - p.y;
	return pp;
}

Point Point::operator*(double d)
{
	Point pp;
	pp.x = d * x;
	pp.y = d * y;
	return pp;
}

double Point::operator*(const Point& p)
{
	double d = x * p.x + y * p.y;
	return d;
}

double Point::operator^(const Point& p)
{
	double d = x * p.y - y * p.x;
	return d;
}

double Point::length()
{
	double l = x * x + y * y;
	l = pow(l, 0.5);
	return l;
}

void Point::normalize()
{
	double l = this->length();
	if (l == 0)
		return;
	x /= l;
	y /= l;
}

double calAngle(Vector& v1, Vector& v2)
{
	double d = v1 * v2;
	d = d * (1 / v1.length() / v2.length());
	d = acos(d);
	return d;
}

bool equalZero(double d)
{
	if (abs(d) < TOLERANCE)
		return true;
	else
		return false;
}