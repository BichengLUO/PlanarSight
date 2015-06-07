#include "Vertex.h"

HalfEdge* Vertex::getIncidnetEdge(Vertex* vertex)
{
    if (edge->getDestination() == vertex)
    {
        return edge;
    }
    else
    {
        for (HalfEdge* e = edge->twin->next; e != edge; e = e->twin->next)
        {
            if (e->getDestination() == vertex)
                return e;
        }
    }
    return NULL;
}

int Vertex::getCountIncident()
{
    int num = 1;
    for (HalfEdge* e = edge->twin->next; e != edge; e = e->twin->next)
    {
        num++;
    }
    return num / 2;
}

void Vertex::print()
{
    printf("VERTEX(%d): point(%lf,%lf)\n", id, coordinate.x, coordinate.y);
}