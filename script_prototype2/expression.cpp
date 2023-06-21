#include "expression.hpp"

//each expression sequence will be converted to just one remaining expression
expression_token expression_t::EvaluateEntireExpression()
{
	if (!is_ready())
		throw scriptError_t(&*tokens.it, "EvaluateEntireExpression() called when the expression was invalid");

	std::cout << "EvaluateEntireExpression(";
	for (auto it = tokens.it; it != tokens.end; ++it) {
		std::cout << it->string<<' ';
	}
	std::cout << '\n';

	//do parentheses check here...
	{

	}

	return EvaluateExpression();
}
expression_token expression_t::EvaluateExpression()
{


	TokenizeExpression();
	std::for_each(sortedTokens.begin(), sortedTokens.end(), [](expression_token& e) { e.set_value_category(); });

	EvaluateExpressionTokens();
	

	return {};
}


void expression_t::TokenizeExpression()
{
	if (tokens.it == tokens.end)
		return;

	while (tokens.it != tokens.end) {
		if (!ParseExpression()) {
			throw scriptError_t(&*tokens.it, std::format("unexpected token \"{}\" used in expression\n", tokens.it->string));
		}

		if (tokens.it == tokens.end)
			break;

		if (!ParseOperator()) {
			if (tokens.it == tokens.end)
				throw scriptError_t(&*tokens.it, std::format("expected an expression before \"{}\"\n", tokens.it->string));

			throw scriptError_t(&*tokens.it, std::format("expected an operator instead of \"{}\"\n", tokens.it->string));

		}

	}

	for (auto& i : sortedTokens)
		i.print();

}
bool expression_t::ParseExpression()
{
	auto& it = tokens.it;
	expression_token token;


	const auto token_peek_unary = [&]()
	{

		if (it == tokens.end || it->tt != tokenType::PUNCTUATION)
			return false;

		if (!is_unary_operator(static_cast<punctuation_e>(LOWORD(it->extrainfo)))){
			return false;
		}

		return true;
	};
	const auto token_peek_name = [&]()
	{
		if (it == tokens.end)
			return false;

		if (it->tt == tokenType::PUNCTUATION)
			return false;

		return true;
	};
	const auto token_peek_postfix = [&]()
	{
		if (it == tokens.end || it->tt != tokenType::PUNCTUATION)
			return false;

		if (!is_postfix_operator(static_cast<punctuation_e>(LOWORD(it->extrainfo)))) {
			return false;
		}

		return true;
	};

	while (token_peek_unary()) {
		std::cout << "insert prefix: " << it->string << '\n';
		token.insert_prefix(*it);
		it++;
	}
	if (!token_peek_name()) {
		
		if (it->tt == tokenType::PUNCTUATION && LOWORD(it->extrainfo) == P_SEMICOLON && it == tokens.end)
			return true;

		return false;
	}
	else {
		token.set_token(*it);
		it++;
	}

	while (token_peek_postfix()) {
		token.insert_postfix(*it);
		it++;
	}

	sortedTokens.push_back(token);

	return true; //expression has valid syntax
}
bool expression_t::ParseOperator()
{
	

	auto& it = tokens.it;
	expression_token token;

	if (it == tokens.end)
		return false;

	if (it->tt != tokenType::PUNCTUATION) {
		return false;
	}

	if (!satisfies_operator(static_cast<punctuation_e>(LOWORD(it->extrainfo)))) {
		return false;
	}
	token.set_token(*it);
	token.op = true;

	++it;

	//if (it == tokens.end) //last token should NOT be an operator
	//	return false;

	sortedTokens.push_back(token);

	return true; //operator has valid syntax
}


void expression_t::EvaluateExpressionTokens()
{
	std::list<expression_token>::iterator itr1, itr2 = sortedTokens.begin();
	const auto& op_end = --sortedTokens.end();
	OperatorPriority op{}, next_op{};

	while (sortedTokens.size() > 2) {
		itr1 = ++sortedTokens.begin();
		itr2 = itr1;
		std::advance(itr2, 2);

		if (!itr1->op)
			throw scriptError_t(&itr1->get_token(), "expected an expression");

		if (itr2 != sortedTokens.end()) {
			
			do {
				op = static_cast<OperatorPriority>(HIWORD(itr1->get_token().extrainfo));
				next_op = static_cast<OperatorPriority>(HIWORD(itr2->get_token().extrainfo));

				if (next_op <= op || itr2 == op_end)
					break;

				std::advance(itr1, 2);
				std::advance(itr2, 2);

			} while (next_op > op);

		}
		itr2 = itr1;


		auto& Operator	= itr1;
		auto& leftVal	= --itr1;
		auto& rightVal	= ++itr2;

		rightVal->rval->set_value<int>(leftVal->rval->get_value<int>() + rightVal->rval->get_value<int>());

		

		sortedTokens.erase(itr1, itr2);
	}

	std::cout << "result: " << itr2->rval->get_value<int>() << '\n';
}