#include "pch.h"


code_type cec::Compiler_ReadNextCode(char start)
{
	std::string parsed, before_space;
	code_type code;
	bool isspace = false, isalnum = false;
	char ch = start;
	
	if (!f.is_open()) {
		CompilerError("file stream is not open");
		return code;
	}
	if (start) {
		isspace = std::isspace(ch);
		
		if (!isspace && syntaxrules.expecting_opening_paranthesis) {
			if (ch != '(') {
				CompilerError("expected a '(' instead of a '", ch, "'");
				return code;
			}
			syntaxrules.expecting_opening_paranthesis = false;
			syntaxrules.expecting_closing_paranthesis = true;
			syntaxrules.opening_paranthesis++;
		}

		parsed.push_back(start);

	}
	while (f.good() && !f.eof()) {

		ch = fs::F_Get(f);
		
		isspace = std::isspace(ch);
		isalnum = std::isalnum(ch);

		if (!isspace && syntaxrules.expecting_opening_paranthesis) {
			if (ch != '(') {
				CompilerError("expected a '(' instead of a '", ch, "'");
				return code;
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



		//after every space or non-alnum character check what the code wants
		if (isspace || !isalnum) {
			//cut the string so that it only saves the contents before the space

			
			auto type = Compile_EvaluateStatement(before_space);

			if (type != StatementType::NO_STATEMENT) {
				if (syntaxrules.expecting_expression) {
					CompilerError("expected an expression");
					return code;
				}
				if (!syntaxrules.typename_allowed) {
					CompilerError("type name is not allowed");
					return code;
				}
				
				else if (syntaxrules.expecting_identifier) {
					CompilerError("expected an identifier");
					return code;
				}
				syntaxrules.expecting_expression = true;
				syntaxrules.typename_allowed = false;
				syntaxrules.expecting_opening_paranthesis = true;
				
				code.statement = type;
				code.code = before_space + Compiler_ReadNextCode(ch).code;
				code.variable_declaration = false;
				std::cout << "returning\n";
				return code; 
				
			}
			else if (cv::ParseVariableDeclaration(parsed)) {

				if (!syntaxrules.typename_allowed || syntaxrules.expecting_expression) {
					CompilerError("type name is not allowed");
					return code;
				}
				
				syntaxrules.expecting_identifier = true;
				syntaxrules.typename_allowed = false;
				syntaxrules.expecting_expression = false;

				code.variable_declaration = true;
				code.statement = StatementType::NO_STATEMENT;
				code.code = before_space + Compiler_ReadNextCode(ch).code;
				std::cout << "returning2\n";

				return code;
			}

			before_space.clear();

		}

		if (ch == ';') {
			if (syntaxrules.expecting_closing_paranthesis && (syntaxrules.opening_paranthesis != syntaxrules.closing_paranthesis)) {
				CompilerError("expected a ')'");
				return code;
			}
			syntaxrules.opening_paranthesis = 0;
			syntaxrules.closing_paranthesis = 0;
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}


		std::cout << ch;
		parsed.push_back(ch);
		before_space.push_back(ch);


		if (std::isspace(before_space.back()) || !std::isalnum(before_space.back()))
			before_space.clear();

	}
}