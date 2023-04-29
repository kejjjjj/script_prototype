#include "pch.h"

void decl::EvaluateDeclaration(const std::string_view& type, std::string::iterator& it)
{

	if (!srules.typename_allowed) {
		throw std::exception("type name is not allowed");
		return;
	}

	srules.expecting_identifier = true;

	const auto expression = cec::Compiler_ParseExpression(';', it);

	auto res = expr::EvaluateEntireExpression(expression);



}