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

CPolygon::~CPolygon()
{
	pointArray.clear();
	loopArray.clear();
}

void CPolygon::addPoint(Point& p)
{
	pointArray.push_back(p);
}

void CPolygon::addLoop(PointArray& pa)
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

bool CPolygon::addOuterLoop(PointArray& pa)
{
	if (loopArray.size() != 0)
		return false;

	addLoop(pa);
	if (!loopCCWTest(0))
		reverseLoop(0);

	return true;
}

bool CPolygon::addInnerLoop(PointArray& pa)
{
	if (loopArray.size() == 0)
		return false;

	addLoop(pa);
	if (loopCCWTest(loopArray.size() - 1))
		reverseLoop(loopArray.size() - 1);

	return true;
}

bool CPolygon::pointInLoopTest(Point& p, int loopID)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int pointSize = lPtr->pointIDArray.size();
	Vector v1, v2;
	double angle;
	double direction;
	double sum = 0;

	for (int i = 0; i < pointSize - 1; i++)
	{
		v1 = pointArray[lPtr->pointIDArray[i]] - p;
		v2 = pointArray[lPtr->pointIDArray[i + 1]] - p;
		angle = calAngle(v1, v2);
		direction = v1 ^ v2;
		if (direction > 0)
			sum += angle;
		else
			sum -= angle;
	}
	v1 = pointArray[lPtr->pointIDArray[pointSize - 1]] - p;
	v2 = pointArray[lPtr->pointIDArray[0]] - p;
	angle = calAngle(v1, v2);
	direction = v1 ^ v2;
	if (direction > 0)
		sum += angle;
	else
		sum -= angle;

	if (equalZero(sum))
		return false;
	else
		return true;
}

bool CPolygon::pointInPolygonTest(Point& p)
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

bool CPolygon::loopCCWTest(int loopID)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	double max_y = 0;
	int max_y_point_id = 0;
	int loopPointsSize = lPtr->pointIDArray.size();
	for (int i = 0; i < loopPointsSize; i++)
	{
		if (pointArray[lPtr->pointIDArray[i]].y > max_y)
		{
			max_y = pointArray[lPtr->pointIDArray[i]].y;
			max_y_point_id = i;
		}
	}
	Point p1 = pointArray[(max_y_point_id - 1 + loopPointsSize) % loopPointsSize];
	Point p2 = pointArray[max_y_point_id];
	Point p3 = pointArray[(max_y_point_id + 1) % loopPointsSize];

	double dp = (p2 - p1) ^ (p3 - p2);
	if (dp < 0)
		return false;
	else if (dp == 0)
		return p1.x > p2.x && p2.x > p3.x;
	else
		return true;
}

void CPolygon::reverseLoop(int loopID)
{
	Loop* lPtr = &(loopArray[loopID]);
	std::reverse(lPtr->pointIDArray.begin(), lPtr->pointIDArray.end());
}

void CPolygon::clear()
{
	pointArray.clear();
	loopArray.clear();
}