#ifndef _MESH_UPDATE_H
#define _MESH_UPDATE_H

#include <vector>
#include "shapes.h"

int findPointInTriangles(const std::vector<p2t::Triangle*> &mesh, const p2t::Point &p);
inline double toLeft(const p2t::Point &p1, const p2t::Point &p2, const p2t::Point &p3);
bool pointInTriangle(const p2t::Point &pt, const p2t::Point &v1, const p2t::Point &v2, const p2t::Point &v3);

#endif