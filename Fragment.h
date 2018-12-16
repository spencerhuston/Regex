#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "State.h"

class Fragment
{
	public:
		//Fragment start and end states
		std::shared_ptr<State> _start;
		std::shared_ptr<State> _end;

		//edges from the end of the fragment
		std::vector< std::shared_ptr<Edge> > * _edges;
		
		inline Fragment() {}

		inline Fragment(std::shared_ptr<State> start)
		{ _start = start; }
};

#endif
