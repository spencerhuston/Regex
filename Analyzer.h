#ifndef ANALYZER_H
#define ANALYZER_H

#include <cstdint>
#include <memory>

#include "Regex.h"

class Analyzer
{
	public:
		struct Node
		{
			uint8_t c;

			std::shared_ptr<Node> _left;
			std::shared_ptr<Node> _right;
			std::shared_ptr<Node> _parent;	
		};

		std::shared_ptr<Node> _root;

		void build_tree(std::vector<Regex::Token> tokens);
		void analyze();		
};

#endif
