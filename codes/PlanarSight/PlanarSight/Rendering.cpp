#include "stdafx.h"
#include "Rendering.h"


Rendering::Rendering()
{
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