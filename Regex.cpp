#include "Regex.h"

// Scans the re-formatted expression and tokenizes it, also deals with escaped characters
//	
//	param: std::string expression, the re-formatted one
//
//	return: std::vector<Regex::Token>, token stream of the given expression
//
std::vector<Regex::Token> Regex::scan(std::string expression)
{
	std::vector<Token> tokens;

	bool escape = false;

	for (int i = 0; i < expression.length(); i++)
	{
		Token t;

		uint8_t c = expression[i]; 

		if (c == '\\')
		{
			if (!escape)
			{
				escape = true;
				continue;
			}
		} //else match corresponding token
		else if (c == '*') t.op = Regex::STAR;
		else if (c == '+') t.op = Regex::PLUS;
		else if (c == '?') t.op = Regex::QUESTION;
		else if (c == '|') t.op = Regex::OR;
		else if (c == '.') t.op = Regex::CAT;
		else if (isalnum(c))
		{
			t.op = Regex::CHARACTER;
			t.number = (isdigit(c)) ? true : false;
		}
		
		//if escaped character override token type to be a CHARACTER
		if (escape)
		{
			t.op = Regex::CHARACTER;
			escape = false;
		}

		t.c = c;
		tokens.push_back(t);	
	}

	return tokens;
}

// Parses the token stream generated during scanning and constructs a corresponding NFA
// roughly according to Thompson's construction algorithm
//
//	params: std::vector<Regex::Token> tokens, the token stream made during scanning
//		Node * start, the starting state (used for recursive calls for sub-expressions)
//		bool is_sub, indicates whether current expression is a sub-expression, makes sure end-state is not marked as a matching state
//
//	return: Node *, returns the end-state of the current expression (also used for sub-expressions)
//
std::shared_ptr<Node> Regex::parse(std::vector<Regex::Token> tokens)
{
	std::shared_ptr<Node> start;

	for (auto const & token: tokens)
	{
		switch (token.op)
		{
			case Regex::CHARACTER:
			{

			}
				break;
			case Regex::CAT:
			{

			}
				break;
			case Regex::STAR:
			{

			}
				break;
			case Regex::PLUS:
			{

			}
				break;
			case Regex::QUESTION:
			{

			}
				break;
			case Regex::OR:
			{

			}
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return NULL;
		}
	}
	
	return start;
}

// Simulates the NFA with a given string to match
//	
//	params: Node * start, the starting state of the NFA
//		std::string str, the string to match
//
//	return: none, void
//	
void Regex::run(std::shared_ptr<Node> start, std::string str)
{
	int i = 0;

	while (start)
	{
		//matched if no more of input string to read
		//and the current state is a match
		if (i == str.length() && start->last)
		{
			std::cout << "Match\n";
			return;
		}

		//checks all edges against current character in input string
		bool match = false;
		for (auto const & edge: start->edges)
		{
			if (edge->c == str[i] || edge->sigma)
			{
				match = true;
				start = edge->out;
				
				//if empty transition don't advance in the input string
				if (!(edge->sigma)) i++;

				break;	
			}
		}

		if (!match)
		{
			std::cout << "No match\n";
			return;
		}
	}	
	std::cout << "Match\n";
}

//used for testing purposes for scanning phase
void Regex::print_scan(std::vector<Regex::Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].op == Regex::CHARACTER)
		{
			std::cout << tokens[i].op << '>';
			if (tokens[i].number)
				std::cout << (int)(tokens[i].c) - 48 << ' ';
			else
				std::cout << tokens[i].c << ' ';
		}
		else
			std::cout << tokens[i].op << ' ';
	}
}
