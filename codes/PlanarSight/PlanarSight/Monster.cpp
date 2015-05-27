#include "Monster.h"

double Monster::maxViewAngle = PI / 6;
double Monster::minViewAngle = PI / 8;
double Monster::speed = 0.5;
double Monster::viewDistance = 100;

Monster::Monster(Point p) : pos(p)
{
	double rd = randomDouble();
	walkDirection = rd * DOUBLE_PI;
	viewDirection = walkDirection;
	rd = randomDouble();
	range = minViewAngle + rd * (maxViewAngle - minViewAngle);
	//range = PI / 20;
}