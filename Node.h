#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Edge.h"

class Edge;

class Node
{
	public:
		int state;
		std::vector<Edge *> trans;
		Node * prev;
};

#endif
