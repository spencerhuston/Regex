#ifndef EDGE_H
#define EDGE_H

#include <memory>
#include <cstdint>

#include "State.h"

class State;

class Edge
{
	public:
		//character for transition
		uint8_t _c;
		
		//transition is empty
		bool _sigma, _modifiable;

		//states at beginning and end of transition
		std::shared_ptr<State> _in;
		std::shared_ptr<State> _out;

		Edge(uint8_t c, bool sigma, std::shared_ptr<State> in, std::shared_ptr<State> out)
		{
			_c = c;
			_sigma = sigma;
			_in = in;
			_out = out;
		}

		Edge(bool sigma, std::shared_ptr<State> in, std::shared_ptr<State> out)
		{
			_sigma = sigma;
			_in = in;
			_out = out;
		}
};

#endif
