
#ifndef _declaratio
#define _declaratio

#include "pch.hpp"
#include "script.hpp"
#include "datatype.hpp"
#include "variable.hpp"

class declaration_t {
public:

	declaration_t(const token_statement_t& expression) : tokens(expression)
	{
		tokens.end++;
	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }

	void declare_and_initialize();
private:
	enum class declaration_modifiers_e : char
	{
		ARRAY
	};
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

};


#endif