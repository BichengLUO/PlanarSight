#pragma once
#include "Polygon.h"
#include "DisjointSet.h"
#include "shapes.h"
#include "MeshUpdate.h"
#include "Monster.h"
#include "DCEL.h"
#include <iostream>
#include <algorithm>

class Rendering
{
protected:

public:
	std::vector<p2t::Triangle*> initialMesh;
	std::vector<p2t::Triangle*> splitedMesh;
	PointArray sortedPointArray;
	SegmentArray sortedSegmentArray;
	IntArray pPolarID;
	DoubleArray pPolarValues;
	IntArray pPolarOrder;
	int xLeft;
	int xRight;
	double xLeftParam;
	double xRightParam;
	IntArray visArray;

    DCEL* dcel;

	CPolygon* basePolygon;
	PolygonArray visPolygons;
	PolygonArray playerVisPolygon;
	PointArray loopBuf;
	MonsterArray monsters;
	Player player;
	static int playerSpeed;
	bool drawOuterWall;
	bool drawInnerWall;
	bool drawMonster;

	bool showVisPolygon;
	bool showTriangulation;
	bool showMeshEdgeLabels;
	bool showSortedSegment;
	bool showDualGraph;
	bool show3DView;
	bool showLinearSet;
	bool showAllMonsters;
	bool useDCELSort;

	bool gameStart;
	bool moving;
	bool preprocessFinished;
	bool gameOver;

public:
	Rendering();
	~Rendering();
	void draw();
	void showPolygonInfo();
	void process();
	void preprocess();
	void randomPlacePlayer();
	void drawPolygon(CPolygon& p);
	void drawPolygon3D(CPolygon& p);
	void drawVisPolygon(CPolygon& p);
	void drawVisPolygon3D(CPolygon& p, double offset);
	void drawLoop(CPolygon& p, int loopID);
	void drawLoop3D(CPolygon &p, int loopID, int tex_id);
	void drawFloor();
	void drawUnfinishedLoop(PointArray& pa);
	void drawPoint(Point& p);
	void drawPoint(Point& p, double size);
	void drawPoint3D(Point& p, double size);
	bool addPointIntoLoopBuf(Point& p);
	bool loopFinished();
	void drawMonsters(MonsterArray& pa);
	void drawMonsters3D(MonsterArray& ma);
	bool addMonster(Point& p);
	void drawPlayer(Point& p);
	void drawPlayer3D(Point& p);
	void playerWalk(int flag);
	bool playerMoveTo(Point& p);
	void monsterWalk(int monsterID);
	void clear();
	CPolygon calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder);
	bool calcLineLineIntersection(Point& result, Point& a1, double polar, Point& a3, Point& a4);
	void calcVisPolygon();
    void getPolarOrder(int monsterID, PointArray& pa, PointArray& pb, PointArray& p, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder);
    void getPolarOrderByDCEL(int monsterID, PointArray& pa, PointArray& pb, PointArray& p, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder);
	void Test();
	void drawTrianglesMesh(const std::vector<p2t::Triangle*> &mesh);
	void drawSortedSegments(const PointArray &pa, const SegmentArray &sOrder);
	void drawDualGraphBackground();
	void drawDualGraph(const PointArray &pa, double rc, double gc, double bc);
	void drawDualGraph(const Point &p, double rc, double gc, double bc);
	void drawLinearSetBackground();
	void drawLinearSet();

	void change2D();
	void exit2D();
};

