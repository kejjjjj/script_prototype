#include "expression.hpp"

//each expression sequence will be converted to just one remaining expression
expression_token expression_t::EvaluateEntireExpression()
{
	if (!is_ready())
		throw scriptError_t(&*tokens.it, "empty expression is not allowed");

	std::cout << "EvaluateEntireExpression(";
	for (auto it = tokens.it; it != tokens.end; ++it) {
		std::cout << it->string<<' ';
	}
	std::cout << ")\n";

	//do parentheses check here...
	{

	}

	EvaluateExpression();

	return result;
}
void expression_t::EvaluateExpression()
{


	TokenizeExpression();

	std::for_each(sortedTokens.begin(), sortedTokens.end(), [](expression_token& t) {
		t.eval_postfix();
		t.eval_prefix();
	});

	EvaluateExpressionTokens();
	

	return;
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

			throw scriptError_t(&*tokens.it, std::format("expected an operator or \";\" instead of \"{}\"\n", tokens.it->string));

		}

	}

}
bool expression_t::ParseExpression()
{
	auto& it = tokens.it;
	expression_token token;
	const auto token_peek_unary = [&]()
	{
		const auto cast_exists = ParseUnaryCast(token);
		if (cast_exists) {
			return true;
		}
		

		if (it == tokens.end || it->tt != tokenType::PUNCTUATION)
			return false;

		const auto punctuation = static_cast<punctuation_e>(LOWORD(it->extrainfo));

		if (bad_unary_operator(punctuation)) {
			throw scriptError_t(&*it, std::format("expected unary operator instead of \"{}\"", it->string));
		}

		if (!is_unary_operator(punctuation)){
			return false;
		}

		

		token.insert_prefix(*it);
		it++;

		return true;
	};
	const auto token_peek_name = [&]()
	{
		if (it == tokens.end)
			return false;

		if (it->tt == tokenType::PUNCTUATION )
			return false;

		token.set_token(*it);
		it++;

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
	}
	if (!token_peek_name()) {
		
		if (it->tt == tokenType::PUNCTUATION && LOWORD(it->extrainfo) == P_SEMICOLON && it == tokens.end) // semicolon (end of expression)
			return true;

		if(ExpressionParseParentheses(token) == false)
			return false;

	}

	while (token_peek_postfix()) {
		token.insert_postfix(*it);
		it++;
	}
	token.set_value_category();
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
	token.set_value_category();
	sortedTokens.push_back(token);

	return true; //operator has valid syntax
}

bool expression_t::ParseUnaryCast(expression_token& token)
{
	if ((tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN) == false)
		return false;
	
	tokens.it++;
		
	if (tokens.it->tt != tokenType::BUILT_IN_TYPE) {
		tokens.it--;
		return false;
	}

	token.insert_prefix(*tokens.it);

	tokens.it++;
	
	if ((tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_CLOSE) == false)
		throw scriptError_t(&*tokens.it, "expected to find a \")\"");

	tokens.it++;

	std::cout << "unary cast!\n";

	return true;
}
bool expression_t::ExpressionParseParentheses(expression_token& token)
{
	if ((tokens.it->tt == tokenType::PUNCTUATION && LOWORD(tokens.it->extrainfo) == punctuation_e::P_PAR_OPEN) == false)
		return false;

	//creates a copy that will find the matching )
	auto parentheses_statement = token_statement_t{ .it = tokens.it, .begin = tokens.it, .end = tokens.end };
	ExpressionFindMatchingParenthesis(parentheses_statement);

	//skip the (
	++tokens.it;

	//parentheses_statement.it contains the position of the matching ), so it will be the end
	const token_statement_t statement = token_statement_t{ .it = tokens.it, .begin = tokens.it, .end = parentheses_statement.it };

	std::list<token_t*> backup_prefix = token.prefix;
	std::list<token_t*> backup_postfix = token.postfix;

	token = expression_t(statement).EvaluateEntireExpression();

	token.prefix = backup_prefix;
	token.postfix = backup_postfix;


	tokens.it = parentheses_statement.it;
	++tokens.it;
	//std::cout << "continuing iteration from " << tokens.it->string << '\n';

	return true;
}
void expression_t::ExpressionFindMatchingParenthesis(token_statement_t& token)
{
	const auto is_opening = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_PAR_OPEN;
	};
	const auto is_closing = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_PAR_CLOSE;
	};

	if (is_opening(*token.it) == false)
		throw scriptError_t(&*token.it, "this should never happen");

	token.it++;
	size_t numOpen = 1;
	size_t numClosing = 0;

	while (token.it != token.end) {
		if (is_opening(*token.it))
			numOpen++;

		else if (is_closing(*token.it)) {
			numClosing++;

			if (numOpen == numClosing) {
				return;
			}
		}

		token.it++;
	}
	throw scriptError_t(&*token.it, "expected to find a \")\"");

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


		auto& Operator	= itr1->get_token();
		auto& leftVal	= --itr1;
		auto& rightVal	= ++itr2;

		using FunctionType = std::function<expression_token(expression_token&, expression_token&)>;
		using OptionalFunctionType = std::optional<FunctionType>;

		const auto& evalFunctions = evaluationFunctions::getInstance();
		const auto punctuationType = static_cast<punctuation_e>(LOWORD(Operator.extrainfo));
		const OptionalFunctionType function = evalFunctions.find_function(punctuationType);

		if (!function.has_value())
			throw scriptError_t(&Operator, std::format("no evaluation function for the \"{}\" operator", Operator.string));

		*itr2 = function.value()(*leftVal, *rightVal);
	

		sortedTokens.erase(itr1, itr2);
	}

	result = *itr2;
}