#ifndef _MESH_2_GRAPH_H
#define _MESH_2_GRAPH_H

#include "MeshUpdate.h"
#include "Graph.h"

Graph* mesh2Graph(const Mesh &mesh, const p2t::Point p, int splitedEdgeLablesCount);

#endif