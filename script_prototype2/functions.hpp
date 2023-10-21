#pragma once

#include "script.hpp"
#include "scope.hpp"

class function_c
{
public:
	function_c(scr_scope_t* _scope, const code_segment_t& _code) : code(_code), scope(_scope) {

	}

	void parse_declaration();

private:
	scr_scope_t* scope = 0;
	code_segment_t code;

	std::string identifier;
};