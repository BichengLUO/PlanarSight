#include "stdafx.h"
#include "MeshUpdate.h"

void insertPointToUpdateTriangles(std::vector<p2t::Triangle*> &mesh, const p2t::Point &p)
{
	int ind = findPointInTriangles(mesh, p);
	if (ind != -1)
	{
		p2t::Triangle *startTri = mesh[ind];
		startTri->mark_to_be_splited = true;

		
	}
}

int findPointInTriangles(const std::vector<p2t::Triangle*> &mesh, const p2t::Point &p)
{
	std::vector<p2t::Triangle*>::const_iterator it;
	int ind = 0;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		p2t::Point *p1 = (*it)->GetPoint(0);
		p2t::Point *p2 = (*it)->GetPoint(1);
		p2t::Point *p3 = (*it)->GetPoint(2);

		if (pointInTriangle(p, *p1, *p2, *p3))
			return ind;
		ind++;
	}
	return -1;
}

inline double toLeft(const p2t::Point &p1, const p2t::Point &p2, const p2t::Point &p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(const p2t::Point &pt, const p2t::Point &v1, const p2t::Point &v2, const p2t::Point &v3)
{
	bool b1, b2, b3;

	b1 = toLeft(pt, v1, v2) <= 0.0f;
	b2 = toLeft(pt, v2, v3) <= 0.0f;
	b3 = toLeft(pt, v3, v1) <= 0.0f;

	return ((b1 == b2) && (b2 == b3));
}