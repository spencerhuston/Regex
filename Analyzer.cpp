#include "Analyzer.h"

Analyzer::Analyzer(std::vector<Regex::Token> tokens)
{
	_tokens = tokens;
	build_tree(_tokens);
	analyze();
}

//Builds parse tree from token stream
//
//	params: vector<Token> token, the original token stream
//
//	return: none
//
void Analyzer::build_tree(std::vector<Regex::Token> tokens)
{
	uint16_t index = 0;

	//stream already in postfix so stack is the natural choice to parse it
	std::stack< std::shared_ptr<Node> > tree;

	for (auto const & token : tokens)
	{
		//character is always leaf
		//OR and CAT have 2 children nodes
		//*, ?, and + have only left child node since they are left-associative
		switch (token.op)
		{
			case Regex::CHARACTER:
			{
				std::shared_ptr<Node> leaf(new Node());
				leaf->_c = token.c;
				leaf->_prec = 4;
				leaf->_parent = NULL, leaf->_left = NULL, leaf->_right = NULL;
				tree.push(leaf);		
			}
				break;
			case Regex::CAT:
			{
				std::shared_ptr<Node> l2(tree.top());
				tree.pop();
				std::shared_ptr<Node> l1(tree.top());
				tree.pop();
		
				std::shared_ptr<Node> cat(new Node());
				cat->_c = token.c;
				cat->_prec = 2;
				cat->_parent = NULL, cat->_left = l1, cat->_right = l2;
				l2->_parent = cat, l1->_parent = cat;
				tree.push(cat);
			}
				break;
			case Regex::STAR:
			case Regex::PLUS:
			case Regex::QUESTION:
			{
				std::shared_ptr<Node> l1(tree.top());
				tree.pop();

				std::shared_ptr<Node> unary_op(new Node());
				unary_op->_c = (token.c == '*') ? '*' : ((token.c == '?') ? '?' : '+');
				unary_op->_prec = 1;
				unary_op->_parent = NULL, unary_op->_left = l1, unary_op->_right = NULL;
				l1->_parent = unary_op;
				tree.push(unary_op);
			}
				break;
			case Regex::OR:
			{
				std::shared_ptr<Node> l2(tree.top());
				tree.pop();
				std::shared_ptr<Node> l1(tree.top());
				tree.pop();
		
				std::shared_ptr<Node> _or(new Node());
				_or->_c = '|';
				_or->_prec = 3;
				_or->_parent = NULL, _or->_left = l1, _or->_right = l2;
				l2->_parent = _or, l1->_parent = _or;
				tree.push(_or);
			}
				break;
			default:
				std::cout << "Analysis error\n";
				return;
		}
	}

	_root = tree.top();
	tree.pop();
}

//finds operators below current precedence-1 op and removes them from the tree
void Analyzer::check_prec(std::shared_ptr<Analyzer::Node> node)
{
	if (node == NULL)
		return;
	
	check_prec(node->_left);
	check_prec(node->_right);

	if (node->_prec == 1)
	{
		if (node->_parent != NULL && node == node->_parent->_left)
			node->_parent->_left = node->_left;	
		else if (node->_parent != NULL && node == node->_parent->_right)
			node->_parent->_right = node->_left;

		node->_left->_parent = node->_parent;
	}
}

void Analyzer::preorder(std::shared_ptr<Analyzer::Node> node)
{
	if (node == NULL)
		return;
	
	if (node->_prec == 1)
	{
		check_prec(node->_left);
		check_prec(node->_right);
	}
	else
	{
		preorder(node->_left);
		preorder(node->_right);
	}
}

//postorder traversal to rebuild token stream
void Analyzer::remake_tokens(std::shared_ptr<Analyzer::Node> node, std::vector<Regex::Token> & tokens)
{
	if (node == NULL)
		return;

	remake_tokens(node->_left, tokens);
	remake_tokens(node->_right, tokens);

	Regex::Token t;
	t.c = node->_c;

	if (node->_prec == 4) t.op = Regex::CHARACTER;
	else if (node->_prec == 3) t.op = Regex::OR;
	else if (node->_prec == 2) t.op = Regex::CAT;
	else if (node->_prec == 1 && node->_c == '*') t.op = Regex::STAR;
	else if (node->_prec == 1 && node->_c == '+') t.op = Regex::PLUS;
	else if (node->_prec == 1 && node->_c == '?') t.op = Regex::QUESTION;
	
	tokens.push_back(t);
}

void Analyzer::analyze()
{
	preorder(_root);
	std::vector<Regex::Token> tokens;
	remake_tokens(_root, tokens);

	_tokens = tokens;	
}
