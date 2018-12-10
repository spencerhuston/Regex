#ifndef EDGE_H
#define EDGE_H

#include "Node.h"

class Node;

class Edge
{
	public:
		char c;
		bool sigma;

		Node * in;
		Node * out;
};

#endif
