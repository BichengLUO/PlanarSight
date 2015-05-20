#include "Polygon.h"

Loop::~Loop()
{
	pointIDArray.clear();
	polygon = NULL;
}

void Loop::addPoint(int id)
{
	pointIDArray.push_back(id);
}

Polygon::~Polygon()
{
	pointArray.clear();
	loopArray.clear();
}

void Polygon::addPoint(Point& p)
{
	pointArray.push_back(p);
}

void Polygon::addLoop(PointArray& pa)
{
	int pointID = pointArray.size();
	int loopSize = pa.size();
	Loop l;

	for (int i = 0; i < loopSize; i++)
	{
		l.addPoint(pointID);
		addPoint(pa[i]);
		pointID++;
	}
	l.polygon = this;
	loopArray.push_back(l);
}

bool Polygon::addOuterLoop(PointArray& pa)
{
	if (loopArray.size() != 0)
		return false;

	addLoop(pa);
	return true;
}

bool Polygon::addInnerLoop(PointArray& pa)
{
	if (loopArray.size() == 0)
		return false;

	addLoop(pa);
	return true;
}

bool Polygon::pointInLoopTest(Point& p, int loopID)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int pointSize = lPtr->pointIDArray.size();
	Vector v1, v2;
	double angle;
	double sum = 0;

	for (int i = 0; i < pointSize - 1; i++)
	{
		v1 = pointArray[lPtr->pointIDArray[i]] - p;
		v2 = pointArray[lPtr->pointIDArray[i + 1]] - p;
		angle = calAngle(v1, v2);
		sum += angle;
	}
	v1 = pointArray[lPtr->pointIDArray[pointSize - 1]] - p;
	v2 = pointArray[lPtr->pointIDArray[0]] - p;
	angle = calAngle(v1, v2);
	sum += angle;

	if (equalZero(sum))
		return false;
	else
		return true;
}

bool Polygon::pointInPolygonTest(Point& p)
{
	if (!pointInLoopTest(p, 0))
		return false;

	int loopSize = loopArray.size();
	for (int i = 1; i < loopSize; i++)
	{
		if (pointInLoopTest(p, i))
			return false;
	}

	return true;
}