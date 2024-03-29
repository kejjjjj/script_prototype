#include "o_unary.hpp"

void unaryFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	unary_functions.insert({ P_INCR, increment });
	unary_functions.insert({ P_DECR, decrement });
	unary_functions.insert({ P_SUB, negate });

	init = true;
}

void unaryFunctions::increment(expression_token& token)
{
	if (token.is_lvalue() == false) {
		throw scriptError_t(&token.get_token(), "increment operator cannot be used on a non-lvalue");
	}

	auto lvalue = token.lval;

	if (lvalue->is_array())
		throw scriptError_t(&token.get_token(), "cannot increment an array");

	switch (token.get_type()) {
	case dataTypes_e::CHAR:
		break;
	case dataTypes_e::INT:
		break;
	case dataTypes_e::FLOAT:
		break;
	}

	token.lvalue_to_rvalue();
}
void unaryFunctions::decrement(expression_token& token)
{
	if (token.is_lvalue() == false) {
		throw scriptError_t(&token.get_token(), "decrement operator cannot be used on a non-lvalue");
	}

	auto lvalue = token.lval;

	if (lvalue->is_array())
		throw scriptError_t(&token.get_token(), "cannot decrement an array");

	token.lvalue_to_rvalue();
}

void unaryFunctions::negate(expression_token& token)
{
	switch (token.get_type()) {
	case dataTypes_e::CHAR:
		//LOG("negating from " << token.get_char() << " to ");
		//token.set_value<int>(-token.get_char());
		//LOG(token.get_char() << '\n');
		return;
	case dataTypes_e::INT:
		//LOG("negating from " << token.get_int() << " to ");
		//token.set_value<int>(-token.get_int());
		//LOG(token.get_int() << '\n');
		return;
	case dataTypes_e::FLOAT:
		//LOG("negating from " << token.get_float() << " to ");
		//token.set_value<float>(-token.get_float());
		//LOG(token.get_float() << '\n');
		return;
	}

	throw scriptError_t(&token.get_token(), "cannot negate this type!");

}