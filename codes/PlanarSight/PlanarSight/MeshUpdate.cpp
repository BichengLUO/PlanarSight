#include "stdafx.h"
#include "MeshUpdate.h"
#include "ConvexHull.h"

#define DOUBLE_EQUAL(a,b) (abs((a)-(b))<=1e-6)
#define DOUBLE_GREATER(a,b) ((a)-(b)>1e-6)
#define POINT_EQUAL(a,b) (abs((a).x-(b).x)<=1e-6&&abs((a).y-(b).y)<=1e-6)

int edgeLabelsCount = 0;
int splitedEdgeLablesCount;

std::vector<p2t::Point*> initialMeshPointsMemory;
std::vector<p2t::Point*> splitedMeshPointsMemory;
std::vector<p2t::Triangle*> splitedMeshTrianglesMemory;

Mesh buildInitialMesh(const CPolygon &basePolygon)
{
	edgeLabelsCount = 0;
	Mesh initialMesh = buildMeshFromPolygon(basePolygon);
	for (int i = 1; i < basePolygon.loopArray.size(); i++)
	{
		Mesh holeMesh = buildMeshFromInnerLoop(basePolygon.loopArray[i]);
		initialMesh.insert(initialMesh.end(), holeMesh.begin(), holeMesh.end());
	}
	std::vector<Loop> ears = getEarsFromOuterLoop(basePolygon.loopArray[0]);
	for (int i = 0; i < ears.size(); i++)
	{
		Mesh earMesh = buildMeshFromInnerLoop(ears[i]);
		initialMesh.insert(initialMesh.end(), earMesh.begin(), earMesh.end());
	}
	rebuildTrianglesRelationship(initialMesh);
	markPolygonEdges(initialMesh, basePolygon);
	return initialMesh;
}

std::vector<Loop> getEarsFromOuterLoop(const Loop &loop)
{
	std::vector<Loop> ears;
	Loop ch = convexHull(loop);
	IntArray::const_iterator it = std::find(loop.pointIDArray.begin(), loop.pointIDArray.end(), ch.pointIDArray[0]);
	int j = 0;
	int k = it - loop.pointIDArray.begin();
	bool lastEqual = true;
	for (int i = 0; i < loop.pointIDArray.size() || !lastEqual; i++)
	{
		if (loop.pointIDArray[k] == ch.pointIDArray[j])
		{
			if (!lastEqual)
				ears[ears.size() - 1].pointIDArray.push_back(loop.pointIDArray[k]);
			j++;
			j = j % ch.pointIDArray.size();
			lastEqual = true;
		}
		else
		{
			if (lastEqual)
			{
				Loop ear;
				ear.polygon = loop.polygon;
				ears.push_back(ear);
				int prev_k = (k - 1 + loop.pointIDArray.size()) % loop.pointIDArray.size();
				ears[ears.size() - 1].pointIDArray.push_back(loop.pointIDArray[prev_k]);
			}
			ears[ears.size() - 1].pointIDArray.push_back(loop.pointIDArray[k]);
			lastEqual = false;
		}
		k++;
		k = k % loop.pointIDArray.size();
	}
	return ears;
}

Mesh buildMeshFromPolygon(const CPolygon &basePolygon)
{
	Mesh initialMesh;
	if (basePolygon.loopArray.size() == 0)
		return initialMesh;
	vector<p2t::Point*> polyline;
	for (int i = 0; i < basePolygon.loopArray[0].pointIDArray.size(); i++)
	{
		int pointID = basePolygon.loopArray[0].pointIDArray[i];
		Point p = basePolygon.pointArray[pointID];
		p2t::Point *np = new p2t::Point(p.x, p.y);
		polyline.push_back(np);
		initialMeshPointsMemory.push_back(np);
	}
	p2t::CDT cdt(polyline);
	for (int i = 1; i < basePolygon.loopArray.size(); i++)
	{
		vector<p2t::Point*> hole;
		for (int j = 0; j < basePolygon.loopArray[i].pointIDArray.size(); j++)
		{
			int pointID = basePolygon.loopArray[i].pointIDArray[j];
			Point p = basePolygon.pointArray[pointID];
			p2t::Point *np = new p2t::Point(p.x, p.y);
			hole.push_back(np);
			initialMeshPointsMemory.push_back(np);
		}
		cdt.AddHole(hole);
	}
	cdt.Triangulate();
	initialMesh = cdt.GetTriangles();
	return initialMesh;
}

Mesh buildMeshFromInnerLoop(const Loop &loop)
{
	vector<p2t::Point*> polyline;
	for (int i = loop.pointIDArray.size() - 1; i >= 0; i--)
	{
		int pointID = loop.pointIDArray[i];
		Point p = loop.polygon->pointArray[pointID];
		p2t::Point *np = new p2t::Point(p.x, p.y);
		polyline.push_back(np);
		initialMeshPointsMemory.push_back(np);
	}
	p2t::CDT cdt(polyline);
	cdt.Triangulate();
	Mesh mesh = cdt.GetTriangles();
	return mesh;
}

Mesh buildMeshFromOuterLoop(const Loop &loop)
{
	vector<p2t::Point*> polyline;
	for (int i = 0; i < loop.pointIDArray.size(); i++)
	{
		int pointID = loop.pointIDArray[i];
		Point p = loop.polygon->pointArray[pointID];
		p2t::Point *np = new p2t::Point(p.x, p.y);
		polyline.push_back(np);
		initialMeshPointsMemory.push_back(np);
	}
	p2t::CDT cdt(polyline);
	cdt.Triangulate();
	Mesh mesh = cdt.GetTriangles();
	return mesh;
}

Mesh insertPointToUpdateTriangles(const Mesh &mesh, const p2t::Point &p)
{
	splitedEdgeLablesCount = edgeLabelsCount;
	Mesh splitedMesh;
	int ind = findPointInTriangles(mesh, p);
	if (ind != -1)
	{
		p2t::Triangle *next_tri = mesh[ind];
		p2t::Point *next_p = new p2t::Point(p.x, p.y);
		splitedMeshPointsMemory.push_back(next_p);
		p2t::Triangle *ot[2];

		firstTriangleBackwardSplit(splitedMesh, *next_tri, *next_p, ot);
		while (next_tri != NULL)
		{
			next_tri->mark_to_be_splited = true;

			p2t::Point *p1 = next_tri->GetPoint(0);
			p2t::Point *p2 = next_tri->GetPoint(1);
			p2t::Point *p3 = next_tri->GetPoint(2);

			p2t::Point *p1p2 = new p2t::Point(-1, -1);
			p2t::Point *p2p3 = new p2t::Point(-1, -1);
			p2t::Point *p3p1 = new p2t::Point(-1, -1);
			rayIntersectTriangle(*next_tri, *next_p, p1p2, p2p3, p3p1);

			p2t::Point *nextPointCandidate;
			p2t::Triangle *nextTriangleCandidate;
			p2t::Triangle *t[4];
			int status = -1;
			if (p1p2->x != -1 && p1p2->y != -1)
			{
				status = 0;
				t[0] = new p2t::Triangle(*p1, *p1p2, *next_p);
				t[1] = new p2t::Triangle(*p1p2, *p2, *next_p);
				t[2] = new p2t::Triangle(*p2, *p3, *next_p);
				t[3] = new p2t::Triangle(*p3, *p1, *next_p);

				t[2]->edges[2] = next_tri->edges[0];
				t[3]->edges[2] = next_tri->edges[1];
				t[0]->edges[0] = t[1]->edges[1] = splitedEdgeLablesCount++;
				t[1]->edges[0] = t[2]->edges[1] = splitedEdgeLablesCount++;
				t[2]->edges[0] = t[3]->edges[1] = splitedEdgeLablesCount++;
				t[3]->edges[0] = t[0]->edges[1] = splitedEdgeLablesCount++;

				t[0]->polygon_edge[2] = next_tri->polygon_edge[2];
				t[1]->polygon_edge[2] = next_tri->polygon_edge[2];
				t[2]->polygon_edge[2] = next_tri->polygon_edge[0];
				t[3]->polygon_edge[2] = next_tri->polygon_edge[1];

				delete p2p3;
				delete p3p1;
				splitedMeshPointsMemory.push_back(p1p2);
				nextPointCandidate = p1p2;
				nextTriangleCandidate = next_tri->neighbors_[2];
			}
			else if (p2p3->x != -1 && p2p3->y != -1)
			{
				status = 1;
				t[0] = new p2t::Triangle(*p2, *p2p3, *next_p);
				t[1] = new p2t::Triangle(*p2p3, *p3, *next_p);
				t[2] = new p2t::Triangle(*p3, *p1, *next_p);
				t[3] = new p2t::Triangle(*p1, *p2, *next_p);

				t[2]->edges[2] = next_tri->edges[1];
				t[3]->edges[2] = next_tri->edges[2];
				t[0]->edges[0] = t[1]->edges[1] = splitedEdgeLablesCount++;
				t[1]->edges[0] = t[2]->edges[1] = splitedEdgeLablesCount++;
				t[2]->edges[0] = t[3]->edges[1] = splitedEdgeLablesCount++;
				t[3]->edges[0] = t[0]->edges[1] = splitedEdgeLablesCount++;

				t[0]->polygon_edge[2] = next_tri->polygon_edge[0];
				t[1]->polygon_edge[2] = next_tri->polygon_edge[0];
				t[2]->polygon_edge[2] = next_tri->polygon_edge[1];
				t[3]->polygon_edge[2] = next_tri->polygon_edge[2];

				delete p1p2;
				delete p3p1;
				splitedMeshPointsMemory.push_back(p2p3);
				nextPointCandidate = p2p3;
				nextTriangleCandidate = next_tri->neighbors_[0];
			}
			else if (p3p1->x != -1 && p3p1->y != -1)
			{
				status = 2;
				t[0] = new p2t::Triangle(*p3, *p3p1, *next_p);
				t[1] = new p2t::Triangle(*p3p1, *p1, *next_p);
				t[2] = new p2t::Triangle(*p1, *p2, *next_p);
				t[3] = new p2t::Triangle(*p2, *p3, *next_p);

				t[2]->edges[2] = next_tri->edges[2];
				t[3]->edges[2] = next_tri->edges[0];
				t[0]->edges[0] = t[1]->edges[1] = splitedEdgeLablesCount++;
				t[1]->edges[0] = t[2]->edges[1] = splitedEdgeLablesCount++;
				t[2]->edges[0] = t[3]->edges[1] = splitedEdgeLablesCount++;
				t[3]->edges[0] = t[0]->edges[1] = splitedEdgeLablesCount++;

				t[0]->polygon_edge[2] = next_tri->polygon_edge[1];
				t[1]->polygon_edge[2] = next_tri->polygon_edge[1];
				t[2]->polygon_edge[2] = next_tri->polygon_edge[2];
				t[3]->polygon_edge[2] = next_tri->polygon_edge[0];

				delete p1p2;
				delete p2p3;
				splitedMeshPointsMemory.push_back(p3p1);
				nextPointCandidate = p3p1;
				nextTriangleCandidate = next_tri->neighbors_[1];
			}
			else
			{
				status = 3;
				t[0] = new p2t::Triangle(*p1, *p2, *next_p);
				t[1] = new p2t::Triangle(*p2, *p3, *next_p);
				t[2] = new p2t::Triangle(*p3, *p1, *next_p);
				t[3] = NULL;

				t[0]->edges[2] = next_tri->edges[2];
				t[1]->edges[2] = next_tri->edges[0];
				t[2]->edges[2] = next_tri->edges[1];
				t[0]->edges[0] = t[1]->edges[1] = splitedEdgeLablesCount++;
				t[1]->edges[0] = t[2]->edges[1] = splitedEdgeLablesCount++;
				t[2]->edges[0] = t[0]->edges[1] = splitedEdgeLablesCount++;

				t[0]->polygon_edge[2] = next_tri->polygon_edge[2];
				t[1]->polygon_edge[2] = next_tri->polygon_edge[0];
				t[2]->polygon_edge[2] = next_tri->polygon_edge[1];

				delete p1p2;
				delete p2p3;
				delete p3p1;
				nextPointCandidate = next_p;
				int edge = findEdgePointStands(*next_tri, *next_p);
				nextTriangleCandidate = next_tri->neighbors_[edge];
			}
			for (int i = 0; i < 4; i++)
			{
				if (t[i] != NULL)
				{
					p2t::Point t1 = *t[i]->GetPoint(0);
					p2t::Point t2 = *t[i]->GetPoint(1);
					p2t::Point t3 = *t[i]->GetPoint(2);

					if (DOUBLE_EQUAL(p2t::Cross(t2 - t1, t3 - t1), 0))
					{
						if (POINT_EQUAL(t1, t2))
						{
							t[(i + 1) % 4]->edges[1] = t[(i + 3) % 4]->edges[0];
							t[1 - i]->edges[2] = next_tri->edges[(status + 2) % 3];
						}
						else if (POINT_EQUAL(t3, t1))
							if (status == 3) t[(i + 1) % 3]->edges[1] = t[i]->edges[2];
							else t[0]->edges[1] = next_tri->edges[(status + 1) % 3];
						else if (POINT_EQUAL(t3, t2))
							if (status == 3) t[(i + 2) % 3]->edges[0] = t[i]->edges[2];
							else t[1]->edges[0] = next_tri->edges[status];
						else
						{
							if (i == 2 || i == 3)
							{
								if (t[(i + 1) % 4] == NULL)
									t[(i + 2) % 4]->polygon_edge[1] = t[i]->polygon_edge[2];
								else t[(i + 1) % 4]->polygon_edge[1] = t[i]->polygon_edge[2];
								if (t[(i + 3) % 4] == NULL)
									t[(i + 2) % 4]->polygon_edge[0] = t[i]->polygon_edge[2];
								else t[(i + 3) % 4]->polygon_edge[0] = t[i]->polygon_edge[2];
								if ((ot[0] != NULL && *ot[0]->GetPoint(0) == t1) ||
									(ot[1] != NULL && *ot[1]->GetPoint(1) == t2))
								{
									if (ot[0] != NULL) ot[0]->edges[2] = t[i]->edges[1];
									if (ot[1] != NULL) ot[1]->edges[2] = t[i]->edges[0];
								}
								else if ((ot[0] != NULL && *ot[0]->GetPoint(0) == t2) ||
									(ot[1] != NULL && *ot[1]->GetPoint(1) == t1))
								{
									if (ot[0] != NULL) ot[0]->edges[2] = t[i]->edges[0];
									if (ot[1] != NULL) ot[1]->edges[2] = t[i]->edges[1];
								}
							}
						}
						delete t[i];
						t[i] = NULL;
					}
					else
					{
						splitedMesh.push_back(t[i]);
						splitedMeshTrianglesMemory.push_back(t[i]);
					}
				}
			}
			for (int i = 0; i < 2; i++)
				ot[i] = t[i];
			next_tri = nextTriangleCandidate;
			next_p = nextPointCandidate;
		}
		for (int i = 0; i < 2; i++)
		if (ot[i] != NULL)
			ot[i]->edges[2] = splitedEdgeLablesCount++;
	}

	Mesh::const_iterator it;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		if (!(*it)->mark_to_be_splited)
			splitedMesh.push_back(*it);
		else
			(*it)->mark_to_be_splited = false;
	}
	return splitedMesh;
}

void firstTriangleBackwardSplit(Mesh &splitedMesh, p2t::Triangle &tri, p2t::Point &p, p2t::Triangle *ot[])
{
	int edge = -1;
	edge = findEdgePointStands(tri, p);
	if (edge != -1)
	{
		const p2t::Point *p1 = tri.GetPoint(0);
		const p2t::Point *p2 = tri.GetPoint(1);
		const p2t::Point *p3 = tri.GetPoint(2);
		if ((p1->x != p.x || p1->y != p.y) &&
			(p2->x != p.x || p2->y != p.y) &&
			(p3->x != p.x || p3->y != p.y))
		{
			bool downEdge = false;
			if (edge == 0)
				downEdge = p2->x == p3->x || ((p2->x < p3->x) ^ (p2t::Cross(*p3 - *p2, *p1 - *p3) < 0));
			else if (edge == 1)
				downEdge = p1->x == p3->x || ((p1->x < p3->x) ^ (p2t::Cross(*p3 - *p1, *p2 - *p3) < 0));
			else if (edge == 2)
				downEdge = p1->x == p2->x || ((p1->x < p2->x) ^ (p2t::Cross(*p2 - *p1, *p3 - *p2) < 0));
			if (downEdge)
			{
				tri.neighbors_[edge]->mark_to_be_splited = true;
				p2t::Point *n1 = tri.neighbors_[edge]->GetPoint(0);
				p2t::Point *n2 = tri.neighbors_[edge]->GetPoint(1);
				p2t::Point *n3 = tri.neighbors_[edge]->GetPoint(2);
				p2t::Triangle *t1;
				p2t::Triangle *t2;
				if (tri.neighbors_[edge]->neighbors_[0] == &tri)
				{
					t1 = new p2t::Triangle(*n3, *n1, p);
					t1->edges[1] = splitedEdgeLablesCount++;
					t1->edges[2] = tri.edges[1];
					t2 = new p2t::Triangle(*n1, *n2, p);
					t2->edges[0] = splitedEdgeLablesCount++;
					t2->edges[2] = tri.edges[2];
					t1->edges[0] = t2->edges[1] = splitedEdgeLablesCount++;

					t1->polygon_edge[2] = tri.polygon_edge[1];
					t2->polygon_edge[2] = tri.polygon_edge[2];
				}
				else if (tri.neighbors_[edge]->neighbors_[1] == &tri)
				{
					t1 = new p2t::Triangle(*n1, *n2, p);
					t1->edges[1] = splitedEdgeLablesCount++;
					t1->edges[2] = tri.edges[2];
					t2 = new p2t::Triangle(*n2, *n3, p);
					t2->edges[0] = splitedEdgeLablesCount++;
					t2->edges[2] = tri.edges[0];
					t1->edges[0] = t2->edges[1] = splitedEdgeLablesCount++;

					t1->polygon_edge[2] = tri.polygon_edge[2];
					t2->polygon_edge[2] = tri.polygon_edge[0];
				}
				else if (tri.neighbors_[edge]->neighbors_[2] == &tri)
				{
					t1 = new p2t::Triangle(*n2, *n3, p);
					t1->edges[1] = splitedEdgeLablesCount++;
					t1->edges[2] = tri.edges[0];
					t2 = new p2t::Triangle(*n3, *n1, p);
					t2->edges[0] = splitedEdgeLablesCount++;
					t2->edges[2] = tri.edges[1];
					t1->edges[0] = t2->edges[1] = splitedEdgeLablesCount++;

					t1->polygon_edge[2] = tri.polygon_edge[0];
					t2->polygon_edge[2] = tri.polygon_edge[1];
				}
				splitedMesh.push_back(t1);
				splitedMeshTrianglesMemory.push_back(t1);
				splitedMesh.push_back(t2);
				splitedMeshTrianglesMemory.push_back(t2);
				ot[0] = t1;
				ot[1] = t2;
			}
		}
	}
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
	if (p1->x == p.x && p1->y == p.y)
	{
		bool right = p2->x > p1->x && p3->x > p1->x;
		bool ccw = p2t::Cross(*p2 - *p1, *p3 - *p1) > 0;
		if (right ^ ccw)
			return 2;
		else
			return 1;
	}
	if (p2->x == p.x && p2->y == p.y)
	{
		bool right = p1->x > p2->x && p3->x > p2->x;
		bool ccw = p2t::Cross(*p1 - *p2, *p3 - *p2) > 0;
		if (right ^ ccw)
			return 2;
		else
			return 0;
	}
	if (p3->x == p.x && p3->y == p.y)
	{
		bool right = p1->x > p3->x && p2->x > p3->x;
		bool ccw = p2t::Cross(*p1 - *p3, *p2 - *p3) > 0;
		if (right ^ ccw)
			return 1;
		else
			return 0;
	}
	if ((p1->x < p.x && p2->x > p.x) || (p2->x < p.x && p1->x > p.x))
	{
		double ny = p1->y + (p2->y - p1->y) * ((p.x - p1->x) / (p2->x - p1->x));
		if (((p1->y < ny && p2->y > ny) || (p2->y < ny && p1->y > ny)) && DOUBLE_EQUAL(ny, p.y))
			return 2;
	}
	if ((p2->x < p.x && p3->x > p.x) || (p3->x < p.x && p2->x > p.x))
	{
		double ny = p2->y + (p3->y - p2->y) * ((p.x - p2->x) / (p3->x - p2->x));
		if (((p2->y < ny && p3->y > ny) || (p3->y < ny && p2->y > ny)) && DOUBLE_EQUAL(ny, p.y))
			return 0;
	}
	if ((p3->x < p.x && p1->x > p.x) || (p1->x < p.x && p3->x > p.x))
	{
		double ny = p3->y + (p1->y - p3->y) * ((p.x - p3->x) / (p1->x - p3->x));
		if (((p3->y < ny && p1->y > ny) || (p1->y < ny && p3->y > ny)) && DOUBLE_EQUAL(ny, p.y))
			return 1;
	}
	return -1;
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
		if (DOUBLE_GREATER(ny, p.y))
		{
			p1p2->x = p.x;
			p1p2->y = ny;
			return;
		}
	}
	if ((p2->x <= p.x && p3->x > p.x) || (p3->x < p.x && p2->x >= p.x))
	{
		double ny = p2->y + (p3->y - p2->y) * ((p.x - p2->x) / (p3->x - p2->x));
		if (DOUBLE_GREATER(ny, p.y))
		{
			p2p3->x = p.x;
			p2p3->y = ny;
			return;
		}
	}
	if ((p3->x <= p.x && p1->x > p.x) || (p1->x < p.x && p3->x >= p.x))
	{
		double ny = p3->y + (p1->y - p3->y) * ((p.x - p3->x) / (p1->x - p3->x));
		if (DOUBLE_GREATER(ny, p.y))
		{
			p3p1->x = p.x;
			p3p1->y = ny;
			return;
		}
	}
}

void markPolygonEdges(Mesh &mesh, const CPolygon &basePolygon)
{
	for (int i = 0; i < mesh.size(); i++)
	{
		const p2t::Point &p1 = *mesh[i]->GetPoint(0);
		const p2t::Point &p2 = *mesh[i]->GetPoint(1);
		const p2t::Point &p3 = *mesh[i]->GetPoint(2);

		for (int j = 0; j < basePolygon.loopArray.size(); j++)
		{
			const Loop &loop = basePolygon.loopArray[j];
			int loopCount = loop.pointIDArray.size();
			for (int k = 0; k < loopCount; k++)
			{
				const Point &sp = loop.polygon->pointArray[loop.pointIDArray[k]];
				const Point &ep = loop.polygon->pointArray[loop.pointIDArray[(k + 1) % loopCount]];

				if ((p1.x == sp.x && p1.y == sp.y &&
					p2.x == ep.x && p2.y == ep.y) ||
					(p1.x == ep.x && p1.y == ep.y &&
					p2.x == sp.x && p2.y == sp.y))
					mesh[i]->polygon_edge[2] = true;
				if ((p2.x == sp.x && p2.y == sp.y &&
					p3.x == ep.x && p3.y == ep.y) ||
					(p2.x == ep.x && p2.y == ep.y &&
					p3.x == sp.x && p3.y == sp.y))
					mesh[i]->polygon_edge[0] = true;
				if ((p3.x == sp.x && p3.y == sp.y &&
					p1.x == ep.x && p1.y == ep.y) ||
					(p3.x == ep.x && p3.y == ep.y &&
					p1.x == sp.x && p1.y == sp.y))
					mesh[i]->polygon_edge[1] = true;
			}
		}
	}
}

void rebuildTrianglesRelationship(Mesh &mesh)
{
	for (int i = 0; i < mesh.size(); i++)
	{
		const p2t::Point &p1 = *mesh[i]->GetPoint(0);
		const p2t::Point &p2 = *mesh[i]->GetPoint(1);
		const p2t::Point &p3 = *mesh[i]->GetPoint(2);

		mesh[i]->neighbors_[0] = NULL;
		mesh[i]->neighbors_[1] = NULL;
		mesh[i]->neighbors_[2] = NULL;
		for (int j = 0; j < mesh.size(); j++)
		{
			const p2t::Point &k1 = *mesh[j]->GetPoint(0);
			const p2t::Point &k2 = *mesh[j]->GetPoint(1);
			const p2t::Point &k3 = *mesh[j]->GetPoint(2);

			bool cond1 = ((p1.x != k1.x) || (p1.y != k1.y)) && ((p2 == k2 && p3 == k3) || (p2 == k3 && p3 == k2));
			bool cond2 = ((p1.x != k2.x) || (p1.y != k2.y)) && ((p2 == k1 && p3 == k3) || (p2 == k3 && p3 == k1));
			bool cond3 = ((p1.x != k3.x) || (p1.y != k3.y)) && ((p2 == k1 && p3 == k2) || (p2 == k2 && p3 == k1));
			int ne = -1;

			if (cond1 || cond2 || cond3)
			{
				mesh[i]->neighbors_[0] = mesh[j];
				if (mesh[i]->edges[0] == -1)
				{
					if (cond1) ne = 0;
					else if (cond2) ne = 1;
					else if (cond3) ne = 2;
					if (mesh[j]->edges[ne] == -1)
						mesh[i]->edges[0] = edgeLabelsCount++;
					else mesh[i]->edges[0] = mesh[j]->edges[ne];
				}
				continue;
			}
			cond1 = ((p2.x != k1.x) || (p2.y != k1.y)) && ((p1 == k2 && p3 == k3) || (p1 == k3 && p3 == k2));
			cond2 = ((p2.x != k2.x) || (p2.y != k2.y)) && ((p1 == k1 && p3 == k3) || (p1 == k3 && p3 == k1));
			cond3 = ((p2.x != k3.x) || (p2.y != k3.y)) && ((p1 == k1 && p3 == k2) || (p1 == k2 && p3 == k1));
			if (cond1 || cond2 || cond3)
			{
				mesh[i]->neighbors_[1] = mesh[j];
				if (mesh[i]->edges[1] == -1)
				{
					if (cond1) ne = 0;
					else if (cond2) ne = 1;
					else if (cond3) ne = 2;
					if (mesh[j]->edges[ne] == -1)
						mesh[i]->edges[1] = edgeLabelsCount++;
					else mesh[i]->edges[1] = mesh[j]->edges[ne];
				}
				continue;
			}
			cond1 = ((p3.x != k1.x) || (p3.y != k1.y)) && ((p1 == k2 && p2 == k3) || (p1 == k3 && p2 == k2));
			cond2 = ((p3.x != k2.x) || (p3.y != k2.y)) && ((p1 == k1 && p2 == k3) || (p1 == k3 && p2 == k1));
			cond3 = ((p3.x != k3.x) || (p3.y != k3.y)) && ((p1 == k1 && p2 == k2) || (p1 == k2 && p2 == k1));
			if (cond1 || cond2 || cond3)
			{
				mesh[i]->neighbors_[2] = mesh[j];
				if (mesh[i]->edges[2] == -1)
				{
					if (cond1) ne = 0;
					else if (cond2) ne = 1;
					else if (cond3) ne = 2;
					if (mesh[j]->edges[ne] == -1)
						mesh[i]->edges[2] = edgeLabelsCount++;
					else mesh[i]->edges[2] = mesh[j]->edges[ne];
				}
				continue;
			}
		}
		for (int k = 0; k < 3; k++)
		if (mesh[i]->edges[k] == -1)
			mesh[i]->edges[k] = edgeLabelsCount++;
	}
}

int findPointInTriangles(const Mesh &mesh, const p2t::Point &p)
{
	Mesh::const_iterator it;
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

void clearInitialMeshMemory(Mesh &initialMesh)
{
	for (int i = 0; i < initialMeshPointsMemory.size(); i++)
		delete initialMeshPointsMemory[i];
	initialMeshPointsMemory.clear();
	for (int i = 0; i < initialMesh.size(); i++)
		delete initialMesh[i];
	initialMesh.clear();
}

void clearSplitedMeshMemory()
{
	for (int i = 0; i < splitedMeshPointsMemory.size(); i++)
		delete splitedMeshPointsMemory[i];
	splitedMeshPointsMemory.clear();
	for (int i = 0; i < splitedMeshTrianglesMemory.size(); i++)
		delete splitedMeshTrianglesMemory[i];
	splitedMeshTrianglesMemory.clear();
}