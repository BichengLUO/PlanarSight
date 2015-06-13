#include "Polygon.h"
#include <string>
#include <sstream>

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
	Point p1, p2;
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int pointSize = lPtr->pointIDArray.size();
	int i;
	int j = pointSize - 1;
	bool oddNodes = false;

	for (i = 0; i < pointSize; i++)
	{
		p1 = pointArray[lPtr->pointIDArray[i]];
		p2 = pointArray[lPtr->pointIDArray[j]];
		if ((p1.y < p.y && p2.y >= p.y || p2.y < p.y && p1.y >= p.y) && (p1.x <= p.x || p2.x <= p.x))
		{
			oddNodes ^= (p1.x + (p.y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x) < p.x);
		}
		j = i;
	}
	return oddNodes;

	/*Vector v1, v2;
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
		return true;*/
}

bool CPolygon::pointInPolygonTest(Point& p)
{
	int loopSize = loopArray.size();
	if (loopSize == 0)
		return true;

	if (!pointInLoopTest(p, 0))
		return false;

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

bool CPolygon::edgeEdgeIntersectionTest(Point& a1, Point& a2, Point& b1, Point& b2)
{
	Vector a1a2 = a2 - a1;
	Vector b1b2 = b2 - b1;
	Vector b1a1 = a1 - b1;
	Vector a1b1 = b1 - a1;

	double s = b1b2 ^ a1a2;
	if (equalZero(s))
		return false;

	double pa = (b1a1 ^ b1b2) / s;
	double pb = (a1a2 ^ a1b1) / s;

	if (pa > 0 && pa < 1 && pb > 0 && pb < 1)
		return true;
	else
		return false;
}

bool CPolygon::edgeLoopIntersectionTest(Point& a1, Point& a2, int loopID)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int loopPointSize = lPtr->pointIDArray.size();
	int preID, curID;

	curID = lPtr->pointIDArray[0];
	preID = lPtr->pointIDArray[loopPointSize - 1];
	if (edgeEdgeIntersectionTest(a1, a2, pointArray[preID], pointArray[curID]))
		return true;
	for (int i = 1; i < loopPointSize; i++)
	{
		preID = curID;
		curID = lPtr->pointIDArray[i];
		if (edgeEdgeIntersectionTest(a1, a2, pointArray[preID], pointArray[curID]))
			return true;
	}

	return false;
}

bool CPolygon::loopSelfIntersectionTest(PointArray& pa)
{
	int size = pa.size();
	for (int i = 0; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (edgeEdgeIntersectionTest(pa[i], pa[(i + 1) % size], pa[j], pa[(j + 1) % size]))
				return true;
		}
	}

	return false;
}

bool CPolygon::edgePolygonIntersectionTest(Point& p1, Point& p2)
{
	int loopSize = loopArray.size();
	for (int i = 0; i < loopSize; i++)
		if (edgeLoopIntersectionTest(p1, p2, i))
			return true;

	return false;
}

bool CPolygon::edgeLoopIntersectionNormal(Point& a1, Point& a2, int loopID, double& result)
{
	Loop* lPtr;
	lPtr = &(loopArray[loopID]);
	int loopPointSize = lPtr->pointIDArray.size();
	int preID, curID;
	bool flag = false;
	Vector xAxis(1, 0);

	curID = lPtr->pointIDArray[0];
	preID = lPtr->pointIDArray[loopPointSize - 1];
	if (edgeEdgeIntersectionTest(a1, a2, pointArray[preID], pointArray[curID]))
		flag =  true;
	else
	{
		for (int i = 1; i < loopPointSize; i++)
		{
			preID = curID;
			curID = lPtr->pointIDArray[i];
			if (edgeEdgeIntersectionTest(a1, a2, pointArray[preID], pointArray[curID]))
			{
				flag = true;
				break;
			}
		}
	}
	
	Vector edge = pointArray[curID] - pointArray[preID];
	double angle = calAngle(xAxis, edge);
	if ((xAxis ^ edge) < 0)
		angle = DOUBLE_PI - angle;
	result = angle;

	return flag;
}

bool CPolygon::edgePolygonIntersectionNormal(Point& p1, Point& p2, double& result)
{
	int loopSize = loopArray.size();
	for (int i = 0; i < loopSize; i++)
		if (edgeLoopIntersectionNormal(p1, p2, i, result))
			return true;

	return false;
}

void CPolygon::exportToFile(std::ofstream &output)
{
	for (int i = 0; i < pointArray.size(); i++)
		output << "v " << pointArray[i].x << " " << pointArray[i].y << std::endl;

	for (int i = 0; i < loopArray.size(); i++)
	{
		output << "l";
		for (int j = 0; j < loopArray[i].pointIDArray.size(); j++)
			output << " " << loopArray[i].pointIDArray[j];
		output << std::endl;
	}
}

void CPolygon::importFromFile(std::ifstream &input)
{
	clear();
	std::string line;
	while (std::getline(input, line))
	{
		std::istringstream iss(line);
		if (line[0] == 'v')
		{
			double x, y;
			char v;
			iss >> v >> x >> y;
			pointArray.push_back(Point(x, y));
		}
		if (line[0] == 'l')
		{
			Loop loop;
			loop.polygon = this;
			string token;
			while (std::getline(iss, token, ' '))
			{
				if (token[0] != 'l')
					loop.pointIDArray.push_back(atoi(token.c_str()));
			}
			loopArray.push_back(loop);
		}
	}
	if (!loopCCWTest(0))
		reverseLoop(0);
	for (int i = 1; i < loopArray.size(); i++)
	if (loopCCWTest(i))
		reverseLoop(i);
}

void CPolygon::clear()
{
	pointArray.clear();
	loopArray.clear();
}