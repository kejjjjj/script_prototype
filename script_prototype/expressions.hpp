#pragma once

#ifndef expre
#define expre

#include "pch.h"

enum class ExpressionType_e
{
	EXPR_ASSIGNMENT,
	EXPR_COMPARE,
	EXPR_CALCULATION
};

enum class Operand_e {
	ADDITION,
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION
};

struct expression_s
{
	std::string preOP;
	std::string Operand;
	std::string postOP;

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

	void TokenizeExpression(std::string& expr_str, expression_s* expr);  

	float EvaluateExpression(std::string& expr);
	bool ParseExpression(std::string &expr);
	bool ParseExpressionNumbers(std::string& expr);

	float EvaluateExpressionStack(std::vector<expression_stack>& es);

	std::string expression_str;
};

#endif