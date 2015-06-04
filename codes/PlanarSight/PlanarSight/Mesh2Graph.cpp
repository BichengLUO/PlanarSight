#include "Mesh2Graph.h"

SegmentArray mesh2SegArray(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	PointArray &pa)
{
	bool *polygonEdge = new bool[splitedEdgeLablesCount];
	pa.resize(2 * splitedEdgeLablesCount);
	Graph *graph = mesh2Graph(mesh, p, splitedEdgeLablesCount, pa, polygonEdge);
	SegmentArray sOrder = graph2SegArray(*graph, pa, polygonEdge);
	delete graph;
	delete[] polygonEdge;
	return sOrder;
}

SegmentArray graph2SegArray(const Graph &graph, PointArray &pa, const bool *polygonEdge)
{
	IntArray sortedEdgeLabels;
	graph.topologicalSort(sortedEdgeLabels);
	SegmentArray sOrder;
	PointArray new_pa;
	for (int i = 0; i < sortedEdgeLabels.size(); i++)
	{
		if (polygonEdge[sortedEdgeLabels[i]])
		{
			new_pa.push_back(pa[2 * sortedEdgeLabels[i]]);
			new_pa.push_back(pa[2 * sortedEdgeLabels[i] + 1]);
			sOrder.push_back(Segment(new_pa.size() - 2, new_pa.size() - 1));
		}
	}
	pa.clear();
	pa.insert(pa.end(), new_pa.begin(), new_pa.end());
	return sOrder;
}

Graph* mesh2Graph(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	PointArray &pa, bool *polygonEdge)
{
	Graph *graph = new Graph(splitedEdgeLablesCount);
	memset(polygonEdge, false, splitedEdgeLablesCount * sizeof(bool));
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

		pa[2 * (*it)->edges[0]] = Point(p2->x, p2->y);
		pa[2 * (*it)->edges[0] + 1] = Point(p3->x, p3->y);
		polygonEdge[(*it)->edges[0]] = (*it)->polygon_edge[0];

		pa[2 * (*it)->edges[1]] = Point(p3->x, p3->y);
		pa[2 * (*it)->edges[1] + 1] = Point(p1->x, p1->y);
		polygonEdge[(*it)->edges[1]] = (*it)->polygon_edge[1];

		pa[2 * (*it)->edges[2]] = Point(p1->x, p1->y);
		pa[2 * (*it)->edges[2] + 1] = Point(p2->x, p2->y);
		polygonEdge[(*it)->edges[2]] = (*it)->polygon_edge[2];
	}
	return graph;
}

