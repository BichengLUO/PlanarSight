#include "stdafx.h"
#include "Rendering.h"


Rendering::Rendering()
{
	basePolygon = new CPolygon();
}


Rendering::~Rendering()
{
}

//绘制地图的主方法，每次重绘都会被调用
void Rendering::draw()
{
	glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glVertex2d(20, 20);
	glColor3d(0, 1, 0);
	glVertex2d(20, 40);
	glColor3d(0, 0, 1);
	glVertex2d(40, 20);
	glEnd();
}

void Rendering::drawPolygon(CPolygon& p)
{
	int loopSize = p.loopArray.size();

	glColor3d(0, 1, 0);
	drawLoop(p, 0);

	glColor3d(1, 0, 0);
	for (int i = 1; i < loopSize; i++)
		drawLoop(p, i);
}

void Rendering::drawLoop(CPolygon& p, int loopID)
{
	int pointSize = p.loopArray[loopID].pointIDArray.size();
	int index;
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < pointSize; i++)
	{
		index = p.loopArray[loopID].pointIDArray[i];
		glVertex2d(p.pointArray[index].x, p.pointArray[index].y);
	}
	glEnd();
}

void Rendering::drawUnfinishedLoop(PointArray& pa)
{
	int pointSize = pa.size();
	glColor3d(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < pointSize; i++)
		glVertex2d(pa[i].x, pa[i].y);
	glEnd();
}