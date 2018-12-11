#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Edge.h"

class Edge;

class Node
{
	public:
		//do I really need this?
		int state;

		//indicates the current state is a possible end (used during NFA run)
		bool last;

		//list of edges to next possible states
		std::vector<Edge *> edges;

		//previous and next states
		Node * prev, * next;
};

#endif
