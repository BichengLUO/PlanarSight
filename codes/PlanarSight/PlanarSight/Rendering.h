#pragma once
#include "Polygon.h"
#include "DisjointSet.h"
#include "shapes.h"
#include "MeshUpdate.h"
#include <iostream>

class Rendering
{
protected:

public:
	std::vector<p2t::Triangle*> initialMesh;
	std::vector<p2t::Triangle*> splitedMesh;
	CPolygon* basePolygon;
	PolygonArray visPolygons;
	PointArray loopBuf;
	PointArray monsters;
	Point player;
	static int playerSpeed;
	bool drawOuterWall;
	bool drawInnerWall;
	bool drawMonster;
	bool showVisPolygon;
	bool gameStart;
	bool moving;

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
	void drawPlayer(Point& p);
	void playerWalk(int flag);
	bool playerMoveTo(Point& p);
	void clear();
	CPolygon calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarPos, DoubleArray& pPolarValues, IntArray& pPolarOrder, double rangeMin, double rangeMax);
	bool calcLineLineIntersection(Point& result, Point& a1, double polar, Point& a3, Point& a4);
	void Test();
	void drawTrianglesMesh(const std::vector<p2t::Triangle*> &mesh);
};

