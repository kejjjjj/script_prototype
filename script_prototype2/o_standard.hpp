#ifndef ostandr_
#define ostandr_

#include "pch.hpp"
#include "script.hpp"
#include "expression_token.hpp"


class evaluationFunctions
{
public:
	static auto& getInstance() {
		static evaluationFunctions instance;
		return instance;
	}
	void createFunctions();

	auto find_function(const punctuation_e p) const {
		const auto found = eval_functions.find(p);
		return found != eval_functions.end() ? std::make_optional(found->second) : std::nullopt;
	}

private:
	std::unordered_map<punctuation_e, std::function<expression_token(expression_token&, expression_token&)>> eval_functions;
	static expression_token arithmetic_addition(expression_token& left, expression_token& right);
	static expression_token assignment(expression_token& left, expression_token& right);

	
};
#endif