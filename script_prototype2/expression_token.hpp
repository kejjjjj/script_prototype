#ifndef expr_tok
#define expr_tok

#include "pch.hpp"
#include "script.hpp"
#include "rvalue.hpp"
#include "variable.hpp"

struct expression_token
{
	expression_token() = default;
	expression_token(token_t& _token) : token(_token) {

	}
	void insert_prefix(token_t& _token) { prefix.push_back(&_token); }
	void insert_postfix(token_t& _token) { postfix.push_back(&_token); }
	void set_token(token_t& _token) { token = _token; }
	token_t& get_token() noexcept { return token; }

	void set_value_category();
	void eval_postfix();
	void eval_prefix();

	void lvalue_to_rvalue();
	template<typename T> T implicit_cast() const;

	void implicit_cast(expression_token& other);

	void print() const noexcept;

	auto get_type() const {
		if (is_lvalue())
			return lval->get_type();
		else if (is_rvalue())
			return rval->get_type();

		throw scriptError_t(&token, "get_type() without value ok");
	}

	int get_int() const { 
		if (is_lvalue())
			return lval->get_int();
		else if (is_rvalue())
			return rval->get_int();

		throw scriptError_t(&token, "get_int() without value ok");
	}
	float get_float() const {
		if (is_lvalue())
			return lval->get_float();
		else if (is_rvalue())
			return rval->get_float();

		throw scriptError_t(&token, "get_float() without value ok");
	}
	bool is_lvalue() const noexcept { return lval; }
	bool is_rvalue() const noexcept { return rval.use_count(); }

	template<typename T>
	void set_value(T value) {
		if (is_rvalue())
			rval->set_value(value);
		else if (is_lvalue())
			lval->set_value(value);
	}

	bool op = false;
	std::shared_ptr<rvalue> rval;
	Variable* lval = 0;
private:
	
	void cast_weaker_operand(expression_token& other);

	std::list<token_t*> prefix;
	std::list<token_t*> postfix;
	token_t token;
};


#endif