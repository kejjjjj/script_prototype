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

#define IsAnyOperator(x)	(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' \
							|| x == '^' || x == '%' || x == '=' || x == '!' || x == '~' || x == '.' || x == '?' || x == '[' || x == ']')

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
	//FIXME - make rvalue constructor require an expression_token instead of just a type :)
	//edit maybe not?
	rvalue(const VarType _type, const unsigned __int16 size = 0) : type(_type){
		
		switch (_type) {
		case VarType::VT_INT:
			value.buffer = std::make_shared<char*>(new char[4]);
			value.buf_size = sizeof(int);

			break;
		case VarType::VT_FLOAT:
			value.buffer = std::make_shared<char*>(new char[4]);
			value.buf_size = sizeof(float);
			break;
		case VarType::VT_STRING:
			if(!size)
				throw std::exception("empty string literal is not allowed");
			
			value.buffer = std::make_shared<char*>(new char[size+1]);
			char* buf = (char*)value.buffer.get();
			buf[size] = '\0';
			value.buf_size = size+1;
			break;
		case VarType::VT_CHAR:
			value.buffer = std::make_shared<char*>(new char[1]);
			value.buf_size = sizeof(char);
			break;
		}
	}
	~rvalue()
	{
		value.buffer.reset();
	}
	rvalue() = delete;
	VariableValue value;
private:
	
	VarType type;
public:
	int get_int() const {
		return *reinterpret_cast<int*>(value.buffer.get());
	}
	float get_float() const {
		return *reinterpret_cast<float*>(value.buffer.get());
	}
	double get_double() const {
		return *reinterpret_cast<double*>(value.buffer.get());
	}
	char* get_string() const {
		return reinterpret_cast<char*>(value.buffer.get());
	}
	template <typename T>
	void set_value(const T v) {
		*reinterpret_cast<T*>(value.buffer.get()) = v;
	}
	void set_string(char* str) {
		const auto len = strlen(str);
		//str[len - 1] = '\0';

		if (len != value.buf_size) {
			value.buffer.reset();
			value.buffer = std::make_shared<char*>(new char[len+1]);
			char* alloc = (char*)value.buffer.get();
			alloc[len] = '\0';

		}
		memcpy(value.buffer.get(), str, len);
		value.buf_size = strlen(get_string()); //a bit more expensive but works surely :clueless:
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
struct temp_value_s
{
	Variable* ref = 0;
	//VariableValue value;
	bool increment = false;
};

namespace expr
{

	struct expression_token
	{
		~expression_token() {
			
			if (is_lvalue())
				lval.reset();
			else if(is_rvalue()){

				if (rval.use_count() < 2) {
					rval->value.buffer.reset();
					rval.reset();
				}
			}
		};

		std::string content;
		std::list<std::string> prefix;
		std::list<std::string> postfix;
		bool op = false;
		bool whitespace = false; //this boolean only exists if the FIRST character is a whitespace
		bool string_literal = false;
		bool char_literal = false;
		VarType tokentype = VarType::VT_INVALID;
		std::shared_ptr<rvalue> rval;
		std::shared_ptr<lvalue> lval;
		//temp_value_s temp;

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
		char* get_string() const {
			if (is_rvalue())
				return (rval->get_string());
			else if (is_lvalue())
				return (lval->ref->get_string());

			throw std::exception("not an lvalue nor rvalue");
		}
		void set_string(char* str)
		{
			if (is_rvalue())
				rval->set_string(str);
			else if (is_lvalue())
				lval->ref->set_string(str);
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

	expression_token EvaluateEntireExpression(const std::string& str);
	expression_token EvaluateExpression(const std::string& str);
	void TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens, int tokens_left = -1);
	void SetTokenValueCategory(expression_token& token);
	void EvaluatePostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens);
	void EvaluatePrefix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end);
	bool EvaluatePeriodPostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end, std::list<expression_token>& tokens);
	void EvaluatePostfixArithmetic(expression_token& token, bool increment);
	bool EvaluateSubscript(expression_token& token);
	bool EvaluatePeriodPrefix(std::list<expression_token>::iterator& it);
	void EvaluatePrefixArithmetic(expression_token& token, bool increment);
	bool ExpressionCompatibleOperands(const expression_token& left, const expression_token& right);
	void ExpressionMakeRvalue(expression_token& token);
	void ExpressionCastWeakerOperand(expression_token& left, expression_token& right);
	void ExpressionSetTempValue(temp_value_s& token);
	expression_token EvaluateExpressionTokens(std::list<expression_token>& tokens);

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

	inline std::list<temp_value_s> expression_postfixes;


	inline std::unordered_map <std::string_view, std::function<expression_token(expression_token&, expression_token&)>> eval_funcs =
	{  
		{"+", [](expression_token& left, expression_token& right) -> expression_token
		{
			expression_token result;
			ExpressionMakeRvalue(left);
			ExpressionMakeRvalue(right);

			result.rval = std::shared_ptr<rvalue>(new rvalue(right.get_type(),  (right.get_type() == VarType::VT_STRING ? (unsigned short)strlen(right.get_string()) : 0u)));

			ExpressionCastWeakerOperand(left, right);
			result.set_type(right.get_type());
			float fright{};
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)	
						 fright = (int)right.get_float();
					else fright = right.get_int();

					result.set_value(int(left.get_int() + fright));
					result.content = std::to_string(result.get_int());
					return result;
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
						 fright = right.get_float();
					else fright = right.get_int();

					result.set_value((left.get_float() + fright));
					result.content = std::to_string(result.get_float());
					return result;

				case VarType::VT_STRING:
					#pragma warning( suppress : 4996)
					std::string str = "\"" + std::string(strcat(left.get_string(), right.get_string())) + "\"";
					result.set_string((char*)str.c_str());
					result.content = (result.get_string());
					return result;
			}
			return result;
		}},		
		/*{"-", [](expression_token& left, expression_token& right) -> std::string
		{
			ExpressionMakeRvalue(left);
			ExpressionCastWeakerOperand(left, right);
			float fright{};
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() - fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() * fright);

					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					if (fright == 0.f)
						throw std::exception("division by zero");

					right.set_value<int>(left.get_int() / fright);

					

					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() == fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() != fright);

					
					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() < fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() > fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() <= fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() >= fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
			switch (left.get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					right.set_value<int>(left.get_int() && fright);


					return std::to_string(right.get_int());
				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
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
		switch (left.get_type()) {
			case VarType::VT_INT:

				if (right.get_type() == VarType::VT_FLOAT)
					fright = right.get_float();
				else fright = right.get_int();

				right.set_value<int>(left.get_int() || fright);


				return std::to_string(right.get_int());
			case VarType::VT_FLOAT:

				if (right.get_type() == VarType::VT_FLOAT)
					fright = right.get_float();
				else fright = right.get_int();
				right.set_type(VarType::VT_INT);

				right.set_value<int>(left.get_float() || fright);

				return std::to_string(right.get_int());

			case VarType::VT_STRING:
				throw std::exception("invalid expression");
		}
		return "";

		} },*/
		{ "=", [](const expression_token& left, const expression_token& right) -> expression_token
		{
			if (!left.is_lvalue())
				throw std::exception("left operand must be a modifiable lvalue");

			auto var = left.lval->ref;

			expression_token result;

			result.lval = left.lval;
			result.set_type(left.get_type());
			result.content = var->name;

			if (left.lval->ref->is_reference()) {
				result.lval->ref->name = left.lval->ref->name;
			}

			
			result.lval->ref->set_expression(&right);

			if (var->is_array())
				return result;

			switch (var->get_type()) {
				case VarType::VT_INT:
					result.content = std::to_string(result.get_int());
					return result;

				case VarType::VT_FLOAT:
					result.content = std::to_string(result.get_float());
					return result;

				case VarType::VT_STRING:
					result.content = result.get_string();
					return result;
			}
			return result;
		}},
		{ "<=>", [](const expression_token& left, const expression_token& right) -> expression_token
		{
			if (!left.is_lvalue() || !right.is_lvalue())
				throw std::exception("both swap operands must be modifiable lvalues");
			expression_token result;
			auto left_op = left.lval->ref;
			auto right_op = right.lval->ref;

			int left_int = left.get_int();
			float left_float = left.get_int();
			char left_str[32];

			memcpy(left_str, left.get_string(), left_op->value.buf_size);



			float fright{};

			switch (left_op->get_type()) {
				case VarType::VT_INT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = (int)right.get_float();
					else fright = right.get_int();

					left_op->set_value<int>(fright);
					
					result.content = std::to_string(left_op->get_int());

					break;

				case VarType::VT_FLOAT:

					if (right.get_type() == VarType::VT_FLOAT)
						fright = right.get_float();
					else fright = right.get_int();

					left_op->set_value<float>(fright);

					result.content = std::to_string(left_op->get_float());

					break;

				case VarType::VT_STRING:
					left_op->set_value<char*>(right_op->get_string());
					break;
			}

			switch (right_op->get_type()) {
			case VarType::VT_INT:

				fright = (int)left_int;


				right_op->set_value<int>(fright);

				break;

			case VarType::VT_FLOAT:

				if (left.get_type() == VarType::VT_FLOAT)
					fright = left_float;
				else fright = left_int;

				right_op->set_value<float>(fright);
				break;

			case VarType::VT_STRING:
				right_op->set_value<char*>(left_str);
				break;
			}
			
			result.lval = left.lval;

			return left;
		}}

	};

}
#endif