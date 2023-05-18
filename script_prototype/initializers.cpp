#include "pch.h"

void init::SetVariableInitializer(Variable& target, const std::string& expression)
{

	const auto result = expr::EvaluateEntireExpression(expression);

	if (target.is_reference()) {
		if (!result.is_lvalue())
			throw std::exception("reference initializer must be an lvalue");
	}


}