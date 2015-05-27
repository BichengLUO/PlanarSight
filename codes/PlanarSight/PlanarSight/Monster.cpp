#include "Monster.h"

double Monster::maxViewAngle = PI / 6;
double Monster::minViewAngle = PI / 8;
double Monster::speed = 1;
double Monster::viewDistance = 100;

Monster::Monster(Point p) : pos(p)
{
	double rd = randomDouble();
	direction = rd * DOUBLE_PI;
	rd = randomDouble();
	range = minViewAngle + rd * (maxViewAngle - minViewAngle);
}