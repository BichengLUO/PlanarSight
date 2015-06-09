#ifndef _VERTEX_H
#define _VERTEX_H

#include <cstdio>
#include <string>
#include <vector>
#include "HalfEdge.h"
#include "Point.h"

using namespace std;

class HalfEdge;

class Vertex
{
public:
    int id;
    Point coordinate;
    HalfEdge* edge;

public:
    Vertex(){}
    Vertex(const Point p) : coordinate(p), id(-1), edge(NULL){}
    Vertex(const Point p, int _id) : coordinate(p), id(_id), edge(NULL){}
    ~Vertex(){}

    // return the half edge from this vertex to the given vertex
    HalfEdge* getIncidnetEdge(const Vertex* vertex);

    int getCountIncident();

    void print();
};

#endif