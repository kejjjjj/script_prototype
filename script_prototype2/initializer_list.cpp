#include "initializer_list.hpp"

initializer_list_t::initializer_list_t(const token_statement_t& _statement, Variable& _target) : statement(_statement), targetVar(_target)
{
	if (statement.begin == statement.end)
		throw scriptError_t(&*statement.begin, "how did this initializer list exception occur?");

	if (auto substr = find_curlybracket_substring(statement)) {
		statement.it++; //ignore first {
		statement.end = --(substr.value().it); //ignore last }
	}
	else
		throw scriptError_t(&*statement.it, "HOW is this real");


}
void initializer_list_t::evaluate()
{
	evaluate_list(targetVar);
}
void initializer_list_t::evaluate_list(Variable& target)
{
	if (!target.is_array())
		throw scriptError_t(&*statement.it, "initializer lists for non-array types are not supported yet! :)");

	std::list<token_statement_t> initializer_lists;
	tokenize_lists(initializer_lists);

	if(!initializer_lists.empty())
		target.resize_array(initializer_lists.size());

	size_t arrayIndex = 0;
	for (auto& i : initializer_lists) {

		initializer_list_t ilist(i, target.arrayElements[arrayIndex]);
		ilist.evaluate();

		arrayIndex++;
	}

	throw scriptError_t("yippee!\n");

	int ok[2][2] = { {0, 0}, {2, 2} };
}

void initializer_list_t::tokenize_lists(std::list<token_statement_t>& statements)
{
	token_statement_t statement_copy = statement;
	token_statement_t new_statement;
	while (auto result = find_curlybracket_substring(statement_copy)) {

		new_statement = result.value();

		statements.push_back(token_statement_t{ 
			.it = statement_copy.it, 
			.begin = statement_copy.begin, 
			.end = new_statement.it });

		if (new_statement.it == statement.end)
			return;

		statement_copy = new_statement;
		
		
	}

	throw scriptError_t(&*statement_copy.it, "expected a \",\"");
}

//accepts } as statement.end iterator
std::optional<token_statement_t> initializer_list_t::find_curlybracket_substring(const token_statement_t& statement_)
{
	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE;
	};

	token_statement_t token = statement_;

	if (is_opening(*token.it) == false)
		return std::nullopt;

	token.it++;
	size_t numOpen = 1;
	size_t numClosing = 0;

	while (token.it != token.end) {
		if (is_opening(*token.it))
			numOpen++;

		else if (is_closing(*token.it)) {
			numClosing++;

			if (numOpen == numClosing) {
				return token;
			}
		}

		token.it++;
	}

	if (is_closing(*token.it)) {
		numClosing++;

		if (numOpen == numClosing) {
			return token;
		}
	}

	throw scriptError_t(&*token.it, ("expected to find a \"}\""));
}