#include "return_statement.hpp"
#include "scope.hpp"
#include "expression.hpp"


void eval_return_statement(code_segment_t code, scr_scope_t* block)
{
	function_scope* function_scope = block->is_function_scope();


	if (function_scope == nullptr) {
		throw scriptError_t(&*code.it, "the return keyword is only allowed in function scopes");
	}
	
	//lowest scope
	if (block->get_lower()->is_global_scope()) {
		function_scope->function_will_return = true;
	}

	code.print();

	code.it++; //skip the return keyword

	expression_token result = eval_expression(block, code);

	//convert the return type to an lvalue for a moment

	result.implicit_force_cast(function_scope->return_datatype);

	//throw scriptError_t(&*code.it, "gg");


}