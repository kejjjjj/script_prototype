#include "pch.h"


void init::SetVariableInitializer(Variable& target, const std::string& expression)
{
	//NOTE: the expression argument string doesn't contain the "target = " part of the expression, only the expression after that

	const auto result = expr::EvaluateEntireExpression(expression);

	if (result.tokentype == VarType::VT_INVALID) { //this means no initializer
		if (target.is_reference()) {
			stack_variables.erase(target.name);
			throw std::exception("a reference declaration requires an initializer");
		}
		return;
	}

	const auto left_type = target.s_getvariabletype();
	const auto right_type = result.is_lvalue() ? result.lval->ref->s_getvariabletype() : VarTypes[int(result.rval->get_type())];

	if (!expr::ExpressionCompatibleOperands(target, result)) {
		stack_variables.erase(target.name);
		throw std::exception(std::format("a variable of type \"{}\" cannot be initialized with type \"{}\"", left_type, right_type).c_str());
	}

	if (target.is_reference()) {
		if (!result.is_lvalue()) {
			stack_variables.erase(target.name);
			throw std::exception("reference initializer must be an lvalue");
		}

		if (result.get_type() != target.get_type()) {
			stack_variables.erase(target.name);
			throw std::exception(std::format("a reference of type \"{}\" cannot be initialized with type \"{}\"", left_type, right_type).c_str());

		}
		memcpy_s(target.reference.get(), sizeof(Variable), result.lval->ref, sizeof(Variable));
		std::cout << std::format("\"{}\" reference updated! (now points to \"{}\")\n", target.name, target.reference->name);

	}

	target.set_value(&result);

	expr::rules.reset();


}