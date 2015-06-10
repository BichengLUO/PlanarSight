#ifndef _GRAPH_H
#define _GRAPH_H

#include <vector>
#include <stack>

// A structure to represent an adjacency list node
typedef struct _AdjListNode
{
	int dest;
	_AdjListNode* next;
	_AdjListNode() : dest(-1), next(NULL) {}
	_AdjListNode(int d) : dest(d), next(NULL) {}
} AdjListNode;

// A structure to represent an adjacency list
typedef struct _AdjList
{
	AdjListNode *head;  // pointer to head node of list
	int incomingDegree;
} AdjList;

// A structure to represent a graph. A graph is an array of adjacency lists.
// Size of array will be V (number of vertices in graph)
class Graph
{
public:
	Graph(int ct);
	void addEdge(int src, int dest);
	void addEdge(int src, int dest, AdjListNode *new_node);
	void printGraph();
	void topologicalSort(std::vector<int> &sortedList) const;
	~Graph();

protected:
	int count;
	AdjList* array;
	std::vector<AdjListNode*> allListNodes;
};

#endif