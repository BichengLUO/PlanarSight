#ifndef _DCEL_H
#define _DCEL_H

#include "Vertex.h"
#include "HalfEdge.h"
#include "Face.h"
#include "Line.h"
#include <vector>

using namespace std;

class Vertex;
class HalfEdge;
class Face;
class Line;

typedef vector<Vertex*> VertexArray;
typedef vector<HalfEdge*> HalfEdgeArray;
typedef vector<Face*> FaceArray;
typedef vector<Line> LineArray;

static int vertexId = 0;
static const double eps = 1e-8;
static const double bigEps = 1e-7 / 4;
static const double INF = 1e6;

class DCEL
{
public :
    VertexArray vertexs;
    HalfEdgeArray edges;
    FaceArray faces;

    LineArray lines;

public :
    DCEL(){
        vertexs.clear();
        edges.clear();
        faces.clear();
        lines.clear();
    }
    ~DCEL(){}

    // build the rectangle boundary
    void preInitialize(const PointArray &points);

    void initialize(const PointArray &points);

    // the point is located on the edge
    void splitEdge(HalfEdge* edge, Point &point);

    // the halfEdge edge is incident of the face
    // the edge->getDestination() is another Vertex
    void splitFace(HalfEdge* edge, Vertex* v, int id);

    void addLine(Line &line, int id);
    
    // only return the order of all the lines intersected with this line
    void query(Line &line, IntArray &pPolarOrder, int &pPolarOrderNum);

    void deleteAll();

    void print();

};


#endif