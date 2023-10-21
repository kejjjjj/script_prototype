#pragma once

#ifndef _declaratio
#define _declaratio

#include "pch.hpp"
#include "script.hpp"
#include "datatype.hpp"
#include "variable.hpp"
#include "scope.hpp"
#include "expression.hpp"


enum class declaration_modifiers_e : char
{
	ARRAY
};


class variable_declaration_t {
public:

	variable_declaration_t(scr_scope_t* scope, const code_segment_t& expression);
	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	void declare_and_initialize(bool initializer_allowed);
private:

	void get_variable_declaration_type();
	void get_identifier_name();
	void get_type_modifiers();
	void apply_modifiers(Variable* _target);

	bool parse_subscript();

	bool has_initializer(bool initializer_allowed);
	void initialize();

	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	code_segment_t tokens;
	Variable* target = 0;
	std::list<declaration_modifiers_e> typeModifiers;
	scr_scope_t* block = 0;
};

#endif