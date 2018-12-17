#ifndef ANALYZER_H
#define ANALYZER_H

#include <cstdint>
#include <memory>
#include <stack>

#include "Regex.h"

// Optimizes and fixes the token stream using parse tree
class Analyzer
{
	private:
		//tree node
		struct Node
		{
			//character and precedence of matching token
			uint8_t _c;
			uint16_t _prec;

			std::shared_ptr<Node> _left;
			std::shared_ptr<Node> _right;
			std::shared_ptr<Node> _parent;	
		};

		std::shared_ptr<Node> _root;
		std::vector<Regex::Token> _tokens;

	public:
		Analyzer(std::vector<Regex::Token> tokens);
		void build_tree(std::vector<Regex::Token> tokens);
		void remake_tokens(std::shared_ptr<Node> node, std::vector<Regex::Token> & tokens);
		void analyze();	

		//helper methods
		void preorder(std::shared_ptr<Node> node);
		void check_prec(std::shared_ptr<Node> node);
		inline std::vector<Regex::Token> get_tokens()
		{ return _tokens; }
};

#endif
