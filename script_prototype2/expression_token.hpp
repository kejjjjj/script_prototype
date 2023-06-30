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
	void insert_postfix(const token_statement_t& _token, const punctuation_e p) { postfix.push_back( { _token, p } ); }
	void set_token(token_t& _token) { token = _token; }
	token_t& get_token() noexcept { 
		if(is_rvalue()) 
			return rval->token;
		return token; }

	void set_value_category();
	void eval_postfix();
	void eval_prefix();

	void lvalue_to_rvalue();
	template<typename T> T implicit_cast() const;

	void implicit_cast(expression_token& other);
	bool is_literal() const 
	{ 
		if (is_lvalue() || is_rvalue())
			return false; 

		return token.tt >= tokenType::NUMBER_LITERAL && token.tt <= tokenType::CHAR_LITERAL;
	}

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
	char get_char() const {
		if (is_lvalue())
			return lval->get_char();
		else if (is_rvalue())
			return rval->get_char();

		throw scriptError_t(&token, "get_char() without value ok");
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
	size_t size_of() const {
		if (is_rvalue())
			return rval->value.buf_size;
		else if (is_lvalue())
			return lval->value.buf_size;

		throw scriptError_t(&token, "unknown expression used in size_of()");


	}
	bool is_integral() const noexcept { return get_type() > dataTypes_e::CHAR && get_type() < dataTypes_e::FLOAT; }

	bool op = false;
	std::shared_ptr<rvalue> rval;
	Variable* lval = 0;

	std::list<token_t*> prefix;
	std::list<std::pair<token_statement_t, punctuation_e>> postfix;
private:
	
	void cast_weaker_operand(expression_token& other);

	token_t token;
};


#endif