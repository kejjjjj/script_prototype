#include "pch.h"

void decl::EvaluateDeclaration(const std::string_view& type, std::string::iterator& it_)
{

	if (!srules.typename_allowed) {
		throw std::exception("type name is not allowed");
		return;
	}

	srules.typename_allowed = false;
	srules.expecting_identifier = true;

	auto expression = cec::Compiler_ParseExpression(';', it_);

	auto it = expression.begin(); auto end = expression.end();
	std::list<expr::expression_token> tokens;
	TokenizeExpression(it, end, tokens);
	auto t_it = tokens.begin();

	if (tokens.front().tokentype != VarType::VT_STRING) {
		throw std::exception("expected an identifier");
	}

	if (tokens.size() > 1) {
		if((++t_it)->op && t_it->content != "=" || !t_it->op)
		{
			throw std::exception("expected a ';'");
		}
	}

	DeclareVariable(type, tokens.front().content);

	expr::EvaluateEntireExpression(expression); //now it can be evaluated since it's been pushed to the stack

}