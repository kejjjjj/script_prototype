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

	return {};
}


void expression_t::TokenizeExpression()
{
	if (tokens.it == tokens.end)
		return;

	expression_token token;
	bool unary_allowed = tokens.it->tt == tokenType::PUNCTUATION ? false : true;
	bool postfix_allowed = false;
	bool exit = false;

	const auto token_peek_unary = [&](const token_expression_t& token)
	{
		if (!unary_allowed || token.it->tt < tokenType::PUNCTUATION)
			return false;

		if (!is_unary_operator(static_cast<punctuation_e>(token.it->extrainfo)))
			throw scriptError_t(&*token.it, std::format("unrecognized unary operator \"{}\"", token.it->string));

		return true;
	};
	
	const auto token_peek_name = [&](const token_expression_t& token)
	{
		if (unary_allowed && token.it->tt < tokenType::PUNCTUATION) {
			postfix_allowed = true;
			return true;
		}
		
		//if unary is not allowed, then this is a normal operator
		else if (!unary_allowed && token.it->tt == tokenType::PUNCTUATION) {
			postfix_allowed = false;
			return true;
		}

		throw scriptError_t(&*token.it, std::format("expected a \";\" instead of \"{}\"", token.it->string));

		return false;

	};

	const auto token_peek_postfix = [&](const token_expression_t& token)
	{
		if (!postfix_allowed)
			return false;

		//code here to check if it satisfies operator

		if (!is_postfix_operator(static_cast<punctuation_e>(token.it->extrainfo)))
			throw scriptError_t(&*token.it, std::format("unrecognized unary operator \"{}\"", token.it->string));

		return true;
	};


	while (tokens.it != tokens.end) {

		
		while (token_peek_unary(tokens)) {
			token.insert_prefix(*tokens.it);
			tokens.it++;
		}
		if (token_peek_name(tokens)) {
			token.set_token(*tokens.it);
			tokens.it++;
		}
		while (token_peek_postfix(tokens)) {
			token.insert_postfix(*tokens.it);
			tokens.it++;
		}




	}

	//while (tokens.it != tokens.end) {

	//	if (tokens.it->tt < tokenType::PUNCTUATION) {

	//		if (postfix_allowed) {
	//			exit = true;
	//		}
	//		else {
	//			token.set_token(*tokens.it);

	//			unary_allowed = false;
	//			postfix_allowed = true;
	//		}
	//	}

	//	else if (tokens.it->tt == tokenType::PUNCTUATION) {

	//		if (unary_allowed) {
	//			if (!is_unary_operator(static_cast<punctuation_e>(tokens.it->extrainfo)))
	//				throw scriptError_t(&*tokens.it, std::format("unrecognized unary operator \"{}\"", tokens.it->string));

	//			token.insert_prefix(*tokens.it);
	//		}
	//		else if (!unary_allowed && !postfix_allowed) {
	//			token.set_token(*tokens.it);
	//			exit = true;
	//		}

	//		else if (!unary_allowed && postfix_allowed) {

	//			//true if there is an operator after text
	//			if (satisfies_operator(static_cast<punctuation_e>(tokens.it->extrainfo))) {
	//				exit = true;
	//			}
	//			else {
	//				if (!is_postfix_operator(static_cast<punctuation_e>(tokens.it->extrainfo)))
	//					throw scriptError_t(&*tokens.it, std::format("unrecognized postfix operator \"{}\"", tokens.it->string));
	//				token.insert_postfix(*tokens.it);
	//			}
	//		}

	//	}


	//	if (!CanExpressionContinue(token.get_token(), ++tokens.it))
	//		throw scriptError_t(&*tokens.it, std::format("cannot continue expression due to unexpected token: \"{}\"", tokens.it->string));


	//	if (exit)
	//		break;

	//}

	sortedTokens.push_back(token);

	TokenizeExpression();

}
bool expression_t::CanExpressionContinue(const token_t& token, const std::list<token_t>::iterator& nextToken) const noexcept
{
	//last token cannot be a punctuation symbol
	if (nextToken == tokens.end) {
		return token.tt < tokenType::PUNCTUATION;
	}

	//token.tt should NEVER be a unary or postfix here
	else if (token.tt == tokenType::PUNCTUATION) {

		if (nextToken->tt < tokenType::PUNCTUATION)
			return true;
		
		else if (nextToken->tt == tokenType::PUNCTUATION) {

			if (is_unary_operator(static_cast<punctuation_e>(nextToken->extrainfo))) {
				return true;
			}
			
		}

		return false;
	}

	//a token after text cannot be more text
	else if (token.tt < tokenType::PUNCTUATION) {

		if (nextToken->tt == tokenType::PUNCTUATION) {
			return true;
		}

		return false;
	}

	return true;

}
bool expression_t::ExpectToken(const token_t& token, const expected_token& expectedToken) const noexcept
{
	if (expectedToken.tt == tokenType::INVALID_LITERAL)
		return true;

	if (token.tt != expectedToken.tt)
		return false;
	



	return true;

}