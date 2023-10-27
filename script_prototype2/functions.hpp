#pragma once

#include "script.hpp"
#include "scope.hpp"
//#include "declaration.hpp"

struct function_scope
{
	std::string identifier;
	size_t numArgs = 0;
	std::list<datatype_declaration> param_types;
	datatype_declaration return_datatype;
	bool function_will_return = false;
};

void emit_function_declaration(script_t& script, scr_scope_t*& _scope, code_segment_t _code);