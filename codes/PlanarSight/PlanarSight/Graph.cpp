#include "stdafx.h"
#include "Graph.h"

// A utility function that creates a graph of V vertices
Graph::Graph(int ct)
{
	count = ct;

	// Create an array of adjacency lists.  Size of array will be V
	array = new AdjList[count];

	// Initialize each adjacency list as empty by making head as NULL
	int i;
	for (i = 0; i < count; ++i)
		array[i].head = NULL;
}

// Adds an edge to an directed graph
void Graph::addEdge(int src, int dest)
{
	// Add an edge from src to dest.  A new node is added to the adjacency
	// list of src.  The node is added at the begining
	AdjListNode* newNode = new AdjListNode(dest);
	newNode->next = array[src].head;
	array[src].head = newNode;
	// Add new node to nodes list for future deleting
	allListNodes.push_back(newNode);
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

Graph::~Graph()
{
	for (int i = 0; i < allListNodes.size(); i++)
		delete allListNodes[i];
	delete[] array;
}