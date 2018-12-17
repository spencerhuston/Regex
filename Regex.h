#ifndef REGEX_H
#define REGEX_H

#include <ctype.h>
#include <cstdint>

#include <string>
#include <vector>
#include <stack>

#include <iostream>

#include <memory>

#include "Edge.h"
#include "State.h"
#include "Fragment.h"

// 1.) Scan/tokenize
// 2.) Parse/build NFA
// 3.) Run/simulate NFA with a given string
class Regex
{
	private:
		friend class Analyzer;
	
		enum Ops { STAR, PLUS, QUESTION, 
			   OR, CHARACTER, CAT };

		//Constructed during scanning, used during parsing to make NFA
		struct Token
		{
			//indicates type of token
			Ops op;
			
			//character that generated the token
			uint8_t c;
		};

		//token stream generated during scanning		
		std::vector<Token> _tokens;

	public:
		std::vector<Token> scan(std::string expression);
		std::shared_ptr<State> parse(std::vector<Token> tokens);
		void run(std::shared_ptr<State> start, std::string str);

		//operator parsing
		void character(std::stack< std::shared_ptr<Fragment> > & nfa, const Regex::Token token);
		void concat(std::stack< std::shared_ptr<Fragment> > & nfa);
		void star(std::stack< std::shared_ptr<Fragment> > & nfa);
		void plus(std::stack< std::shared_ptr<Fragment> > & nfa);
		void question(std::stack< std::shared_ptr<Fragment> > & nfa);
		void _or(std::stack< std::shared_ptr<Fragment> > & nfa);

		//helper methods
		void change_edges(std::shared_ptr<Fragment> & f1, std::shared_ptr<Fragment> & f2);
		void change_edges(std::shared_ptr<Fragment> & f2, std::shared_ptr<State> & s2);
		void change_edges(Fragment & f1, Fragment & f2);
		void add_states(std::vector< std::shared_ptr<State> > & nstates, std::shared_ptr<Edge> e, std::vector< std::shared_ptr<State> > & visited);
		
		//inlined
		inline bool contains(const std::vector<uint8_t> matched, const uint8_t c)
		{
			for (auto const & m : matched)
				if (m == c)
					return true;
			return false;
		}

		inline bool contains(const std::vector< std::shared_ptr<State> > nstates, const std::shared_ptr<State> s)
		{
			for (auto const & n : nstates)
				if (n == s)
					return true;
			return false;
		}

		inline void set_tokens(std::vector<Token> tokens)
		{ _tokens = tokens; }

		inline std::vector<Token> get_tokens()
		{ return _tokens; }
};

#endif
