#pragma once

#ifndef cmptimexpr
#define cmptimexpr

#include "pch.h"

//Purpose: check for syntax errors
class CompilerExpression
{
public:

	struct ExprData
	{
		ExprData() = default;
		~ExprData() = default;
		VarType vType = VarType::VT_INVALID; //used when comparing types in expressions like (a + 3) and we need to know what type a is
		//VariableValue vValue;

	};

	CompilerExpression(const std::string expr) : expression_str(expr), error_exists(false), e{} {};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);
	ExpressionType EvaluateExpressionType(const std::string_view& operand);
	bool NextOperatorIsLegal(char previous_op, char op);


	bool EvaluateExpression(const std::string_view& expr, ExprData& data);
	bool ParseExpression(std::string& expr);
	bool ParseExpressionNumbers(std::string& expr, ExprData& data);
	bool VariableNameIsLegal(const std::string_view& var);
	std::string CleanupExpression(const std::string_view& str);

	bool EvaluateExpressionStack(std::list<expression_stack>& es, ExprData& data);
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