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

	CompilerExpression(const std::string expr, std::string& rtScript) : expression_str(expr), fscript(rtScript), e{} {};

	void TokenizeExpression(const std::string_view& expr_str, expression_s* expr);
	ExpressionType EvaluateExpressionType(expression_s* expr);
	bool NextOperatorIsLegal(char previous_op, char op);


	bool EvaluateExpression(const std::string_view& expr);
	bool ParseExpression(std::string& expr);
	bool ParseExpressionNumbers(std::string& expr);
	std::string CleanupExpression(const std::string_view& str);
	void TestExpression(const std::string_view& str);

	bool EvaluateExpressionStack(std::list<expression_stack>& es);
	VarType GetOperandType(const std::string_view& operand);

	struct e_s
	{
		expression_s expression;
		size_t operands;

	}e;

	std::string expression_str;
	std::string& fscript; //full script compressed

protected:
	bool error_exists = 0;
	Variable var;

};


#endif