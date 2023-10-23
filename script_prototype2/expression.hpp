#pragma once

#ifndef __expres
#define __expres

//#include "script.hpp"
#include "expression_token.hpp"
//#include "o_standard.hpp"

class expression_t{
public:
	expression_t(scr_scope_t* scope, const code_segment_t& expression) : tokens(expression), block(scope)
	{
		++tokens.end;
	};

	//void evaluate() override;

	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	expression_token EvaluateEntireExpression();
	
	static void ExpressionFindMatchingParenthesis(code_segment_t& token);
	static std::optional<code_segment_t> find_matching_parenthesis(const code_segment_t& token);


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
	code_segment_t tokens;
	std::list<expression_token> sortedTokens;
	scr_scope_t* block = 0;
};

#endif