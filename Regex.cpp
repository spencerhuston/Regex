#include "Regex.h"

// Scans the re-formatted expression and tokenizes it, also deals with escaped characters
//	
//	param: std::string expression, postfix version
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
		else if (isalnum(c)) t.op = Regex::CHARACTER;
		
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

//Points fragment 1's edges to fragment 2's start state
//If no edges to point, constructs an empty (sigma) edge from f1 to f2
//
//	params: Fragment * f1
//		Fragment f2
//
//	return: none
//
void Regex::change_edges(std::shared_ptr<Fragment> & f1, std::shared_ptr<Fragment> & f2)
{
	if (f1->_edges->size() == 0)
	{
		std::shared_ptr<Edge> e(new Edge(true, f1->_end, NULL));
		e->_out = f2->_start;
		e->_modifiable = true;
		f1->_edges->push_back(e);
	}
	else
		for (auto const & e : *(f1->_edges))
			if (e->_modifiable != false)
				e->_out = f2->_start;
}

//Points fragment 1's edges to state s2
void Regex::change_edges(std::shared_ptr<Fragment> & f1, std::shared_ptr<State> & s2)
{
	if (f1->_edges->size() == 0)
	{
		std::shared_ptr<Edge> e(new Edge(true, f1->_end, NULL));
		e->_out = s2;
		e->_modifiable = true;
		f1->_edges->push_back(e);
	}
	else
		for (auto const & e : *(f1->_edges))
			if (e->_modifiable != false)
				e->_out = s2;
}

//Points fragment 1's edges to fragment 2's start state
//Used for plus method since f1 and f2 are not shared_ptr's there
void Regex::change_edges(Fragment & f1, Fragment & f2)
{
	if (f1._edges->size() == 0)
	{
		std::shared_ptr<Edge> e(new Edge(true, f1._end, f2._start));
		e->_modifiable = true;
		f1._edges->push_back(e);
	}
	else
		for (auto const & e : *(f1._edges))
			if (e->_modifiable != false)
				e->_out = f2._start;
}

//Construct character nfa fragment with token
//
//	params: stack<Fragment *> nfa, NFA being built (all other parsing ops have this)
//		Token token, token with character c to give to corresponding edge
//
//	return: none
//
// Basic fragment consisting of a single state and edge
void Regex::character(std::stack< std::shared_ptr<Fragment> > & nfa, const Regex::Token token)
{
	std::shared_ptr<State> in(new State());
	std::shared_ptr<Edge> e(new Edge(token.c, false, in, NULL));
	e->_modifiable = true;
	in->_edges.push_back(e);
	in->_match = false;

	std::shared_ptr<Fragment> f(new Fragment(in));
	f->_edges = &in->_edges;
	f->_end = NULL;

	nfa.push(f);
}

//Construct concatenation nfa fragment
//
// points f1 to f2's start state
void Regex::concat(std::stack< std::shared_ptr<Fragment> > & nfa)
{
	std::shared_ptr<Fragment> f2(nfa.top());
	nfa.pop();
	std::shared_ptr<Fragment> f1(nfa.top());
	nfa.pop();

	change_edges(f1, f2);

	std::shared_ptr<Fragment> f(new Fragment(f1->_start));
	f->_edges = f2->_edges;
	f->_end = (f2->_end == NULL) ? NULL : f2->_end;

	nfa.push(f);
}

//Construct 'zero or more' fragment
//
// points f1 at itself then constructs new empty edge to nowhere
void Regex::star(std::stack< std::shared_ptr<Fragment> > & nfa)
{
	std::shared_ptr<Fragment> f1(nfa.top());
	nfa.pop();

	f1->_end = (f1->_end == NULL) ? f1->_start : f1->_end;

	change_edges(f1, f1);

	for (auto const & e : f1->_start->_edges)
		e->_modifiable = false;

	std::shared_ptr<Edge> e(new Edge(true, f1->_start, NULL));
	e->_modifiable = true;
	f1->_start->_edges.push_back(e);
	
	std::shared_ptr<Fragment> f(new Fragment(f1->_start));
	f->_edges = &f1->_start->_edges;
	f->_end = NULL;

	nfa.push(f);	
}

//Construct 'one or more' fragment
void Regex::plus(std::stack< std::shared_ptr<Fragment> > & nfa)
{
	//get top of stack and create copy
	Fragment f1 = *nfa.top();
	Fragment f2 = *nfa.top();
	
	nfa.pop();

	std::vector< std::shared_ptr<Edge> > * e1 = f1._edges;		
	std::vector< std::shared_ptr<Edge> > e2 = *(f1._edges);
	f1._edges = e1;
	f2._edges = &e2;
	
	State s1 = *(f1._start);
	s1._match = false;	
	State s2 = *(f1._start);
	s2._match = false;
	f1._start = std::make_shared<State>(s1);
	f2._start = std::make_shared<State>(s2);

	if (f1._end == NULL)
	{
		std::shared_ptr<State> en1(new State());
		en1->_match = false;
		std::shared_ptr<State> en2(new State());
		en2->_match = false;
		f1._end = en1, f2._end = en2;
	}
	else
	{
		State en1 = *(f1._end);
		en1._match = false;
		State en2 = *(f1._end);
		en2._match = false;
		f1._end = std::make_shared<State>(en1);
		f2._end = std::make_shared<State>(en2);
	}
	
	//'one'
	change_edges(f1, f2);
	//'or more'
	change_edges(f2, f2);

	for (auto const & e : f2._start->_edges)
		e->_modifiable = false;

	std::shared_ptr<Edge> e(new Edge(true, f2._start, NULL));
	e->_modifiable = true;
	f2._start->_edges.push_back(e);

	std::shared_ptr<Fragment> f(new Fragment(f1._start));
	f->_edges = &f2._start->_edges;
	f->_end = NULL;

	nfa.push(f);
}

//Construct 'zero or one' fragment
// same idea as OR but with empty fragment
void Regex::question(std::stack< std::shared_ptr<Fragment> > & nfa)
{
	std::shared_ptr<Fragment> f1(nfa.top());
	nfa.pop();
	
	std::shared_ptr<State> f2_start(new State());
	f2_start->_match = false;
	std::shared_ptr<Fragment> f2(new Fragment(f2_start));
	
	std::shared_ptr<State> s(new State());
	std::shared_ptr<Edge> e1(new Edge(true, s, f1->_start));
	e1->_modifiable = true;
	std::shared_ptr<Edge> e2(new Edge(true, s, f2->_start));
	e2->_modifiable = true;

	s->_edges.push_back(e1), s->_edges.push_back(e2);

	std::shared_ptr<Fragment> f(new Fragment(s));
	
	std::shared_ptr<State> s2(new State());
	s2->_match = false;

	change_edges(f1, s2);

	std::shared_ptr<Edge> f2_e(new Edge(true, f2_start, s2));	
	f2_e->_modifiable = true;
	f2_start->_edges.push_back(f2_e);
	f2->_edges = &f2_start->_edges;

	f->_end = s2;
	f->_edges = &s2->_edges;

	nfa.push(f);
}

//Construct OR fragment
//
// made by constructing new start state and pointing 2
// empty edges at f1 and f2's start states, then constructing
// an end state and point f1 and f2 at it
void Regex::_or(std::stack< std::shared_ptr<Fragment> > & nfa)
{
	std::shared_ptr<Fragment> f2(nfa.top());
	nfa.pop();
	std::shared_ptr<Fragment> f1(nfa.top());
	nfa.pop();

	std::shared_ptr<State> s(new State());
	s->_match = false;

	std::shared_ptr<Edge> e1(new Edge(true, s, f1->_start));
	e1->_modifiable = true;
	std::shared_ptr<Edge> e2(new Edge(true, s, f2->_start));
	e2->_modifiable = true;

	s->_edges.push_back(e1), s->_edges.push_back(e2);

	std::shared_ptr<Fragment> f(new Fragment(s));
	
	std::shared_ptr<State> s2(new State());

	change_edges(f1, s2);
	change_edges(f2, s2);
	
	f->_end = s2;
	f->_edges = &s2->_edges;
	
	nfa.push(f);
}

// Parses the token stream generated during scanning and constructs a corresponding NFA
// roughly according to Thompson's construction algorithm
//
//	params: vector<Token> tokens, the token stream made during scanning
//
//	return: State *, the beginning state of the NFA
//
std::shared_ptr<State> Regex::parse(std::vector<Regex::Token> tokens)
{
	std::stack< std::shared_ptr<Fragment> > nfa; 

	for (auto const & token : tokens)
	{
		switch (token.op)
		{
			case Regex::CHARACTER:
				character(nfa, token);
				break;
			case Regex::CAT:
				concat(nfa);
				break;
			case Regex::STAR:
				star(nfa);
				break;
			case Regex::PLUS:
				plus(nfa);
				break;
			case Regex::QUESTION:
				question(nfa);
				break;
			case Regex::OR:
				_or(nfa);
				break;
			default:
				std::cout << "Parsing error: " << token.op << '\n';
				return NULL;
		}
	}

	std::shared_ptr<Fragment> end(nfa.top());
	nfa.pop();

	//if no last state, create new one and point last fragment to it
	if (end->_end == NULL)
	{
		std::shared_ptr<State> last(new State());
		last->_match = true;

		for (auto const & e : *(end->_edges))
			if (e->_modifiable != false)
				e->_out = last;
	}
	else
		end->_end->_match = true;

	return end->_start;
}

//Add states by traversing NFA through empty edges
//
//	params: vector<State *> nstates, list of next possible states
//		Edge * e, empty edge with at least 1 state to add (more can be added recursively)
//
//	return: none
//
void Regex::add_states(std::vector< std::shared_ptr<State> > & nstates, std::shared_ptr<Edge> e, std::vector< std::shared_ptr<State> > & visited)
{
	//matching state hit
	if (e->_out->_edges.size() == 0 && !contains(visited, e->_out))
	{
		nstates.push_back(e->_out);
		visited.push_back(e->_out);
		return;
	}
	
	for (auto const & e2 : e->_out->_edges)
	{
		if (e2->_sigma)	
			add_states(nstates, e2, visited);
		else if (!contains(visited, e2->_in))
		{
			nstates.push_back(e2->_in);
			visited.push_back(e2->_in);
		}
	}
}



// Simulates the NFA with a given string to match
//	
//	params: State * start, the starting state of the NFA
//		string str, the string to match
//
//	return: none, void
//	
void Regex::run(std::shared_ptr<State> start, std::string str)
{
	//current possible states and next possible states
	std::vector< std::shared_ptr<State> > cstates;
	std::vector< std::shared_ptr<State> > nstates;
	std::vector< std::shared_ptr<State> > visited;

	//characters already matched by other edges leading to next possible states
	std::vector<uint8_t> matched;

	cstates.push_back(start);

	int i = 0;
	while (i < str.length())
	{
		uint8_t c = str[i];

		bool found = false;

		//check all edges leading out from all current possible states
		for (auto const & s : cstates)
			for (auto const & e : s->_edges)
			{
				//if matched with str[i] then edges other state to nstates
				//add str[i] to list of current iteration's matched characters
				if (e->_c == c)
				{
					nstates.push_back(e->_out);
					found = true;

					if (!contains(matched, c))
						matched.push_back(c), i++;
				} 
				//if an empty edge and nothing already matched,
				// add next possible states given no match
				else if (e->_sigma && matched.size() == 0)
				{
					add_states(nstates, e, visited);
					found = true;
				}
			}
		
		//if no character matches and no empty strings, no match	
		if (!found)
		{
			std::cout << "No match\n";
			return;
		} //else set current states to next states and clear
		else
		{
			cstates = nstates;
			nstates.clear();
			visited.clear();
			matched.clear();
		}
	}
	
	std::vector< std::shared_ptr<State> > checks(cstates);

	//given last possible states, check for other states through empty edges
	for (auto const & s : cstates)
		for (auto const & e : s->_edges)
		{	
			if (e->_sigma)
				add_states(checks, e, visited);
		}

	//check if match is found
	for (auto const & s : checks)
	{
		if (s->_match == true)
		{
			std::cout << "Match\n";
			return;
		}
	}

	std::cout << "No match\n";
}
