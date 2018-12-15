#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "State.h"

class Fragment
{
	public:
		std::shared_ptr<State> _start;
		std::vector< std::shared_ptr<Edge> > * _edges;

		inline Fragment(std::shared_ptr<State> start)
		{ _start = start; }
};

#endif
