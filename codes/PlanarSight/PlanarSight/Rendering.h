#pragma once
#include "Polygon.h"
#include "DisjointSet.h"

class Rendering
{
protected:

public:
	CPolygon* basePolygon;
	PolygonArray visPolygons;
	PointArray loopBuf;
	PointArray monsters;
	bool drawOuterWall;
	bool drawInnerWall;
	bool drawMonster;

public:
	Rendering();
	~Rendering();
	void draw();
	void drawPolygon(CPolygon& p);
	void drawLoop(CPolygon& p, int loopID);
	void drawUnfinishedLoop(PointArray& pa);
	void drawPoint(Point& p);
	void drawPoint(Point& p, double size);
	bool loopFinished();
	void drawMonsters(PointArray& pa);
	bool addMonster(Point& p);
	void clear();
	CPolygon calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarPos, DoubleArray& pPolarValues, IntArray& pPolarOrder, double rangeMin, double rangeMax);
	bool calcLineLineIntersection(Point& result, Point& a1, double polar, Point& a3, Point& a4);
	void Test();
};

