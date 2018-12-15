#include <iostream>
#include <string>

#include "Regex.h"
#include "Format.h"

int main(int argc, char ** argv)
{
	std::string HELP_MSG;
	HELP_MSG += std::string("Usage: ./regex '[regular expression]' '[string to match]' '[2nd string]' ...\n") +
		    	        "Operators:\n" + 
		    	      	"	* 	0 or more times\n" +
		    		"	+ 	1 or more times\n" +
		    		"	? 	0 or 1 time\n" +
		     		"	[x-z] 	x or y or z\n" +
		    		"	| 	or\n" +
		    		"	- 	list alphabet in [ ]\n" +
		    		"	( ) 	surround or's, ex: (a|b)\n" +
				"	\\ 	escape next character\n";

	if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
	{
		std::cout << HELP_MSG;
		return 0;
	}
	else
	{
		Regex regex;
		std::string expr = argv[1];
				
		Format format(expr);
		expr = format.get_expression();

		//get token list from scanner	
		regex.set_tokens(regex.scan(expr));

		std::shared_ptr<State> start;
		start = regex.parse(regex.get_tokens());

		if (start == NULL)
		{
			std::cout << "NFA construction error\n";
			return 1;
		}

		for (int i = 2; i < argc; i++)
			regex.run(start, std::string(argv[i]));
	}

	return 0;
}
