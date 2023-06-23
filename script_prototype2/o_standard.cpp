#include "o_standard.hpp"

void evaluationFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	eval_functions.insert({ P_ADD, arithmetic_addition });

	init = true;
}

expression_token evaluationFunctions::arithmetic_addition(expression_token& left, expression_token& right)
{
	left.lvalue_to_rvalue();
	right.lvalue_to_rvalue();

	left.implicit_cast(right);

	//at this point both operands have the same datatype

	expression_token result;
	result.rval = std::shared_ptr<rvalue>(new rvalue({.type=left.get_type(), .size=NULL, .token=&left.get_token()}));

	switch (left.get_type()) {
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