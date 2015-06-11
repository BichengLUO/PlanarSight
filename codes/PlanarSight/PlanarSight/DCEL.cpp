#include "DCEL.h"

void DCEL::preInitialize(const PointArray &points)
{
    faces.clear();
    edges.clear();
    vertexs.clear();
    lines.clear();

    int num = points.size();
    // face[0] is the outer face, and it never be splited
    Face* faceOut = new Face();
    faces.push_back(faceOut);
    Face* faceIn = new Face();
    faces.push_back(faceIn);

    for (int i = 0; i < num; i++)
    {
        Point point = points[i];
        Vertex* vertex = new Vertex(point, vertexId++);
        vertexs.push_back(vertex);

        HalfEdge* left = new HalfEdge(vertex);
        HalfEdge* right = new HalfEdge(vertex);
        edges.push_back(left);
        edges.push_back(right);
        vertex->edge = left;

        left->face = faces[1];
        right->face = faces[0];
    }

    faceIn->edge = edges[0];
    faceOut->edge = edges[1];

    for (int i = 0; i < num; i++)
    {
        int mod = 2 * num;
        int j = i * 2;
        edges[j]->prev = edges[(j - 2 + mod) % mod];
        edges[j]->next = edges[(j + 2 + mod) % mod];
        edges[j]->twin = edges[(j + 3 + mod) % mod];

        edges[j + 1]->prev = edges[(j + 1 + 2 + mod) % mod];
        edges[j + 1]->next = edges[(j + 1 - 2 + mod) % mod];
        edges[j + 1]->twin = edges[(j + 1 - 3 + mod) % mod];
    }

}

void DCEL::initialize(const PointArray &points)
{
    PointArray prePoints;
    prePoints.clear();
    prePoints.push_back(Point(-INF, -INF));
    prePoints.push_back(Point(INF, -INF));
    prePoints.push_back(Point(INF, INF));
    prePoints.push_back(Point(-INF, INF));
    preInitialize(prePoints);

    for (int i = 0; i < points.size(); i++)
    {
        Line temLine = Line(points[i].x, 1, -points[i].y);
        addLine(temLine, i);
    }
}

void DCEL::splitEdge(HalfEdge* edge, Point &point)
{
    Vertex* vertex = new Vertex(point, vertexId++);
    vertexs.push_back(vertex);
    // edge and edgeTwin just become shorter, not necessary be deleted
    HalfEdge* edgeTwin = edge->twin;

    HalfEdge* edgeAdd = new HalfEdge(vertex);
    edges.push_back(edgeAdd);
    HalfEdge* edgeTwinAdd = new HalfEdge(vertex);
    edges.push_back(edgeTwinAdd);

    edgeAdd->face = edge->face;
    edgeAdd->twin = edgeTwin;
    edgeAdd->next = edge->next;
    edgeAdd->prev = edge;
    edgeAdd->id = edge->id;

    edge->next->prev = edgeAdd;
    edge->next = edgeAdd;
    edge->twin = edgeTwinAdd;

    edgeTwinAdd->face = edgeTwin->face;
    edgeTwinAdd->twin = edge;
    edgeTwinAdd->next = edgeTwin->next;
    edgeTwinAdd->prev = edgeTwin;
    edgeTwinAdd->id = edgeTwin->id;

    edgeTwin->next->prev = edgeTwinAdd;
    edgeTwin->next = edgeTwinAdd;
    edgeTwin->twin = edgeAdd;
    
    vertex->edge = edgeAdd;
}

void DCEL::splitFace(HalfEdge* edge, Vertex* v, int id)
{
    Vertex* u = edge->getDestination();
    HalfEdge* end = v->edge;
    while (end->face != edge->face)
    {
        end = end->twin->next;
    }

    Face* newFace = new Face();
    faces.push_back(newFace);
    HalfEdge* left = new HalfEdge(u);
    edges.push_back(left);
    HalfEdge* right = new HalfEdge(v);
    edges.push_back(right);

    newFace->edge = right;

    left->face = edge->face;
    left->twin = right;
    left->prev = edge;
    left->next = end;
    left->id = id;

    right->face = newFace;
    right->twin = left;
    right->prev = end->prev;
    right->next = edge->next;
    right->id = id;

    edge->face->edge = left;
    
    right->next->prev = right;
    right->prev->next = right;
    for (HalfEdge* e = right->next; e != right; e = e->next)
    {
        e->face = newFace;
    }

    edge->next = left;
    end->prev = left;
}

int sss = 0;
void DCEL::addLine(Line &line, int id)
{
    lines.push_back(line);

    // get the start intersection point and halfEdge
    // faces[0] is a magic face for usage
    Point startPoint = Point(INF, INF);
    HalfEdge* startEdge = NULL;
    bool flagSplit = false;
    for (HalfEdge* e = faces[0]->edge; startEdge == NULL || e != faces[0]->edge; e = e->next)
    {
        if (line.isIntersection(e) == true)
        {
            Point tem = line.getIntersection(e);
            if (tem.x < startPoint.x || (fabs(tem.x - startPoint.x) < eps && tem.y < startPoint.y))
            {
                flagSplit = true;
                startPoint = tem;
                startEdge = e->twin;
            }
        }
        else
        {
            if (line.isOnLine(e->origin->coordinate) == true)
            {
                Point tem = e->origin->coordinate;
                if (tem.x < startPoint.x || (fabs(tem.x - startPoint.x) < eps && tem.y < startPoint.y))
                {
                    flagSplit = false;
                    startPoint = tem;
                    startEdge = e->twin;
                }
            }
        }
    }

    if (startEdge == NULL)
    {
        printf("Error: do not get the startEdge !\n");
        return;
    }
    if (flagSplit)
    {
        splitEdge(startEdge, startPoint);
    }

    while (1)
    {
        Point endPoint;
        HalfEdge* endEdge;
        HalfEdge *e;
        for (e = startEdge->next->next; line.isOnLine(e->getDestination()->coordinate) == false &&
            line.isIntersection(e) == false; e = e->next);
        // cross through current face with a vertex
        if (line.isOnLine(e->getDestination()->coordinate) == true)
        {
            endPoint = e->getDestination()->coordinate;
            for (endEdge = e->next; 1; endEdge = endEdge->twin->next)
            {
                if (e->id == endEdge->id)
                    break;
            }
            endEdge = endEdge->twin;
            if (endEdge != e){
                splitFace(startEdge, endEdge->getDestination(), id);
            }

            startPoint = endPoint;
            startEdge = endEdge;
        }
        else
        // cross through current face with a halfEdge
        {
            endPoint = line.getIntersection(e);
            splitEdge(e, endPoint);
            endEdge = e->twin;
            splitFace(startEdge, endEdge->origin, id);

            startPoint = endPoint;
            startEdge = endEdge->prev;
        }

        // if the new startPoint is on the boundary, just stop
        if (fabs(startPoint.x) > INF - INF * eps || fabs(startPoint.y) > INF - INF * eps)
            break;
    }
}

void DCEL::query(Line &line, IntArray& pPolarOrder, int &pPolarOrderNum)
{
    // get the start intersection point and halfEdge
    // faces[0] is a magic face for usage
    Point startPoint = Point(INF, INF);
    HalfEdge* startEdge = NULL;
    for (HalfEdge* e = faces[0]->edge; startEdge == NULL || e != faces[0]->edge; e = e->next)
    {
        if (line.isIntersection(e) == true)
        {
            Point tem = line.getIntersection(e);
            if (tem.x < startPoint.x || (fabs(tem.x - startPoint.x) < eps && tem.y < startPoint.y))
            {
                startPoint = tem;
                startEdge = e->twin;
            }
        }
        else
        {
            if (line.isOnLine(e->origin->coordinate) == true)
            {
                Point tem = e->origin->coordinate;
                if (tem.x < startPoint.x || (fabs(tem.x - startPoint.x) < eps && tem.y < startPoint.y))
                {
                    startPoint = tem;
                    startEdge = e->twin->next;
                }
            }
        }
        if (startPoint.x < INF - INF * eps)
            break;
    }

    if (startEdge == NULL)
    {
        printf("Query Error: do not get the startEdge !\n");
        return;
    }

    while (1)
    {
        Point endPoint;
        HalfEdge* endEdge;

        HalfEdge *e;
        for (e = startEdge->next; line.isIntersection(e) == false && 
            line.isOnLine(e->getDestination()->coordinate) == false; e = e->next);
        // cross through current face with a vertex
        if (line.isOnLine(e->getDestination()->coordinate) == true)
        {
            endPoint = e->getDestination()->coordinate;
            for (endEdge = e->next; 1; endEdge = endEdge->twin->next)
            {
                if (e->id == endEdge->id)
                    break;
            }
            endEdge = endEdge->twin;
            int cnt = endEdge->getDestination()->getCountIncident();
            cnt = cnt / 2;
            HalfEdge* temE = e->next;
            for (int i = 0; i < cnt; i++)
            {
                if (temE->id >= 0)
                {
                    pPolarOrder[pPolarOrderNum++] = temE->id;
                }
                temE = temE->twin->next;
            }

            startPoint = endPoint;
            startEdge = endEdge->next;
        }
        else
        // cross through current face with a halfEdge
        {
            endPoint = line.getIntersection(e);
            endEdge = e->twin;

            if (e->id >= 0)
            {
                pPolarOrder[pPolarOrderNum++] = e->id;
            }

            startPoint = endPoint;
            startEdge = endEdge;
        }

        // if the new startPoint is on the boundary, just stop
        if (fabs(startPoint.x) > INF - INF * eps || fabs(startPoint.y) > INF - INF * eps)
            break;
    }

}

void DCEL::deleteAll()
{
    for (int i = 0; i < vertexs.size(); i++)
        vertexs.clear();
    for (int i = 0; i < edges.size(); i++)
        edges.clear();
    for (int i = 0; i < faces.size(); i++)
        faces.clear();
}

void DCEL::print()
{
    printf("Vertexs: %d\n", vertexs.size());
    for (int i = 0; i < vertexs.size(); i++)
        vertexs[i]->print();
    printf("Edges: %d\n", edges.size());
    for (int i = 0; i < edges.size(); i++)
        edges[i]->print();
    printf("Faces: %d\n", faces.size());
    for (int i = 0; i < faces.size(); i++)
        faces[i]->print();
    
}