#include "r_operator_standard.hpp"

void runtimeEvaluationFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	eval_functions.insert({ P_ADD, arithmetic_addition });
	eval_functions.insert({ P_ASSIGN, assignment });

	init = true;
}

r_expression_token runtimeEvaluationFunctions::arithmetic_addition(r_expression_token& left, r_expression_token& right)
{

	left.lvalue_to_rvalue();
	right.lvalue_to_rvalue();

	left.implicit_cast(right);

	//at this point both operands have the same datatype

	r_expression_token result;
	result.rval = std::unique_ptr<rvalue>(new rvalue({ .type = left.get_type(), .size = NULL, .token = &left.get_token() }));

	switch (left.get_type()) {
	case dataTypes_e::CHAR:
		//std::cout << left.get_char() << " + " << right.get_char() << " = ";
		result.set_value<int>(left.get_char() + right.get_char());
		//std::cout << result.get_char() << '\n';
		break;
	case dataTypes_e::INT:
		//std::cout << left.get_int() << " + " << right.get_int() << " = ";
		result.set_value<int>(left.get_int() + right.get_int());
		//std::cout << result.get_int() << '\n';
		break;
	case dataTypes_e::FLOAT:
		//std::cout << left.get_float() << " + " << right.get_float() << " = ";

		result.set_value<float>(left.get_float() + right.get_float());
		//std::cout << result.get_float() << '\n';

		break;
	}

	return result;

}
r_expression_token runtimeEvaluationFunctions::assignment(r_expression_token& left, r_expression_token& right)
{

	if (left.lval->is_array()) {
		left.lval->replace_array(right.lval->arrayElements, right.lval->numElements);
		left.lval->isInitialized = true;
		return std::move(left);
	}

	right.lvalue_to_rvalue();

	left.implicit_cast(right);


	switch (left.get_type()) {
	case dataTypes_e::CHAR:
	//	std::cout << "assigning from " << left.get_char() << " -> ";
		left.set_value<int>(right.get_char());
		//std::cout << left.get_char() << '\n';
		break;
	case dataTypes_e::INT:
	//	std::cout << "assigning from " << left.get_int() << " -> ";
		left.set_value<int>(right.get_int());
		//std::cout << left.get_int() << '\n';
		break;
	case dataTypes_e::FLOAT:
		//std::cout << "assigning from " << left.get_float() << " -> ";
		left.set_value<float>(right.get_float());
		//std::cout << left.get_float() << '\n';
		break;
	}
	left.lval->isInitialized = true;
	return std::move(left);
}