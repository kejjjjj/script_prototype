#pragma once

#include "scope.hpp"
#include "variable.hpp"

class r_declaration_t
{
	r_declaration_t(scr_scope_t* scope, compiler_information& declaration) : block(scope) {



	}

	scr_scope_t* block = 0;
};