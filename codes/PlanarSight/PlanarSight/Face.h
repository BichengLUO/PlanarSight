#ifndef _FACE_H
#define _FACE_H

#include <cstdio>
#include <string>
#include <vector>
#include "HalfEdge.h"

using namespace std;

class HalfEdge;

class Face
{
public:
    HalfEdge* edge;

public:
    Face(){}
    Face(HalfEdge *e) : edge(e){}
    ~Face(){}

    // return the number of edges on this face
    int getEdgeCount();

    void print();
};

#endif