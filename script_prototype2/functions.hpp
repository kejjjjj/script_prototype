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

class function_c
{
public:
	function_c(scr_scope_t*& _scope, const code_segment_t& _code) : code(_code), scope(_scope) {
		function = std::move(std::unique_ptr<function_scope>(new function_scope));
	}

	function_c(const function_c&) = delete;
	function_c& operator=(const function_c&) = delete;

	void parse_declaration(script_t& script);


private:

	size_t parse_parameters(code_segment_t& declarations, size_t numArgs);
	void parse_returntype(code_segment_t& _code);
	void create_function(script_t& script, code_segment_t& _code);

	scr_scope_t*& scope;
	code_segment_t code;

	std::unique_ptr<function_scope> function;
};