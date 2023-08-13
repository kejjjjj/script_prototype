#include "if_statement.hpp"
#include "expression.hpp"

void if_statement::evaluate_statement()
{
	auto& it = statement.it;
	//skip the keyword
	++it;


	//std::cout << "evaluate_if_statement( ";
	//for (auto _it = it; _it != statement.end; ++_it) {
	//	std::cout << _it->string << ' ';
	//}
	//std::cout << " )\n";
	
	auto parentheses_statement = token_statement_t{ .it = it, .begin = it, .end = statement.end };
	parentheses_statement.end++;
	expression_t::ExpressionFindMatchingParenthesis(parentheses_statement);

	//skip the (
	++it;

	//_statement.it contains the position of the matching ), so it will be the end
	const token_statement_t _statement = token_statement_t{ .it = it, .begin = it, .end = --parentheses_statement.it };

	auto token = expression_t(block, _statement).EvaluateEntireExpression();

	if (token.is_integral() == false) {
		throw scriptError_t(&token.get_token(), "expression must be convertible to a boolean type");
	}

	if (token.implicit_cast<int>()) {
		std::cout << "yippee the if statement will execute because the expression evaluated to: " << token.get_int() << '\n';
	}else
		std::cout << "oh no the if statement will not execute because the expression evaluated to: " << token.get_int() << '\n';

	it = ++parentheses_statement.it;
	++it;

}