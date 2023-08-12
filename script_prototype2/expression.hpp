#pragma once

#ifndef __expres
#define __expres

#include "script.hpp"
#include "expression_token.hpp"
#include "o_standard.hpp"

class expression_t {
public:
	expression_t(const token_statement_t& expression) : tokens(expression)
	{
		++tokens.end;
	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	expression_token EvaluateEntireExpression();
	
	static void ExpressionFindMatchingParenthesis(token_statement_t& token);


private:

	enum unary_parentheses_e
	{
		INVALID,
		UNARY_CAST,
		EXPRESSION
	};

	void TokenizeExpression();
	bool ParseExpression();
	bool ParseOperator();
	bool ParseUnaryCast(expression_token& token);
	bool ExpressionParseParentheses(expression_token& token);

	void EvaluateExpression();
	void EvaluateExpressionTokens();

	expression_token result;
	token_statement_t tokens;
	std::list<expression_token> sortedTokens;
};

#endif