#include "if_statement.hpp"
#include "expression.hpp"
#include "parsing_utils.hpp"

enum class conditional_statement_t : char
{
	IF_STATEMENT,
	//ELSE_IF_STATEMENT,
	ELSE_STATEMENT
};

void create_conditional_statement(script_t& script, scr_scope_t* scope, code_segment_t& _code, const conditional_statement_t t);


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

	auto token = eval_expression(scope, substr);

	if (token.is_integral() == false) {
		throw scriptError_t(&token.get_token(), "expression must be convertible to a boolean type");
	}

	code.it = code.end;
	code.it++; // go to the { token

	create_conditional_statement(script, scope, code, conditional_statement_t::IF_STATEMENT);
}

void eval_else_statement(script_t& script, scr_scope_t*& block, code_segment_t code)
{
	LOG("ELSE STATEMENT\n");

	auto context = block->get_scope_context();

	if (!context.has_value() || context.value() != statementType_e::IF_STATEMENT) {
		throw scriptError_t(&*code.it, "the \"else\" keyword is only allowed after an \"if\" block");
	}

	
	++code.it; //skip the else keyword

	if (code.it->tt == tokenType::IF_STATEMENT) { //else if block
		return eval_if_statement(script, block, code);
	}

	block = create_scope_without_range(block);

	create_conditional_statement(script, block, code, conditional_statement_t::ELSE_STATEMENT);

}

void create_conditional_statement(script_t& script, scr_scope_t* scope, code_segment_t& _code, const conditional_statement_t t)
{

	if ([](const token_t& token) {
		return token.tt == tokenType::PUNCTUATION && LOWORD(token.extrainfo) == punctuation_e::P_CURLYBRACKET_OPEN;
		}(*_code.it) == false) {
		throw scriptError_t(&*_code.it, std::string("expected \"{\", but found \"") + _code.it->string + "\"");
	}

	script.S_SetIterator(_code.it);

	set_range_for_scope(script, scope);

	statementType_e st;

	switch (t) {
	case conditional_statement_t::IF_STATEMENT:
		st = statementType_e::IF_STATEMENT;
		break;
	//case conditional_statement_t::ELSE_IF_STATEMENT:
	//	st = statementType_e::ELSE_IF_STATEMENT;
	//	break;
	case conditional_statement_t::ELSE_STATEMENT:
		st = statementType_e::ELSE_STATEMENT;
		break;
	default:
		throw scriptError_t(&*script.S_GetIterator(), "conditional_statement_t: NULL");
	}

	scope->emit_to_lower_scope(st);

}
