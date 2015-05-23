#ifndef _MESH_UPDATE_H
#define _MESH_UPDATE_H

#include <vector>
#include "poly2tri.h"
#include "Polygon.h"

std::vector<p2t::Triangle*> buildInitialMesh(const CPolygon &basePolygon);
void addEarsForOuterLoop(std::vector<p2t::Triangle*> &mesh, const Loop &loop);
std::vector<p2t::Triangle*> buildMeshFromPolygon(const CPolygon &basePolygon);
std::vector<p2t::Triangle*> buildMeshFromInnerLoop(const Loop &loop);
std::vector<p2t::Triangle*> buildMeshFromOuterLoop(const Loop &loop);

std::vector<p2t::Triangle*> insertPointToUpdateTriangles(std::vector<p2t::Triangle*> &mesh, const p2t::Point &p);
int findEdgePointStands(p2t::Triangle &tri, const p2t::Point &p);
void rayIntersectTriangle(p2t::Triangle &tri, const p2t::Point &p, p2t::Point *p1p2, p2t::Point *p2p3, p2t::Point *p3p1);
void rebuildTrianglesRelationship(std::vector<p2t::Triangle*> &mesh);
int findPointInTriangles(const std::vector<p2t::Triangle*> &mesh, const p2t::Point &p);
inline double toLeft(const p2t::Point &p1, const p2t::Point &p2, const p2t::Point &p3);
bool pointInTriangle(const p2t::Point &pt, const p2t::Point &v1, const p2t::Point &v2, const p2t::Point &v3);

#endif