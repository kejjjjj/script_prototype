#include "pch.h"


void init::SetVariableInitializer(Variable& target, const std::string& expression)
{
	//NOTE: the expression argument string doesn't contain the "target = " part of the expression, only the expression after that

	auto begin = expression.begin();


	if (auto str = IsInitializerList(begin, expression.end()))
		return EvaluateInitializerList(&target, std::string(str.value()));

	const auto result = expr::EvaluateEntireExpression(expression);

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
	auto og_end = str.end();
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
	if (!var->is_array())
		throw std::exception("an initializer list used on a non-array type");

	std::list<std::string> tokens;
	TokenizeInitializerLists(expression, tokens);
	
	if (tokens.size() > var->numElements) {
		throw std::exception("too many initializer values");
	}

	auto begin = tokens.begin();
	for (int i = 0; i < var->numElements; i++) {

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

			const auto expr = expr::EvaluateEntireExpression(*begin++);

			var->arr[i].initialize_expression(&expr);
		}

	}

}
void init::TokenizeInitializerLists(std::string expr, std::list<std::string>& tokens)
{
	auto substr = FindMatchingCurlyBracket(expr);

	bool substr_found = substr.result_string != "empty" && !substr.result_string.empty();


	while (substr_found) {
		tokens.push_back(substr.result_string);

		expr.erase((size_t)substr.opening, (size_t)substr.strlength+1);

		if (expr.empty())
			break;

		auto begin = expr.begin();

		FindComma(begin, expr.end());

		substr = FindMatchingCurlyBracket(expr);
		substr_found = substr.result_string != "empty" && !substr.result_string.empty();
	}
	for (auto& i : tokens)
		std::cout << i << '\n';

}
Substr_s init::FindMatchingCurlyBracket(const std::string_view& expr) 
{
	int32_t idx = -1;
	int32_t opening{ 0 }, closing{ 0 }, count_opening{ 0 }, count_closing{ 0 };
	for (const auto& i : expr) {
		idx++;

		if (i == '{') {
			
			if(!count_opening)
				opening = idx;

			count_opening++;
			
		}
		else if (i == '}') {
			

			closing = idx;
			count_closing++;
			
		}

		if (count_opening > 0 && count_opening == count_closing) {
			break;
		}



	}

	if (opening && opening >= closing) {
		throw std::exception("expected a \"}\"");
	}

	const int len = closing - opening;
	//note: cuts out the s and e characters
	std::string result_string = std::string(expr).substr(opening + 1ull, len-1ull);
	//if (count_opening && count_closing && result_string.empty())
	//	result_string = ("empty");

	if (!count_opening && !count_closing)
		result_string = "";

	return { count_opening, count_closing, opening, len, result_string };
}

void init::FindComma(std::string::const_iterator& it, std::string::const_iterator end) {

	while (std::isspace(*it)) {

		if (it == end) {
			throw std::exception("expected a \",\"");
		}

		++it;
	}

	if(*it != ',')
		throw std::exception("expected a \",\"");
	

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