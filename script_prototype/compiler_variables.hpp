#pragma once

#ifndef __compvar
#define __compvar

#include "pch.h"

namespace cv
{
	inline void CheckRules(const expected_rules e) {


		switch (e) {

		case E_EXPLICIT_TYPE:
			if (syntaxrules.expecting_explicit_type)
				CompilerError("expected an explicit type");
			return;
		case E_INITIALIZER:
			if (syntaxrules.expecting_initializer)
				CompilerError("expected an initializer");
			return;
		case E_EXPRESSION:
			if (syntaxrules.expecting_expression)
				CompilerError("expected an expression");
			return;
		case E_OPERAND:
			if (syntaxrules.expecting_operand)
				CompilerError("expected an operand");
			return;
		case E_TYPENAME:
			if (!syntaxrules.typename_allowed)
				CompilerError("expected a type name");
			return;
		case E_SEMICOLON:
			if(syntaxrules.expecting_semicolon)
				CompilerError("expected a \";\"");
			return;
		case E_IDENTIFIER:
			if (syntaxrules.expecting_identifier)
				CompilerError("expected an identifier");
			return;
		case E_OPENING_PARANTHESIS:
			CompilerError("expected a \"(\"");
			return;
		case E_CLOSING_PARANTHESIS:
			CompilerError("expected a \")\"");
			return;
		case E_DOT_IS_ALLOWED:
			if (!syntaxrules.dot_is_allowed)
				CompilerError("the character \".\" was unexpected");

			return;
		case E_CONSTANT_NUMERIC:
			if (syntaxrules.expecting_constant_numeric_value)
				CompilerError("expected a constant numeric value");

			return;
		case E_END_OF_NUMBER:
			if (syntaxrules.expecting_end_of_number && syntaxrules.expecting_expression)
				CompilerError("extra text after expected end of number");
			return;
		case E_POSTFIX:
			if (!syntaxrules.postfix_allowed)
				CompilerError("postfix operator is not allowed here");
			return;
		case E_UNARY:
			if (!syntaxrules.postfix_allowed)
				CompilerError("unary operator is not allowed here");
			return;
		default:
			return;
		}

	}


	bool ParseDeclarationType(const std::string_view& full_statement);
	bool ParseTypeQualifier(const std::string_view& full_statement);
	bool ParseIdentifier(const std::string_view& full_statement);
	bool ParseInitializer(const std::string_view& full_statement);
	bool ParseNumericValue(const std::string_view& full_statement);

	std::string ParseOperator(std::string::iterator& it);
}

#endif