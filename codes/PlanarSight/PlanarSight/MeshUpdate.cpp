#include "stdafx.h"
#include "MeshUpdate.h"

std::vector<p2t::Triangle*> insertPointToUpdateTriangles(std::vector<p2t::Triangle*> &mesh, const p2t::Point &p)
{
	std::vector<p2t::Triangle*> splitedMesh;
	int ind = findPointInTriangles(mesh, p);
	if (ind != -1)
	{
		p2t::Triangle *next_tri = mesh[ind];
		p2t::Point next_p = p;
		while (next_tri != NULL)
		{
			next_tri->mark_to_be_splited = true;

			p2t::Point *p1 = next_tri->GetPoint(0);
			p2t::Point *p2 = next_tri->GetPoint(1);
			p2t::Point *p3 = next_tri->GetPoint(2);

			p2t::Point *p1p2 = new p2t::Point(-1, -1);
			p2t::Point *p2p3 = new p2t::Point(-1, -1);
			p2t::Point *p3p1 = new p2t::Point(-1, -1);
			rayIntersectTriangle(*next_tri, p, p1p2, p2p3, p3p1);

			p2t::Point nextPointCandidate;
			p2t::Triangle *nextTriangleCandidate;
			p2t::Triangle *t[4];
			if (p1p2->x != -1 && p1p2->y != -1)
			{
				t[0] = new p2t::Triangle(*p1, *p1p2, next_p);
				t[1] = new p2t::Triangle(*p1p2, *p2, next_p);
				t[2] = new p2t::Triangle(*p2, *p3, next_p);
				t[3] = new p2t::Triangle(*p3, *p1, next_p);

				delete p2p3;
				delete p3p1;
				nextPointCandidate = *p1p2;
				nextTriangleCandidate = next_tri->neighbors_[2];
			}
			else if (p2p3->x != -1 && p2p3->y != -1)
			{
				t[0] = new p2t::Triangle(*p2, *p2p3, next_p);
				t[1] = new p2t::Triangle(*p2p3, *p3, next_p);
				t[2] = new p2t::Triangle(*p3, *p1, next_p);
				t[3] = new p2t::Triangle(*p1, *p2, next_p);

				delete p1p2;
				delete p3p1;
				nextPointCandidate = *p2p3;
				nextTriangleCandidate = next_tri->neighbors_[0];
			}
			else if (p3p1->x != -1 && p3p1->y != -1)
			{
				t[0] = new p2t::Triangle(*p3, *p3p1, next_p);
				t[1] = new p2t::Triangle(*p3p1, *p1, next_p);
				t[2] = new p2t::Triangle(*p1, *p2, next_p);
				t[3] = new p2t::Triangle(*p2, *p3, next_p);

				delete p1p2;
				delete p2p3;
				nextPointCandidate = *p3p1;
				nextTriangleCandidate = next_tri->neighbors_[1];
			}
			else
			{
				t[0] = new p2t::Triangle(*p1, *p2, next_p);
				t[1] = new p2t::Triangle(*p2, *p3, next_p);
				t[2] = new p2t::Triangle(*p3, *p1, next_p);
				t[4] = NULL;

				delete p1p2;
				delete p2p3;
				delete p3p1;
				nextPointCandidate = p;
				int edge = findEdgePointStands(*next_tri, next_p);
				nextTriangleCandidate = next_tri->neighbors_[edge];
			}
			for (int i = 0; i < 4; i++)
			{
				if (t[i] != NULL)
				{
					p2t::Point *p1 = t[i]->GetPoint(0);
					p2t::Point *p2 = t[i]->GetPoint(1);
					p2t::Point *p3 = t[i]->GetPoint(2);

					if ((p1->x == p2->x && p1->y == p2->y) ||
						(p2->x == p3->x && p2->y == p3->y) ||
						(p1->x == p3->x && p1->y == p3->y))
						delete t[i];
					else
						mesh.push_back(t[i]);
				}
			}
			next_tri = nextTriangleCandidate;
			next_p = nextPointCandidate;
		}
	}

	std::vector<p2t::Triangle*>::iterator it;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		if (!(*it)->mark_to_be_splited)
			splitedMesh.push_back(*it);
		else
			(*it)->mark_to_be_splited = false;
	}
	return splitedMesh;
}

int findEdgePointStands(p2t::Triangle &tri, const p2t::Point &p)
{
	const p2t::Point *p1 = tri.GetPoint(0);
	const p2t::Point *p2 = tri.GetPoint(1);
	const p2t::Point *p3 = tri.GetPoint(2);

	if (p1->x == p2->x && p1->x == p.x)
		return 2;
	if (p2->x == p3->x && p2->x == p.x)
		return 0;
	if (p3->x == p1->x && p1->x == p.x)
		return 1;
	if ((p1->x <= p.x && p2->x > p.x) || (p2->x < p.x && p1->x >= p.x))
	{
		double ny = p1->y + (p2->y - p1->y) * ((p.x - p1->x) / (p2->x - p1->x));
		if (((p1->y <= ny && p2->y > ny) || (p2->y < ny && p1->y >= ny)) && ny == p.y)
			return 2;
	}
	if ((p2->x <= p.x && p3->x > p.x) || (p3->x < p.x && p2->x >= p.x))
	{
		double ny = p2->y + (p3->y - p2->y) * ((p.x - p2->x) / (p3->x - p2->x));
		if (((p2->y <= ny && p3->y > ny) || (p3->y < ny && p2->y >= ny)) && ny == p.y)
			return 0;
	}
	if ((p3->x <= p.x && p1->x > p.x) || (p1->x < p.x && p3->x >= p.x))
	{
		double ny = p3->y + (p1->y - p3->y) * ((p.x - p3->x) / (p1->x - p3->x));
		if (((p3->y <= ny && p1->y > ny) || (p1->y < ny && p3->y >= ny)) && ny == p.y)
			return 1;
	}
}

void rayIntersectTriangle(p2t::Triangle &tri, const p2t::Point &p, p2t::Point *p1p2, p2t::Point *p2p3, p2t::Point *p3p1)
{
	const p2t::Point *p1 = tri.GetPoint(0);
	const p2t::Point *p2 = tri.GetPoint(1);
	const p2t::Point *p3 = tri.GetPoint(2);

	if (p1->x == p2->x && p1->x == p.x)
	{
		p1p2->x = p.x;
		p1p2->y = p1->y > p2->y ? p1->y : p2->y;
		return;
	}
	if (p2->x == p3->x && p2->x == p.x)
	{
		p2p3->x = p.x;
		p2p3->y = p2->y > p3->y ? p2->y : p3->y;
		return;
	}
	if (p3->x == p1->x && p1->x == p.x)
	{
		p3p1->x = p.x;
		p3p1->y = p3->y > p1->y ? p3->y : p1->y;
		return;
	}
	if ((p1->x <= p.x && p2->x > p.x) || (p2->x < p.x && p1->x >= p.x))
	{
		double ny = p1->y + (p2->y - p1->y) * ((p.x - p1->x) / (p2->x - p1->x));
		if (((p1->y <= ny && p2->y > ny) || (p2->y < ny && p1->y >= ny)) && ny > p.y)
		{
			p1p2->x = p.x;
			p1p2->y = ny;
			return;
		}
	}
	if ((p2->x <= p.x && p3->x > p.x) || (p3->x < p.x && p2->x >= p.x))
	{
		double ny = p2->y + (p3->y - p2->y) * ((p.x - p2->x) / (p3->x - p2->x));
		if (((p2->y <= ny && p3->y > ny) || (p3->y < ny && p2->y >= ny)) && ny > p.y)
		{
			p2p3->x = p.x;
			p2p3->y = ny;
			return;
		}
	}
	if ((p3->x <= p.x && p1->x > p.x) || (p1->x < p.x && p3->x >= p.x))
	{
		double ny = p3->y + (p1->y - p3->y) * ((p.x - p3->x) / (p1->x - p3->x));
		if (((p3->y <= ny && p1->y > ny) || (p1->y < ny && p3->y >= ny)) && ny > p.y)
		{
			p3p1->x = p.x;
			p3p1->y = ny;
			return;
		}
	}
}

void rebuildTrianglesRelationship(std::vector<p2t::Triangle*> &mesh)
{
	std::vector<p2t::Triangle*>::iterator it;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		const p2t::Point *p1 = (*it)->GetPoint(0);
		const p2t::Point *p2 = (*it)->GetPoint(1);
		const p2t::Point *p3 = (*it)->GetPoint(2);

		(*it)->neighbors_[0] = NULL;
		(*it)->neighbors_[1] = NULL;
		(*it)->neighbors_[2] = NULL;
		std::vector<p2t::Triangle*>::const_iterator it2;
		for (it2 = mesh.begin(); it2 != mesh.end(); ++it2)
		{
			const p2t::Point *k1 = (*it2)->GetPoint(0);
			const p2t::Point *k2 = (*it2)->GetPoint(1);
			const p2t::Point *k3 = (*it2)->GetPoint(2);

			if ((p1 != k1 && ((p2 == k2 && p3 == k3) || (p2 == k3 && p3 == k2))) ||
				(p1 != k2 && ((p2 == k1 && p3 == k3) || (p2 == k3 && p3 == k1))) ||
				(p1 != k3 && ((p2 == k1 && p3 == k2) || (p2 == k2 && p3 == k1))))
				(*it)->neighbors_[0] = *it2;
			if ((p2 != k1 && ((p1 == k2 && p3 == k3) || (p1 == k3 && p3 == k2))) ||
				(p2 != k2 && ((p1 == k1 && p3 == k3) || (p1 == k3 && p3 == k1))) ||
				(p2 != k3 && ((p1 == k1 && p3 == k2) || (p1 == k2 && p3 == k1))))
				(*it)->neighbors_[1] = *it2;
			if ((p3 != k1 && ((p1 == k2 && p2 == k3) || (p1 == k3 && p2 == k2))) ||
				(p3 != k2 && ((p1 == k1 && p2 == k3) || (p1 == k3 && p2 == k1))) ||
				(p3 != k3 && ((p1 == k1 && p2 == k2) || (p1 == k2 && p2 == k1))))
				(*it)->neighbors_[2] = *it2;
		}
	}
}

int findPointInTriangles(const std::vector<p2t::Triangle*> &mesh, const p2t::Point &p)
{
	std::vector<p2t::Triangle*>::const_iterator it;
	int ind = 0;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		const p2t::Point *p1 = (*it)->GetPoint(0);
		const p2t::Point *p2 = (*it)->GetPoint(1);
		const p2t::Point *p3 = (*it)->GetPoint(2);

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