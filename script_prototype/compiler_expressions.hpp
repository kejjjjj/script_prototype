#pragma once

#ifndef cmptimexpr
#define cmptimexpr

#include "pch.h"

//Purpose: check for syntax errors
class CompilerExpression
{
public:
	CompilerExpression(const std::string expr) : expression_str(expr), error_exists(false), e{} {};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);
	ExpressionType EvaluateExpressionType(const std::string_view& operand);
	bool NextOperatorIsLegal(char previous_op, char op);


	bool EvaluateExpression(const std::string_view& expr);
	bool ParseExpression(std::string& expr);
	bool ParseExpressionNumbers(std::string& expr);

	bool EvaluateExpressionStack(std::list<expression_stack>& es);
	VarType GetOperandType(const std::string_view& operand);
	struct e_s
	{
		expression_s expression;
		size_t operands;

	}e;

	std::string expression_str;

protected:
	bool error_exists;
};


#endif