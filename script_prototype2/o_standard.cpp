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
	
	left.test_operand_compatibility(right);

	left.lvalue_to_rvalue();
	right.lvalue_to_rvalue();

	auto pos = left.rval->get_code_pos();

	left.implicit_cast(right);

	//at this point both operands have the same datatype

	expression_token result;
	result.rval = std::unique_ptr<rvalue>(new rvalue({.type=left.get_type(), .size=NULL, .token=&left.get_token()}));

	//switch (left.get_type()) {
	//case dataTypes_e::CHAR:
	//	result.set_value<int>(left.get_char() + right.get_char());
	//	break;
	//case dataTypes_e::INT:
	//	result.set_value<int>(left.get_int() + right.get_int());
	//	break;
	//case dataTypes_e::FLOAT:
	//	result.set_value<float>(left.get_float() + right.get_float());
	//	break;
	//}

	return result;

}
expression_token evaluationFunctions::assignment(expression_token& left, expression_token& right)
{
	if (!left.is_lvalue())
		throw scriptError_t(&left.get_token(), "left operand must be modifiable");

	left.test_operand_compatibility(right);


	if (left.lval->is_array()) {

		//OK JUST FOR TESTING PURPOSES I WILL NOT CHECK IF THE ARRAYS ARE COMPATIBLE

		left.lval->replace_array(right.lval->arrayElements, right.lval->numElements);
		left.lval->isInitialized = true;
		return std::move(left);
	}

	right.lvalue_to_rvalue();

	left.implicit_cast(right);


	//switch (left.get_type()) {
	//case dataTypes_e::CHAR:
	//	left.set_value<int>(right.get_char());
	//	break;
	//case dataTypes_e::INT:
	//	left.set_value<int>(right.get_int());
	//	break;
	//case dataTypes_e::FLOAT:
	//	left.set_value<float>(right.get_float());
	//	break;
	//}
	left.lval->isInitialized = true;
	return std::move(left);
}