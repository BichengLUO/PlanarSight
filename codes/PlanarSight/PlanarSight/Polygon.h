// 定义多边形及其中包含的数据结构

#pragma once
#include "Point.h"

class CPolygon;

class Segment
{
public:
	int aID;
	int bID;
public:
	Segment(){}
	Segment(int a, int b): aID(a), bID(b){}
};
typedef vector<Segment> SegmentArray;

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
	bool loopCCWTest(int loopID);
	void reverseLoop(int loopID);
	bool edgeEdgeIntersectionTest(Point& a1, Point& a2, Point& b1, Point& b2);
	bool edgeLoopIntersectionTest(Point& a1, Point& a2, int loopID);
	bool edgePolygonIntersectionTest(Point& a1, Point& a2);
	bool edgeLoopIntersectionNormal(Point& a1, Point& a2, int loopID, double& result);
	bool edgePolygonIntersectionNormal(Point& a1, Point& a2, double& result);
	bool loopSelfIntersectionTest(PointArray& pa);
	void clear();
};
typedef vector<CPolygon> PolygonArray;