#include "HalfEdge.h"

Vertex* HalfEdge::getDestination()
{
    return twin->origin;
}

void HalfEdge::print()
{
    printf("HALFEDGE{(%d,%d), id:%d}\n", origin->id, getDestination()->id, id);
}