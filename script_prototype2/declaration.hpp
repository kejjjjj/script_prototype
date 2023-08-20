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


class declaration_t {
public:

	declaration_t(scr_scope_t* scope, const token_statement_t& expression);
	bool is_ready() const noexcept { return tokens.it != tokens.end; }
	friend class declaration_t_compiler;
	void declare_and_initialize();
private:

	void get_declaration_type();
	void get_identifier_name();
	void get_type_modifiers();
	void apply_modifiers(Variable& _target);

	bool parse_subscript();

	bool has_initializer();
	void initialize();

	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	token_statement_t tokens;
	Variable* target = 0;
	std::list<declaration_modifiers_e> typeModifiers;
	scr_scope_t* block = 0;
	std::unique_ptr<declaration_t_compiler> compiler_declr;
};


class declaration_t_compiler
{
public:
	friend declaration_t;
	declaration_t_compiler() = default;
	declaration_t_compiler(const declaration_t& d) {

		dtype = d.dtype;
		typeModifiers = d.typeModifiers;
		
		for (auto it = d.tokens.it; it != d.tokens.end; it++) { tokens.push_back(*it); }
		tokens.push_back(*d.tokens.end);


	}

	void add_initializer(compiler_information&& c) {
		initializer.push_back(std::move(c));
	}

private:
	std::list<declaration_modifiers_e> typeModifiers;
	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	std::list<token_t> tokens;
	bool initializer_list = false;
	std::list<compiler_information> initializer;

};

#endif