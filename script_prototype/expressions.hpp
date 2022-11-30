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

struct expression_s {

	std::string preOP;
	std::string Operand;
	std::string postOP;
	ExpressionType_e type;
};

class Expression
{
public:
	Expression(const std::string expr) : expression_str(expr){};

	std::string RemoveWhiteSpaces(std::string& expr);
	void TokenizeExpression(expression_s* expr);

	bool EvaluateExpression();
	bool AssignmentIsSane(expression_s* expr);

	ExpressionType_e GetExpressionType();


	std::string expression_str;

};

#endif