#include "pch.h"


void init::SetVariableInitializer(Variable& target, const std::string& expression)
{
	//NOTE: the expression argument string doesn't contain the "target = " part of the expression, only the expression after that

	auto begin = expression.begin();


	if (const auto str = IsInitializerList(begin, expression.end()))
		return EvaluateInitializerList(&target, std::string(str.value()));

	auto result = expr::EvaluateEntireExpression(expression);
		 
	if (result.tokentype == VarType::VT_INVALID) { //this means no initializer
		throw std::exception("expected an initializer");
		return;
	}

	const auto left_type = target.s_getvariabletype();
	const auto right_type = result.is_lvalue() ? result.lval->ref->s_getvariabletype() : VarTypes[int(result.rval->get_type())];

	if (!expr::ExpressionCompatibleOperands(target, result)) {
		stack_variables.erase(target.name);
		throw std::exception(std::format("a variable of type \"{}\" cannot be initialized with type \"{}\"", left_type, right_type).c_str());
	}

	target.initialize_expression(&result);

	expr::rules.reset();
}

std::optional<std::string> init::IsInitializerList(std::string::const_iterator& it, std::string::const_iterator end)
{
	while (std::isspace(*it))
		++it;

	if (*it != '{') {
		return std::nullopt;
	}
	auto str = std::string(it, end);
	const auto og_end = str.end();
	auto beg = str.begin();
	auto start = str.begin();

	token_t token;

	std::string::iterator iend = str.end();


	while (beg != og_end) { //find last '}'
		token = cec::Compiler_ReadToken(beg, '\0', str.end());

		if (token.t_type == token_t::OTHER) {
			if (token.value == "}") {
				iend = beg;
				continue;
			}
		}
	}

	const auto result = (std::string(++start, --iend));

	return result;

}
void init::EvaluateInitializerList(Variable* var, const std::string& expression)
{
	//var = var->this_or_ref();
	if (!var->is_array()) {
		return EvaluateInitializerListForNonArray(var, expression);

	}
	std::list<std::string> tokens;
	TokenizeInitializerLists(expression, tokens);
	
	if (tokens.size() > var->numElements) {
		throw std::exception("too many initializer values");
	}
	auto begin = tokens.begin();
	for (size_t i = 0; i < var->numElements; i++) {

		if (begin == tokens.end())
			break;

		EvaluateInitializerList(&var->arr[i], *begin++);
	}

	if (tokens.empty()) {
		//most nested element
		

		auto new_expr = std::string(expression);
		auto expr_begin = new_expr.begin();

		TokenizeListArguments(expr_begin, new_expr.end(), tokens);

		if (tokens.size() > var->numElements) {
			throw std::exception("too many initializer values");
		}
		begin = tokens.begin();
		for (int i = 0; i < var->numElements; i++) {
			if (begin == tokens.end())
				break;

			auto expr = expr::EvaluateEntireExpression(*begin++);

			var->arr[i].initialize_expression(&expr);
		}

	}
}
void init::EvaluateInitializerListForNonArray(Variable* var, const std::string& expression)
{
	std::list<std::string> tokens;

	const auto substr = FindMatchingCurlyBracket(const_cast<std::string&>(expression));
	const bool substr_found = substr.result_string != "empty" && !substr.result_string.empty();



	if (substr_found) {
		throw std::exception(std::format("only one level of braces is allowed on an initializer for an object of type \"{}\"", var->s_getvariabletype()).c_str());
	}

	auto new_expr = std::string(expression);
	auto expr_begin = new_expr.begin();

	TokenizeListArguments(expr_begin, new_expr.end(), tokens);

	if (tokens.size() != 1) {
		throw std::exception(std::format("only one initializer value can be used for type \"{}\"", var->s_getvariabletype()).c_str());
	}

	auto expr = expr::EvaluateEntireExpression(tokens.front());

	var->initialize_expression(&expr);

}
void init::TokenizeInitializerLists(std::string expr, std::list<std::string>& tokens)
{
	auto substr = FindMatchingCurlyBracket(expr);



	bool substr_found = substr.result_string != "empty" && !substr.result_string.empty();


	while (substr_found) {
				
		tokens.push_back(substr.result_string);

		expr.erase((size_t)substr.opening, (size_t)substr.strlength+2ull);

		if (expr.empty() || StringIsBlank(expr))
			break;

		auto begin = expr.begin();

		FindComma(begin, expr.end());

		substr = FindMatchingCurlyBracket(expr);
		substr_found = substr.result_string != "empty" && !substr.result_string.empty();
	}

}
Substr_s init::FindMatchingCurlyBracket(std::string& expr) 
{

	auto it = expr.begin();
	auto end = expr.end();

	size_t idx = 0;
	size_t opening{ 0 }, closing{ 0 }, count_opening{ 0 }, count_closing{ 0 };

	while (it != end) {
		const token_t token = cec::Compiler_ReadToken(it, '\0', end);

		if (token.t_type == token_t::tokentype::OTHER) {
			if (token.value == "{") {
				if (!count_opening)
					opening = idx;

				count_opening++;
			}
			else if(token.value == "}") {
				closing = idx;
				count_closing++;
			}

			if (count_opening > 0 && count_opening == count_closing) {
				break;
			}

		}

		if (it == end && count_opening == 1)
			throw std::exception("expected a \"}\"");


		idx += token.value.size();

	}

	if (opening && opening >= closing) {
		throw std::exception("expected a \"}\"");
	}

	const size_t len = closing - opening - count_opening;

	std::string result_string = std::string(expr).substr(opening + 1ull, len);

	if (!count_opening && !count_closing)
		result_string = "";

	return { count_opening, count_closing, opening, len, result_string };

}

void init::FindComma(std::string::const_iterator& it, std::string::const_iterator end) {

	while (std::isspace(*it)) {

		if (it == end) {
			throw std::exception("1. expected a \",\"");
		}

		++it;
	}

	if(*it != ',')
		throw std::exception(std::format("expected a \",\" instead of \"{}\"", std::string(it, end)).c_str());
	

}
void init::TokenizeListArguments(std::string::iterator& begin, std::string::iterator end, std::list<std::string>& tokens)
{

	token_t token;

	std::string this_str;

	while (begin != end) {
		token = cec::Compiler_ReadToken(begin, '\0', end);

		if (token.t_type == token_t::OTHER) {
			if (token.value == ",") {
				tokens.push_back(this_str), this_str = "";
				continue;
			}
		}

		this_str += token.value;
	}

	if (!this_str.empty())
		tokens.push_back(this_str);
}