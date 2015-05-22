#include "stdafx.h"
#include "Rendering.h"


Rendering::Rendering()
{
	basePolygon = new CPolygon();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;
}


Rendering::~Rendering()
{
}

//绘制地图的主方法，每次重绘都会被调用
void Rendering::draw()
{
	/*glBegin(GL_TRIANGLES);
	glColor3d(1, 0, 0);
	glVertex2d(20, 20);
	glColor3d(0, 1, 0);
	glVertex2d(20, 40);
	glColor3d(0, 0, 1);
	glVertex2d(40, 20);
	glEnd();*/

	if (drawOuterWall || drawInnerWall)
	{
		drawUnfinishedLoop(loopBuf);
	}

	if (initialMesh.size() > 0)
		drawTrianglesMesh(initialMesh);
	drawPolygon(*basePolygon);
	drawMonsters(monsters);
}

void Rendering::drawPolygon(CPolygon& p)
{
	int loopSize = p.loopArray.size();
	if (loopSize == 0)
		return;

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

	glColor3d(1, 1, 1);
	for (int i = 0; i < pointSize; i++)
		drawPoint(pa[i]);
}

void Rendering::drawPoint(Point& p)
{
	drawPoint(p, 3);
}

void Rendering::drawPoint(Point& p, double size)
{
	int n = 100;
	double angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		angle = DOUBLE_PI * i / n;
		glVertex2d(p.x + size * cos(angle), p.y + size * sin(angle));
	}
	glEnd();
}

bool Rendering::loopFinished()
{
	if (loopBuf.size() < 3)
	{
		loopBuf.clear();
		return true;
	}

	bool flag;
	if (drawOuterWall)
		flag = basePolygon->addOuterLoop(loopBuf);
	else if (drawInnerWall)
		flag = basePolygon->addInnerLoop(loopBuf);

	loopBuf.clear();
	return flag;
}

bool Rendering::addMonster(Point& p)
{
	if (basePolygon->loopArray.size() == 0)
		return false;

	if (!basePolygon->pointInPolygonTest(p))
		return false;

	monsters.push_back(p);
	initialMesh = insertPointToUpdateTriangles(initialMesh, p2t::Point(p.x, p.y));
	return true;
}

void Rendering::drawMonsters(PointArray& pa)
{
	int pointSize = pa.size();
	glColor3d(0, 1, 1);
	for (int i = 0; i < pointSize; i++)
	{
		drawPoint(pa[i], 5);
		glBegin(GL_LINES);
		glVertex2d(pa[i].x, pa[i].y);
		glVertex2d(pa[i].x, 700);
		glEnd();
	}
}

void Rendering::clear()
{
	basePolygon->clear();
	for (int i = 0; i < visPolygons.size(); i++)
		visPolygons[i].clear();
	loopBuf.clear();
	monsters.clear();
	initialMesh.clear();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;
}

void Rendering::drawTrianglesMesh(const std::vector<p2t::Triangle*> &mesh)
{
	std::vector<p2t::Triangle*>::const_iterator it;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		const p2t::Point *p1 = (*it)->GetPoint(0);
		const p2t::Point *p2 = (*it)->GetPoint(1);
		const p2t::Point *p3 = (*it)->GetPoint(2);

		glBegin(GL_POLYGON);
		glColor3d((p1->x + p2->x + p3->x) / (3.0 * 800),
			(p1->y + p2->y + p3->y) / (3.0 * 700),
			0.8);
		glVertex2d(p1->x, p1->y);
		glVertex2d(p2->x, p2->y);
		glVertex2d(p3->x, p3->y);
		glEnd();
	}
}