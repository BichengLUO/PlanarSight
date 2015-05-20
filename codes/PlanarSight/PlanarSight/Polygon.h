// 定义多边形及其中包含的数据结构

#pragma once
#include "Point.h"

class Polygon;

class Segment
{
public:
	int aID;
	int bID;
	Polygon* polygon;

public:
	Segment(){}
};

class Loop
{
public:
	IntArray pointIDArray;
	Polygon* polygon;

public:
	Loop(){}
	~Loop();

	void addPoint(int id);
};
typedef vector<Loop> LoopArray;

class Polygon
{
public:
	PointArray pointArray;
	LoopArray loopArray;

public:
	Polygon(){}
	~Polygon();

	void addPoint(Point& p);
	void addLoop(PointArray& pa);
	bool addOuterLoop(PointArray& pa);
	bool addInnerLoop(PointArray& pa);
	bool pointInLoopTest(Point& p, int loopID);
	bool pointInPolygonTest(Point& p);
};