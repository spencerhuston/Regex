#include <iostream>
#include <string>

#include "Regex.h"

int main(int argc, char ** argv)
{
	std::string HELP_MSG;
	HELP_MSG += std::string("Run without argument for interactive mode\n") + 
		    	        "When running with argument, use single quotes around expression\n" +
		    	        "Operators:\n" + 
		    	      	"	* = 0 or more times\n" +
		    		"	+ = 1 or more times\n" +
		    		"	? = 0 or 1 time\n" +
		     		"	{x,y} = x up to y times\n" +
		     		"	[x-z] = x or y or z\n" +
		    		"	| = or\n" +
		   		"	, = list count in { }\n" +
		    		"	- = list alphabet in [ ]\n" +
		    		"	( ) = surround or's, ex: (a|b)\n";

	if (argc == 3)
	{
		Regex regex;
		std::string expr = argv[1];
		std::string str = argv[2];

		if (expr == "help")
			std::cout << HELP_MSG;
		else
		{
			expr = regex.expand_range(expr);
			if (expr.empty())
				return 1;
			regex.set_tokens(regex.scan(expr));
			regex.print_scan(regex.get_tokens());
			std::cout << '\n';
			Node * start = regex.parse(regex.get_tokens());
			regex.run(start, str);
		}
	}

	std::cout << '\n';

	return 0;
}
