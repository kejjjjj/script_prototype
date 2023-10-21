#pragma once

#include "script.hpp"
#include "scope.hpp"


struct r_expression_token
{


	void insert_prefix(token_t& _token) { prefix.push_back(_token); }
	void insert_postfix(const code_segment_t& _token, const punctuation_e p) { postfix.push_back({ _token, p }); }

	void set_token(token_t& _token) { token = _token; }
	token_t& get_token() noexcept {
		if (is_rvalue())
			return rval->token;
		return token;
	}

	void set_scope(scr_scope_t* _block) noexcept(true) { block = _block; }

	void lvalue_to_rvalue();

	void eval_postfix(scr_scope_t* block);
	void eval_prefix();

	template<typename T> T implicit_cast() const;
	void implicit_cast(r_expression_token& other);
	void cast_weaker_operand(r_expression_token& other);

	int get_int() const {
		if (is_lvalue())
			return lval->get_int();
		return rval->get_int();
	}
	float get_float() const {
		if (is_lvalue())
			return lval->get_float();
		return rval->get_float();
	}
	char get_char() const {
		if (is_lvalue())
			return lval->get_char();
		return rval->get_char();

	}
	auto get_type() const {
		if (is_lvalue())
			return lval->get_type();
		return rval->get_type();
	}


	bool is_lvalue() const noexcept { return lval; }
	bool is_rvalue() const noexcept { return rval.get(); }


	template<typename T>
	void set_value(T value) {
		if (is_rvalue())
			rval->set_value(value);
		else if (is_lvalue())
			lval->set_value(value);
	}
	size_t size_of() const {
		if (is_rvalue())
			return rval->value.buf_size;
		return lval->value.buf_size;
	}
	size_t array_depth() const {
		if (is_rvalue())
			return 0ull;
		return lval->array_depth();
	}

	std::underlying_type_t<punctuation_e> op = 0;
	std::underlying_type_t<OperatorPriority> op_priority = 0;

	std::unique_ptr<rvalue> rval;
	Variable* lval = 0;

	std::list<token_t> prefix;
	std::list<std::pair<code_segment_t, punctuation_e>> postfix;
private:

	token_t token;
	scr_scope_t* block = 0;

};

//static_assert(sizeof(r_expression_token) == sizeof(expression_token), "sizeof(r_expression_token) == sizeof(expression_token)");


