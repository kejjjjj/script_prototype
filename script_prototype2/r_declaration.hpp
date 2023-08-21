#pragma once

#include "scope.hpp"
#include "variable.hpp"
#include "declaration.hpp"

class r_declaration_t
{
public:
	friend declaration_t_compiler;
	r_declaration_t(scr_scope_t* scope, const compiler_information& declaration);

	void declare_and_initialize();

private:

	void apply_modifiers(Variable& _target);
	void initialize();

	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	std::list<token_t> tokens;
	Variable* target = 0;
	std::list<declaration_modifiers_e> typeModifiers;
	scr_scope_t* block = 0;
	bool initializer_list = false;
	bool has_initializer = false;
	std::list<compiler_information> initializers;

};