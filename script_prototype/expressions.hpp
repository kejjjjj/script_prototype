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
	std::string Operator;
	std::string postOP;
	ExpressionType type = ExpressionType::EXPR_ASSIGNMENT;
};

struct expression_stack
{
	std::string content;
	std::string Operator;
};


#define IsCalculationOp(x)  (x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '~' || x == '|' || x == '^' || x == '!' || x == '%')
#define IsOperator(x)  (x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '!' || x == '%' || x == '=')

#define BadCalculationOp(x) (x != '(' && x != ')' && !IsCalculationOp(x) && x != '=')


class Expression
{
public:
	Expression(const std::string expr) : expression_str(expr){};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);  
	ExpressionType EvaluateExpressionType(const std::string_view& operand);
	bool NextOperatorIsLegal(char previous_op, char op);


	float EvaluateExpression(const std::string_view& expr);
	bool ParseExpression(std::string &expr);
	float ParseExpressionNumbers(std::string& expr);

	float EvaluateExpressionStack(std::list<expression_stack>& es);

	struct e_s
	{
		expression_s expression;
		size_t operands = 0;

	}e{};

	std::string expression_str;
};

#endif