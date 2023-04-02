#include "pch.h"

//an expression like: 101 * (100 + ((2 + 3) & 3 ^ 4)) * (2 | 4 * -(-2 + 3 / (30 - 2 | 3)) ^ 2) can be used here
//assumes that there are no syntax errors
std::string expr::EvaluateEntireExpression(const std::string& str)
{

	const Parenthesis_s par = GetStringWithinParentheses(str);

	//parentheses exists
	if (!par.result_string.empty()) {

	}

	auto result = expr::EvaluateExpression(str);
	return result;
}
std::string expr::EvaluateExpression(const std::string& str)
{
	//if (ValidNumber(str)) {
	//	return std::string(str);
	//}
	std::cout << "EvaluateExpression(" << str << ")\n";
	std::string s_str = std::string(str);
	auto it = s_str.begin();
	auto end = s_str.end();
	auto tokens = TokenizeExpression(it, end);

	std::cout << "made this token: " << tokens << '\n';

	return tokens;
}
std::string expr::TokenizeExpression(std::string::iterator& it, std::string::iterator& end)
{
	std::string stringtoken;
	bool kill_loop = false;
	int a = 1;
	rules.next_unary = true;
	rules.next_postfix = false;
	
	while (!kill_loop && it != end) {

		const token_t token = cec::Compiler_ReadToken(it, '\0', end);

		switch (token.t_type) {
		case token_t::tokentype::STRING:
		case token_t::tokentype::DIGIT:
			rules.next_unary = false;
			rules.next_postfix = true;
			stringtoken += token.value;
			break;
		case token_t::tokentype::OPERATOR:


			if (IsUnaryOperator(token.value) && (rules.next_unary)) {
				stringtoken += token.value;
			}
			else if (IsPostfixOperator(token.value) && rules.next_postfix) {
				stringtoken += token.value;
			}
			else {
				it -= token.value.length();
				kill_loop = true;
			}
			break;
		case token_t::tokentype::WHITESPACE:
			stringtoken += ' ';
			break;
		}

	}


	return stringtoken;
}