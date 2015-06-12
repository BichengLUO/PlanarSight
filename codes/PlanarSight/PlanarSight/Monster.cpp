#include "Monster.h"

double Monster::maxViewAngle = PI / 6;
double Monster::minViewAngle = PI / 8;
double Monster::viewDistance = 100;

Monster::Monster(Point p) : pos(p)
{
	double rd = randomDouble();
	walkDirection = rd * DOUBLE_PI;
	viewDirection = walkDirection;
	rd = randomDouble();
	range = minViewAngle + rd * (maxViewAngle - minViewAngle);
	speed = 0.8;
	hunt = false;
	visible = true;
	//range = PI / 20;
}

Player::Player()
{
	range = PI / 5;
	direction = 0;
}

void Player::init(Point p)
{
	pos = p;
	dBuf.push_back(p);
}

void Player::calcDirection(Point p)
{
	dBuf.push_back(p);
	if (dBuf.size() > 10)
		dBuf.pop_front();

	/*int size = dBuf.size();
	Point pt = Point(0, 0);
	for (int i = 0; i < size; i++)
		pt = pt + dBuf[i];
	pt = pt * (1.0 / size);*/

	double d = direction * 0.5 + calPolar(dBuf.front(), p) * 0.5;
	double newDirection = calPolar(dBuf.front(), p);
	if (abs(direction - newDirection) >= PI)
		d += PI;
	if (d > DOUBLE_PI)
		d -= DOUBLE_PI;

	direction = d;
}