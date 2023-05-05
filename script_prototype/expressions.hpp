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
	rvalue(const VarType _type) : type(_type){
		switch (_type) {
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
	VarType type;
public:
	int get_int() const {
		return *reinterpret_cast<int*>(value.buffer);
	}
	float get_float() const {
		return *reinterpret_cast<float*>(value.buffer);
	}
	double get_double() const {
		return *reinterpret_cast<double*>(value.buffer);
	}
	char* get_string() const {
		return reinterpret_cast<char*>(value.buffer);
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(value.buffer) = v;
	}
	auto get_type() const {
		return type;
	}
	void set_type(const VarType atype) {
		type = atype;
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
		int get_int() const {
			if(is_rvalue())
				return (rval->get_int());
			else if(is_lvalue())
				return (lval->ref->get_int());

			throw std::exception("not an lvalue nor rvalue");
		}
		float get_float() const {
			if (is_rvalue())
				return (rval->get_float());
			else if (is_lvalue())
				return (lval->ref->get_float());

			throw std::exception("not an lvalue nor rvalue");
		}
		double get_double() const {
			if (is_rvalue())
				return (rval->get_double());
			else if (is_lvalue())
				return (lval->ref->get_double());

			throw std::exception("not an lvalue nor rvalue");
		}
		char* get_string() const {
			if (is_rvalue())
				return (rval->get_string());
			else if (is_lvalue())
				return (lval->ref->get_string());

			throw std::exception("not an lvalue nor rvalue");
		}
		template<typename T>
		void set_value(T value) {
			if (is_rvalue())
				rval->set_value(value);
			else if (is_lvalue())
				lval->ref->set_value(value);
		}
		auto get_type() const {
			if (is_rvalue())
				return rval->get_type();
			else if (is_lvalue())
				return lval->ref->get_type();

			throw std::exception("unknown expression used in get_type()");

			return VarType::VT_INVALID;
		}
		void set_type(const VarType atype) {
			tokentype = atype;

			if (is_rvalue())
				return rval->set_type(atype);
			else if (is_lvalue())
				return lval->ref->set_type(atype);

		}
		bool is_integral() const {
			return get_type() == VarType::VT_INT;
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
	void ExpressionMakeRvalue(expression_token& token);
	void ExpressionCastWeakerOperand(expression_token& left, expression_token& right);

	std::string EvaluateExpressionTokens(std::list<expression_token>& tokens);

	//void Eval(expression_token& left, expression_token& right, std::function<void(expression_token&, expression_token&)>& eval_fc);

	struct s_rules
	{
		bool next_postfix = false;
		bool next_unary = true;
		bool next_operator = false;
		bool ignore_postfix = false;
		bool operator_allowed = false;

		void reset()
		{
			next_postfix = false;
			next_unary = true;
			next_operator = false;
			ignore_postfix = false;
			operator_allowed = false;
		}

	}inline rules;


	inline std::unordered_map <std::string_view, std::function<std::string(expression_token&, expression_token&)>> eval_funcs = 
	{  
		{"+", [](expression_token& left, expression_token& right) -> std::string
		{
			ExpressionMakeRvalue(left);
			ExpressionCastWeakerOperand(left, right);
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)	
						 fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value(int(left.get_int() + fright));

					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						 fright = right.get_float();
					else fright = right.get_int();

					right.set_value((left.get_float() + fright));
					return std::to_string(right.get_float());

				case VarType::VT_STRING:
					#pragma warning( suppress : 4996)
					return strcat(left.get_string(), right.get_string());
			}
			return "";
		}},		
		{"-", [](expression_token& left, expression_token& right) -> std::string
		{
			ExpressionMakeRvalue(left);
			ExpressionCastWeakerOperand(left, right);
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() - fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<float>(left.get_float() - fright);

					return std::to_string(right.get_float());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		}},
		{"*", [](expression_token& left, expression_token& right) -> std::string
		{
			ExpressionMakeRvalue(left);
			ExpressionCastWeakerOperand(left, right);
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() * fright);

					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<float>(left.get_float() * fright);

					return std::to_string(right.get_float());
				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		}},
		{"/", [](expression_token& left, expression_token& right) -> std::string
		{
			ExpressionMakeRvalue(left);
			ExpressionCastWeakerOperand(left, right);
			

			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					if (fright == 0.f)
						throw std::exception("division by zero");

					right.set_value<int>(left.get_int() / fright);

					

					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					if (fright == 0.f)
						throw std::exception("division by zero");

					right.set_value<float>(left.get_float() / fright);

					return std::to_string(right.get_float());
				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		}},
		{ "%", [](const expression_token& left, expression_token& right) -> std::string
		{

			if (!left.is_integral() || !right.is_integral())
				throw std::exception("modulo operands must have an integral type");

			const int fright = right.get_int();
			if (fright == 0)
				throw std::exception("division by zero");

			right.set_value<int>(left.get_int() % fright);

			return std::to_string(right.get_int());
		
		}},
		{ "<<", [](const expression_token& left, expression_token& right) -> std::string
		{
			if (!left.is_integral() || !right.is_integral())
				throw std::exception("shift operands must have an integral type");

			const int fright = right.get_int();

			right.set_value<int>(left.get_int() << fright);

			return std::to_string(right.get_int());

		}},
		{ ">>", [](const expression_token& left, expression_token& right) -> std::string
		{
			if (!left.is_integral() || !right.is_integral())
				throw std::exception("shift operands must have an integral type");

			const int fright = right.get_int();

			right.set_value<int>(left.get_int() >> fright);

			return std::to_string(right.get_int());

		}},
		{ "==", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() == fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_float() == fright);
					right.set_type(VarType::VT_INT);
					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ "!=", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() != fright);

					
					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_float() != fright);
					right.set_type(VarType::VT_INT);
					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ "<", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() < fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_float() < fright);
					right.set_type(VarType::VT_INT);

					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ ">", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() > fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_float() > fright);
					right.set_type(VarType::VT_INT);

					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ "<=", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() <= fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_float() <= fright);
					right.set_type(VarType::VT_INT);

					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ ">=", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() >= fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();
					right.set_type(VarType::VT_INT);

					right.set_value<int>(left.get_float() >= fright);

					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";
		} },
		{ "&", [](const expression_token& left, expression_token& right) -> std::string
		{
			if (!left.is_integral() || !right.is_integral())
				throw std::exception("bitwise operands must have an integral type");

			const int fright = right.get_int();

			right.set_value<int>(left.get_int() & fright);

			return std::to_string(right.get_int());
		} },
		{ "^", [](const expression_token& left, expression_token& right) -> std::string
{
			if (!left.is_integral() || !right.is_integral())
				throw std::exception("bitwise operands must have an integral type");

			const int fright = right.get_int();

			right.set_value<int>(left.get_int() ^ fright);

			return std::to_string(right.get_int());
		} },
		{ "|", [](const expression_token& left, expression_token& right) -> std::string
{
			if (!left.is_integral() || !right.is_integral())
				throw std::exception("bitwise operands must have an integral type");

			const int fright = right.get_int();

			right.set_value<int>(left.get_int() | fright);

			return std::to_string(right.get_int());

		} },
		{ "&&", [](const expression_token& left, expression_token& right) -> std::string
		{
			float fright{};
			switch (left.tokentype) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() && fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();
					right.set_type(VarType::VT_INT);

					right.set_value<int>(left.get_float() && fright);

					return std::to_string(right.get_int());

				case VarType::VT_STRING:
					throw std::exception("invalid expression");
			}
			return "";

		} },
		{ "&&", [](const expression_token& left, expression_token& right) -> std::string
		{
		float fright{};
		switch (left.tokentype) {
			case VarType::VT_INT:

				if (right.tokentype == VarType::VT_FLOAT)
					fright = right.get_float();
				else fright = right.get_int();

				right.set_value<int>(left.get_int() || fright);


				return std::to_string(right.get_int());
			case VarType::VT_FLOAT:

				if (right.tokentype == VarType::VT_FLOAT)
					fright = right.get_float();
				else fright = right.get_int();
				right.set_type(VarType::VT_INT);

				right.set_value<int>(left.get_float() || fright);

				return std::to_string(right.get_int());

			case VarType::VT_STRING:
				throw std::exception("invalid expression");
		}
		return "";

		} },
		{ "=", [](const expression_token& left, expression_token& right) -> std::string
		{
			if (!left.is_lvalue())
				throw std::exception("left operand must be a modifiable lvalue");

			auto var = left.lval->ref;

			float fright{};
			switch (var->get_type()) {
				case VarType::VT_INT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();
					//std::cout << "int operands: { " << var->get_int() << ", " << int(fright) << " }\n";
					var->set_int(fright);

					//std::cout << std::format("the value of \"{}\" changed to \"{}\"\n", var->name, var->get_int());
					
					right.set_value<int>(fright);

					return std::to_string(var->get_int());
				case VarType::VT_FLOAT:

					if (right.tokentype == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					var->set_float(fright);

					//std::cout << std::format("the value of \"{}\" changed to \"{}\"\n", var->name, var->get_float());
					//std::cout << "float operands: { " << var->get_float() << ", " << (fright) << " }\n";
					right.set_value<float>(fright);

					return std::to_string(var->get_float());
				case VarType::VT_STRING:
					var->set_string(right.get_string());
					right.set_value<char*>(right.get_string());

					return var->get_string();
			}
			return "";
		}}

	};

}
#endif