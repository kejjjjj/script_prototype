#include "o_postfix.hpp"
#include "expression.hpp"
void postfixFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	postfix_functions.insert({ P_BRACKET_OPEN, subscript });

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

	const int index = result.get_int();

	if (index >= operand.lval->numElements) {
		throw scriptError_t(&*statement->expression.it, std::format("attempted to access array index {} when maxSize was {}", index, operand.lval->numElements));

	}

	operand.lval = operand.lval->arrayElements[index].get();

	LOG("array lvalue updated\n");

	return;
}




//misc

bool ExpressionFindMatchingBracket(code_segment_t& token)
{
	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_CLOSE;
	};

	if (is_opening(*token.it) == false)
		return 0;

	token.it++;
	size_t numOpen = 1;
	size_t numClosing = 0;

	while (token.it != token.end) {
		if (is_opening(*token.it))
			numOpen++;

		else if (is_closing(*token.it)) {
			numClosing++;

			if (numOpen == numClosing) {
				return 1;
			}
		}

		token.it++;
	}

	throw scriptError_t(&*token.it, "expected to find a \"]\"");

}