#include "initializer_list.hpp"
#include "expression.hpp"

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

	const auto get_initializer_list_type = [&target](const std::list<token_statement_t>& lists)
	{
		std::string r;
		std::for_each(lists.begin(), lists.end(), [&r](const token_statement_t&) {r += "[]"; });
		return r;

	};

	std::list<token_statement_t> initializer_lists;
	tokenize_lists(initializer_lists);

	if (!initializer_lists.empty()) {

		if (!target.is_array()) {
			const auto type = get_type_as_text(target.get_type());
			throw scriptError_t(&*statement.it, std::format("\"{}\" is incompatible with \"{}\"", type + get_initializer_list_type(initializer_lists), type));
		}

		target.resize_array(initializer_lists.size());
	}
	size_t arrayIndex = 0;
	for (auto& i : initializer_lists) {

		initializer_list_t ilist(i, target.arrayElements[arrayIndex]);
		ilist.evaluate();
		arrayIndex++;
	}


	if (!initializer_lists.empty())
		return;

	
	initialize_array(target);

}
void initializer_list_t::initialize_array(Variable& targetArray)
{

	if (!targetArray.is_array()) {
		const auto type = targetArray.s_getvariabletype();
		throw scriptError_t(&*statement.it, std::format("\"{}\" is incompatible with \"{}\"", type + "[]", type));
	}

	std::list<token_statement_t> expressions;
	tokenize_values(expressions);

	if (expressions.empty())
		throw scriptError_t("include at least one initializer");

	targetArray.resize_array(expressions.size());
	expression_token left_operand, right_operand;

	const auto assign_function = evaluationFunctions::getInstance().find_function(P_ASSIGN);

	if (!assign_function.has_value())
		throw scriptError_t("initialize_array(): how?");

	size_t arrayIndex = 0;
	for (auto& i : expressions) {

		right_operand = expression_t(i).EvaluateEntireExpression();
		left_operand = targetArray.arrayElements[arrayIndex].to_expression();

		assign_function.value()(left_operand, right_operand);

		arrayIndex++;

	}
}
void initializer_list_t::tokenize_lists(std::list<token_statement_t>& statements)
{
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};
	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
	};
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

		new_statement.it++;

		if (!is_comma(*new_statement.it)) {
			throw scriptError_t(&*new_statement.it, "expected a \",\"");

		}

		new_statement.it++;

		if (!is_opening(*new_statement.it)) {
			throw scriptError_t(&*new_statement.it, "expected a \"{\"");
		}

		statement_copy = new_statement;
		
	}

	//throw scriptError_t(&*statement_copy.it, "ummm?");
}
void initializer_list_t::tokenize_values(std::list<token_statement_t>& statements)
{
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};

	auto& it = statement.it;
	std::list<token_t>::iterator beginning = it;

	while (it != statement.end) {

		if (is_comma(*it)) {
			--it; //get the ending
			statements.push_back({ .it = beginning, .begin = beginning, .end = it });
			beginning = (++(++it))--; //skip the ,
		}

		++it;
	}

	if (is_comma(*it)) {
		statements.push_back({ .it = it, .begin = it, .end = it }); //push_back an expression that will instantly fail :)
	}else
		statements.push_back({ .it = beginning, .begin = beginning, .end = it });

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