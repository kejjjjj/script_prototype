#pragma once

#ifndef expre
#define expre

#include "pch.h"

enum class ExpressionType
{
	EXPR_ASSIGNMENT, // a = 1; 
	EXPR_ASSIGNMENT2, // the += like operators
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

#define IsAnyOperator(x)	(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '%' || x == '=' || x == '!' || x == '~' || x == '.')

#define IsCalculationOp(x)  (x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '%')
#define IsOperator(x)		(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '%' || x == '=')
#define IsAssignment2Op(x)	(x == '+' || x == '-' || x == '/' || x == '*' || x == '&' || x == '|' || x == '^' || x == '%')
#define IsDualOp(x)			(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '!' || x == '%' || x == '=')
#define IsPrefixOp(x)		(x == '-' || x == '!' || x == '~' || x == '+')
#define IsPostEqualOp(x)	(x == '=') //==
#define BadCalculationOp(x) (x != '(' && x != ')' && !IsCalculationOp(x) && x != '=' && x != '~')
#define IsValidSyntaxForName(x) (std::isalnum(x) && x != '_')
#define UnaryArithmeticOp(x) (x == "++" || x == "--")

namespace expr
{
	struct expression_token
	{
		std::string content;
		std::list<std::string> prefix;
		std::list<std::string> postfix;
		bool op = false;
		bool whitespace = false; //this boolean only exists if the FIRST character is a whitespace
	};

	std::string EvaluateEntireExpression(const std::string& str);
	std::string EvaluateExpression(const std::string& str);
	void TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens);
	void EvaluatePostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens);
	void EvaluatePrefix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end);
	bool EvaluatePeriodPostfix(std::list<expression_token>::iterator& it, std::list<expression_token>& tokens);
	bool EvaluatePeriodPrefix(std::list<expression_token>::iterator& it);

	std::string EvaluateExpressionTokens(std::list<expression_token>& tokens);
	struct s_rules
	{
		bool next_postfix = false;
		bool next_unary = true;
		bool next_operator = false;
		bool ignore_postfix = false;
	}inline rules;
}
#endif