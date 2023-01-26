#pragma once

#ifndef rntimexpr
#define rntimexpr

#include "pch.h"

//Purpose: run the actual script
//doesn't expect syntax errors
class RuntimeExpression
{
public:

	RuntimeExpression(const std::string aScript) : script(aScript) {};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);
	ExpressionType EvaluateExpressionType(expression_s* expr);
	bool ParseExpression(std::string& expr);
	bool ParseAssignment();
	bool ParseAssignment2();
	bool ParseExpressionNumbers(std::string& expr);
	std::string EvaluateExpression(const std::string_view& expr);
	std::string EvaluateExpressionStack(std::list<expression_stack>& es);
	std::string EvaluateSingular(const std::string& es);

	
	VarType GetOperandType(const std::string_view& operand);
	
	struct e_s
	{
		expression_s expression;
		size_t operands = 0;
	}e{};

private:
	std::string script;
	size_t codepos = 0;
	Variable* var = 0;

	bool add_to_value = false;
};

#endif