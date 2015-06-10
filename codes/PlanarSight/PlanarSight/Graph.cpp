#include "stdafx.h"
#include "Graph.h"
#include <queue>

// A utility function that creates a graph of V vertices
Graph::Graph(int ct)
{
	count = ct;

	// Create an array of adjacency lists.  Size of array will be V
	array = new AdjList[count];

	// Initialize each adjacency list as empty by making head as NULL
	int i;
	for (i = 0; i < count; ++i)
	{
		array[i].head = NULL;
		array[i].incomingDegree = 0;
	}
}

// Adds an edge to an directed graph
void Graph::addEdge(int src, int dest)
{
	// Add an edge from src to dest.  A new node is added to the adjacency
	// list of src.  The node is added at the begining
	AdjListNode* newNode = new AdjListNode(dest);
	newNode->next = array[src].head;
	array[src].head = newNode;
	array[dest].incomingDegree++;
	// Add new node to nodes list for future deleting
	allListNodes.push_back(newNode);
}

// Adds an edge to an directed graph
void Graph::addEdge(int src, int dest, AdjListNode *new_node)
{
	// Add an edge from src to dest.  A new node is added to the adjacency
	// list of src.  The node is added at the begining
	new_node->dest = dest;
	AdjListNode* newNode = new_node;
	newNode->next = array[src].head;
	array[src].head = newNode;
	array[dest].incomingDegree++;
}

// A utility function to print the adjacenncy list representation of graph
void Graph::printGraph()
{
	for (int v = 0; v < count; ++v)
	{
		AdjListNode* pCrawl = array[v].head;
		printf("\n Adjacency list of vertex %d\n head ", v);
		while (pCrawl)
		{
			printf("-> %d", pCrawl->dest);
			pCrawl = pCrawl->next;
		}
		printf("\n");
	}
}

// The function to do Topological Sort. It uses recursive topologicalSortUtil()
void Graph::topologicalSort(std::vector<int> &sortedList) const
{
	std::queue<int> s;
	for (int i = 0; i < count; i++)
	if (array[i].incomingDegree == 0)
		s.push(i);
	while (!s.empty())
	{
		int n = s.front();
		s.pop();
		sortedList.push_back(n);
		AdjListNode* pCrawl = array[n].head;
		while (pCrawl)
		{
			array[pCrawl->dest].incomingDegree--;
			if (array[pCrawl->dest].incomingDegree == 0)
				s.push(pCrawl->dest);
			pCrawl = pCrawl->next;
		}
	}
}

Graph::~Graph()
{
	for (int i = 0; i < allListNodes.size(); i++)
		delete allListNodes[i];
	delete[] array;
}