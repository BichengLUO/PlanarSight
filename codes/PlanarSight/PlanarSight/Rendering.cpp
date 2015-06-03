#include "stdafx.h"
#include "Rendering.h"
#include "Mesh2Graph.h"

Rendering::Rendering()
{
	basePolygon = new CPolygon();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;

	showVisPolygon = false;
	showTriangulation = false;
	showMeshEdgeLabels = false;
	showDualGraph = false;

	gameStart = false;
	player.x = 370;
	player.y = 310;
	moving = false;
	preprocessFinished = false;

	srand((unsigned)time(0));
}

int Rendering::playerSpeed = 3;

Rendering::~Rendering()
{
	clear();
}

//绘制地图的主方法，每次重绘都会被调用
void Rendering::draw()
{
	drawPolygon(*basePolygon);
	if (showDualGraph)
	{
		drawDualGraphBackground();
		drawDualGraph(basePolygon->pointArray, 0, 0, 1);
	}
	if (drawOuterWall || drawInnerWall)
	{
		drawUnfinishedLoop(loopBuf);
		if (showDualGraph)
			drawDualGraph(loopBuf, 1, 1, 0);
	}

	drawPlayer(player);

	int monsterSize = monsters.size();
	if (gameStart)
		for (int i = 0; i < monsterSize; i++)
			monsterWalk(i);
	drawMonsters(monsters);

	if (gameStart && monsters.size() > 0)
	{
		//清空上次的新剖分出来的三角形信息
		clearSplitedMeshMemory();
		p2t::Point p(monsters[0].pos.x, monsters[0].pos.y);
		int selc; //表示新剖分出来的边数
		splitedMesh = insertPointToUpdateTriangles(initialMesh, p, &selc); //生成新的三角剖分网格

		//清空上次的排序线段和顶点信息
		sortedPointArray.clear();
		sortedSegmentArray.clear();
		sortedSegmentArray = mesh2SegArray(splitedMesh, p, selc, sortedPointArray); //生成新的排序线段和顶点
	}
	if (showTriangulation)
		if (splitedMesh.size() > 0)
			drawTrianglesMesh(splitedMesh);
	else if (initialMesh.size() > 0)
		drawTrianglesMesh(initialMesh);

	if (gameStart && monsterSize > 0)
	{
		pPolarID.clear();
		pPolarValues.clear();
		pPolarOrder.clear();
		visPolygons.clear();
		getPolarOrder(0, sortedPointArray, pPolarID, pPolarValues, pPolarOrder);
		CPolygon cp = calcVisPolygon(0, sortedPointArray, sortedSegmentArray, pPolarID, pPolarValues, pPolarOrder);
		visPolygons.push_back(cp);
	}
	//calcVisPolygon();
	if (showVisPolygon)
	{
		int size = visPolygons.size();
		for (int i = 0; i < size; i++)
			drawVisPolygon(visPolygons[i]);
	}

	if (showDualGraph)
	{
		for (int i = 0; i < monsterSize; i++)
			drawDualGraph(monsters[i].pos, 0, 1, 1);
	}	

	if (showSortedSegment)
		drawSortedSegments(sortedPointArray, sortedSegmentArray);
}

void Rendering::preprocess()
{
	clearInitialMeshMemory(initialMesh);
	initialMesh = buildInitialMesh(*basePolygon);
	preprocessFinished = true;
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

void Rendering::drawVisPolygon(CPolygon& p)
{
	glColor3d(1, 1, 1);
	drawLoop(p, 0);
}

void Rendering::drawLoop(CPolygon& p, int loopID)
{
	int pointSize = p.loopArray[loopID].pointIDArray.size();
	int index;
	glLineWidth(1.0);
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
	glLineWidth(1.0);
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

bool Rendering::addPointIntoLoopBuf(Point& p)
{
	if (!basePolygon->pointInPolygonTest(p))
		return false;
	loopBuf.push_back(p);
	if (loopBuf.size() <= 1)
		return true;
	if (basePolygon->edgePolygonIntersectionTest(loopBuf[loopBuf.size() - 2], p))
	{
		loopBuf.pop_back();
		return false;
	}
	return true;
}

bool Rendering::loopFinished()
{
	/*if (drawOuterWall)
	{
		loopBuf[0] = Point(100, 500);
		loopBuf[1] = Point(300, 100);
		loopBuf[2] = Point(500, 500);
		loopBuf[3] = Point(300, 400);
	}
	else
	{
		loopBuf[0] = Point(200, 300);
		loopBuf[1] = Point(200, 200);
		loopBuf[2] = Point(300, 200);
		loopBuf[3] = Point(300, 300);
	}*/

	bool flag;
	if (loopBuf.size() < 3)
		flag = true;
	else if (basePolygon->edgePolygonIntersectionTest(loopBuf[loopBuf.size() - 1], loopBuf[0]))
		flag = false;
	else if (basePolygon->loopSelfIntersectionTest(loopBuf))
		flag = false;
	else
	{
		if (drawOuterWall)
			basePolygon->addOuterLoop(loopBuf);
		else if (drawInnerWall)
			basePolygon->addInnerLoop(loopBuf);
		flag = true;
	}	

	loopBuf.clear();
	return flag;
}

bool Rendering::addMonster(Point& p)
{
	if (basePolygon->loopArray.size() == 0)
		return false;

	if (!basePolygon->pointInPolygonTest(p))
		return false;

	Monster m(p);
	/*m.pos = Point(250, 150);
	m.viewDirection = m.walkDirection = PI / 4;*/
	monsters.push_back(m);

	return true;
}

void Rendering::drawMonsters(MonsterArray& ma)
{
	int pointSize = ma.size();
	glColor3d(0, 1, 1);
	for (int i = 0; i < pointSize; i++)
		drawPoint(ma[i].pos, 5);
}

void Rendering::drawPlayer(Point& p)
{
	glColor3d(1, 1, 0);
	drawPoint(player, 7);
}

void Rendering::playerWalk(int keyFlag)
{
	cout << keyFlag << endl;
	Point p = player;
	switch (keyFlag)
	{
	case 'A':
		p.x -= playerSpeed;
		break;
	case 'W':
		p.y += playerSpeed;
		break;
	case 'D':
		p.x += playerSpeed;
		break;
	case 'S':
		p.y -= playerSpeed;
		break;
	}

	if (basePolygon->pointInPolygonTest(p))
		player = p;
}

bool Rendering::playerMoveTo(Point& p)
{
	if (basePolygon->pointInPolygonTest(p))
	{
		player = p;
		return true;
	}
	else
		return false;
}

void Rendering::monsterWalk(int monsterID)
{
	Monster* mPtr;
	mPtr = &(monsters[monsterID]);
	double rd = randomDouble();
	double angleNew = mPtr->walkDirection - mPtr->range + 2 * mPtr->range * rd;
	double angle = angleNew * 0.3 + mPtr->walkDirection * 0.7;
	double turn;
	Vector direction(Monster::speed * cos(angle), Monster::speed * sin(angle));
	Point p = mPtr->pos + direction;
	if (basePolygon->pointInPolygonTest(p))
	{
		mPtr->pos = p;
		mPtr->walkDirection = angle;
		turn = angle - mPtr->viewDirection;
		if (abs(turn) > PI)
		{
			if (turn > 0)
				turn -= DOUBLE_PI;
			else
				turn += DOUBLE_PI;
		}
		mPtr->viewDirection += turn / 50;
	}
	else
	{
		double r;
		basePolygon->edgePolygonIntersectionNormal(mPtr->pos, p, r);
		r = 2 * r - angle;
		if (r > DOUBLE_PI)
			r -= DOUBLE_PI;
		else if (r < 0)
			r += DOUBLE_PI;
		mPtr->viewDirection = r;
		mPtr->walkDirection = r;

		/*p = mPtr->pos - direction;
		mPtr->pos = p;
		mPtr->viewDirection = angle + PI;
		mPtr->walkDirection = angle + PI;*/
	}
	if (mPtr->walkDirection > DOUBLE_PI)
		mPtr->walkDirection -= DOUBLE_PI;
	else if (mPtr->walkDirection < 0)
		mPtr->walkDirection += DOUBLE_PI;

	if (mPtr->viewDirection > DOUBLE_PI)
		mPtr->viewDirection -= DOUBLE_PI;
	else if (mPtr->viewDirection < 0)
		mPtr->viewDirection += DOUBLE_PI;
}

void Rendering::clear()
{
	basePolygon->clear();
	int visSize = visPolygons.size();
	for (int i = 0; i < visSize; i++)
		visPolygons[i].clear();
	loopBuf.clear();
	monsters.clear();
	clearInitialMeshMemory(initialMesh);
	sortedPointArray.clear();
	sortedSegmentArray.clear();
	splitedMesh.clear();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;
	preprocessFinished = false;
}

// 计算可见多边形
// pa：包含原多边形中的点及更新三角剖分中新加入的点
// sOrder：多边形中所有边所在线段，按距离查询点的偏序关系排放
// pPolarID：指示pa中所有点在极角排序中的次序，与pa一一对应
// pPolarVal：指示pa中所有点的极角值，与pa一一对应
// pPolarOrder：存储一组关于pa中点的索引，按pa中点的极角序排放
CPolygon Rendering::calcVisPolygon(int monsterID, PointArray& pa, SegmentArray& sOrder, IntArray& pPolarID, DoubleArray& pPolarVal, IntArray& pPolarOrder)
{
	int zeroNum = 0;							//极角为0的点的数量
	int pointSize = pa.size();
	int maxPolarID = 0;
	for (int i = 0; i < pointSize; i++)
	{
		if (pPolarID[i] > maxPolarID)
			maxPolarID = pPolarID[i];
	}		

	int xSize = maxPolarID + 1;		//x方向所需的区间数量
	int segmentSize = sOrder.size();
	int left, right;
	int x;
	int tmp;
	bool reverseFlag;
	DisjointSet ds(xSize + 1);
	IntArray vis;								//指示每个x向上可见的线段ID
	vis.insert(vis.begin(), xSize, -1);	

	for (int i = 0; i < segmentSize; i++)
	{
		reverseFlag = false;

		left = pPolarID[sOrder[i].aID];		//线段左端点对应的x值
		right = pPolarID[sOrder[i].bID];		//线段右端点对应的x值
		if (left > right)
		{
			if (right != 0)
				reverseFlag = true;
			else
			{
				if (pPolarVal[sOrder[i].aID] < PI)
					reverseFlag = true;
			}					
		}
		else
		{
			if (left == 0 && pPolarVal[sOrder[i].bID] > PI)
				reverseFlag = true;
		}
		if (reverseFlag)
		{
			tmp = sOrder[i].aID;
			sOrder[i].aID = sOrder[i].bID;
			sOrder[i].bID = tmp;
			tmp = left;
			left = right;
			right = tmp;
		}

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

	double rangeMin = monsters[monsterID].viewDirection - monsters[monsterID].range - HALF_PI;
	double rangeMax = monsters[monsterID].viewDirection + monsters[monsterID].range - HALF_PI;
	if (rangeMin < 0)
		rangeMin += DOUBLE_PI;
	if (rangeMax < 0)
		rangeMax += DOUBLE_PI;
	else if (rangeMax > DOUBLE_PI)
		rangeMax -= DOUBLE_PI;

	int rangeLeft = zeroNum - 1;			//可视范围的左边界所在的x区间
	int rangeRight = zeroNum - 1;		//可视范围的右边界所在的x区间
	int rangeLeftY;			//可视范围的左边界所在区间的y值
	int rangeRightY;			//可视范围的右边界所在区间的y值
	int rangeLengthX;			//可视范围的区间在x上的长度，不包含两个端点
	int pID;
	for (int i = 0; i < pointSize; i++)
	{
		pID = pPolarOrder[i];
		if (pPolarVal[pID] > rangeMin)
			break;
		rangeLeft = pPolarID[pID];
	}
	for (int i = 0; i < pointSize; i++)
	{
		pID = pPolarOrder[i];
		if (pPolarVal[pID] > rangeMax)
			break;
		rangeRight = pPolarID[pID];
	}
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
	pointBuf.push_back(monsters[monsterID].pos);

	//计算可视范围左边界与线段集的交点
	calcLineLineIntersection(pLeft, monsters[monsterID].pos, rangeMin + HALF_PI, pa[sOrder[rangeLeftY].aID], pa[sOrder[rangeLeftY].bID]);
	pointBuf.push_back(pLeft);
	preY = rangeLeftY;

	xFlag = rangeLeft + 1;
	for (int i = 0; i < rangeLengthX; i++)
	{
		if (xFlag >= xSize)
			xFlag = 0;

		curY = vis[xFlag];
		if (curY == -1)
			curY = preY;
		else
		{
			//由y值更大的可视线段切换到更小的
			if (curY > preY)
			{
				polar = pPolarVal[sOrder[preY].bID];
				calcLineLineIntersection(pMid, monsters[monsterID].pos, polar + HALF_PI, pa[sOrder[curY].aID], pa[sOrder[curY].bID]);
				pointBuf.push_back(pa[sOrder[preY].bID]);
				pointBuf.push_back(pMid);
			}
			//由y值更小的可视线段切换到更大的
			else if (curY < preY)
			{
				polar = pPolarVal[sOrder[curY].aID];
				calcLineLineIntersection(pMid, monsters[monsterID].pos, polar + HALF_PI, pa[sOrder[preY].aID], pa[sOrder[preY].bID]);
				pointBuf.push_back(pMid);
				pointBuf.push_back(pa[sOrder[curY].aID]);
			}
		}		

		preY = curY;
		xFlag++;
	}

	//计算可视范围右边界与线段集的交点
	calcLineLineIntersection(pRight, monsters[monsterID].pos, rangeMax + HALF_PI, pa[sOrder[rangeRightY].aID], pa[sOrder[rangeRightY].bID]);
	pointBuf.push_back(pRight);

	cp.addLoop(pointBuf);
	return cp;
}

bool Rendering::calcLineLineIntersection(Point& result, Point& a1, double polar, Point& b1, Point& b2)
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

void Rendering::calcVisPolygon()
{
	int monsterSize = monsters.size();
	vector<Monster>::iterator it = monsters.begin();
	for (int i = 0; i < monsterSize; i++)
	{
		visPolygons[i].clear();
		PointArray pa;
		Point p = it->pos;
		Vector v1(Monster::viewDistance * cos(it->viewDirection - it->range), Monster::viewDistance * sin(it->viewDirection - it->range));
		Vector v2(Monster::viewDistance * cos(it->viewDirection + it->range), Monster::viewDistance * sin(it->viewDirection + it->range));
		pa.push_back(p);
		pa.push_back(p + v1);
		pa.push_back(p + v2);
		visPolygons[i].addLoop(pa);
		
		it++;
	}
}

void Rendering::getPolarOrder(int monsterID, PointArray& pa, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder)
{
	int pointSize = pa.size();
	PolarArray polar;
	Polar p;
	double angle;
	int zeroNum = 0;
	for (int i = 0; i < pointSize; i++)
	{
		angle = calPolar(monsters[monsterID].pos, pa[i]);
		angle -= HALF_PI;
		if (angle < 0)
			angle += DOUBLE_PI;
		/*if (angle == 0)
			zeroNum++;*/

		pPolarID.push_back(-1);
		pPolarValues.push_back(angle);

		p.id = i;
		p.value = angle;
		polar.push_back(p);
	}

	sort(polar.begin(), polar.end(), polarSortLess);

	int index = 0;
	pPolarID[polar[0].id] = 0;
	pPolarOrder.push_back(polar[0].id);
	for (int i = 1; i < pointSize; i++)
	{
		if (polar[i].value != polar[i - 1].value)
			index++;
		pPolarID[polar[i].id] = index;
		pPolarOrder.push_back(polar[i].id);
	}	

	/*zeroNum--;
	for (int i = 0; i < pointSize; i++)
	{
		pPolarID[i] -= zeroNum;
		if (pPolarID[i] < 0)
			pPolarID[i] = 0;
	}*/
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

	calcVisPolygon(0, pa, sa, polarID, pPolarVal, pPolarOrder);
}

void Rendering::drawTrianglesMesh(const std::vector<p2t::Triangle*> &mesh)
{
	glLineWidth(1.0);
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

		glPushAttrib(GL_ENABLE_BIT);
		glLineStipple(1, 0xF0F0);
		glEnable(GL_LINE_STIPPLE);
		glBegin(GL_LINE_LOOP);
		glColor3d(0.5, 0.5, 0.5);
		glVertex2d(p1->x, p1->y);
		glVertex2d(p2->x, p2->y);
		glVertex2d(p3->x, p3->y);
		glEnd();
		glPopAttrib();
	}

	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		const p2t::Point *p1 = (*it)->GetPoint(0);
		const p2t::Point *p2 = (*it)->GetPoint(1);
		const p2t::Point *p3 = (*it)->GetPoint(2);

		char edgeLable[10];
		glColor3d(0.8, 0.3, 0.0);

		if (showMeshEdgeLabels)
		{
			if ((*it)->polygon_edge[2])
				sprintf(edgeLable, "[%d]", (*it)->edges[2]);
			else
				sprintf(edgeLable, "%d", (*it)->edges[2]);
			glRasterPos2d((p1->x + p2->x) / 2.0, (p1->y + p2->y) / 2.0);
			for (int i = 0; i < strlen(edgeLable); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);

			if ((*it)->polygon_edge[0])
				sprintf(edgeLable, "[%d]", (*it)->edges[0]);
			else
				sprintf(edgeLable, "%d", (*it)->edges[0]);
			glRasterPos2d((p2->x + p3->x) / 2.0, (p2->y + p3->y) / 2.0);
			for (int i = 0; i < strlen(edgeLable); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);

			if ((*it)->polygon_edge[1])
				sprintf(edgeLable, "[%d]", (*it)->edges[1]);
			else
				sprintf(edgeLable, "%d", (*it)->edges[1]);
			glRasterPos2d((p1->x + p3->x) / 2.0, (p1->y + p3->y) / 2.0);
			for (int i = 0; i < strlen(edgeLable); i++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);
		}
		else
		{
			if ((*it)->polygon_edge[2])
			{
				sprintf(edgeLable, "[%d]", (*it)->edges[2]);
				glRasterPos2d((p1->x + p2->x) / 2.0, (p1->y + p2->y) / 2.0);
				for (int i = 0; i < strlen(edgeLable); i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);
			}

			if ((*it)->polygon_edge[0])
			{
				sprintf(edgeLable, "[%d]", (*it)->edges[0]);
				glRasterPos2d((p2->x + p3->x) / 2.0, (p2->y + p3->y) / 2.0);
				for (int i = 0; i < strlen(edgeLable); i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);
			}

			if ((*it)->polygon_edge[1])
			{
				sprintf(edgeLable, "[%d]", (*it)->edges[1]);
				glRasterPos2d((p1->x + p3->x) / 2.0, (p1->y + p3->y) / 2.0);
				for (int i = 0; i < strlen(edgeLable); i++)
					glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, edgeLable[i]);
			}
		}
	}
}

void Rendering::drawSortedSegments(const PointArray &pa, const SegmentArray &sOrder)
{
	glLineWidth(3.0);
	for (int i = 0; i < sOrder.size(); i++)
	{
		Point pointA = pa[sOrder[i].aID];
		Point pointB = pa[sOrder[i].bID];

		glColor3d(1 - i / (double)sOrder.size(),
			1 - i / (double)sOrder.size(),
			1 - i / (double)sOrder.size());
		glBegin(GL_LINES);
		glVertex2d(pointA.x, pointA.y);
		glVertex2d(pointB.x, pointB.y);
		glEnd();
	}
}

void Rendering::drawDualGraphBackground()
{
	int width = 740;
	int height = 620;

	glLineWidth(1.0);
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(10, 10);
	glVertex2d(10 + width / 5.0, 10);
	glVertex2d(10 + width / 5.0, 10 + height / 5.0);
	glVertex2d(10, 10 + height / 5.0);
	glEnd();

	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2d(10, 10);
	glVertex2d(10 + width / 5.0, 10);
	glVertex2d(10 + width / 5.0, 10 + height / 5.0);
	glVertex2d(10, 10 + height / 5);
	glEnd();
}

void Rendering::drawDualGraph(const PointArray &pa, double rc, double gc, double bc)
{
	int width = 740;
	int height = 620;

	for (int i = 0; i < pa.size(); i++)
	{
		double a = pa[i].x - width / 2.0;
		double b = pa[i].y - height / 2.0;;

		a /= 200;
		Point cand[4];
		cand[0] = Point(-width / 2.0, a * width / 2.0 + b);
		cand[1] = Point((b + height / 2.0) / a, -height / 2.0);
		cand[2] = Point(width / 2.0, -a * width / 2.0 + b);
		cand[3] = Point((b - height / 2.0) / a, height / 2.0);

		glColor3d(rc, gc, bc);
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 4; j++)
		if (cand[j].x <= width / 2.0 && cand[j].x >= -width / 2.0 &&
			cand[j].y <= height / 2.0 && cand[j].y >= -height / 2.0)
			glVertex2d((cand[j].x + width / 2.0) / 5.0 + 10, (cand[j].y + height / 2.0) / 5.0 + 10);
		glEnd();
	}
}

void Rendering::drawDualGraph(const Point &p, double rc, double gc, double bc)
{
	int width = 740;
	int height = 620;

	double a = p.x - width / 2.0;
	double b = p.y - height / 2.0;;

	a /= 200;
	Point cand[4];
	cand[0] = Point(-width / 2.0, a * width / 2.0 + b);
	cand[1] = Point((b + height / 2.0) / a, -height / 2.0);
	cand[2] = Point(width / 2.0, -a * width / 2.0 + b);
	cand[3] = Point((b - height / 2.0) / a, height / 2.0);

	glColor3d(rc, gc, bc);
	glBegin(GL_LINE_STRIP);
	for (int j = 0; j < 4; j++)
	if (cand[j].x <= width / 2.0 && cand[j].x >= -width / 2.0 &&
		cand[j].y <= height / 2.0 && cand[j].y >= -height / 2.0)
		glVertex2d((cand[j].x + width / 2.0) / 5.0 + 10, (cand[j].y + height / 2.0) / 5.0 + 10);
	glEnd();
}