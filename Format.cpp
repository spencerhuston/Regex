#include "Format.h"

Format::Format(std::string expression)
{
	_expression = expression;
	_expression = expand_range(_expression);
	_expression = add_cat(_expression);
	_expression = in_to_post(_expression);
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

std::string Format::add_cat(std::string expression)
{
	std::string new_expr;
	std::vector<uint8_t> ops = {'|', '?', '+', '*'};
	
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

