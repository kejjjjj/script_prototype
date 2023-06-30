
#ifndef o_postfx
#define o_postfx

#include "pch.hpp"
#include "expression_token.hpp"

class postfixFunctions
{
public:
	static auto& getInstance() {
		static postfixFunctions instance;
		return instance;
	}

	void createFunctions();

	auto find_function(const punctuation_e p) const {
		const auto found = postfix_functions.find(p);
		return found != postfix_functions.end() ? std::make_optional(found->second) : std::nullopt;
	}

private:
	std::unordered_map<punctuation_e, std::function<void(expression_token&, std::optional<token_statement_t*> statement)>> postfix_functions;

	static void subscript(expression_token&, std::optional<token_statement_t*> statement);

};

bool ExpressionFindMatchingBracket(token_statement_t& token);

#endif