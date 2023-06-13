#include "pch.h"


StatementType Compile_EvaluateStatement2(const std::string_view& expr)
{
	for (auto& [name, statement] : statement_type) {
		if (!expr.compare(name))
			return statement;
	}

	return StatementType::NO_STATEMENT;
}