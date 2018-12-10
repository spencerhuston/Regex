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

void Regex::parse(std::vector<Regex::Token> tokens)
{
	Node * start;
	start->state = 1;
	
	Node * current = start;

	for (auto const & token: tokens)
	{
		switch (token.op)
		{
			case Regex::CHARACTER:
			{
				Edge * trans;
				trans->c = token.c;
				trans->in = current;
				
				Node * out;
				trans->out = out;
				current = out;
			}
				break;
			case Regex::STAR:
				break;
			case Regex::PLUS:
				break;
			case Regex::QUESTION:
				break;
			case Regex::OR:
				break;
			case Regex::EXPRESSION:
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return;
		}
	}
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
