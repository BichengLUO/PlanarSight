#ifndef _MESH_UPDATE_H
#define _MESH_UPDATE_H

#include <vector>
#include "poly2tri.h"
#include "Polygon.h"
#include "Graph.h"

typedef std::vector<p2t::Triangle*> Mesh;

Mesh buildInitialMesh(const CPolygon &basePolygon);
std::vector<Loop> getEarsFromOuterLoop(const Loop &loop);
Mesh buildMeshFromPolygon(const CPolygon &basePolygon);
Mesh buildMeshFromInnerLoop(const Loop &loop);
Mesh buildMeshFromOuterLoop(const Loop &loop);

Mesh insertPointToUpdateTriangles(const Mesh &mesh, const p2t::Point &p);
void firstTriangleBackwardSplit(Mesh &splitedMesh, p2t::Triangle &tri, p2t::Point &p, p2t::Triangle *ot[]);
int findEdgePointStands(p2t::Triangle &tri, const p2t::Point &p);
void rayIntersectTriangle(p2t::Triangle &tri, const p2t::Point &p, p2t::Point *p1p2, p2t::Point *p2p3, p2t::Point *p3p1);

void rebuildTrianglesRelationship(Mesh &mesh);
int findPointInTriangles(const Mesh &mesh, const p2t::Point &p);
inline double toLeft(const p2t::Point &p1, const p2t::Point &p2, const p2t::Point &p3);
bool pointInTriangle(const p2t::Point &pt, const p2t::Point &v1, const p2t::Point &v2, const p2t::Point &v3);

#endif