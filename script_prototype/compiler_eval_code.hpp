#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

enum expected_rules
{
	E_EXPLICIT_TYPE,
	E_INITIALIZER,
	E_EXPRESSION,
	E_OPERAND,
	E_TYPENAME,
	E_SEMICOLON,
	E_IDENTIFIER,
	E_OPENING_PARANTHESIS,
	E_CLOSING_PARANTHESIS,
	E_DOT_IS_ALLOWED,
	E_CONSTANT_NUMERIC,
	E_END_OF_NUMBER,
	E_POSTFIX,
	E_UNARY

};

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
	bool dot_is_allowed = false;
	bool expecting_constant_numeric_value = false;
	bool expecting_end_of_number = false;
	bool unary_allowed = true;
	bool postfix_allowed = false;
	bool next_unary_is_not_an_operator = false;
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
		dot_is_allowed = false;
		expecting_constant_numeric_value = false;
		expecting_end_of_number = false;
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