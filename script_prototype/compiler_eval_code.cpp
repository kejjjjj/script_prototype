#include "pch.h"

code_type cec::Compiler_ReadNextCode2(std::string::iterator& it)
{
	std::string parsed, before_space;
	code_type code;
	bool isspace = false, isalnum = false, clear = false, within_quotes = false, paranthesis = false;
	bool first_char_number = false;
	char ch;

	while (true) {

		ch = *it;

		

		isspace = std::isspace(ch);
		isalnum = std::isalnum(ch);
		
		if (isalnum) {
			first_char_number = std::isdigit(ch);
			while (isalnum)
			{
				parsed.push_back(ch);
				before_space.push_back(ch);
				ch = *++it;

				if (first_char_number && !std::isdigit(ch))
					break;

				isalnum = std::isalnum(ch);

			}
			ch = *--it;
		}
		else {
			parsed.push_back(ch);
			if (!std::isspace(ch))
				before_space.push_back(ch);
			else {
				if (!syntaxrules.Operator.empty()) {
					syntaxrules.Operator.clear();
					syntaxrules.expecting_operand = true;
				}
			}
		}

		if (before_space.empty()) {
			++it;
			continue;
		}
		
		std::cout << before_space;

		Compiler_ReadParanthesis(ch, isspace);

		auto type = Compile_EvaluateStatement(before_space);

		if (type != StatementType::NO_STATEMENT) {

			Compiler_ReadStatement();
			code.statement = type;
			//syntaxrules.opening_paranthesis++;
			//parsed.pop_back();
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			code.variable_declaration = false;
			return code;

		}
		else if (cv::ParseDeclarationType(before_space)) {

			if (!syntaxrules.typename_allowed || syntaxrules.expecting_expression) {
				CompilerError("type name is not allowed");
				return code;
			}
			if (syntaxrules.expecting_semicolon) {
				CompilerError("expected a ';'");
				return code;
			}
			syntaxrules.expecting_identifier = true;
			syntaxrules.typename_allowed = false;
			syntaxrules.expecting_explicit_type = false;
			syntaxrules.expecting_variable_declaration = true;
			syntaxrules.expecting_semicolon = false;
			code.variable_declaration = true;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}
		else if (cv::ParseTypeQualifier(before_space)) {
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
			if (syntaxrules.expecting_semicolon) {
				CompilerError("expected a ';'");
				return code;
			}

			syntaxrules.expecting_explicit_type = true;
			syntaxrules.expecting_initializer = false;
			syntaxrules.expecting_semicolon = false;
			syntaxrules.typename_allowed = true;
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}
		else if (cv::ParseNumericValue(before_space)) {

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
			if (syntaxrules.expecting_identifier) {
				CompilerError("expected an identifier");
				return code;
			}
			syntaxrules.expecting_semicolon = true;
			syntaxrules.expecting_expression = false;
			syntaxrules.expecting_operand = false;
			syntaxrules.typename_allowed = false;

			syntaxrules.Operator.clear();

			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;

			return code;
		}
		else if (cv::ParseIdentifier(before_space)) {

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
			syntaxrules.expecting_semicolon = true;
			syntaxrules.expecting_expression = false;
			syntaxrules.expecting_operand = false;
			syntaxrules.typename_allowed = false;

			if (syntaxrules.expecting_variable_declaration)
				syntaxrules.expecting_initializer = true;

			syntaxrules.Operator.clear();

			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;

			return code;

		}
		else if (syntaxrules.expecting_initializer) {
			if (cv::ParseInitializer(before_space)) {

				if (syntaxrules.expecting_identifier) {
					CompilerError("expected an identifier");
					return code;
				}
				if (syntaxrules.expecting_expression) {
					CompilerError("expected an expression");
					return code;
				}
				syntaxrules.expecting_expression = true;
				syntaxrules.expecting_initializer = false;
				syntaxrules.typename_allowed = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode2(++it).code;

				return code;
			}
			CompilerError("expected an initializer");
			return code;
		}
		auto op = before_space.front();

		if (IsOperator(op)) {
			std::cout << "\npushing[" << op << "]\n";
			syntaxrules.Operator.push_back(op);
		}
		if (IsAnOperator2(syntaxrules.Operator)) {
			if (syntaxrules.expecting_operand) {
				CompilerError("expected an operand");
				return code;
			}
			syntaxrules.expecting_expression = true;
			syntaxrules.expecting_semicolon = false;


			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;

		}
		else if (!syntaxrules.Operator.empty()) {
			--it;
			syntaxrules.expecting_operand = true;
			syntaxrules.Operator.clear();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}



		

		if (ch == ';') {
			Compiler_ReadSemicolon();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

		if (ch == '(')
			syntaxrules.expecting_expression = true;

		else if (ch == ')' && syntaxrules.expecting_expression) {
			CompilerError("expected an expression");
			return code;

		}
		else if (ch == ')' && syntaxrules.expecting_expression && (syntaxrules.opening_paranthesis == syntaxrules.closing_paranthesis))
			syntaxrules.expecting_expression = false;

		

		//end of statement expression
		if ((syntaxrules.opening_paranthesis == syntaxrules.closing_paranthesis) && syntaxrules.expecting_closing_paranthesis) {

			syntaxrules.opening_paranthesis = 0;
			syntaxrules.closing_paranthesis = 0;
			syntaxrules.expecting_closing_paranthesis = false;
			syntaxrules.expecting_expression = false;
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

		before_space.clear();

		++it;

	}
}

//only checks for syntax errors
//ignores the stack completely, so this will happily return uninitialized variables/functions
code_type cec::Compiler_ReadNextCode(std::string::iterator& it)
{
	std::string parsed, before_space;
	code_type code;
	bool isspace = false, isalnum = false, clear = false, within_quotes = false, paranthesis = false;
	char ch;

	if (*it == ')' && syntaxrules.closing_paranthesis)
		syntaxrules.closing_paranthesis--;

	while (true) {

		ch = *it++;
		
		

		isspace = std::isspace(ch);
		isalnum = std::isalnum(ch);

		within_quotes = (ch == '"') ? !within_quotes : within_quotes;

		//increments parantheses count and checks for expected (
		paranthesis = ch == '(' || ch == ')';
		clear = Compiler_ReadParanthesis(ch, isspace) || paranthesis;
		


		//after every space or non-alnum character check what the code wants
		if ((isspace || !isalnum) && !before_space.empty()) {
			//cut the string so that it only saves the contents before the space

			
			auto type = Compile_EvaluateStatement(before_space);

			if (type != StatementType::NO_STATEMENT) {

				Compiler_ReadStatement();
				code.statement = type;
				//syntaxrules.opening_paranthesis++;
				code.code = parsed + Compiler_ReadNextCode(--it).code;
				code.variable_declaration = false;
				return code; 
				
			}
			else if (cv::ParseDeclarationType(before_space)) {

				if (!syntaxrules.typename_allowed || syntaxrules.expecting_expression) {
					CompilerError("type name is not allowed");
					return code;
				}
				if (syntaxrules.expecting_semicolon) {
					CompilerError("expected a ';'");
					return code;
				}
				syntaxrules.expecting_identifier = true;
				syntaxrules.typename_allowed = false;
				syntaxrules.expecting_explicit_type = false;
				syntaxrules.expecting_variable_declaration = true;
				syntaxrules.expecting_semicolon = false;
				code.variable_declaration = true;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(--it).code;
				return code;
			}
			else if (cv::ParseTypeQualifier(before_space)) {
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
				if (syntaxrules.expecting_semicolon) {
					CompilerError("expected a ';'");
					return code;
				}
				
				syntaxrules.expecting_explicit_type = true;
				syntaxrules.expecting_initializer = false;
				syntaxrules.expecting_semicolon = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(--it).code;
				return code;
			}
			else if (cv::ParseIdentifier(before_space)) {

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
				syntaxrules.expecting_expression = false;
				syntaxrules.expecting_operand = false;

				if(syntaxrules.expecting_variable_declaration)
					syntaxrules.expecting_initializer = true;

				syntaxrules.Operator.clear();
			
				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode(--it).code;

				return code;

			}
			else if (syntaxrules.expecting_initializer) {
				if (cv::ParseInitializer(before_space)) {

					if (syntaxrules.expecting_identifier) {
						CompilerError("expected an identifier");
						return code;
					}
					if (syntaxrules.expecting_expression) {
						CompilerError("expected an expression");
						return code;
					}
					syntaxrules.expecting_expression = true;
					syntaxrules.expecting_initializer = false;
					code.variable_declaration = false;
					code.statement = StatementType::NO_STATEMENT;
					code.code = parsed + Compiler_ReadNextCode(--it).code;

					return code;
				}
				CompilerError("expected an initializer");
				return code;
			}
			if (before_space.size() == 1) {
				auto op = before_space.front();

				if(IsOperator(op)) {
					std::cout << "\npushing[" << op << "]\n";
					syntaxrules.Operator.push_back(op);
				}
				if (IsAnOperator2(syntaxrules.Operator)) {
					if (syntaxrules.expecting_operand) {
						CompilerError("expected an operand");
						return code;
					}
					syntaxrules.expecting_expression = true;


					code.variable_declaration = false;
					code.statement = StatementType::NO_STATEMENT;
					code.code = parsed + Compiler_ReadNextCode(--it).code;
					return code;

				}
				else if (!syntaxrules.Operator.empty()) {
					--it;
					syntaxrules.expecting_operand = true;
					syntaxrules.Operator.clear();
					code.variable_declaration = false;
					code.statement = StatementType::NO_STATEMENT;
					code.code = parsed + Compiler_ReadNextCode(--it).code;
					return code;
				}
			}
			
			before_space.clear();
			
			syntaxrules.expecting_semicolon = true;
		}

		if (ch == '(')
			syntaxrules.expecting_expression = true;

		else if (ch == ')' && syntaxrules.expecting_expression) {
			CompilerError("expected an expression");
			return code;

		}
		else if (ch == ')' && syntaxrules.expecting_expression && (syntaxrules.opening_paranthesis == syntaxrules.closing_paranthesis))
			syntaxrules.expecting_expression = false;
		

		if (ch == ';') {
			Compiler_ReadSemicolon();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}
		
		parsed.push_back(ch);
		before_space.push_back(ch);

		if (isspace || clear)
			before_space.clear();

		std::cout << ch;


		//end of statement expression
		if ((syntaxrules.opening_paranthesis == syntaxrules.closing_paranthesis) && syntaxrules.expecting_closing_paranthesis) {
			
			syntaxrules.opening_paranthesis = 0;
			syntaxrules.closing_paranthesis = 0;
			syntaxrules.expecting_closing_paranthesis = false;
			syntaxrules.expecting_expression = false;
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
	else if ((syntaxrules.opening_paranthesis != syntaxrules.closing_paranthesis)) {
		CompilerError("expected a '", syntaxrules.opening_paranthesis < syntaxrules.closing_paranthesis ? '(' : ')', "'");
	}
	else if (syntaxrules.expecting_expression) {
		CompilerError("expected an expression");
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
bool cec::Compiler_ReadParanthesis(char ch, bool isspace)
{
	if (isspace)
		return 0;

	if (syntaxrules.expecting_opening_paranthesis) {
		if (ch != '(') {
			CompilerError("expected a '(' instead of a '", ch, "'");
			return 0;
		}
			
		syntaxrules.expecting_opening_paranthesis = false;
		syntaxrules.expecting_closing_paranthesis = true;
		syntaxrules.opening_paranthesis++;
		return 1;
	}
//	else if (syntaxrules.expecting_closing_paranthesis) {
		if (ch == '(')
			syntaxrules.opening_paranthesis++;
		else if (ch == ')')
			syntaxrules.closing_paranthesis++;
	// }
	return 0;
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