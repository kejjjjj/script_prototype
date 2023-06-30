#include "o_postfix.hpp"

void postfixFunctions::createFunctions()
{
	static bool init = false;

	if (init)
		return;

	postfix_functions.insert({ P_BRACKET_OPEN, subscript });

	init = true;
}

//it is not necessary for Arg_statement.end to point to the ] character
void postfixFunctions::subscript(expression_token& operand, std::optional<token_statement_t&> Arg_statement)
{
	
	const auto is_opening_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_OPEN;
	};
	const auto is_closing_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_BRACKET_CLOSE;
	};
	if (!Arg_statement.has_value())
		throw scriptError_t("how the frick was subscript postfix called without a value???");

	token_statement_t statement = Arg_statement.value();
	token_statement_t expression_statement = statement;

	if (!is_opening_bracket(*statement.it)) {
		throw scriptError_t(&*statement.it, "how the frick was subscript postfix called without \"[\" as the first token???");
	}

	expression_statement.it++; //skip the [

	while (statement.it != statement.end) {

		if (is_closing_bracket(*statement.it)) {

			expression_statement.end = statement.it;
			expression_statement.end--; //cut off the last ]


		}

		statement.it++;
	}

}