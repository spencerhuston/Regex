#ifndef EDGE_H
#define EDGE_H

#include "Node.h"

class Node;

class Edge
{
	public:
		//character for transition
		char c;
		
		//transition is empty
		bool sigma;

		//states at beginning and end of transition
		Node * in;
		Node * out;
};

#endif
