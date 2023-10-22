#include "return_statement.hpp"
#include "scope.hpp"
#include "expression.hpp"

void eval_return_statement(code_segment_t code, scr_scope_t* block)
{
	if (block->is_global_scope()) {
		throw scriptError_t(&*code.it, "the return keyword is only allowed in function scopes");
	}

	code.print();

	code.it++; //skip the return keyword

	expression_t expression(block, code);
	expression_token result = expression.EvaluateEntireExpression();

	//todo -> test if result has a matching type with the function return type

	throw scriptError_t(&*code.it, "gg");


}