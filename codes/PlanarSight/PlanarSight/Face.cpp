#include "Face.h"

int Face::getEdgeCount()
{
    int num = 1;
    for (HalfEdge* e = edge; e->next != edge; e = e->next)
    {
        num++;
    }
    return num;
}

void Face::print()
{
    int num = getEdgeCount();
    printf("FACE(n:%d):{", num);
    HalfEdge* e = edge;
    for (int i = 0; i < num - 1; i++)
    {
        printf("%d,",e->origin->id);
        e = e->next;
    }
    printf("%d}\n", e->origin->id);
}