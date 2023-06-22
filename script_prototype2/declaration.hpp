
#ifndef _declaratio
#define _declaratio

#include "pch.hpp"
#include "script.hpp"
#include "datatype.hpp"
class declaration_t {
public:
	declaration_t(const token_statement_t& expression) : tokens(expression)
	{

	};

	bool is_ready() const noexcept { return tokens.it != tokens.end; }

	void declare_and_initialize();
private:

	void get_declaration_type();
	void get_identifier_name();
	bool get_type_modifiers();

	dataTypes_e dtype = dataTypes_e::UNKNOWN;
	std::string identifier;
	token_statement_t tokens;

};


#endif