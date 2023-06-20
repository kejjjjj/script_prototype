#pragma once

#ifndef __expres
#define __expres

#include "script.hpp"
#include "expression_token.hpp"

class expression_t {
public:
	expression_t(const token_expression_t& expression) : tokens(expression)
	{
	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	expression_token EvaluateEntireExpression();

private:

	void TokenizeExpression();
	bool ParseExpression();
	bool ParseOperator();
	expression_token EvaluateExpression();
	void EvaluateExpressionTokens();

	expression_token result;
	token_expression_t tokens;
	std::list<expression_token> sortedTokens;
};

#endif