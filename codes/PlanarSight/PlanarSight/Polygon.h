// 定义多边形及其中包含的数据结构

#pragma once
#include "Point.h"

class CPolygon;

class Segment
{
public:
	int aID;
	int bID;
	CPolygon* polygon;

public:
	Segment(){}
};

class Loop
{
public:
	IntArray pointIDArray;
	CPolygon* polygon;

public:
	Loop(){}
	~Loop();

	void addPoint(int id);
};
typedef vector<Loop> LoopArray;

class CPolygon
{
public:
	PointArray pointArray;
	LoopArray loopArray;

public:
	CPolygon(){}
	~CPolygon();

	void addPoint(Point& p);
	void addLoop(PointArray& pa);
	bool addOuterLoop(PointArray& pa);
	bool addInnerLoop(PointArray& pa);
	bool pointInLoopTest(Point& p, int loopID);
	bool pointInPolygonTest(Point& p);
};
typedef vector<CPolygon> PolygonArray;