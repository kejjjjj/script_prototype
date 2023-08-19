#pragma once
#include "punctuation.hpp"
#include "r_expression_token.hpp"

class runtimeEvaluationFunctions
{
public:
	static auto& getInstance() {
		static runtimeEvaluationFunctions instance;
		return instance;
	}
	void createFunctions();

	auto find_function(const punctuation_e p) const {
		return eval_functions.find(p)->second;
		
	}

private:
	std::unordered_map<punctuation_e, std::function<r_expression_token(r_expression_token&, r_expression_token&)>> eval_functions;
	static r_expression_token arithmetic_addition(r_expression_token& left, r_expression_token& right);
	static r_expression_token assignment(r_expression_token& left, r_expression_token& right);


};