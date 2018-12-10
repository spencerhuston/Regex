#include "Regex.h"

//expands [ - ]
std::string Regex::expand_range(std::string expression)
{
	std::string new_expr;
	std::vector<char> local;
	bool in_bracket = false;

	for (int i = 0; i < expression.length(); i++)
	{
		char c = expression[i];

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
			char left = expression[i - 1];
			char right = expression[i + 1];
					
			for (int j = (int)left; j <= (int)right; j++)
				local.push_back((char)j);	
		}
		else if (!in_bracket)
			new_expr += c;
	}

	std::cout << new_expr << '\n';

	return new_expr;
}

std::vector<Regex::Token> Regex::scan(std::string expression)
{
	std::vector<Token> tokens;

	bool escape = false;

	for (int i = 0; i < expression.length(); i++)
	{
		Token t;

		char c = expression[i]; 
	
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
		}
		else if (c == '*') t.op = Regex::STAR;
		else if (c == '+') t.op = Regex::PLUS;
		else if (c == '?') t.op = Regex::QUESTION;
		else if (c == '|') t.op = Regex::OR;
		else if (isalnum(c))
		{
			t.op = Regex::CHARACTER;
			t.number = (isdigit(c)) ? true : false;
		}
		
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

Node * Regex::parse(std::vector<Regex::Token> tokens)
{
	Node * start = new Node();
	start->state = 1;
	
	Node * current = start;

	int index = 0;

	for (auto const & token: tokens)
	{
		switch (token.op)
		{
			case Regex::CHARACTER:
			{
				Edge * trans = new Edge();
				trans->c = token.c;
				trans->in = current;
				trans->in->edges.push_back(trans);
				
				if (index != 0 && tokens[index - 1].op == Regex::OR)
				{
					trans->out = current->edges[current->edges.size() - 2]->out;	
					current = trans->out;
				}
				else
				{
					Node * out = new Node();
					out->state = (current->state)++;
					trans->out = out;
					current = out;
				}
				
				current->prev = trans->in;
			}
				break;
			case Regex::STAR:
			{
				//cycle back to itself
				current = current->prev;
				current->edges[current->edges.size() - 1]->out = current;
				
				Node * out = new Node();
				Edge * zero = new Edge();
				zero->in = current;
				zero->out = out;
				zero->sigma = true;

				current->edges.push_back(zero);

				out->prev = current;
				current = out;
			}
				break;
			case Regex::PLUS:
			{
				Edge * trans = new Edge();
				trans->c = current->prev->edges[current->prev->edges.size() - 1]->c;
				trans->in = current, trans->out = current;

				current->edges.push_back(trans);
			}
				break;
			case Regex::QUESTION:
			{
				Edge * zero = new Edge();
				zero->sigma = true, zero->out = current, zero->in = current->prev;
				current->prev->edges.push_back(zero);		
			}
				break;
			case Regex::OR:
				current = current->prev;
				break;
			case Regex::EXPRESSION:
			{
					
			}
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return NULL;
		}

		index++;
	}

	current->last = true;

	return start;
}

void Regex::run(Node * start, std::string str)
{
	int i = 0;

	while (start)
	{
		if (i == str.length() && start->last)
		{
			std::cout << "Match\n";
			return;
		}

		bool match = false;
		for (auto const & edge: start->edges)
		{
			if (edge->c == str[i] || edge->sigma)
			{
				match = true;
				start = edge->out;
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
