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
	rules.next_postfix = false;
	rules.next_unary = true;
	rules.next_operator = false;

	//if (ValidNumber(str)) {
	//	return std::string(str);
	//}
	std::cout << "EvaluateExpression(" << str << ")\n";
	std::string s_str = std::string(str);
	auto it = s_str.begin(); auto end = s_str.end();
	std::list<expression_token> tokens;
	TokenizeExpression(it, end, tokens);
	auto tbegin = tokens.begin(); auto tend = tokens.end();
	EvaluatePostfix(tbegin, tend); tbegin = tokens.begin();
	EvaluatePrefix(tbegin, tend);

	std::cout << "made this token: " << '\n';

	for (auto& i : tokens) {

		std::cout << '(';
		if (!i.op) {
			for (auto& prefix : i.prefix) {
				std::cout << prefix;
			}
			std::cout << i.content;
			for (auto& postfix : i.postfix) {
				std::cout << postfix;
			}
		}
		else
			std::cout << i.content;

		std::cout << ")\n";
	}
	return "";
}
void expr::TokenizeExpression(std::string::iterator& it, std::string::iterator& end, std::list<expression_token>& tokens)
{
	expression_token expr_token;

	bool kill_loop = false;
	rules.next_unary = true;
	rules.next_postfix = false;
	
	while (!kill_loop && it != end) {

		const token_t token = cec::Compiler_ReadToken(it, '\0', end);
		std::cout << "token: " << token.value << '\n';
		switch (token.t_type) {
		case token_t::tokentype::STRING:
		case token_t::tokentype::DIGIT:

			if (rules.next_operator) {
				throw std::exception("expected an expression");
			}
			if (!rules.next_unary && rules.next_postfix) {
				it -= token.value.length();
				kill_loop = true;
				break;
			}
			rules.next_unary = false;
			rules.next_postfix = true;
			expr_token.content = token.value;
			break;
		case token_t::tokentype::OPERATOR:

			if (rules.next_operator) {
				expr_token.op = true;
				if(!SatisfiesOperator(token.value))
					throw std::exception("expected an expression");

				expr_token.content = token.value;
				kill_loop = true;
				break;
			}
			if (IsUnaryOperator(token.value) && (rules.next_unary)) {
				expr_token.prefix.push_back(token.value);

			}
			else if (IsPostfixOperator(token.value) && rules.next_postfix) {
				expr_token.postfix.push_back(token.value);
			}
			else {
				it -= token.value.length();
				kill_loop = true;
			}
			break;
		case token_t::tokentype::WHITESPACE:
			//expr_token.content += ' ';
			//kill_loop = true;
			break;
		}

	}
	rules.next_operator = !rules.next_operator;
	tokens.push_back(expr_token);
	if(it != end)
		TokenizeExpression(it, end, tokens);

	

	return;
}
void expr::EvaluatePostfix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end)
{
	if (it == end)
		return;

	auto& token = *it;

	if (token.postfix.empty())
		return EvaluatePostfix(++it, end);

	if (!ValidNumber(token.content))
		throw std::exception("EvaluatePostfix(): variables are not supported yet");

	if (IsConst(token.content))
		throw std::exception("expression must be non-const");

	//this will NOT execute until variable support

	std::string op;
	op.push_back(token.postfix.front().front());
	token.content = Eval(token.content, "1", op);
	token.postfix.pop_front();
	
	return EvaluatePostfix(it, end);
}
void expr::EvaluatePrefix(std::list<expression_token>::iterator& it, std::list<expression_token>::iterator& end)
{
	if (it == end)
		return;

	auto& token = *it;

	if (token.prefix.empty()) {
		return EvaluatePrefix(++it, end);
	}

	if (!ValidNumber(token.content))
		throw std::exception("EvaluatePrefix(): variables are not supported yet");

	if (IsConst(token.content) && token.prefix.back().size() > 1)
		throw std::exception("expression must be non-const");

	switch (token.prefix.back().front())
	{
		case '-':
			token.content = Eval(token.content, "-1", "*");
			break;
		case '+':
			token.content = Eval(token.content, "-1", "*");
			break;
		case '~':
			token.content = Eval(token.content, "0", "~");
			break;
		case '!':
			token.content = Eval(token.content, "0", "==");
			break;
		default:
			break;
	}

	token.prefix.pop_back();

	return EvaluatePrefix(it, end);

}