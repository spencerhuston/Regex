#include "Regex.h"

void Regex::scan(std::string expression)
{
	this->expression = expression;

	//check if number is in brace
	//check if previous char was escape character
	bool in_brace = false, escape = false;

	for (char const &c: expression)
	{
		Token t;
	
		if (c == '\\')
		{
			if (escape == false)
			{
				escape = true;
				continue;
			}
			else if (escape == true)
				t.op = Regex::CHARACTER;	
		}
		else if (isalpha(c))
		{
			if (escape)
			{
				std::cout << "Invalid format, cannot escape a letter\n";
				return;
			}

			t.op = Regex::CHARACTER;
		}
		else if (isdigit(c))
		{
			if (escape)
			{
				std::cout << "Invalid format, cannot escape a number\n";
				return;
			}

			//not in a brace (not a counter, therefore does not belong in alphabet)
			if (!in_brace)
				t.op = Regex::NUMBER;
			else
				t.op = Regex::COUNTER;		
			
			t.number = true;
		}
		else if (!escape)
		{
			if (c == '*') t.op = Regex::STAR;
			else if (c == '+') t.op = Regex::PLUS;
			else if (c == '?') t.op = Regex::QUESTION;
			else if (c == '{') 
			{
				t.op = Regex::LEFT_BRACE;
				in_brace = true;
			}	
			else if (c == '}') 
			{
				t.op = Regex::RIGHT_BRACE;
				in_brace = false;
			}	
			else if (c == '^') t.op = Regex::ARROW;	
			else if (c == '$') t.op = Regex::DOLLAR;	
			else if (c == '[') t.op = Regex::LEFT_BRACKET;	
			else if (c == ']') t.op = Regex::RIGHT_BRACKET;
			else if (c == '(') t.op = Regex::LEFT_PAREN;
			else if (c == ')') t.op = Regex::RIGHT_PAREN;	
			else if (c == '|') t.op = Regex::OR;	
			else if (c == ',') t.op = Regex::COMMA;
			else if (c == '-') t.op = Regex::DASH;
		}
		else if (escape)
		{
			t.op = Regex::CHARACTER;

			escape = false;
		}
		
	
		t.c = c;
		tokens.push_back(t);
	}
}

void Regex::parse()
{

}

void Regex::add_alpha(char c)
{
	if (std::find(alphabet.begin(), alphabet.end(), c) == alphabet.end())
		alphabet.push_back(c);
}

void Regex::print_scan()
{
	for (int i = 0; i < alphabet.size(); i++)
		std::cout << alphabet[i];
	std::cout << '\n';

	for (int i = 0; i < tokens.size(); i++)
	{
		std::cout << tokens[i].op;
		if (tokens[i].op == Regex::CHARACTER)
			std::cout << ">" << tokens[i].c;
		else if (tokens[i].op == Regex::NUMBER || tokens[i].op == Regex::COUNTER)
			std::cout << ">" << (int)tokens[i].c - 48;
		std::cout << " ";
	}
	std::cout << '\n';
}
