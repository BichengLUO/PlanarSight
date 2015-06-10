#ifndef _MESH_2_GRAPH_H
#define _MESH_2_GRAPH_H

#include "MeshUpdate.h"
#include "Graph.h"

SegmentArray mesh2SegArray(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	int basePolygonPointsCount, PointArray &new_pa);
SegmentArray graph2SegArray(const Graph &graph, IntArray &pla, const bool *polygonEdge);
Graph* mesh2Graph(const Mesh &mesh, const p2t::Point &p, int splitedEdgeLablesCount,
	int basePolygonPointsCount, PointArray &new_pa, IntArray &pla, bool *polygonEdge,
	AdjListNode *nodeMemPool);

#endif