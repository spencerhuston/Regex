#include "Regex.h"

// expands [x-z] into (x|y|z), makes single pass scanning possible
//
//	params:	std::string expression, takes in the original expression to re-format for actual scanner
//
//	return:	std::string, returns the re-formatted version of the original input string
//
std::string Regex::expand_range(std::string expression)
{
	std::string new_expr;

	//stores what's inside the square brackets
	std::vector<uint8_t> local;
	bool in_bracket = false;

	for (int i = 0; i < expression.length(); i++)
	{
		uint8_t c = expression[i];

		if (c == '[')
		{
			if (in_bracket)
			{
				std::cout << "Cannot have nested brackets\n";
				return "";
			}

			in_bracket = true;
			local.clear();	

			new_expr += '(';
		}
		else if (c == ']')
		{
			if (!in_bracket)
			{
				std::cout << "Missing \'[\'\n";
				return "";
			}

			//end of [x-z] hit, fills new expression with x|y|z)
			for (int j = 0; j < local.size() - 1; j++)
			{
				new_expr += local[j];
				new_expr += '|';
			}

			new_expr += local[local.size() - 1];
			new_expr += ')';

			in_bracket = false;
		}
		else if (c == '-' && in_bracket && (i >= 2 || expression.length() - i >= 2))
		{
			//gets range of characters in [x-z] and stores them in vector for use when ] is hit	
			uint8_t left = expression[i - 1];
			uint8_t right = expression[i + 1];
					
			for (int j = (int)left; j <= (int)right; j++)
				local.push_back((uint8_t)j);	
		}
		else if (!in_bracket)
			new_expr += c;
	}

	return new_expr;
}

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

		//skips over because it is handled when ( is hit	
		if (c == ')')
			continue;

		if (c == '\\')
		{
			if (!escape)
			{
				escape = true;
				continue;
			}
		}
		else if (c == '(')
		{	
			int parens = 0;
		
			//finds matching ) and creates sub-stream for tokens inside the subexpression	
			for (int j = i; j < expression.length(); j++)
			{
				if (expression[j] == '(') parens++;
				else if (expression[j] == ')') parens--;
				
				if (j != i && parens == 0)
				{
					t.expression = scan(expression.substr(i + 1, j - i - 1));			
					t.op = Regex::EXPRESSION;
					
					i = j;

					break;
				}
			}
		} //else match corresponding token
		else if (c == '*') t.op = Regex::STAR;
		else if (c == '+') t.op = Regex::PLUS;
		else if (c == '?') t.op = Regex::QUESTION;
		else if (c == '|') t.op = Regex::OR;
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
std::shared_ptr<Node> Regex::parse(std::vector<Regex::Token> tokens, std::shared_ptr<Node> start, bool is_sub)
{
	std::shared_ptr<Node> current = start;

	int index = 0;

	for (auto const & token: tokens)
	{
		switch (token.op)
		{
			//constructs new transition edge and new state and connects the two
			case Regex::CHARACTER:
			{
				std::shared_ptr<Edge> trans(new Edge());
				trans->c = token.c;
				trans->in = current;
				trans->in->edges.push_back(trans);
				
				//if previous token was an OR, match the 
				if (index != 0 && tokens[index - 1].op == Regex::OR)
				{
					if (index > 1 && tokens[index - 2].op == Regex::EXPRESSION)
					{
						trans->out = current->next;
						current = trans->out;	
					}
					else
					{
						trans->out = current->edges[current->edges.size() - 2]->out;	
						current = trans->out;
					}
				}
				else
				{
					std::shared_ptr<Node> out(new Node());
					trans->out = out;
					current->next = out;
					current = out;
				}
				
				current->prev = trans->in;
			}
				break;
			//takes previous edge from concatenation (by default) and connects it to its own starting state
			//then constructs new empty transition and new state and connects those
			case Regex::STAR:
			{
				//cycle back to itself
				current = current->prev;

				if (index > 0 && tokens[index - 1].op == Regex::EXPRESSION)
				{
					std::shared_ptr<Node> temp(current);
					while (temp->edges[0]->out != current->next)
						temp = temp->edges[0]->out;
					temp->edges[0]->out = current;
				}
				else
					current->edges[current->edges.size() - 1]->out = current;

				std::shared_ptr<Node> out(new Node());
				std::shared_ptr<Edge> zero(new Edge());
				zero->in = current;
				zero->out = out;
				zero->sigma = true;

				current->edges.push_back(zero);

				out->prev = current;
				current = out;
			}
				break;
			//Constructs new edge that cycles to current state
			//The *1 or more* aspect is done by previous concatenation done by default
			case Regex::PLUS:
			{
				if (tokens[index - 1].op != Regex::EXPRESSION)
				{
					std::shared_ptr<Edge> trans(new Edge());
	
					trans->c = current->prev->edges[current->prev->edges.size() - 1]->c;
					trans->in = current, trans->out = current, current->next = current;
				
					current->edges.push_back(trans);
				}
			}
				break;
			//functions the same as OR but constructs new empty transition
			case Regex::QUESTION:
			{
				std::shared_ptr<Edge> zero(new Edge());
				zero->sigma = true, zero->out = current, zero->in = current->prev;
				current->prev->edges.push_back(zero);		
			}
				break;
			//goes back a state, the OR is checked by a concatenation and new edge is matched with
			//original and next states of other character transition from first part of OR
			case Regex::OR:
				current = current->prev;
				break;
			//still working on
			case Regex::EXPRESSION:
			{
				std::shared_ptr<Node> expr_start(current);
				current = parse(token.expression, current, true);			

				current->prev = expr_start;
				current->prev->next = current;

				if (index + 1 < tokens.size() - 1 && tokens[index + 1].op == Regex::PLUS)
				{
					std::shared_ptr<Node> extra(current);
					current = parse(token.expression, current, true);
						
					current->prev = extra;
					current->prev->next = current;

					std::shared_ptr<Node> temp(extra);
					while (temp->edges[0]->out != current)
						temp = temp->edges[0]->out;
					temp->edges[0]->out = extra;

				}
			}
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return NULL;
		}

		index++;
	}

	if (!is_sub)
		current->last = true;

	return current;
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
	std::cout << '(' << ' ';
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].op == Regex::EXPRESSION)
			print_scan(tokens[i].expression);
		else if (tokens[i].op == Regex::CHARACTER)
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
	std::cout << ')' << ' ';
}
