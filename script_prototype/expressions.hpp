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

struct rvalue
{
	rvalue(const VarType type) {
		switch (type) {
		case VarType::VT_INT:
			value.buffer = new int;
			value.buf_size = sizeof(int);

			break;
		case VarType::VT_FLOAT:
			value.buffer = new float;
			value.buf_size = sizeof(float);
			break;
		case VarType::VT_STRING:
			throw std::exception("rvalue(VarType type): VT_STRING case not supported");
			break;
		}
	}
	~rvalue()
	{
		delete value.buffer;
	}
	rvalue() = delete;

private:
	VariableValue value;

public:
	int get_int(int index) const {
		return *reinterpret_cast<int*>(value.buffer);
	}
	float get_float(int index) const {
		return *reinterpret_cast<float*>(value.buffer);
	}
	double get_double(int index) const {
		return *reinterpret_cast<double*>(value.buffer);
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(value.buffer) = v;
	}
	
};
struct lvalue
{
	//lvalue() : ref(nullptr) {};
	//~lvalue() = default;
	Variable* ref = 0;
};


namespace expr
{

	struct expression_token
	{
		std::string content;
		std::list<std::string> prefix;
		std::list<std::string> postfix;
		bool op = false;
		bool whitespace = false; //this boolean only exists if the FIRST character is a whitespace
		VarType tokentype = VarType::VT_INVALID;
		std::shared_ptr<rvalue> rval;
		std::shared_ptr<lvalue> lval;

		bool is_rvalue() const {
			return rval.get() != nullptr;
		}
		bool is_lvalue() const {
			return lval.get() != nullptr;
		}

	};

	std::string EvaluateEntireExpression(const std::string& str);
	std::string EvaluateExpression(const std::string& str);
	void TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens);
	void SetTokenValueCategory(expression_token& token);
	void EvaluatePostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens);
	void EvaluatePrefix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end);
	bool EvaluatePeriodPostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens);
	bool EvaluatePeriodPrefix(std::list<expression_token>::iterator& it);
	void EvaluatePrefixArithmetic(expression_token& token, bool increment);
	bool ExpressionCompatibleOperands(const VarType left, const VarType right);

	std::string EvaluateExpressionTokens(std::list<expression_token>& tokens);

	//void Eval(expression_token& left, expression_token& right, std::function<void(expression_token&, expression_token&)>& eval_fc);

	struct s_rules
	{
		bool next_postfix = false;
		bool next_unary = true;
		bool next_operator = false;
		bool ignore_postfix = false;
		bool operator_allowed = false;
	}inline rules;

	inline std::unordered_map <std::string_view, std::function<void(expression_token&, expression_token&)>> eval_funcs = 
	{  
		{"+", [](expression_token& left, expression_token& right) -> void
		{



			if (left.is_lvalue()) {

			}

			return;
		}}
	};

}
#endif