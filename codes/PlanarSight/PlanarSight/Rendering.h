#pragma once
#include "Polygon.h"
#include "DisjointSet.h"
#include "shapes.h"
#include "MeshUpdate.h"
#include "Monster.h"
#include <iostream>

class Rendering
{
protected:

public:
	std::vector<p2t::Triangle*> initialMesh;
	std::vector<p2t::Triangle*> splitedMesh;
	PointArray sortedPointArray;
	SegmentArray sortedSegmentArray;

	CPolygon* basePolygon;
	PolygonArray visPolygons;
	PointArray loopBuf;
	MonsterArray monsters;
	Point player;
	static int playerSpeed;
	bool drawOuterWall;
	bool drawInnerWall;
	bool drawMonster;

	bool showVisPolygon;
	bool showTriangulation;
	bool showMeshEdgeLabels;
	bool showSortedSegment;

	bool gameStart;
	bool moving;
	bool preprocessFinished;

public:
	Rendering();
	~Rendering();
	void draw();
	void preprocess();
	void drawPolygon(CPolygon& p);
	void drawLoop(CPolygon& p, int loopID);
	void drawUnfinishedLoop(PointArray& pa);
	void drawPoint(Point& p);
	void drawPoint(Point& p, double size);
	bool addPointIntoLoopBuf(Point& p);
	bool loopFinished();
	void drawMonsters(MonsterArray& pa);
	bool addMonster(Point& p);
	void drawPlayer(Point& p);
	void playerWalk(int flag);
	bool playerMoveTo(Point& p);
	void monsterWalk(int monsterID);
	void clear();
	CPolygon calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarPos, DoubleArray& pPolarValues, IntArray& pPolarOrder, double rangeMin, double rangeMax);
	bool calcLineLineIntersection(Point& result, Point& a1, double polar, Point& a3, Point& a4);
	void calcVisPolygon();
	void Test();
	void drawTrianglesMesh(const std::vector<p2t::Triangle*> &mesh);
	void drawSortedSegments(const PointArray &pa, const SegmentArray &sOrder);
};

