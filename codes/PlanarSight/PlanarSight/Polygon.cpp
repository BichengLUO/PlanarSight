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
	int pointSize = lPtr->pointIDArray.size();
	int upMostID;
	double upMost = 0;
	double yFlag;
	Point p;
	int p1ID, p2ID;
	Vector v1, v2;
	double angle;
	double sum = 0;
	double direction;

	for (int i = 0; i < pointSize; i++)
	{
		yFlag = pointArray[lPtr->pointIDArray[i]].y;
		if (yFlag > upMost)
		{
			upMost = yFlag;
			upMostID = i;
		}
	}
	p = pointArray[lPtr->pointIDArray[upMostID]];

	p1ID = upMostID + 1;
	for (int i = 0; i < pointSize; i++)
	{
		if (p1ID >= pointSize)
			p1ID = 0;

		p2ID = p1ID + 1;
		if (p2ID >= pointSize)
			p2ID = 0;

		v1 = pointArray[lPtr->pointIDArray[p1ID]] - p;
		v2 = pointArray[lPtr->pointIDArray[p2ID]] - p;
		angle = calAngle(v1, v2);
		direction = v1 ^ v2;
		if (direction > 0)
			sum += angle;
		else
			sum -= angle;
	}

	if (sum > 0)
		return true;
	else
		return false;
}

void CPolygon::reverseLoop(int loopID)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int pointSize = lPtr->pointIDArray.size();
	int tmp;
	for (int i = 0; i < pointSize; i++)
	{
		tmp = lPtr->pointIDArray[i];
		lPtr->pointIDArray[i] = lPtr->pointIDArray[pointSize - 1 - i];
		lPtr->pointIDArray[pointSize - 1 - i] = tmp;
	}
}

void CPolygon::clear()
{
	pointArray.clear();
	loopArray.clear();
}