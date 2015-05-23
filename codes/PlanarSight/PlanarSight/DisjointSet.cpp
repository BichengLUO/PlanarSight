#include "DisjointSet.h"

DisjointSet::DisjointSet(int n)
{
	setNum = n;
	for (int i = 0; i < n; i++)
	{
		parentID.push_back(i);
		treeSize.push_back(1);
		maxVal.push_back(i);
	}
}

DisjointSet::~DisjointSet()
{
	parentID.clear();
	treeSize.clear();
	maxVal.clear();
}

int DisjointSet::find(int x)
{
	int root = x;
	int tmp;
	while (root != parentID[root])
		root = parentID[root];
	while (x != root)
	{
		tmp = parentID[x];
		parentID[x] = root;
		x = tmp;
	}
	return root;
}

int DisjointSet::findSetMax(int x)
{
	int root = find(x);
	return maxVal[root];
}

void DisjointSet::link(int x, int y)
{
	int rootX = find(x);
	int rootY = find(y);
	if (rootX == rootY)
		return;

	int max = maxVal[rootX] > maxVal[rootY] ? maxVal[rootX] : maxVal[rootY];
	if (treeSize[rootX] < treeSize[rootY])
	{
		parentID[rootX] = rootY;
		treeSize[rootY] += treeSize[rootX];
		maxVal[rootY] = max;
	}
	else
	{
		parentID[rootY] = rootX;
		treeSize[rootX] += treeSize[rootY];
		maxVal[rootX] = max;
	}
	setNum--;
}