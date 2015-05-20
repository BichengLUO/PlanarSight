#pragma once
#include "Polygon.h"

class Rendering
{
protected:

public:
	CPolygon* basePolygon;
	PolygonArray visPolygons;
	PointArray loopBuf;

public:
	Rendering();
	~Rendering();
	void draw();
	void drawPolygon(CPolygon& p);
	void drawLoop(CPolygon& p, int loopID);
	void drawUnfinishedLoop(PointArray& pa);
};

