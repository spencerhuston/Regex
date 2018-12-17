#include "Analyzer.h"

void Analyzer::build_tree(std::vector<Regex::Token> tokens)
{
	for (auto const & t : tokens)
	{
		switch (t.op)
		{
			case Regex::CHARACTER:
				break;
			case Regex::CAT:
				break;
			case Regex::STAR:
				break;
			case Regex::PLUS:
				break;
			case Regex::QUESTION:
				break;
			case Regex::OR:
				break;
			default:
				std::cout << "Analysis error\n";
				return;
		}
	}
}

void Analyzer::analyze()
{

}
