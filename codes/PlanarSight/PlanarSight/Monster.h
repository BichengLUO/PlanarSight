#pragma once

#include "Point.h"

class Monster
{
public:
	Point pos;
	double direction;
	double range;
	static double maxViewAngle;
	static double minViewAngle;
	static double speed;
	static double viewDistance;

public:
	Monster(Point p);
};
typedef vector<Monster> MonsterArray;