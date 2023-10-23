#include "initializer_list.hpp"
#include "expression.hpp"
#include "o_standard.hpp"

initializer_list_t::initializer_list_t(scr_scope_t* scope, const code_segment_t& _statement, Variable* _target)
	: statement(_statement), targetVar(_target), block(scope)
{
	if (statement.begin == statement.end)
		throw scriptError_t(&*statement.begin, "how did this initializer list exception occur?");

	//if (auto substr = find_curlybracket_substring(statement)) {
	//	statement.it++; //ignore first {
	//	statement.end = --(substr.value().second.end); //ignore last }
	//}
	//else
	//	throw scriptError_t(&*statement.it, "HOW is this real");


}
void initializer_list_t::evaluate()
{
	evaluate_list(targetVar);
}
void initializer_list_t::evaluate_list(Variable* target)
{
	if (!target->is_array()) {
		throw scriptError_t(&*statement.it, "too many braces for an initializer list");
	}

	//LOG("depth: " << target.array_depth() << " | for statement: ";
	//for (auto it = statement.it; it != statement.end; it++) {
	//	LOG(it->string;
	//}
	//LOG(statement.end->string << '\n';

	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};

	size_t elementIndex = 0ull;

	const auto assign_function = evaluationFunctions::getInstance().find_function(P_ASSIGN);

	if (!assign_function.has_value())
		throw scriptError_t("evaluate_list2(): how?");

	while (true) {

		if (elementIndex+1ull > target->numElements)
			target->resize_array(elementIndex+1ull);

		if (auto bracket_statement = find_curlybracket_substring(statement)) {


			auto& _statement = bracket_statement.value();

			_statement.print();
			
			size_t zeroDepth = target->array_depth();

			//LOG("and the array depth for that is " << zeroDepth-1 << '\n';

			_statement.it++;
			_statement.end--;

			//LOG("----- current array tree -----\n";
			//target.print();
			//LOG("----- current array tree -----\n";

			if (target->arrayElements[elementIndex]->array_depth() != zeroDepth - 1) {
				//LOG("what the heeeck!\n";
				target->set_array_depth(zeroDepth);
			}

			initializer_list_t ilist(block, _statement, target->arrayElements[elementIndex].get());
			(statement.it = ++_statement.end); //iterate to the next character

			if (statement.it != statement.end)
				statement.it++;

			ilist.evaluate();
			
		}
		else if(const auto expression_statement = read_expression(statement)) {
			
			auto expression = expression_t(block, expression_statement.value()).EvaluateEntireExpression();
			

			auto l = target->arrayElements[elementIndex]->to_expression();
			l.set_token(expression.get_token());
			assign_function.value()(l, expression);
		}
		else throw scriptError_t(&*statement.it, "fuck you");

		if (statement.it == statement.end)
			break;

		if (!is_comma(*statement.it))
			throw scriptError_t(&*statement.it, "expected a \",\"");

		statement.it++; //skip the ,

		elementIndex++;
	}

	LOG("elements in list: " << elementIndex << '\n');
}

std::optional<code_segment_t> initializer_list_t::read_expression(code_segment_t& _statement)
{
	const auto is_comma = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_COMMA;
	};

	auto& it = _statement.it;
	std::list<token_t>::iterator beginning = it;
	
	code_segment_t result;

	while (it != _statement.end) {

		if (is_comma(*it)) {
			--it; //get the ending
			result = { .it = beginning, .begin = beginning, .end = it };
			++it; //go back to the comma
			return result;
		}

		++it;
	}

	return result = {.it = beginning, .begin = beginning, .end = it };
}
//accepts } as statement.end iterator
std::optional<code_segment_t> initializer_list_t::find_curlybracket_substring(const code_segment_t& statement_)
{
	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_CLOSE;
	};

	code_segment_t token = statement_;

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
				return code_segment_t{.it = statement_.it, .begin = statement_.it, .end = token.it };
			}
		}


		token.it++;
	}

	if (is_closing(*token.it)) {
		numClosing++;

		if (numOpen == numClosing) {
			return code_segment_t{ .it = statement_.it, .begin = statement_.it, .end = token.it };
		}
	}

	throw scriptError_t(&*token.it, ("expected to find a \"}\""));
}