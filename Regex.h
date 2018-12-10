#ifndef REGEX_H
#define REGEX_H

#include <ctype.h>

#include <string>
#include <vector>
#include <algorithm>

#include <iostream>

#include "Edge.h"
#include "Node.h"

class Regex
{
	private:
		// 0 *
		// 1 +
		// 2 ?
		// 3 |
		// 4 [a-zA-Z0-9]
		// 5 concatenation: ab = a then b
		// 6 another expression
		enum Ops { STAR, PLUS, QUESTION, 
			   OR, CHARACTER, EXPRESSION };

		struct Token
		{
			Ops op;
			char c;
			
			//used to convert from raw ascii since all characters are stored as char
			bool number;

			std::vector<Token> expression;
		};

		std::string _expression;
		
		std::vector<Token> _tokens;
		std::vector<Node> states;

	public:
		std::string expand_range(std::string expression);
		std::vector<Token> scan(std::string expression);
		void parse(std::vector<Token> tokens);

		//helper methods
		void print_scan(std::vector<Token> tokens);

		inline void set_tokens(std::vector<Token> tokens)
		{ _tokens = tokens; }

		inline std::vector<Token> get_tokens()
		{ return _tokens; }
};

#endif
