#pragma once

#ifndef __expres
#define __expres

#include "script.hpp"

struct expression_token
{
	expression_token() = default;
	expression_token(token_t& _token) : token(_token) {

	}
	void insert_prefix(token_t& token) {
		prefix.push_back(&token);
	}
	void insert_postfix(token_t& token) {
		postfix.push_back(&token);
	}
	void set_token(token_t& _token) {
		token = _token;
	}
	token_t& get_token() noexcept { return token; }
private:
	std::list<token_t*> prefix;
	std::list<token_t*> postfix;
	token_t token;
};

struct expected_token
{
	std::string string;
	tokenType tt = tokenType::INVALID_LITERAL;
	size_t extrainfo = 0;
};

class expression_t {
public:
	expression_t(const token_expression_t& expression) : tokens(expression)
	{
	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	expression_token EvaluateEntireExpression();

private:

	void TokenizeExpression();
	expression_token EvaluateExpression();

	bool CanExpressionContinue(const token_t& token, const std::list<token_t>::iterator& nextToken) const noexcept;
	bool ExpectToken(const token_t& token, const expected_token& expectedToken) const noexcept;

	token_expression_t tokens;
	std::list<expression_token> sortedTokens;
};

#endif