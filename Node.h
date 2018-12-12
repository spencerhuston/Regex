#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>

#include "Edge.h"

class Edge;

class Node
{
	public:
		//indicates the current state is a possible end (used during NFA run)
		bool last;

		//list of edges to next possible states
		std::vector< std::shared_ptr<Edge> > edges;

		//previous and next states
		std::shared_ptr<Node> prev;
	        std::shared_ptr<Node> next;
};

#endif
