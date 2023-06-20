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

	void print() const noexcept
	{
		std::cout << "\n---------TOKEN---------\n";
		for (const auto& i : prefix)
			std::cout << i->string << ' ';
		std::cout << '\n';
		std::cout << token.string << '\n';
		for (const auto& i : postfix)
			std::cout << i->string << ' ';
		
		std::cout << '\n';



	}
private:
	std::list<token_t*> prefix;
	std::list<token_t*> postfix;
	token_t token;
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
	bool ParseExpression();
	bool ParseOperator();
	expression_token EvaluateExpression();

	token_expression_t tokens;
	std::list<expression_token> sortedTokens;
};

#endif