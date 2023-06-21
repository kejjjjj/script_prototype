
#ifndef __statm
#define __statm

#include "pch.hpp"
#include "script.hpp"

enum class statementType_e : char
{
	EXPRESSION,
	DECLARATION
};
statementType_e statement_resolve_datatype_ambiguity(token_statement_t& tokens);
statementType_e statement_determine(const token_statement_t& tokens);

#endif