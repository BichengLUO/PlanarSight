#include "Mesh2Graph.h"



Graph* mesh2Graph(const Mesh &mesh, const p2t::Point p, int splitedEdgeLablesCount)
{
	Graph *graph = new Graph(splitedEdgeLablesCount);
	Mesh::const_iterator it;
	for (it = mesh.begin(); it != mesh.end(); ++it)
	{
		const p2t::Point *p1 = (*it)->GetPoint(0);
		const p2t::Point *p2 = (*it)->GetPoint(1);
		const p2t::Point *p3 = (*it)->GetPoint(2);

		double sign1 = toLeft(p, *p2, *p3);
		double sign2 = toLeft(p, *p3, *p1);
		double sign3 = toLeft(p, *p1, *p2);

		//R1,R2,R3
		if (sign1 > 0 && sign2 < 0 && sign3 < 0)
		{
			graph->addEdge((*it)->edges[1], (*it)->edges[0]);
			graph->addEdge((*it)->edges[2], (*it)->edges[0]);
		}
		if (sign2 > 0 && sign1 < 0 && sign3 < 0)
		{
			graph->addEdge((*it)->edges[0], (*it)->edges[1]);
			graph->addEdge((*it)->edges[2], (*it)->edges[1]);
		}
		if (sign3 > 0 && sign1 < 0 && sign2 < 0)
		{
			graph->addEdge((*it)->edges[1], (*it)->edges[2]);
			graph->addEdge((*it)->edges[0], (*it)->edges[2]);
		}
		//~R1,~R2,~R3
		if (sign1 < 0 && sign2 > 0 && sign3 > 0)
		{
			graph->addEdge((*it)->edges[0], (*it)->edges[1]);
			graph->addEdge((*it)->edges[0], (*it)->edges[2]);
		}
		if (sign2 < 0 && sign1 > 0 && sign3 > 0)
		{
			graph->addEdge((*it)->edges[1], (*it)->edges[0]);
			graph->addEdge((*it)->edges[1], (*it)->edges[2]);
		}
		if (sign3 < 0 && sign1 > 0 && sign2 > 0)
		{
			graph->addEdge((*it)->edges[2], (*it)->edges[1]);
			graph->addEdge((*it)->edges[2], (*it)->edges[0]);
		}
		//l21,l31,l12,l13,l32,l23
		if (sign1 == 0 && sign2 > 0)
			graph->addEdge((*it)->edges[2], (*it)->edges[1]);
		if (sign2 == 0 && sign1 > 0)
			graph->addEdge((*it)->edges[2], (*it)->edges[0]);
		if (sign3 == 0 && sign1 > 0)
			graph->addEdge((*it)->edges[1], (*it)->edges[0]);
		if (sign1 == 0 && sign3 > 0)
			graph->addEdge((*it)->edges[1], (*it)->edges[2]);
		if (sign2 == 0 && sign3 > 0)
			graph->addEdge((*it)->edges[0], (*it)->edges[2]);
		if (sign3 == 0 && sign2 > 0)
			graph->addEdge((*it)->edges[0], (*it)->edges[1]);
	}
	return graph;
}

