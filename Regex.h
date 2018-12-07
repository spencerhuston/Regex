#ifndef REGEX_H
#define REGEX_H

#include <ctype.h>

#include <string>
#include <vector>
#include <algorithm>

#include <iostream>

class Node;

#include "Edge.h"
#include "Node.h"

class Regex
{
	private:
		// 0 *
		// 1 +
		// 2 ?
		// 3 {
		// 4 }
		// 5 ^
		// 6 $
		// 7 [
		// 8 ]
		// 9 |
		// 10 ,
		// 11 -
		// 12 [a-zA-Z] (letter, in alphabet)
		// 13 [0-9] (number, in alphabet)
		// 14 {0-9} (counter, not in alphabet)
		// 15 (
		// 16 )
		enum Ops { STAR, PLUS, QUESTION, LEFT_BRACE, RIGHT_BRACE, 
			   ARROW, DOLLAR, LEFT_BRACKET, RIGHT_BRACKET, 
			   OR, COMMA, DASH, CHARACTER, NUMBER, COUNTER,
	       		   LEFT_PAREN, RIGHT_PAREN }; 
		
		struct Token
		{
			Ops op;
			char c;
			
			//used to convert from raw ascii since all characters are stored as char
			bool number;
		};

		std::string expression;
		
		std::vector<char> alphabet;
		std::vector<Token> tokens;
		std::vector<Node> states;

	public:
		void scan(std::string expression);
		void parse();

		//helper methods
		void add_alpha(char c);
		void print_scan();
};

#endif
