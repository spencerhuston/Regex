#ifndef NODE_H
#define NODE_H

#include <vector>
#include <memory>

#include "Edge.h"

class Edge;

class State
{
	public:
		bool _match;
		std::vector< std::shared_ptr<Edge> > _edges;

	inline State() {}
};

#endif
