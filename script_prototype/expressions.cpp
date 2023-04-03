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
	std::list<expression_token> tokens;
	rules.next_postfix = false;
	rules.next_unary = true;
	rules.next_operator = false;
	TokenizeExpression(it, end, tokens);

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
		std::cout << ") -> " << TokenToValue(i);


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
std::string expr::TokenToValue(const expression_token& token)
{
	std::string result = token.content;
	if (token.op)
		return token.content;

	if (!ValidNumber(token.content))
		throw std::exception("TokenToValue(): expected a digit");

	const auto EvalPostfix = [](const std::string& value, const std::string& postfix)->std::string
	{
		if (IsConst(value)) 
			throw std::exception("expression must be non-const");
			
		if (postfix == "++")
			return Eval(value, "1", "+");

		else if (postfix == "--")
			return Eval(value, "1", "-");
		
		throw std::exception(std::format("unexpected postfix \"{}\"", postfix).c_str());
	};

	for (auto& postfix : token.postfix) {
		result = EvalPostfix(result, postfix);
	}
	
	const auto EvalPrefix = [](const std::string& value, const std::string& prefix)->std::string
	{
		
		if (prefix.size() == 1) {
			std::string result = value;
			switch (prefix.front()) {
			case '-':

				if (result.front() == '-') {
					result.erase(0, 1); //if the - already exists, then remove it
				}else
					result.insert(result.begin(), '-'); //if the - doesn't exist, then add it
				return result;
			case '+':
				return Eval(result, "0", "+");
			case '!':
				return Eval(result, "0", "==");
			case '~':
				return Eval(result, "0", "~");

			default:
				throw std::exception("EvalPrefix(): default case");
			}
			
		}

		if (IsConst(value))
			throw std::exception("expression must be non-const");

		if (prefix == "++")
			return Eval(value, "1", "+");

		else if (prefix == "--")
			return Eval(value, "1", "-");

		throw std::exception(std::format("unexpected postfix \"{}\"", prefix).c_str());
	};

	auto begin = token.prefix.begin();
	auto end = token.prefix.end();
	for (auto& it = end; it != begin; --it)
		result = EvalPrefix(result, *it);

	return result;
}