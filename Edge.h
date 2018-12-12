#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <cstdint>

#include "Node.h"

class Node;

class Edge
{
	public:
		//character for transition
		uint8_t c;
		
		//transition is empty
		bool sigma;

		//states at beginning and end of transition
		std::shared_ptr<Node> in;
		std::shared_ptr<Node> out;
};

#endif
