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
	void set_compiler_data();

	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	token_statement_t tokens;
	Variable* target = 0;
	std::list<declaration_modifiers_e> typeModifiers;
	scr_scope_t* block = 0;
	std::unique_ptr<declaration_t_compiler> compiler_declr;
};


#include "r_declaration.hpp"

class declaration_t_compiler
{
public:
	friend declaration_t;
	friend class r_declaration_t;
	declaration_t_compiler() = default;
	declaration_t_compiler(const declaration_t& d) {

		dtype = d.dtype;
		//typeModifiers = d.typeModifiers;

		std::copy(d.typeModifiers.begin(), d.typeModifiers.end(), typeModifiers.begin());
		
		for (auto it = d.tokens.it; it != d.tokens.end; it++) { tokens.push_back(*it); }
		tokens.push_back(*d.tokens.end);

		has_initializer = d.compiler_declr->has_initializer;
		initializer_list = d.compiler_declr->initializer_list;
		identifier = d.identifier;

		std::for_each(d.compiler_declr->initializer.begin(), d.compiler_declr->initializer.end(), [this](compiler_information& i){

			initializer.push_back(std::move(i));
			});

	}

	void add_initializer(compiler_information&& c) {

		auto ptr = (expression_token_compiler*)c.data.get();

		std::cout << ptr->token.string << " is lvalue: " << ptr->b_lvalue << '\n';

		initializer.push_back(std::move(c));
	}

	char* get_copy() {
		const size_t size = totalSize();
		char* yea = new char[size];

		memcpy(yea, this, size);

		return yea;
	}
	size_t totalSize() const noexcept {


		const size_t modifiers = typeModifiers.size() * sizeof(std::underlying_type_t<declaration_modifiers_e>);
		const size_t initializers = initializer.size() * sizeof(compiler_information);
		const size_t token = tokens.size() * sizeof(token_t);
		const size_t string = identifier.size();

		return sizeof(declaration_t_compiler) + modifiers + initializers + token + string 
			+ sizeof(initializer_list) + sizeof(has_initializer) + sizeof(std::underlying_type_t<dataTypes_e>);
	}
private:
	std::vector<declaration_modifiers_e> typeModifiers;
	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	std::vector<token_t> tokens;
	bool initializer_list = false;
	bool has_initializer = false;
	std::vector<compiler_information> initializer;

};

#endif