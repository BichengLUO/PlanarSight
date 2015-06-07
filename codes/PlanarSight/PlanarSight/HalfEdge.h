#ifndef _HALFEDGE_H
#define _HALFEDGE_H

#include <cstdio>
#include <string>
#include <vector>
#include "Face.h"
#include "Vertex.h"

using namespace std;

class Vertex;
class Face;

class HalfEdge
{
public:
    HalfEdge* twin;
    HalfEdge* next;
    HalfEdge* prev;
    Vertex* origin;
    Face* face;
    int id = -1;

public:
    HalfEdge(){}
    HalfEdge(Vertex* vertex)
    {
        origin = vertex;
        twin = NULL;
        next = NULL;
        prev = NULL;
        face = NULL;
    }
    ~HalfEdge(){}

    //return this half edge's destination
    Vertex* getDestination();

    void print();
};

#endif