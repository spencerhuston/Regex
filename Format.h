#ifndef FORMAT_H
#define FORMAT_H

#include <ctype.h>

#include <string>
#include <vector>
#include <stack>
#include <map>

#include <algorithm>

#include <iostream>

// Shunting-yard algorithm implementation with extra preprocessing 
// for [ - ] and special escaped characters (in Format::expand_range)
class Format
{
	private:
		std::string _expression;
		std::map<uint8_t, int> precedence = {{'(', 1}, {'|', 2}, {'.', 3}, 
						     {'?', 4}, {'*', 5}, {'+', 6}};
	public:
		Format(std::string expression);
		std::string expand_range(std::string expression);
		std::string add_cat(std::string expression);
		std::string in_to_post(std::string expression);

		//helper methods
		bool expand_special(std::string & new_expr, uint8_t c);

		inline uint8_t get_precedence(uint8_t c)
		{ return (precedence.find(c) == precedence.end()) ? 6 : precedence.find(c)->second; }

		inline const std::string get_expression()
		{ return _expression; }
};

#endif
