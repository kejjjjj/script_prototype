#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

inline struct syntax_rules
{
	bool expecting_explicit_type = false;
	bool expecting_initializer = false;
	bool expecting_expression = false;
	bool expecting_operand = false;
	bool typename_allowed = true;
	bool expecting_semicolon = false;
	bool expecting_identifier = false;
	bool expecting_opening_paranthesis = false;
	bool expecting_closing_paranthesis = false;
	size_t opening_paranthesis = 0;
	size_t closing_paranthesis = 0;
	std::string Operator;

	//more general ones
	bool expecting_variable_declaration = false; //<type> <var name> = <value>;

	void ClearRules()
	{
		expecting_explicit_type = false;
		expecting_initializer = false;
		expecting_expression = false;
		expecting_operand = false;
		typename_allowed = true;
		expecting_semicolon = false;
		expecting_identifier = false;
		expecting_opening_paranthesis = false;
		expecting_closing_paranthesis = false;
		opening_paranthesis = 0;
		closing_paranthesis = 0;
		Operator.clear();
	}

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
	code_type Compiler_ReadNextCode2(std::string::iterator& it);

	code_type Compiler_ReadNextCode(std::string::iterator& it);
	void Compiler_ReadStatement();
	bool Compiler_ReadParanthesis(char ch, bool isspace);
	void Compiler_ReadSemicolon();

}

#endif