#pragma once

#ifndef expre
#define expre

#include "pch.h"

struct expression_stack
{
	std::string content;
	std::string Operator;
};

struct codepos_s
{
	size_t column{ 0 };
	size_t line{ 0 };

};

#define IsAnyOperator(x)	(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' \
							|| x == '^' || x == '%' || x == '=' || x == '!' || x == '~' || x == '.' || x == '[' || x == ']' || x == '@')
#define IsOperator(x)		(x == '+' || x == '-' || x == '/' || x == '*' || x == '>' || x == '<' || x == '&' || x == '|' || x == '^' || x == '%' || x == '=')
#define UnaryArithmeticOp(x) (x == "++" || x == "--")


struct lvalue
{

	Variable* ref = 0;
};
struct temp_value_s
{
	Variable* ref = 0;
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

		bool is_rvalue() const noexcept {
			return rval.get() != nullptr;
		}
		bool is_lvalue() const noexcept {
			return lval.get() != nullptr;
		}

		bool is_pointer() const {
			if (is_rvalue())
				return rval->is_pointer();

			else if (is_lvalue())
				return lval->ref->is_pointer();

			return false;
		}
		bool is_array() const {
			if (is_rvalue())
				return rval->is_array();

			else if (is_lvalue())
				return lval->ref->is_array();

			return false;
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
		char get_char() const {
			if (is_rvalue())
				return (rval->get_char());
			else if (is_lvalue())
				return (lval->ref->get_char());

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
		std::string s_gettype() const
		{
			if (is_lvalue())
				return lval->ref->s_getvariabletype();

			if (is_rvalue())
				return rval->s_gettype();

			throw std::exception("unknown expression used in s_gettype()");
			return "";
		}
		void set_type(const VarType atype) {
			tokentype = atype;

			if (is_rvalue())
				return rval->set_type(atype);
			else if (is_lvalue())
				return lval->ref->set_type(atype);

			throw std::exception("unknown expression used in set_type()");
		}
		bool is_integral() const {
			return get_type() >= VarType::VT_CHAR || get_type() <= VarType::VT_INT;
		}
		size_t size_of() const {
			if (is_rvalue())
				return rval->value.buf_size;
			else if (is_lvalue())
				return lval->ref->value.buf_size;

			throw std::exception("unknown expression used in size_of()");


		}
		
		template<typename T>
		T implicit_cast() const {
#pragma warning(push)
#pragma warning(disable : 4244) 
			if (typeid(T) == typeid(int)) {
				switch (get_type()) {
				case VarType::VT_CHAR:
					return static_cast<int>(get_char());
				case VarType::VT_INT:
					return get_int();
				case VarType::VT_FLOAT:
					return static_cast<int>(get_float());
				default:
					throw std::exception("this cast is illegal - this should never execute");
				}
			}
			
			if (typeid(T) == typeid(float)) {
				switch (get_type()) {
				case VarType::VT_CHAR:
					return static_cast<float>(get_char()); //why?
				case VarType::VT_INT:
					return static_cast<float>(get_int());
				case VarType::VT_FLOAT:
					return get_float();
				default:
					throw std::exception("this cast is illegal - this should never execute");
				}
			}
#pragma warning(pop)

			throw std::exception("cast() called for an unsupported type");

		}
		size_t pointer_depth() const
		{
			if (is_lvalue())
				return lval->ref->pointer_depth();
			if (is_rvalue())
				return rval->pointer_depth();

			throw std::exception("unknown expression used in pointer_depth()");

		}
		size_t array_depth() const
		{
			if (is_lvalue())
				return lval->ref->array_depth();
			if (is_rvalue())
				return rval->array_depth();

			throw std::exception("unknown expression used in array_depth()");

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
	bool EvaluateAddressOfPrefix(expression_token& token);
	bool EvaluatePeriodPrefix(std::list<expression_token>::iterator& it);
	void EvaluatePrefixArithmetic(expression_token& token, bool increment);
	bool ExpressionCompatibleOperands(const expression_token& left, const expression_token& right);
	bool ExpressionCompatibleArrayOperands(const expression_token& left, const expression_token& right);
	bool ExpressionCompatiblePointerOperands(const expression_token& left, const expression_token& right);

	void ExpressionMakeRvalue(expression_token& token);
	void ExpressionCastWeakerOperand(expression_token& left, expression_token& right);
	void ExpressionSetTempValue(temp_value_s& token);
	expression_token EvaluateExpressionTokens(std::list<expression_token>& tokens);
	void ExpressionImplicitCast(expression_token& left, expression_token& right);
	Substr_s ExpressionFindParentheses(std::string& expression);

	struct s_rules
	{
		bool next_postfix = false;
		bool next_unary = true;
		bool next_operator = false;
		bool ignore_postfix = false;
		bool operator_allowed = false;
		bool suffix_allowed = false;
		void reset()
		{
			next_postfix = false;
			next_unary = true;
			next_operator = false;
			ignore_postfix = false;
			operator_allowed = false;
			suffix_allowed = false;
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

			ExpressionImplicitCast(left, right);

			//only VT_STRING type can be a dynamic size
			//subject to change
			result.rval = std::shared_ptr<rvalue>(new rvalue(right.get_type(), (right.get_type() == VarType::VT_STRING ? strlen(right.get_string()) : 0u)));


			result.set_type(right.get_type());
			switch (left.get_type()) {
			case VarType::VT_CHAR:
				result.set_value(left.get_char() + right.get_char());
				result.content = std::to_string(result.get_char());
				return result;
				case VarType::VT_INT:

					result.set_value(left.get_int() + right.get_int());
					result.content = std::to_string(result.get_int());
					return result;
				case VarType::VT_FLOAT:

					result.set_value(left.get_float() + right.get_float());
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
		{ "=", [](expression_token& left, expression_token& right) -> expression_token
		{
			if (!left.is_lvalue())
				throw std::exception("left operand must be a modifiable lvalue");

			Variable* var = left.lval->ref;

			expression_token result;

			result.lval = left.lval;
			result.set_type(left.get_type());
			result.content = var->name;

			if (var->is_pointer()) {
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
		

	};

}
#endif