#pragma once

#ifndef expre
#define expre

#include "pch.h"

class Expression
{
public:
	Expression(const std::string expr) : expression_str(expr){};

	std::string RemoveWhiteSpaces(std::string& expr);
	void TokenizeExpression();

	bool EvaluateExpression();

	enum class ExpressionType_e
	{
		EXPR_ASSIGNMENT,
		EXPR_COMPARE
	};
	
	enum class Operand_e {
		ADDITION,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION
	};

	struct expression_s {

		std::string preOP;
		Operand_e Operand;
		std::string postOP;
	};

	std::string expression_str;

};

#endif