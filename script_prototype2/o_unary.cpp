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

	switch (token.get_type()) {
	case dataTypes_e::CHAR:
		std::cout << "incrementing lvalue from " << lvalue->get_char() << " to ";

		lvalue->set_value<int>(lvalue->get_char() + 1);
		std::cout << lvalue->get_char() << '\n';

		break;
	case dataTypes_e::INT:
		std::cout << "incrementing lvalue from " << lvalue->get_int() << " to ";

		lvalue->set_value<int>(lvalue->get_int() + 1);
		std::cout << lvalue->get_int() << '\n';

		break;
	case dataTypes_e::FLOAT:
		std::cout << "incrementing lvalue from " << lvalue->get_float() << " to ";
		lvalue->set_value<float>(lvalue->get_float() + 1.f);
		std::cout << lvalue->get_float() << '\n';
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

	switch (token.get_type()) {
	case dataTypes_e::CHAR:
		std::cout << "decrementing lvalue from " << lvalue->get_char() << " to ";

		lvalue->set_value<int>(lvalue->get_char() - 1);
		std::cout << lvalue->get_char() << '\n';

		break;
	case dataTypes_e::INT:
		std::cout << "decrementing lvalue from " << lvalue->get_int() << " to ";
		lvalue->set_value<int>(lvalue->get_int() - 1);
		std::cout << lvalue->get_int() << '\n';
		break;
	case dataTypes_e::FLOAT:
		std::cout << "decrementing from " << lvalue->get_float() << " to ";
		lvalue->set_value<float>(lvalue->get_float() - 1.f);
		std::cout << lvalue->get_float() << '\n';

		break;
	}

	token.lvalue_to_rvalue();
}

void unaryFunctions::negate(expression_token& token)
{
	switch (token.get_type()) {
	case dataTypes_e::CHAR:
		std::cout << "negating from " << token.get_char() << " to ";
		token.set_value<int>(-token.get_char());
		std::cout << token.get_char() << '\n';
		return;
	case dataTypes_e::INT:
		std::cout << "negating from " << token.get_int() << " to ";
		token.set_value<int>(-token.get_int());
		std::cout << token.get_int() << '\n';
		return;
	case dataTypes_e::FLOAT:
		std::cout << "negating from " << token.get_float() << " to ";
		token.set_value<float>(-token.get_float());
		std::cout << token.get_float() << '\n';
		return;
	}

	throw scriptError_t(&token.get_token(), "cannot negate this type!");

}