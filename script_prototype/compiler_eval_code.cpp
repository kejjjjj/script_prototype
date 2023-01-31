#include "pch.h"

//only checks for syntax errors
//ignores the stack completely, so this will happily return uninitialized variables/functions
code_type cec::Compiler_ReadNextCode(char start)
{
	std::string parsed, before_space;
	code_type code;
	bool isspace = false, isalnum = false;
	char ch = start;
	bool within_quotes = false;

	if (!f.is_open()) {
		CompilerError("file stream is not open");
		return code;
	}
	if (start) {
		
		within_quotes = ch == '"';

		if (!std::isspace(ch) && syntaxrules.expecting_opening_paranthesis && !within_quotes) {
			if (ch != '(') {
				CompilerError("expected a '(' instead of a '", ch, "'");
				return code;
			}
			syntaxrules.expecting_opening_paranthesis = false;
			syntaxrules.expecting_closing_paranthesis = true;
			syntaxrules.opening_paranthesis++;
			before_space.push_back(ch);

			if (cv::ParseInitializer(before_space)) {

				syntaxrules.expecting_expression = true;
				syntaxrules.expecting_initializer = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = before_space + Compiler_ReadNextCode(ch).code;

				return code;
			}
			else if (syntaxrules.expecting_initializer) {
				CompilerError("expected an initializer");
				return code;
			}
		}

		if (ch == ';') { //weird...
			Compiler_ReadSemicolon();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

		parsed.push_back(start);

		

	}
	while (f.good() && !f.eof()) {

		ch = fs::F_Get(f);
		
		std::cout << ch;

		isspace = std::isspace(ch);
		isalnum = std::isalnum(ch);

		within_quotes = (ch == '"') ? !within_quotes : within_quotes;

		//increments parantheses count and checks for expected (
		Compiler_ReadParanthesis(ch, isspace);


		//after every space or non-alnum character check what the code wants
		if ((isspace || !isalnum) && !before_space.empty()) {
			//cut the string so that it only saves the contents before the space

			
			auto type = Compile_EvaluateStatement(before_space);

			if (type != StatementType::NO_STATEMENT) {

				Compiler_ReadStatement();
				code.statement = type;
				code.code = before_space + Compiler_ReadNextCode(ch).code;
				code.variable_declaration = false;
				return code; 
				
			}
			else if (cv::ParseDeclarationType(parsed)) {

				if (!syntaxrules.typename_allowed || syntaxrules.expecting_expression) {
					CompilerError("type name is not allowed");
					return code;
				}
				syntaxrules.expecting_identifier = true;
				syntaxrules.typename_allowed = false;
				syntaxrules.expecting_expression = false;
				syntaxrules.expecting_explicit_type = false;
				syntaxrules.expecting_variable_declaration = true;
				syntaxrules.expecting_semicolon = false;

				code.variable_declaration = true;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(ch).code;
				return code;
			}
			else if (cv::ParseTypeQualifier(parsed)) {
				if (syntaxrules.expecting_expression) {
					CompilerError("type qualifier is not allowed");
					return code;
				}
				if (syntaxrules.expecting_explicit_type)
				{
					CompilerError("expected an explicit type");
					return code;
				}
				if (syntaxrules.expecting_identifier) {
					CompilerError("expected an identifier");
					return code;
				}
				syntaxrules.expecting_explicit_type = true;
				syntaxrules.expecting_expression = false;
				syntaxrules.expecting_initializer = false;
				syntaxrules.expecting_semicolon = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(ch).code;
				return code;
			}
			else if (cv::ParseIdentifier(parsed)) {

				if (syntaxrules.expecting_explicit_type) {
					CompilerError("expected an explicit type");
					return code;
				}
				if (syntaxrules.expecting_initializer) {
					CompilerError("expected an initializer");
					return code;
				}
				if (syntaxrules.expecting_semicolon) {
					CompilerError("expected a ';'");
					return code;
				}
				syntaxrules.expecting_identifier = false;
				syntaxrules.expecting_semicolon= true;

				if(syntaxrules.expecting_variable_declaration)
					syntaxrules.expecting_initializer = true;



				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(ch).code;
				
				return code;

			}
			if (cv::ParseInitializer(parsed)) {

				if (syntaxrules.expecting_identifier) {
					CompilerError("expected an identifier");
					return code;
				}

				syntaxrules.expecting_expression = true;
				syntaxrules.expecting_initializer = false;
				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(ch).code;

				return code;
			}
			else if (syntaxrules.expecting_initializer) {
				CompilerError("expected an initializer");
				return code;
			}
			

			before_space.clear();
			
		}

		if (ch == ';') {
			Compiler_ReadSemicolon();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

		parsed.push_back(ch);
		before_space.push_back(ch);

		if (isspace)
			before_space.clear();

		//end of statement expression
		if ((syntaxrules.opening_paranthesis == syntaxrules.closing_paranthesis) && syntaxrules.expecting_closing_paranthesis) {
			syntaxrules.opening_paranthesis = 0;
			syntaxrules.closing_paranthesis = 0;
			syntaxrules.expecting_closing_paranthesis = false;
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

	}
	CompilerError("this should never be reached");
	return code;
}
void cec::Compiler_ReadSemicolon()
{
	if (syntaxrules.expecting_closing_paranthesis && (syntaxrules.opening_paranthesis != syntaxrules.closing_paranthesis)) {
		CompilerError("expected a ')'");
		return;
	}
	else if (syntaxrules.expecting_initializer) {
		CompilerError("expected an initializer");
		return;
	}
	else if (syntaxrules.expecting_identifier) {
		CompilerError("expected an identifier");
		return;
	}

	syntaxrules.ClearRules();

}
void cec::Compiler_ReadParanthesis(char ch, bool isspace)
{
	if (!isspace && syntaxrules.expecting_opening_paranthesis) {
		if (ch != '(') {
			CompilerError("expected a '(' instead of a '", ch, "'");
			return;
		}
		syntaxrules.expecting_opening_paranthesis = false;
		syntaxrules.expecting_closing_paranthesis = true;
		syntaxrules.opening_paranthesis++;
	}
	else if (syntaxrules.expecting_closing_paranthesis) {
		if (ch == '(')
			syntaxrules.opening_paranthesis++;
		else if (ch == ')')
			syntaxrules.closing_paranthesis++;
	}
}
void cec::Compiler_ReadStatement()
{
	if (syntaxrules.expecting_expression) {
		CompilerError("expected an expression");
		return;
	}
	else if (!syntaxrules.typename_allowed) {
		CompilerError("type name is not allowed");
		return;
	}
	else if (syntaxrules.expecting_identifier) {
		CompilerError("expected an identifier");
		return;
	}
	else if (syntaxrules.expecting_explicit_type) {
		CompilerError("expected an explicit type");
		return;
	}
	else if (syntaxrules.expecting_initializer) {
		CompilerError("expected an initializer");
		return;
	}
	syntaxrules.expecting_expression = true;
	syntaxrules.typename_allowed = false;
	syntaxrules.expecting_opening_paranthesis = true;
	syntaxrules.expecting_semicolon = false;
}