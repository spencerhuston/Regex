#ifndef NODE_H
#define NODE_H

#include <vector>

class Node
{
	public:
		std::vector<Edge *> edges;
		int state;
};

#endif
