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

// A recursive function used by topologicalSort
void Graph::topologicalSortUtil(int v, bool visited[], std::stack<int> &Stack) const
{
	// Mark the current node as visited.
	visited[v] = true;

	// Recur for all the vertices adjacent to this vertex
	AdjListNode* pCrawl = array[v].head;
	while (pCrawl)
	{
		if (!visited[pCrawl->dest])
			topologicalSortUtil(pCrawl->dest, visited, Stack);
		pCrawl = pCrawl->next;
	}

	// Push current vertex to stack which stores result
	Stack.push(v);
}

// The function to do Topological Sort. It uses recursive topologicalSortUtil()
void Graph::topologicalSort(std::vector<int> &sortedList) const
{
	std::stack<int> Stack;

	// Mark all the vertices as not visited
	bool *visited = new bool[count];
	for (int i = 0; i < count; i++)
		visited[i] = false;

	// Call the recursive helper function to store Topological Sort
	// starting from all vertices one by one
	for (int i = 0; i < count; i++)
	if (visited[i] == false)
		topologicalSortUtil(i, visited, Stack);

	// Print contents of stack
	while (Stack.empty() == false)
	{
		sortedList.push_back(Stack.top());
		Stack.pop();
	}
	delete[] visited;
}

Graph::~Graph()
{
	for (int i = 0; i < allListNodes.size(); i++)
		delete allListNodes[i];
	delete[] array;
}