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
				f->_end = NULL;

				nfa.push(f);
			}
				break;
			case Regex::CAT:
			{
				std::shared_ptr<Fragment> f2(nfa.top());
				nfa.pop();
				std::shared_ptr<Fragment> f1(nfa.top());
				nfa.pop();
		
				if (f1->_edges->size() == 0)
				{
					std::shared_ptr<Edge> e(new Edge(true, f1->_end, f2->_start));
					f1->_edges->push_back(e);
				}
				else
					for (auto const & e : *(f1->_edges))
						e->_out = f2->_start;
				
				std::shared_ptr<Fragment> f(new Fragment(f1->_start));
				f->_edges = f2->_edges;
				f->_end = (f2->_end == NULL) ? NULL : f2->_end;

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
				std::shared_ptr<Fragment> f1(nfa.top());
				nfa.pop();
				
				std::shared_ptr<State> f2_start(new State());
				std::shared_ptr<Fragment> f2(new Fragment(f2_start));
				
				std::shared_ptr<State> s(new State());
				std::shared_ptr<Edge> e1(new Edge(true, s, f1->_start));
				std::shared_ptr<Edge> e2(new Edge(true, s, f2->_start));
				
				s->_edges.push_back(e1), s->_edges.push_back(e2);

				std::shared_ptr<Fragment> f(new Fragment(s));
				
				std::shared_ptr<State> s2(new State());
					
				if (f1->_edges->size() == 0)
				{
					std::shared_ptr<Edge> f1_e(new Edge(true, f1->_end, s2));	
					f1->_edges->push_back(f1_e);
				}
				else
					for (auto const & e : *(f1->_edges))
						e->_out = s2;
			
				std::shared_ptr<Edge> f2_e(new Edge(true, f2_start, s2));	
				f2_start->_edges.push_back(f2_e);
				f2->_edges = &f2_start->_edges;

				f->_end = s2;
				f->_edges = &s2->_edges;

				nfa.push(f);
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
				
				if (f1->_edges->size() == 0)
				{
					std::shared_ptr<Edge> f1_e(new Edge(true, f1->_end, s2));	
					f1->_edges->push_back(f1_e);
				}
				else
					for (auto const & e : *(f1->_edges))
						e->_out = s2;
			
				if (f2->_edges->size() == 0)
				{
					std::shared_ptr<Edge> f2_e(new Edge(true, f2->_end, s2));
					f2->_edges->push_back(f2_e);
				}
				else
					for (auto const & e : *(f2->_edges))
						e->_out = s2;	
					
				f->_end = s2;
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

	if (end->_end == NULL)
	{
		std::shared_ptr<State> last(new State());
		last->_match = true;

		for (auto const & e : *(end->_edges))
			e->_out = last;
	}
	else
		end->_end->_match = true;

	return end->_start;
}

void Regex::add_states(std::vector< std::shared_ptr<State> > & nstates, std::shared_ptr<Edge> e)
{
	if (e->_out->_edges.size() == 0)
	{
		nstates.push_back(e->_out);
		return;
	}

	for (auto const & e2 : e->_out->_edges)
	{
		if (e2->_sigma)	
			add_states(nstates, e2);
		else
			nstates.push_back(e2->_in);
	}
}

bool Regex::contains(const std::vector<uint8_t> matched, const uint8_t c)
{
	for (auto const & m : matched)
		if (m == c)
			return true;
	return false;
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
	std::vector< std::shared_ptr<State> > cstates;
	std::vector< std::shared_ptr<State> > nstates;
	std::vector<uint8_t> matched;

	cstates.push_back(start);

	int i = 0;
	while (i < str.length())
	{
		uint8_t c = str[i];

		bool found = false;

		for (auto const & s : cstates)
			for (auto const & e : s->_edges)
			{
				if (e->_c == c)
				{
					nstates.push_back(e->_out);
					found = true;

					if (!contains(matched, c))
						matched.push_back(c), i++;
				}
				else if (e->_sigma)
				{
					add_states(nstates, e);
					found = true;
				}
			}

		if (!found)
		{
			std::cout << "No match\n";
			return;
		}
		else
		{
			cstates = nstates;
			nstates.clear();
			matched.clear();
		}
	}
		
	for (auto const & s : cstates)
		for (auto const & e : s->_edges)
			if (e->_sigma)
				add_states(cstates, e);

	for (auto const & s : cstates)
		if (s->_match)
		{
			std::cout << "Match\n";
			return;
		}

	std::cout << "No match\n";
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
