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
	clear();
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
		drawPoint(pa[i], 5);
}

void Rendering::clear()
{
	basePolygon->clear();
	int visSize = visPolygons.size();
	for (int i = 0; i < visSize; i++)
		visPolygons[i].clear();
	loopBuf.clear();
	monsters.clear();
	initialMesh.clear();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;
}

// 计算可见多边形
// pa：包含原多边形中的点及更新三角剖分中新加入的点
// sOrder：多边形中所有边所在线段，按距离查询点的偏序关系排放
// pPolarID：指示pa中所有点在极角排序中的次序，与pa一一对应
// pPolarVal：指示pa中所有点的极角值，与pa一一对应
// pPolarOrder：存储一组关于pa中点的索引，按pa中点的极角序排放
CPolygon Rendering::calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarID, DoubleArray& pPolarVal, IntArray& pPolarOrder, double rangeMin, double rangeMax)
{
	int zeroNum = 0;							//极角为0的点的数量
	int pointSize = pa.size();
	for (int i = 0; i < pointSize; i++)
		if (pPolarVal[i] == 0)
			zeroNum++;
	int xSize = pointSize - zeroNum + 1;		//x方向所需的区间数量
	int segmentSize = sOrder.size();
	int left, right;
	int x;
	DisjointSet ds(xSize);
	IntArray vis;								//指示每个x向上可见的线段ID
	vis.insert(vis.begin(), xSize, -1);

	for (int i = 0; i < segmentSize; i++)
	{
		left = pPolarID[sOrder[i].aID];		//线段左端点对应的x值
		right = pPolarID[sOrder[i].bID];		//线段右端点对应的x值
		if (right == 0)
			right = xSize;
		x = ds.findSetMax(left);				//线段左端点x值所在集合最大值
		while (x < right)
		{
			vis[x] = i;
			ds.link(x, x + 1);
			x = ds.findSetMax(x);
		}
	}

	int rangeLeft = 0;			//可视范围的左边界所在的x区间
	int rangeRight = 0;		//可视范围的右边界所在的x区间
	int rangeLeftY;			//可视范围的左边界所在区间的y值
	int rangeRightY;			//可视范围的右边界所在区间的y值
	int rangeLengthX;			//可视范围的区间在x上的长度，不包含两个端点
	int pID;
	for (int i = zeroNum; i < pointSize; i++)
	{
		pID = pPolarOrder[i];
		if (pPolarVal[pID] > rangeMin)
			break;
		rangeLeft = i;
	}
	for (int i = zeroNum; i < pointSize; i++)
	{
		pID = pPolarOrder[i];
		if (pPolarVal[pID] > rangeMax)
			break;
		rangeRight = i;
	}
	rangeLeft -= zeroNum - 1;
	rangeRight -= zeroNum - 1;
	rangeLeftY = vis[rangeLeft];
	rangeRightY = vis[rangeRight];
	if (rangeMin <= rangeMax)
		rangeLengthX = rangeRight - rangeLeft;
	else
		rangeLengthX = xSize - (rangeLeft - rangeRight);

	PointArray pointBuf;
	CPolygon cp;
	Loop l;
	Point pLeft, pRight, pMid;
	int xFlag;
	int preY, curY;
	double polar;
	pointBuf.push_back(monsters[monsterID]);

	//计算可视范围左边界与线段集的交点
	calcLineLineIntersection(pLeft, monsters[monsterID], rangeMin, pa[sOrder[rangeLeftY].aID], pa[sOrder[rangeLeftY].bID]);
	pointBuf.push_back(pLeft);
	preY = rangeLeftY;

	xFlag = rangeLeft + 1;
	for (int i = 0; i < rangeLengthX; i++)
	{
		if (xFlag >= xSize)
			xFlag = 0;

		curY = vis[xFlag];
		//由y值更大的可视线段切换到更小的
		if (curY > preY)
		{
			polar = pPolarVal[sOrder[preY].bID];
			calcLineLineIntersection(pMid, monsters[monsterID], polar, pa[sOrder[curY].aID], pa[sOrder[curY].bID]);
			pointBuf.push_back(pa[sOrder[preY].bID]);
			pointBuf.push_back(pMid);
		}
		//由y值更小的可视线段切换到更大的
		else if (curY < preY)
		{
			polar = pPolarVal[sOrder[curY].aID];
			calcLineLineIntersection(pMid, monsters[monsterID], polar, pa[sOrder[preY].aID], pa[sOrder[preY].bID]);
			pointBuf.push_back(pMid);
			pointBuf.push_back(pa[sOrder[curY].aID]);
		}

		preY = curY;
		xFlag++;
	}

	//计算可视范围右边界与线段集的交点
	calcLineLineIntersection(pRight, monsters[monsterID], rangeMax, pa[sOrder[rangeRightY].aID], pa[sOrder[rangeRightY].bID]);
	pointBuf.push_back(pRight);

	cp.addLoop(pointBuf);
	return cp;
}

bool calcLineLineIntersection(Point& result, Point& a1, double polar, Point& b1, Point& b2)
{
	Point a2;
	a2.x = 200 * cos(polar);
	a2.y = 200 * sin(polar);
	a2 += a1;

	Vector a1a2 = a2 - a1;
	Vector b1b2 = b2 - b1;
	Vector b1a1 = a1 - b1;
	double t = (b1a1 ^ b1b2) / (b1b2 ^ a1a2);
	result = a1 + (a1a2 * t);
	return true;
}

void Rendering::Test()
{
	Point p;
	PointArray pa;
	pa.insert(pa.begin(), 12, p);

	Segment s;
	SegmentArray sa;
	s.aID = 6;
	s.bID = 7;
	sa.push_back(s);
	s.aID = 0;
	s.bID = 10;
	sa.push_back(s);
	s.aID = 4;
	s.bID = 11;
	sa.push_back(s);
	s.aID = 2;
	s.bID = 3;
	sa.push_back(s);
	s.aID = 10;
	s.bID = 1;
	sa.push_back(s);
	s.aID = 8;
	s.bID = 9;
	sa.push_back(s);
	s.aID = 11;
	s.bID = 5;
	sa.push_back(s);

	IntArray polarID;
	polarID.push_back(10);
	polarID.push_back(4);
	polarID.push_back(3);
	polarID.push_back(9);
	polarID.push_back(8);
	polarID.push_back(1);
	polarID.push_back(6);
	polarID.push_back(7);
	polarID.push_back(2);
	polarID.push_back(5);
	polarID.push_back(0);
	polarID.push_back(0);

	DoubleArray pPolarVal;
	IntArray pPolarOrder;

	calcVisPolygon(0, pa, sa, polarID, pPolarVal, pPolarOrder, 0, DOUBLE_PI);
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
		glBegin(GL_LINE_LOOP);
		glColor3d(0.6, 0.6, 0.8);
		glVertex2d(p1->x, p1->y);
		glVertex2d(p2->x, p2->y);
		glVertex2d(p3->x, p3->y);
		glEnd();
	}
}