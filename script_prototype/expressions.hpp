#pragma once

#ifndef expre
#define expre

#include "pch.h"

enum class ExpressionType
{
	EXPR_ASSIGNMENT, // a = 1; 
	EXPR_CALCULATION //100 + 100 / 3
};

struct expression_s
{
	std::string preOP;
	std::string Operand;
	std::string postOP;
	ExpressionType type;
};

struct expression_stack
{
	std::string content;
	std::string Operator;
};

class Expression
{
public:
	Expression(const std::string expr) : expression_str(expr){};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);  
	ExpressionType EvaluateExpressionType(const std::string_view& operand);

	float EvaluateExpression(const std::string_view& expr);
	bool ParseExpression(std::string &expr);
	bool ParseExpressionNumbers(std::string& expr);

	float EvaluateExpressionStack(std::vector<expression_stack>& es);

	std::string expression_str;
};

#endif