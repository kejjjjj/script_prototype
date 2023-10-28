#pragma once

#ifndef _declaratio
#define _declaratio

#include "pch.hpp"
#include "script.hpp"
#include "datatype.hpp"
#include "variable.hpp"
#include "scope.hpp"
#include "expression.hpp"



class variable_declaration_t {
public:

	variable_declaration_t(scr_scope_t* scope, const code_segment_t& expression);
	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	code_segment_t declare_and_initialize(bool initializer_allowed);
	datatype_declaration get_type() noexcept { return datatype; }
	datatype_declaration get_variable_declaration_type(code_segment_t& iterator);

private:

	void get_variable_declaration_type();
	void get_identifier_name();
	void get_type_modifiers();
	void apply_modifiers(Variable* _target, std::list<declaration_modifiers_e>::iterator& modifier);

	bool parse_subscript();

	bool has_initializer(bool initializer_allowed);
	void initialize();

	std::string identifier;
	code_segment_t tokens;
	Variable* target = 0;
	datatype_declaration datatype;
	scr_scope_t* block = 0;
};

#endif