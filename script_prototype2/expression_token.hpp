#ifndef expr_tok
#define expr_tok

#include "pch.hpp"
#include "script.hpp"
#include "rvalue.hpp"
#include "variable.hpp"
#include "scope.hpp"

struct postfixBase
{

	virtual punctuation_e getType() const = 0;

private:
	
};

struct postfix_squarebracket : public postfixBase
{
	punctuation_e getType() const override { 
		return punctuation_e::P_BRACKET_OPEN; 
	}

	code_segment_t expression;
};
struct postfix_parenthesis : public postfixBase
{
	punctuation_e getType() const override {
		return punctuation_e::P_PAR_OPEN;
	}

	std::list<std::optional<code_segment_t>> args;
};

struct expression_token
{
	expression_token() = default;
	expression_token(token_t& _token) : token(_token) {

	}
	void insert_prefix(token_t& _token) { prefix.push_back(&_token); }
	void insert_postfix(std::shared_ptr<postfixBase>&& _token) { postfix.push_back( (_token) ); }
	void set_token(token_t& _token) { token = _token; }
	token_t& get_token() noexcept { 
		if(is_rvalue()) 
			return rval->token;
		return token; }

	void set_value_category();
	void eval_postfix(scr_scope_t* block);
	void eval_prefix();

	void set_scope(scr_scope_t* _block) noexcept(true) { block = _block; }

	void lvalue_to_rvalue();
	template<typename T> T implicit_cast() const;

	void implicit_cast(expression_token& other);
	void implicit_force_cast(const datatype_declaration& target);
	datatype_declaration get_datatype() const noexcept;
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
		else if (is_lvalue())
			return lval->value.buf_size;

		throw scriptError_t(&token, "unknown expression used in size_of()");


	}
	size_t array_depth() const {
		if (is_rvalue())
			return 0ull;
		else if (is_lvalue())
			return lval->array_depth();

		throw scriptError_t(&token, "unknown expression used in array_depth()");
	}

	void test_operand_compatibility(const expression_token& other) const;
	bool compatible_operand(const expression_token& other) const;
	bool is_compatible_with_type(const datatype_declaration& other) const noexcept;
	bool is_integral() const noexcept { return get_type() >= dataTypes_e::CHAR && get_type() < dataTypes_e::FLOAT && array_depth() == NULL; }

	std::underlying_type_t<punctuation_e> op = 0;
	std::underlying_type_t<OperatorPriority> op_priority = 0;

	std::shared_ptr<rvalue> rval;
	Variable* lval = 0;

	std::list<token_t*> prefix;
	std::list<std::shared_ptr<postfixBase>> postfix;

	void cast_to_type(const dataTypes_e target);

private:
	bool compatible_array_operand(const expression_token& other) const;
	void cast_weaker_operand(expression_token& other);

	token_t token;
	scr_scope_t* block = 0;
};

#endif