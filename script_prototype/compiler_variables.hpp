#pragma once

#ifndef __compvar
#define __compvar

#include "pch.h"

namespace cv
{
	inline void CheckRules(const expected_rules e)
	{
		switch (e) {

		case E_EXPLICIT_TYPE:
			CompilerError("expected an explicit type");
			return;
		case E_INITIALIZER:
			CompilerError("expected an initializer");
			return;
		case E_EXPRESSION:
			if (syntaxrules.expecting_expression)
				CompilerError("expected an expression");
			return;
		case E_OPERAND:
			if(syntaxrules.expecting_operand)
				CompilerError("expected an operand");
			return;
		case E_TYPENAME:
			if(!syntaxrules.typename_allowed)
				CompilerError("expected a type name");
			return;
		case E_SEMICOLON:
			CompilerError("expected a \";\"");
			return;
		case E_IDENTIFIER:
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
			if (syntaxrules.expecting_end_of_number)
				CompilerError("extra text after expected end of number");
			return;
		default:
			return;

		}
	}

	template <typename T, typename ... Args>
	void CheckRules(const T& t, const Args& ... arg) {



		CheckRules(arg...);

	}


	bool ParseDeclarationType(const std::string_view& full_statement);
	bool ParseTypeQualifier(const std::string_view& full_statement);
	bool ParseIdentifier(const std::string_view& full_statement);
	bool ParseInitializer(const std::string_view& full_statement);
	bool ParseNumericValue(const std::string_view& full_statement);

	std::string ParseOperator(std::string::iterator& it);
}

#endif