#include "if_statement.hpp"
#include "expression.hpp"
#include "parsing_utils.hpp"

void eval_if_statement(script_t& script, scr_scope_t*& scope, code_segment_t code)
{
	function_scope* fnc_scope = scope->is_function_scope();

	if (fnc_scope == nullptr)
		throw scriptError_t(&*code.it, "an if statement is only allowed in a function scope");

	scope = create_scope_without_range(scope);

	++code.it; //skip the if keyword

	auto substr_opt = find_matching_parenthesis(code_segment_t{ .it = code.it, .begin = code.it, .end = (++code.end--) });
	
	if (!substr_opt) {
		throw scriptError_t(&*code.it, "expected an expression");
	}
	code_segment_t& substr = substr_opt.value();
	code.end = substr.end; 
	code.end++; //end is now at )

	substr.print();

	auto token = expression_t(scope, substr).EvaluateEntireExpression();

	if (token.is_integral() == false) {
		throw scriptError_t(&token.get_token(), "expression must be convertible to a boolean type");
	}

	if (token.implicit_cast<int>()) {
		LOG("yippee the if statement will execute because the expression evaluated to: " << token.get_int() << '\n');
	}
	else
		LOG("oh no the if statement will not execute because the expression evaluated to: " << token.get_int() << '\n');

	code.it = code.end;
	code.it++; // go to the { token

	create_if_statement(script, scope, code);
}

void create_if_statement(script_t& script, scr_scope_t* scope, code_segment_t& _code)
{

	if (const auto is_opening_curly_bracket = [](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
		}(*_code.it) == false) {
		throw scriptError_t(&*_code.it, std::string("expected \"{\", but found \"") + _code.it->string + "\"");
	}

	script.S_SetIterator(_code.it);

	set_range_for_scope(script, scope);



}
