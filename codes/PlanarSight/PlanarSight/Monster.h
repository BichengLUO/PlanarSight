#pragma once

#include "Point.h"

class Monster
{
public:
	Point pos;
	double walkDirection;
	double viewDirection;
	double range;
	double speed;
	static double maxViewAngle;
	static double minViewAngle;
	static double viewDistance;
	bool hunt;
	bool visible;

public:
	Monster(Point p);
};
typedef vector<Monster> MonsterArray;

class Player
{
public:
	Point pos;
	double direction;
	double range;
	deque<Point> dBuf;

public:
	Player();

	void init(Point p);
	void calcDirection(Point p);
};