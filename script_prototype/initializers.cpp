#include "pch.h"


void init::SetVariableInitializer(Variable& target, const std::string& expression)
{
	//NOTE: the expression argument string doesn't contain the "target = " part of the expression, only the expression after that

	auto begin = expression.begin();


	//if (IsInitializerList(begin, expression.end()))
	//	return EvaluateEntireInitializerList(target, std::string(begin, expression.end()));

	const auto result = expr::EvaluateEntireExpression(expression);

	if (result.tokentype == VarType::VT_INVALID) { //this means no initializer
		throw std::exception("expected an initializer");
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

bool init::IsInitializerList(std::string::const_iterator& it, std::string::const_iterator end)
{
	while (std::isspace(*it))
		++it;

	return *it == '{';

}

//void init::EvaluateEntireInitializerList(std::list<Variable&>& vars, const std::string& expression)
//{
//	auto endpos = expression.find('}');
//
//	if (endpos == std::string::npos)
//		throw std::exception("expected a \"}\"");
//
//	std::string new_expr = expression.substr(1, endpos - 1);
//
//	std::list<std::string> tokens;
//	const auto num_args = TokenizeString(new_expr, ',', tokens);
//	size_t args_processed = 0;
//
//	for (auto& i : tokens)
//		std::cout << i << '\n';
//
//	auto& var = vars.front();
//
//	
//
//	for (auto it = tokens.begin(); it != tokens.end(); ++it) {
//		
//		if (var.is_array()) {
//
//			if (var.numElements >= args_processed) {
//				throw std::exception("too many initializer values");
//			}
//
//			var.arr[args_processed].set_value(expr::EvaluateEntireExpression(*it));
//		}
//
//
//		++args_processed;
//	}
//
//
//
//}