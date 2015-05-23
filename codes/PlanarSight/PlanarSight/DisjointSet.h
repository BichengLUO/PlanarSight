// 定义不相交集的数据机构以及相应的功能

#pragma once
#include "Point.h"

class DisjointSet
{
public:
	IntArray parentID;
	IntArray treeSize;
	IntArray maxVal;
	int setNum;

public:
	DisjointSet(int n);
	~DisjointSet();

	int find(int x);
	int findSetMax(int x);
	void link(int x, int y);
};