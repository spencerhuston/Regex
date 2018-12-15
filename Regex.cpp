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
//		Node * start, the starting state
//
//	return: none, void
//
std::shared_ptr<State> Regex::parse(std::vector<Regex::Token> tokens)
{
	std::stack< std::shared_ptr<Fragment> > nfa; 

	for (auto const & token : tokens)
	{
		switch (token.op)
		{
			case Regex::CHARACTER:
			{		
				std::shared_ptr<State> in(new State());
				std::shared_ptr<Edge> e(new Edge(token.c, false, in, NULL));

				in->_edges.push_back(e);

				std::shared_ptr<Fragment> f(new Fragment(in));
				f->_edges = &in->_edges;

				nfa.push(f);
			}
				break;
			case Regex::CAT:
			{
				std::shared_ptr<Fragment> f2(nfa.top());
				nfa.pop();
				std::shared_ptr<Fragment> f1(nfa.top());
				nfa.pop();
			
				for (auto const & e : *(f1->_edges))
					e->_out = f2->_start;
				
				std::shared_ptr<Fragment> f(new Fragment(f1->_start));
				f->_edges = f2->_edges;

				nfa.push(f);
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
				std::shared_ptr<Fragment> f2(nfa.top());
				nfa.pop();
				std::shared_ptr<Fragment> f1(nfa.top());
				nfa.pop();

				std::shared_ptr<State> s(new State());
				std::shared_ptr<Edge> e1(new Edge(true, s, f1->_start));
				std::shared_ptr<Edge> e2(new Edge(true, s, f2->_start));
				
				s->_edges.push_back(e1), s->_edges.push_back(e2);

				std::shared_ptr<Fragment> f(new Fragment(s));
				
				std::shared_ptr<State> s2(new State());

				for (auto const & e : *(f1->_edges))
					e->_out = s2;
	
				for (auto const & e : *(f2->_edges))
					e->_out = s2;				
				
				f->_edges = &s2->_edges;
				nfa.push(f);
			}
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return NULL;
		}
	}

	std::shared_ptr<Fragment> end(nfa.top());
	nfa.pop();

	std::shared_ptr<State> last(new State());
	last->_match = true;

	for (auto const & e : *(end->_edges))
		e->_out = last;

	return end->_start;
}

// Simulates the NFA with a given string to match
//	
//	params: Node * start, the starting state of the NFA
//		std::string str, the string to match
//
//	return: none, void
//	
void Regex::run(std::shared_ptr<State> start, std::string str)
{
	for (int i = 0; i < str.length(); i++)
	{
		uint8_t c = str[i];

		bool found = false;		
		
		for (auto const & e : start->_edges)
		{
			if (e->_c == c || (e->_c != c && e->_sigma))
			{
				start = e->_out;
				found = true;
				if (e->_sigma) i--;
				break;
			}	
		}

		if (!found)
		{
			std::cout << "No match\n";
			return;
		}
	}

	if (start->_edges.size() == 0 && start->_match)
	{
		std::cout << "Match\n";
		return;
	}
	else
	{
		std::cout << "No match\n";
		return;
	}
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

void Regex::print_nfa(std::shared_ptr<State> start)
{
	int state = 0;
	while (start->_edges.size() && !start->_match)
	{
		std::cout << state++;
		for (auto const & e : start->_edges)
		{
			std::cout << "-" << e->_c << "->";
		}
		start = start->_edges[0]->_out;
	}
	std::cout << '\n';
}
