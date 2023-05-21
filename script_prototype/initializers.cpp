#include "pch.h"


void init::SetVariableInitializer(Variable& target, const std::string& expression)
{
	//NOTE: the expression argument string doesn't contain the "target = " part of the expression, only the expression after that

	auto begin = expression.begin();


	if (IsInitializerList(begin, expression.end()))
		return EvaluateInitializerList(&target, std::string(begin, expression.end()));

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

	if (target.is_reference()) {
		if (!result.is_lvalue()) {
			stack_variables.erase(target.name);
			throw std::exception("reference initializer must be an lvalue");
		}

		if (result.get_type() != target.get_type()) {
			stack_variables.erase(target.name);
			throw std::exception(std::format("a reference of type \"{}\" cannot be initialized with type \"{}\"", left_type, right_type).c_str());

		}
		memcpy_s(target.reference.get(), sizeof(Variable), result.lval->ref, sizeof(Variable));
		std::cout << std::format("\"{}\" reference updated! (now points to \"{}\")\n", target.name, target.reference->name);

	}
	
	target.set_value(&result);

	expr::rules.reset();


}

bool init::IsInitializerList(std::string::const_iterator& it, std::string::const_iterator end)
{
	while (std::isspace(*it))
		++it;

	return *it == '{';

}
void init::EvaluateInitializerList(Variable* var, const std::string& expression)
{
	auto substr = FindMatchingCurlyBracket(expression); //returns the most nested one


	std::list<std::string> tokens;
	const bool substr_found = substr.result_string != "empty" && !substr.result_string.empty();

	TokenizeInitializerLists(substr.result_string, tokens);
	
	if (substr_found) {
			
		if (!var->is_array())
			throw std::exception("an initializer list used on a non-array type");

		if (tokens.size() >= var->numElements) {
			throw std::exception("too many initializer values");
		}

		auto begin = tokens.begin();
		for (int i = 0; i < var->numElements; i++) {

			if (begin == tokens.end())
				break;

			EvaluateInitializerList(&var->arr[i], *begin++);
		}

	}

}
void init::TokenizeInitializerLists(std::string expr, std::list<std::string>& tokens)
{
	auto substr = FindMatchingCurlyBracket(expr);

	bool substr_found = substr.result_string != "empty" && !substr.result_string.empty();


	while (substr_found) {
		tokens.push_back('{' + substr.result_string + '}');

		expr.erase((size_t)substr.opening, (size_t)substr.strlength+1);

		if (expr.empty())
			break;

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