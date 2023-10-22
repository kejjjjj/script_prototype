#pragma once

#include "script.hpp"
#include "scope.hpp"
#include "declaration.hpp"

class function_c
{
public:
	function_c(scr_scope_t*& _scope, const code_segment_t& _code) : code(_code), scope(_scope) {

	}

	void parse_declaration(script_t& script);


private:

	size_t parse_parameters(code_segment_t& declarations, size_t numArgs);
	void parse_returntype(code_segment_t& _code);
	void create_function(script_t& script, code_segment_t& _code);

	scr_scope_t*& scope;
	code_segment_t code;

	std::string identifier;
	size_t numArgs = 0;
	std::list<datatype_declaration> param_types;
	datatype_declaration return_datatype;
};