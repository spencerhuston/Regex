#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Edge.h"

class Edge;

class Node
{
	public:
		int state;
		bool last;
		std::vector<Edge *> edges;
		Node * prev;
};

#endif
