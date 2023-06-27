
#ifndef _ounar
#define _ounar


#include "pch.hpp"
#include "punctuation.hpp"
#include "expression_token.hpp"

class unaryFunctions
{
public:
	static auto& getInstance() {
		static unaryFunctions instance;
		return instance;
	}

	void createFunctions();

	auto find_function(const punctuation_e p) const {
		const auto found = unary_functions.find(p);
		return found != unary_functions.end() ? std::make_optional(found->second) : std::nullopt;
	}

private:
	std::unordered_map<punctuation_e, std::function<void(expression_token&)>> unary_functions;

	static void increment(expression_token& token);
	static void decrement(expression_token& token);
	static void negate(expression_token& token);

private:

};

#endif