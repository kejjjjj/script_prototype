#include "o_standard.hpp"

void evaluationFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	eval_functions.insert({ P_ADD, arithmetic_addition });
	eval_functions.insert({ P_ASSIGN, assignment});

	init = true;
}

expression_token evaluationFunctions::arithmetic_addition(expression_token& left, expression_token& right)
{
	left.lvalue_to_rvalue();
	right.lvalue_to_rvalue();

	auto pos = left.rval->get_code_pos();

	left.implicit_cast(right);

	//at this point both operands have the same datatype

	expression_token result;
	result.rval = std::shared_ptr<rvalue>(new rvalue({.type=left.get_type(), .size=NULL, .token=&left.get_token()}));

	switch (left.get_type()) {
	case dataTypes_e::CHAR:
		std::cout << left.get_char() << " + " << right.get_char() << " = ";
		result.set_value<int>(left.get_char() + right.get_char());
		std::cout << result.get_char() << '\n';
		break;
	case dataTypes_e::INT:
		std::cout << left.get_int() << " + " << right.get_int() << " = ";
		result.set_value<int>(left.get_int() + right.get_int());
		std::cout << result.get_int() << '\n';
		break;
	case dataTypes_e::FLOAT:
		std::cout << left.get_float() << " + " << right.get_float() << " = ";

		result.set_value<float>(left.get_float() + right.get_float());
		std::cout << result.get_float() << '\n';

		break;
	}

	return result;

}
expression_token evaluationFunctions::assignment(expression_token& left, expression_token& right)
{
	if (!left.is_lvalue())
		throw scriptError_t(&left.get_token(), "left operand must be modifiable");

	right.lvalue_to_rvalue();

	left.implicit_cast(right);


	switch (left.get_type()) {
	case dataTypes_e::CHAR:
		std::cout << left.get_char() << " -> ";
		left.set_value<int>(right.get_char());
		std::cout << left.get_char() << '\n';
		break;
	case dataTypes_e::INT:
		std::cout << left.get_int() << " -> ";
		left.set_value<int>(right.get_int());
		std::cout << left.get_int() << '\n';
		break;
	case dataTypes_e::FLOAT:
		std::cout << left.get_float() << " -> ";
		left.set_value<float>(right.get_float());
		std::cout << left.get_float() << '\n';
		break;
	}

	return left;
}