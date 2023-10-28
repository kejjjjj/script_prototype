#include "o_postfix.hpp"
#include "expression.hpp"
void postfixFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	postfix_functions.insert({ P_BRACKET_OPEN, subscript });
	postfix_functions.insert({ P_PAR_OPEN, function_call });

	init = true;
}

void postfixFunctions::subscript(scr_scope_t* block, expression_token& operand, postfixBase* code)
{

	if (operand.is_lvalue() == false) throw scriptError_t(&operand.get_token(), "[] operand must be an lvalue");
	if (operand.lval->is_array() == false)  throw scriptError_t(&operand.get_token(), "[] operand must have an array type");
	//if (Arg_statement.has_value() == false) throw scriptError_t("how the frick was subscript postfix called without a value???");


	postfix_squarebracket* statement = dynamic_cast<postfix_squarebracket*>(code);

	const auto result = eval_expression(block, statement->expression);

	if (result.is_integral() == false) {
		throw scriptError_t(&*statement->expression.it, "expression must evaluate to an integral type");
	}

	//const int index = result.get_int();

	//if (index >= operand.lval->numElements) {
	//	throw scriptError_t(&*statement->expression.it, std::format("attempted to access array index {} when maxSize was {}", index, operand.lval->numElements));

	//}

	operand.lval = operand.lval->arrayElements[0].get();

	LOG("array lvalue updated\n");

	return;
}

void postfixFunctions::function_call(scr_scope_t* block, expression_token& operand, postfixBase* code)
{
	if (!operand.is_function()) {
		throw scriptError_t(&operand.get_token(), std::format("\"{}\" does not have a function type", operand.get_token().string));
	}

	LOG("CALLING FUNCTION\n");

	postfix_parenthesis* callee = dynamic_cast<postfix_parenthesis*>(code);
	auto target_function = operand.function;

	LOG("source args: " << callee->args.size() << " and target args: " << target_function->numArgs << '\n');

	if (callee->args.size() != target_function->numArgs) {
		throw scriptError_t(&operand.get_token(), std::format("expected {} arguments to function call but got {}", target_function->numArgs, callee->args.size()));
	}

	auto target_it = target_function->args.begin();
	size_t currentArg = 1;

	for (auto callee_it = callee->args.begin(); callee_it != callee->args.end(); callee_it++) {

		if (!callee_it->has_value()) {
			throw scriptError_t(&operand.get_token(), std::format("argument {}: empty expression not allowed", currentArg));
			continue;
		}

		const expression_token result = eval_expression(block, callee_it->value());
		const auto left_type = result.is_lvalue() ? result.lval->s_getvariabletype() : get_type_as_text(result.get_type());

		if (!result.is_compatible_with_type(*target_it)) {
			throw scriptError_t(&operand.get_token(), std::format("argument {}: cannot cast from {} to {}", currentArg, left_type, target_it->get_as_text()));

		}

		++target_it;
		++currentArg;
	}

	//todo: expression_token to return type

}