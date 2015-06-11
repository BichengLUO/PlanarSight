#include "Mesh2Graph.h"
#include <Windows.h>

double total_m2g_tm = 0;
int m2g_count = 0;

SegmentArray mesh2SegArray(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	int basePolygonPointsCount, PointArray &new_pa)
{
	bool *polygonEdge = new bool[splitedEdgeLablesCount];
	IntArray pla(2 * splitedEdgeLablesCount);
	AdjListNode *nodeMemPool = new AdjListNode[2 * mesh.size()];

	Graph *graph = mesh2Graph(mesh, p, splitedEdgeLablesCount,
		basePolygonPointsCount, new_pa, pla, polygonEdge, nodeMemPool);
	SegmentArray sOrder = graph2SegArray(*graph, pla, polygonEdge);

	delete[] nodeMemPool;
	delete graph;
	delete[] polygonEdge;
	return sOrder;
}

SegmentArray graph2SegArray(const Graph &graph, IntArray &pla, const bool *polygonEdge)
{
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	IntArray sortedEdgeLabels;
	graph.topologicalSort(sortedEdgeLabels);
	SegmentArray sOrder;
	for (int i = 0; i < sortedEdgeLabels.size(); i++)
	{
		if (polygonEdge[sortedEdgeLabels[i]])
			sOrder.push_back(Segment(pla[2 * sortedEdgeLabels[i]], pla[2 * sortedEdgeLabels[i] + 1]));
	}

	QueryPerformanceCounter(&EndTime);
	double tm = (double)(EndTime.QuadPart - BeginTime.QuadPart) / Frequency.QuadPart;
	total_m2g_tm += tm;

	m2g_count++;
	//printf("%.2f\n", total_m2g_tm * 1000 / m2g_count);
	return sOrder;
}

Graph* mesh2Graph(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	int basePolygonPointsCount, PointArray &new_pa, IntArray &pla, bool *polygonEdge,
	AdjListNode *nodeMemPool)
{
	Graph *graph = new Graph(splitedEdgeLablesCount);
	memset(polygonEdge, false, splitedEdgeLablesCount * sizeof(bool));
	Point *temp = new Point[2 * splitedEdgeLablesCount];
	AdjListNode *nodeMemPoolIndex = nodeMemPool;

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
			graph->addEdge((*it)->edges[1], (*it)->edges[0], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[2], (*it)->edges[0], nodeMemPoolIndex++);
		}
		if (sign2 > 0 && sign1 < 0 && sign3 < 0)
		{
			graph->addEdge((*it)->edges[0], (*it)->edges[1], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[2], (*it)->edges[1], nodeMemPoolIndex++);
		}
		if (sign3 > 0 && sign1 < 0 && sign2 < 0)
		{
			graph->addEdge((*it)->edges[1], (*it)->edges[2], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[0], (*it)->edges[2], nodeMemPoolIndex++);
		}
		//~R1,~R2,~R3
		if (sign1 < 0 && sign2 > 0 && sign3 > 0)
		{
			graph->addEdge((*it)->edges[0], (*it)->edges[1], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[0], (*it)->edges[2], nodeMemPoolIndex++);
		}
		if (sign2 < 0 && sign1 > 0 && sign3 > 0)
		{
			graph->addEdge((*it)->edges[1], (*it)->edges[0], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[1], (*it)->edges[2], nodeMemPoolIndex++);
		}
		if (sign3 < 0 && sign1 > 0 && sign2 > 0)
		{
			graph->addEdge((*it)->edges[2], (*it)->edges[1], nodeMemPoolIndex++);
			graph->addEdge((*it)->edges[2], (*it)->edges[0], nodeMemPoolIndex++);
		}
		//l21,l31,l12,l13,l32,l23
		if (sign1 == 0 && sign2 > 0)
			graph->addEdge((*it)->edges[2], (*it)->edges[1], nodeMemPoolIndex++);
		if (sign2 == 0 && sign1 > 0)
			graph->addEdge((*it)->edges[2], (*it)->edges[0], nodeMemPoolIndex++);
		if (sign3 == 0 && sign1 > 0)
			graph->addEdge((*it)->edges[1], (*it)->edges[0], nodeMemPoolIndex++);
		if (sign1 == 0 && sign3 > 0)
			graph->addEdge((*it)->edges[1], (*it)->edges[2], nodeMemPoolIndex++);
		if (sign2 == 0 && sign3 > 0)
			graph->addEdge((*it)->edges[0], (*it)->edges[2], nodeMemPoolIndex++);
		if (sign3 == 0 && sign2 > 0)
			graph->addEdge((*it)->edges[0], (*it)->edges[1], nodeMemPoolIndex++);

		pla[2 * (*it)->edges[0]] = p2->pointLabel;
		pla[2 * (*it)->edges[0] + 1] = p3->pointLabel;
		temp[2 * (*it)->edges[0]] = Point(p2->x, p2->y);
		temp[2 * (*it)->edges[0] + 1] = Point(p3->x, p3->y);
		polygonEdge[(*it)->edges[0]] = (*it)->polygon_edge[0];

		pla[2 * (*it)->edges[1]] = p3->pointLabel;
		pla[2 * (*it)->edges[1] + 1] = p1->pointLabel;
		temp[2 * (*it)->edges[1]] = Point(p3->x, p3->y);
		temp[2 * (*it)->edges[1] + 1] = Point(p1->x, p1->y);
		polygonEdge[(*it)->edges[1]] = (*it)->polygon_edge[1];

		pla[2 * (*it)->edges[2]] = p1->pointLabel;
		pla[2 * (*it)->edges[2] + 1] = p2->pointLabel;
		temp[2 * (*it)->edges[2]] = Point(p1->x, p1->y);
		temp[2 * (*it)->edges[2] + 1] = Point(p2->x, p2->y);
		polygonEdge[(*it)->edges[2]] = (*it)->polygon_edge[2];
	}

	for (int i = 0; i < 2 * splitedEdgeLablesCount; i++)
	if (pla[i] == -1)
	{
		pla[i] = basePolygonPointsCount++;
		new_pa.push_back(temp[i]);
	}
	delete[] temp;
	return graph;
}

