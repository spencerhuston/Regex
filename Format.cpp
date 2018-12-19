#include "Format.h"

Format::Format(std::string expression)
{
	_expression = expression;
	_expression = expand_range(_expression);
	_expression = add_cat(_expression);
	_expression = in_to_post(_expression);
}

bool Format::expand_special(std::string & new_expr, const uint8_t c)
{
	int start = 0, end = 0;
	bool ns = true;

	std::vector<uint8_t> res({'?', '*', '+', '|', '[', ']', '-', '(', ')'});

	if (std::find(res.begin(), res.end(), c) != res.end())
	{
		new_expr += '\\';
		new_expr += c;
		return true;
	}

	switch (c)
	{
		case 'l': //lowercase
			start = 97, end = 122;
			break;
		case 'L': //uppercase
			start = 65, end = 90;
			break;
		case 's': //whitespace
			ns = false;
			new_expr += '(';

			for (int j = 9; j <= 13; j++)
			{
				new_expr += (uint8_t)j;
				new_expr += '|';
			}

			new_expr += (uint8_t)32;
			
			break;
		case 'S': //no whitespace (visible characters)
			ns = false;

			new_expr += '(';

			for (int j = 33; j < 126; j++)
			{
				if (std::find(res.begin(), res.end(), j) != res.end())
					new_expr += '\\';
			
				new_expr += (uint8_t)j;
				new_expr += '|';
			}

			new_expr += (uint8_t)126;

			break;
		case 'd': //digits
			start = 48, end = 57; 
			break;
		case 'D': //no digits
			ns = false;
				
			break;
		case 'w': //alphanumeric
			ns = false;
	
			new_expr += '(';

			for (int j = 48; j <= 57; j++)
			{
				new_expr += (uint8_t)j;
				new_expr += '|';
			}

			for (int j = 65; j <= 90; j++)
			{
				new_expr += (uint8_t)j;
				new_expr += '|';
			}

			for (int j = 97; j < 122; j++)
			{
				new_expr += (uint8_t)j;
				new_expr += '|';
			}

			new_expr += (uint8_t)122;
			
			break;
		case 'W': //no alphanumeric
			break;
		case 't': //newline
			new_expr += '(';
			new_expr += (uint8_t)9;
			ns = false;
			break;
		case 'n': //tab
			new_expr += '(';
		       	new_expr += (uint8_t)10;
			ns = false;
			break;
		case 'v':
			new_expr += '(';
			new_expr += (uint8_t)11;
			ns = false;
			break;
		case 'f':
			new_expr += '(';
			new_expr += (uint8_t)12;
			ns = false;
			break;
		case 'r':
			new_expr += '(';
			new_expr += (uint8_t)13;
			ns = false;
			break;
		default:
			return false;
			break;
	}

	if (ns) 
	{
		new_expr += '(';

		for (int j = start; j < end; j++)
		{
			new_expr += (uint8_t)j;
			new_expr += '|';
		}

		new_expr += (uint8_t)end;
	}

	new_expr += ')';
	
	return true;
}

bool Format::matches(const uint8_t left, const uint8_t right)
{
	if (isalpha(left) && isalpha(right) && right > left)
	{
		if (isupper(left) && isupper(right))
			return true;
		else if (islower(left) && islower(right))
			return true;
	}
	else if (isdigit(left) && isdigit(right) && left < right)
			return true;

	return false;
}

// expands [x-z] into (x|y|z), makes single pass scanning possible
//
//      params: std::string expression, takes in the original expression to re-format for actual scanner
//
//      return: std::string, returns the re-formatted version of the original input string
//
std::string Format::expand_range(std::string expression)
{
        std::string new_expr;

        //stores what's inside the square brackets
        std::vector<uint8_t> local;
        
	bool in_bracket = false;

        for (int i = 0; i < expression.length(); i++)
        {
                uint8_t c = expression[i];

		if (i + 1 < expression.length() && expression[i + 1] == '{')
			continue;
		else if (c == '[')
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

			if (!matches(left, right))
			{
				std::cout << "Incorrect range: " << left << '-' << right << '\n';
				exit(1);
			}	

                        for (int j = (int)left; j <= (int)right; j++)
                                local.push_back((uint8_t)j);
                }
		else if (c == '{')
		{
			uint8_t expr;
			uint16_t start_expr;

			if (i - 1 >= 0)
				expr = expression[i - 1], start_expr = i - 1;
			else
			{
				std::cout << "Repetition bracket error\n";
				exit(1);
			}
			
			i++;

			uint16_t start = 0, end = 0;
			if (isdigit(expression[i]) && i + 1 < expression.length() && expression[i + 1] == ',')
				start = expression[i] - 48;
			else
			{
				std::cout << "Repetition bracket error\n";
				exit(1);
			}

			if (i + 3 < expression.length() && expression[i + 3] == '}' && isdigit(expression[i + 2]))
			{
				i += 2;
				end = expression[i] - 48;
				i++;
			}
			else
			{
				std::cout << "Repetition bracket error\n";
				exit(1);
			}

			if (expr == ')')
			{
				uint16_t parens = 0, start_i = start_expr;

				while (start_expr - 1 >= 0)
				{
					if (expression[start_expr] == ')') parens++;
					else if (expression[start_expr] == '(') parens--;

					if (parens == 0 && start_expr != start_i)
						break;	

					start_expr--;	
				}

				std::string rep;
				rep = expression.substr(start_expr + 1, start_i - start_expr - 1);
				new_expr = new_expr.substr(0, start_expr);	

				for (int j = 0; j < start; j++)
					new_expr += std::string("(") + rep + ")";
				for (int j = 0; j < end - start; j++)
					new_expr += std::string("(") + rep + ")?";
			}
			else
			{
				for (int j = 0; j < start; j++)
					new_expr += expr;
				for (int j = 0; j < end - start; j++)
				{
					new_expr += expr;
					new_expr += '?';
				}
			}
		}
	      	else if (c == '\\' && i + 1 < expression.length())
		{
			if (expand_special(new_expr, (uint8_t)expression[i + 1])) 
				i++;
         	}
		else if (!in_bracket)
                        new_expr += c;
        }

        return new_expr;
}

std::string Format::add_cat(std::string expression)
{
	std::string new_expr;
	std::vector<uint8_t> ops = {'|', '?', '+', '*', '^', '$'};
	
	for (int i = 0; i < expression.length(); i++)
	{
		uint8_t c = expression[i];

		if (i + 1 < expression.length())
		{
			uint8_t c2 = expression[i + 1];

			new_expr += c;

			bool found = (std::find(ops.begin(), ops.end(), c2) != ops.end());
			if (c != '(' && c2 != ')' && c != '|' && !found)
				new_expr += '.';
		}
	}	

	new_expr += expression[expression.length() - 1];

	return new_expr;
}

std::string Format::in_to_post(std::string expression)
{
	std::string new_expr;
	std::stack<uint8_t> stack;

	for (auto const & c : expression)
	{
		switch (c)
		{
			case '(':
				stack.push(c);
				break;
			case ')':
				while (stack.top() != '(')
					new_expr += stack.top(), stack.pop();
				stack.pop();
				break;
			default:
				while (stack.size() > 0)
				{
					if (get_precedence(stack.top()) >= get_precedence(c))
						new_expr += stack.top(), stack.pop();
					else
						break;
				}

				stack.push(c);
				break;
		}
	}

	while (stack.size() > 0)
		new_expr += stack.top(), stack.pop();

	return new_expr;
}

