#include "stdafx.h"
#include "Rendering.h"
#include "Mesh2Graph.h"

extern int wood_tex_id;
extern int wood2_tex_id;
extern int floor_tex_id;
extern int visp_tex_id;

float total_tm[4] = {0, 0, 0, 0};
int process_count = 0;

Rendering::Rendering()
{
	basePolygon = new CPolygon();
	drawOuterWall = false;
	drawInnerWall = false;
	drawMonster = false;

	showVisPolygon = false;
	showTriangulation = false;
	showMeshEdgeLabels = false;
	showSortedSegment = false;
	showDualGraph = false;
	show3DView = false;
	showLinearSet = false;
	useDCELSort = false;

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
	//数据处理与准备
	process();

	//绘制与渲染
	if (showTriangulation)
		if (splitedMesh.size() > 0)
			drawTrianglesMesh(splitedMesh);
	else if (initialMesh.size() > 0)
		drawTrianglesMesh(initialMesh);

	if (show3DView)
	{
		glEnable(GL_DEPTH_TEST);
		drawFloor();
		drawPolygon3D(*basePolygon);
	}
	else
		drawPolygon(*basePolygon);

	if (drawOuterWall || drawInnerWall)
		drawUnfinishedLoop(loopBuf);

	int monsterSize = monsters.size();
	if (gameStart)
		for (int i = 0; i < monsterSize; i++)
			monsterWalk(i);

	if (showVisPolygon)
	{
		int size = visPolygons.size();
		for (int i = 0; i < size; i++)
		{
			if (show3DView)
				drawVisPolygon3D(visPolygons[i], i / (double)size);
			else
				drawVisPolygon(visPolygons[i]);
		}
	}

	if (show3DView)
	{
		drawMonsters3D(monsters);
		drawPlayer3D(player);
	}
	else
	{
		drawMonsters(monsters);
		drawPlayer(player);
	}	

	if (showSortedSegment)
		drawSortedSegments(sortedPointArray, sortedSegmentArray);

	if (showDualGraph)
	{
		change2D();
		drawDualGraphBackground();
		if (drawOuterWall || drawInnerWall)
			drawDualGraph(loopBuf, 1, 1, 0);
		drawDualGraph(basePolygon->pointArray, 0, 0, 1);
		for (int i = 0; i < monsterSize; i++)
			drawDualGraph(monsters[i].pos, 0, 1, 1);
		exit2D();
	}

	if (showLinearSet)
	{
		change2D();
		drawLinearSetBackground();
		drawLinearSet();
		exit2D();
	}
}

void Rendering::process()
{
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;

	int monsterSize = monsters.size();
	int monID;
	if (gameStart && monsterSize > 0)
	{
		visPolygons.clear();
		for (int i = 0; i < monsterSize; i++)
		{
			monID = i;
			//清空上次的新剖分出来的三角形信息
			clearSplitedMeshMemory();
			p2t::Point p(monsters[monID].pos.x, monsters[monID].pos.y);
			int selc = 1; //表示新剖分出来的边数

			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&BeginTime);
			splitedMesh = insertPointToUpdateTriangles(initialMesh, p, &selc); //生成新的三角剖分网格
			QueryPerformanceCounter(&EndTime);
			float tm1 = (float)(EndTime.QuadPart - BeginTime.QuadPart) / Frequency.QuadPart;

			//清空上次的排序线段和顶点信息
			sortedPointArray.clear();
			sortedSegmentArray.clear();
			PointArray newPointArray;

			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&BeginTime);
			sortedSegmentArray = mesh2SegArray(splitedMesh, p, selc, basePolygon->pointArray.size(), newPointArray); //生成新的排序线段和顶点
			QueryPerformanceCounter(&EndTime);
			float tm2 = (float)(EndTime.QuadPart - BeginTime.QuadPart) / Frequency.QuadPart;

			pPolarID.clear();
			pPolarValues.clear();
			pPolarOrder.clear();
            
			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&BeginTime);
			if (useDCELSort)
				getPolarOrderByDCEL(monID,
				basePolygon->pointArray, newPointArray, sortedPointArray,
				pPolarID, pPolarValues, pPolarOrder);
			else
				getPolarOrder(monID,
				basePolygon->pointArray, newPointArray, sortedPointArray,
				pPolarID, pPolarValues, pPolarOrder);
			QueryPerformanceCounter(&EndTime);
			float tm3 = (float)(EndTime.QuadPart - BeginTime.QuadPart) / Frequency.QuadPart;

			QueryPerformanceFrequency(&Frequency);
			QueryPerformanceCounter(&BeginTime);
			CPolygon cp = calcVisPolygon(monID, sortedPointArray, sortedSegmentArray, pPolarID, pPolarValues, pPolarOrder);
			QueryPerformanceCounter(&EndTime);
			float tm4 = (float)(EndTime.QuadPart - BeginTime.QuadPart) / Frequency.QuadPart;

			visPolygons.push_back(cp);

			total_tm[0] += tm1; 
			total_tm[1] += tm2;
			total_tm[2] += tm3;
			total_tm[3] += tm4;
			process_count++;

			/*printf("MU: %.2f\tTS: %.2f\tAS: %.2f\tLS: %.2f\n",
				(total_tm[0] / process_count) * 1000,
				(total_tm[1] / process_count) * 1000,
				(total_tm[2] / process_count) * 1000,
				(total_tm[3] / process_count) * 1000);*/
		}	
	}
}

void Rendering::preprocess()
{
	clearInitialMeshMemory(initialMesh);
	initialMesh = buildInitialMesh(*basePolygon);
    
    dcel = new DCEL();
    //dcel->initialize(basePolygon->pointArray);

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

void Rendering::drawPolygon3D(CPolygon& p)
{
	int loopSize = p.loopArray.size();
	if (loopSize == 0)
		return;

	drawLoop3D(p, 0, wood_tex_id);

	for (int i = 1; i < loopSize; i++)
		drawLoop3D(p, i, wood2_tex_id);
}

void Rendering::drawVisPolygon(CPolygon& p)
{
	glColor4d(0.4, 0.4, 0.5, 0.6);
	int pointSize = p.loopArray[0].pointIDArray.size();
	int index;
	glBegin(GL_POLYGON);
	for (int i = 0; i < pointSize; i++)
	{
		index = p.loopArray[0].pointIDArray[i];
		glVertex2d(p.pointArray[index].x, p.pointArray[index].y);
	}
	glEnd();
}

void Rendering::drawVisPolygon3D(CPolygon& p, double offset)
{
	int pointSize = p.loopArray[0].pointIDArray.size();
	int index;

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, visp_tex_id);
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	for (int i = 0; i < pointSize; i++)
	{
		index = p.loopArray[0].pointIDArray[i];
		glVertex3d(p.pointArray[index].x, p.pointArray[index].y, 10 + offset);
		glTexCoord2d(0.5, 0.5);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
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

void Rendering::drawLoop3D(CPolygon &p, int loopID, int tex_id)
{
	int pointSize = p.loopArray[loopID].pointIDArray.size();
	int index, next;
	double height = 30;
	double wall_thickness = 2.0;

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_id);
	for (int i = 0; i < pointSize; i++)
	{
		index = p.loopArray[loopID].pointIDArray[i];
		next = p.loopArray[loopID].pointIDArray[(i + 1) % pointSize];
		Point normal = p.pointArray[next] - p.pointArray[index];
		double temp = normal.x;
		normal.x = -normal.y;
		normal.y = temp;
		normal.normalize();

		Point wa = normal;
		wa = wa * wall_thickness;

		glBegin(GL_POLYGON);
		glNormal3d(normal.x, normal.y, 0);
		glVertex3d(p.pointArray[index].x, p.pointArray[index].y, height);
		glTexCoord2d(1, 0);
		glVertex3d(p.pointArray[index].x, p.pointArray[index].y, 0);
		glTexCoord2d(0, 0);
		glVertex3d(p.pointArray[next].x, p.pointArray[next].y, 0);
		glTexCoord2d(0, 1);
		glVertex3d(p.pointArray[next].x, p.pointArray[next].y, height);
		glTexCoord2d(1, 1);
		glEnd();

		glBegin(GL_POLYGON);
		glNormal3d(normal.x, normal.y, 0);
		glVertex3d(p.pointArray[index].x + wa.x, p.pointArray[index].y + wa.y, height);
		glTexCoord2d(1, 0);
		glVertex3d(p.pointArray[index].x + wa.x, p.pointArray[index].y + wa.y, 0);
		glTexCoord2d(0, 0);
		glVertex3d(p.pointArray[next].x + wa.x, p.pointArray[next].y + wa.y, 0);
		glTexCoord2d(0, 1);
		glVertex3d(p.pointArray[next].x + wa.x, p.pointArray[next].y + wa.y, height);
		glTexCoord2d(1, 1);
		glEnd();

		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glVertex3d(p.pointArray[index].x, p.pointArray[index].y, height);
		glVertex3d(p.pointArray[index].x + wa.x, p.pointArray[index].y + wa.y, height);
		glVertex3d(p.pointArray[next].x + wa.x, p.pointArray[next].y + wa.y, height);
		glVertex3d(p.pointArray[next].x, p.pointArray[next].y, height);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
}

void Rendering::drawFloor()
{
	int size = 200;
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, floor_tex_id);
	for (int i = -5; i < 5; i++)
	for (int j = -5; j < 5; j++)
	{
		glBegin(GL_POLYGON);
		glNormal3d(0, 0, 1);
		glVertex3d(i * size, j * size, 1);
		glTexCoord2d(1, 0);
		glVertex3d(i * size, (j + 1) * size, 1);
		glTexCoord2d(0, 0);
		glVertex3d((i + 1) * size, (j + 1) * size, 1);
		glTexCoord2d(0, 1);
		glVertex3d((i + 1) * size, j * size, 1);
		glTexCoord2d(1, 1);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);
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

void Rendering::drawPoint3D(Point& p, double size)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(p.x, p.y, 0);
	glutSolidSphere(size, 10, 10);
	glPopMatrix();
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

void Rendering::drawMonsters3D(MonsterArray& ma)
{
	int pointSize = ma.size();
	glColor3d(0, 1, 1);
	for (int i = 0; i < pointSize; i++)
		drawPoint3D(ma[i].pos, 5);
}

void Rendering::drawPlayer(Point& p)
{
	glColor3d(1, 1, 0);
	drawPoint(player, 7);
}

void Rendering::drawPlayer3D(Point& p)
{
	glColor3d(1, 1, 0);
	drawPoint3D(player, 7);
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
	visPolygons.clear();
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
    dcel->deleteAll();
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

	int rangeLeft = 0;			//可视范围的左边界所在的x区间
	int rangeRight = 0;		//可视范围的右边界所在的x区间
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

	if (monsterID == monsters.size() - 1)
	{
		xLeft = rangeLeft;
		xRight = rangeRight;

		if (pa[sOrder[rangeLeftY].bID].x - pa[sOrder[rangeLeftY].aID].x != 0)
			xLeftParam = (pLeft.x - pa[sOrder[rangeLeftY].aID].x) / (pa[sOrder[rangeLeftY].bID].x - pa[sOrder[rangeLeftY].aID].x);
		else
			xLeftParam = (pLeft.y - pa[sOrder[rangeLeftY].aID].y) / (pa[sOrder[rangeLeftY].bID].y - pa[sOrder[rangeLeftY].aID].y);
		if (pa[sOrder[rangeRightY].bID].x - pa[sOrder[rangeRightY].aID].x != 0)
			xRightParam = (pRight.x - pa[sOrder[rangeRightY].aID].x) / (pa[sOrder[rangeRightY].bID].x - pa[sOrder[rangeRightY].aID].x);
		else
			xRightParam = (pRight.y - pa[sOrder[rangeRightY].aID].y) / (pa[sOrder[rangeRightY].bID].y - pa[sOrder[rangeRightY].aID].y);

		int visSize = vis.size();
		visArray.clear();
		for (int i = 0; i < visSize; i++)
			visArray.push_back(vis[i]);
	}

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

void Rendering::getPolarOrder(int monsterID, PointArray& pa, PointArray& pb, PointArray& points, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder)
{
	points.insert(points.end(), pa.begin(), pa.end());
	points.insert(points.end(), pb.begin(), pb.end());
	int pointSize = points.size();
	PolarArray polar;
	Polar p;
	double angle;
	int zeroNum = 0;
	for (int i = 0; i < pointSize; i++)
	{
		angle = calPolar(monsters[monsterID].pos, points[i]);
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
}

void Rendering::getPolarOrderByDCEL(int monsterID, PointArray& pa, PointArray& pb, PointArray& points, IntArray& pPolarID, DoubleArray& pPolarValues, IntArray& pPolarOrder)
{
    Point pointMonster = monsters[monsterID].pos;
    Line line = Line(pointMonster.x, 1, -pointMonster.y);
    int paSize = pa.size();
    int pbSize = pb.size();
    int pointSize = paSize + pbSize;
    points.insert(points.end(), pa.begin(), pa.end());
    points.insert(points.end(), pb.begin(), pb.end());
    pPolarValues.resize(pointSize);

    for (int i = 0; i < paSize; i++)
    {
        double angle = calPolar(pointMonster, pa[i]);
        angle -= HALF_PI;
        if (angle < 0)
            angle += DOUBLE_PI;

        pPolarValues[i] = angle;
    }

    for (int i = 0; i < pbSize; i++)
    {
        double angle = eps / 100;
        pPolarValues[paSize + i] = angle;
    }

    IntArray alreadyLines;
    alreadyLines.resize(paSize + 10);
    int numAlreadyLines = 0;
    dcel->query(line, alreadyLines, numAlreadyLines);

    IntArray flagLines;
    flagLines.resize(paSize);
    for (int i = 0; i < paSize; i++)
        flagLines[i] = 0;

    IntArray leftLines, rightLines;
    leftLines.clear();
    rightLines.clear();

    for (int i = 0; i < pbSize; i++)
    {
        leftLines.push_back(paSize + i);
    }

    for (int i = 0; i < numAlreadyLines; i++)
    {
        int id = alreadyLines[i];
        flagLines[id] = 1;
        if (pPolarValues[id] <= PI)
        {
            leftLines.push_back(id);
        }
        else
        {
            rightLines.push_back(id);
        }
    }

    for (int i = 0; i < paSize; i++)
	{
        if (flagLines[i] == 1) continue;
        if (fabs(pPolarValues[i] - PI) < 1.0)
        {
            leftLines.push_back(i);
            flagLines[i] = 1;
        }
        else
        {
            rightLines.push_back(i);
            flagLines[i] = 1;
        }
    }
    
    pPolarOrder.clear();
    pPolarOrder.insert(pPolarOrder.end(), leftLines.begin(), leftLines.end());
    pPolarOrder.insert(pPolarOrder.end(), rightLines.begin(), rightLines.end());

    int index = 0;
    pPolarID.resize(pointSize);
    pPolarID[pPolarOrder[0]] = 0;
    for (int i = 1; i < pointSize; i++)
    {
        if (pPolarValues[pPolarOrder[i]] != pPolarValues[pPolarOrder[i - 1]])
            index++;
        pPolarID[pPolarOrder[i]] = index;
    }
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

void Rendering::drawLinearSetBackground()
{
	int width = 740;
	int height = 620;

	glLineWidth(1.0);
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex2d(730, 10);
	glVertex2d(730 - width / 5.0, 10);
	glVertex2d(730 - width / 5.0, 10 + height / 5.0);
	glVertex2d(730, 10 + height / 5.0);
	glEnd();

	glColor3d(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2d(730, 10);
	glVertex2d(730 - width / 5.0, 10);
	glVertex2d(730 - width / 5.0, 10 + height / 5.0);
	glVertex2d(730, 10 + height / 5.0);
	glEnd();
}

void Rendering::drawLinearSet()
{
	int width = 740;
	int height = 620;

	int left = 730 - width / 5 + 5;
	int right = 730 - 5;
	int bottom = 15;
	int top = 10 + height / 5.0 - 5;
	
	glLineWidth(1.0);
	glColor3d(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2d(left, top);
	glVertex2d(left, bottom);
	glVertex2d(right, bottom);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex2d(left, top + 4);
	glVertex2d(left - 3, top - 2);
	glVertex2d(left + 3, top - 2);
	glVertex2d(right + 4, bottom);
	glVertex2d(right - 2, bottom + 3);
	glVertex2d(right - 2, bottom - 3);
	glEnd();

	int yRange = sortedSegmentArray.size();
	if (yRange == 0)
		return;

	int pointSize = sortedPointArray.size();
	int xRange = 0;
	for (int i = 0; i < pointSize; i++)
	{
		if (pPolarID[i] > xRange)
			xRange = pPolarID[i];
	}
	int xScale = (right - left) / (xRange + 1);
	int yScale = (top - bottom) / (yRange + 1);
	int xLeftVal;
	int xRightVal;
	glColor3d(0.2, 0.8, 1);
	glBegin(GL_LINES);
	for (int i = 0; i < yRange; i++)
	{
		xLeftVal = pPolarID[sortedSegmentArray[i].aID];
		xRightVal = pPolarID[sortedSegmentArray[i].bID];
		if (xRightVal == 0)
			xRightVal = xRange + 1;
		glVertex2d(left + xLeftVal * xScale, bottom + 5 + i * yScale);
		glVertex2d(left + xRightVal * xScale, bottom + 5 + i * yScale);
	}
	glEnd();

	int yVal;
	double x, y;
	yVal = visArray[xLeft];
	glColor4d(0.8, 0.8, 0, 0.4);
	glBegin(GL_POLYGON);
	x = left + (xLeft + xLeftParam) * xScale;
	glVertex2d(x, bottom);
	y = bottom + 5 + yVal * yScale;
	glVertex2d(x, y);
	x = left + xLeft * xScale;
	int i = xLeft + 1;
	while (true)
	{
		if (i == xRight + 1)
			break;
		x += xScale;
		glVertex2d(x, y);
		glVertex2d(x, bottom);
		glEnd();

		if (i > xRange)
		{
			i = 0;
			x = left;
		}
		glBegin(GL_POLYGON);
		glVertex2d(x, bottom);
		yVal = visArray[i];
		y = bottom + 5 + yVal * yScale;
		glVertex2d(x, y);

		i++;
	}
	x = left + (xRight + xRightParam) * xScale;
	glVertex2d(x, y);
	glVertex2d(x, bottom);
	glEnd();
	
}

void Rendering::change2D()
{
	if (show3DView)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, 750, 0, 630, -50, 50);

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

		glDisable(GL_DEPTH_TEST);
	}
}

void Rendering::exit2D()
{
	if (show3DView)
	{
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);
	}
}