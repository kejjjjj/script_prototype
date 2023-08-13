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

void postfixFunctions::subscript(scr_scope_t* block, expression_token& operand, std::optional<token_statement_t*> Arg_statement)
{

	if (operand.is_lvalue() == false) throw scriptError_t(&operand.get_token(), "[] operand must be an lvalue");
	if (operand.lval->is_array() == false)  throw scriptError_t(&operand.get_token(), "[] operand must have an array type");
	if (Arg_statement.has_value() == false) throw scriptError_t("how the frick was subscript postfix called without a value???");

	token_statement_t statement = *Arg_statement.value();

	const auto result = expression_t(block, statement).EvaluateEntireExpression();

	if (result.is_integral() == false) {
		throw scriptError_t(&*statement.it, "expression must evaluate to an integral type");
	}

	const int index = result.get_int();

	if (index > operand.lval->numElements) {
		throw scriptError_t(&*statement.it, std::format("attempted to access array index {} when maxSize was {}", index, operand.lval->numElements));

	}

	operand.lval = &operand.lval->arrayElements[index];

	std::cout << "array lvalue updated\n";

	statement.it++; //skip the ]
	return;
}




//misc

bool ExpressionFindMatchingBracket(token_statement_t& token)
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