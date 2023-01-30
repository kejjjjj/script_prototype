#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

inline struct syntax_rules
{
	bool expecting_expression = false;
	bool typename_allowed = true;
	bool expecting_semicolon = false;
	bool expecting_identifier = false;
	bool expecting_opening_paranthesis = false;
	bool expecting_closing_paranthesis = false;
	size_t opening_paranthesis = 0;
	size_t closing_paranthesis = 0;

}syntaxrules;

//contains the parsed code
struct code_type
{
	StatementType statement = StatementType::NO_STATEMENT;
	bool variable_declaration = false;
	std::string code;
};

namespace cec
{
	code_type Compiler_ReadNextCode(char start = '\0');


}

#endif