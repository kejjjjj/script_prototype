#include "pch.h"

//only checks for syntax errors
//ignores the stack completely, so this will happily return uninitialized variables/functions
//this function is recursive, so stack overflows are possible if there are REALLY long expressions
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
				syntaxrules.dot_is_allowed = false;
				syntaxrules.expecting_constant_numeric_value = false;
				syntaxrules.expecting_end_of_number = false;

				if (!syntaxrules.Operator.empty()) { //a space after an operator if this is true
					syntaxrules.Operator.clear();
					syntaxrules.expecting_expression = true;
					syntaxrules.unary_allowed = true;
					syntaxrules.postfix_allowed = false;
					syntaxrules.next_unary_is_not_an_operator = true;

				}
			}
			//if (!IsOperator(ch)) {
			//	if (!syntaxrules.Operator.empty()) {
			//		syntaxrules.Operator.clear();
			//		syntaxrules.expecting_expression = true;
			//	}
			//}
		}
		int a = 1;

		if (before_space.empty()) {
			++it;
			continue;
		}

		std::cout << before_space;

		Compiler_ReadParanthesis(ch, isspace);
		//here before_space contains a keyword

		if (ch == ';') {
			Compiler_ReadSemicolon();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed;
			return code;
		}

		const auto type = Compile_EvaluateStatement(before_space);
		if (type != StatementType::NO_STATEMENT) {
			cv::CheckRules(E_SEMICOLON);
			cv::CheckRules(E_EXPRESSION);
			cv::CheckRules(E_IDENTIFIER);
			cv::CheckRules(E_EXPLICIT_TYPE);
			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_TYPENAME);

			syntaxrules.expecting_expression = true;
			syntaxrules.typename_allowed = false;
			syntaxrules.expecting_opening_paranthesis = true;
			syntaxrules.expecting_semicolon = false;
			syntaxrules.unary_allowed = false;
			syntaxrules.postfix_allowed = false;

			code.statement = type;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			code.variable_declaration = false;
			return code;
		}
		else if (cv::ParseDeclarationType(before_space)) {
			cv::CheckRules(E_CONSTANT_NUMERIC);
			cv::CheckRules(E_END_OF_NUMBER);
			cv::CheckRules(E_SEMICOLON);

			if (!syntaxrules.typename_allowed || syntaxrules.expecting_expression) {
				CompilerError("type name is not allowed");
				return code;
			}
			syntaxrules.expecting_identifier = true;
			syntaxrules.typename_allowed = false;
			syntaxrules.expecting_explicit_type = false;
			syntaxrules.expecting_variable_declaration = true;
			syntaxrules.expecting_semicolon = false;
			syntaxrules.unary_allowed = false;
			syntaxrules.next_unary_is_not_an_operator = true;
			syntaxrules.postfix_allowed = false;


			code.variable_declaration = true;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}
		else if (cv::ParseTypeQualifier(before_space)) {

			cv::CheckRules(E_CONSTANT_NUMERIC);
			cv::CheckRules(E_END_OF_NUMBER);
			cv::CheckRules(E_SEMICOLON);
			cv::CheckRules(E_IDENTIFIER);
			cv::CheckRules(E_EXPLICIT_TYPE);
			cv::CheckRules(E_EXPRESSION);

			syntaxrules.expecting_explicit_type = true;
			syntaxrules.expecting_initializer = false;
			syntaxrules.expecting_semicolon = false;
			syntaxrules.typename_allowed = true;
			syntaxrules.unary_allowed = false;
			syntaxrules.next_unary_is_not_an_operator = false;
			syntaxrules.postfix_allowed = false;

			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}
		else if (cv::ParseNumericValue(before_space)) {

			cv::CheckRules(E_POSTFIX);
			cv::CheckRules(E_EXPLICIT_TYPE);
			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_IDENTIFIER);

			if (syntaxrules.expecting_semicolon && !syntaxrules.expecting_constant_numeric_value) {
				CompilerError("expected a ';'");
				return code;
			}

			syntaxrules.expecting_semicolon = true;
			syntaxrules.expecting_expression = false;
			syntaxrules.expecting_operand = false;
			syntaxrules.typename_allowed = false;
			syntaxrules.dot_is_allowed = true;
			syntaxrules.unary_allowed = false;
			syntaxrules.next_unary_is_not_an_operator = false;


			if (syntaxrules.expecting_constant_numeric_value) { // expecting a spacebar after a decimal point number ends
				syntaxrules.expecting_end_of_number = true;
			}

			syntaxrules.expecting_constant_numeric_value = false;
			syntaxrules.postfix_allowed = false;

			syntaxrules.Operator.clear();

			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}
		else if (cv::ParseIdentifier(before_space)) {
			cv::CheckRules(E_CONSTANT_NUMERIC);
			cv::CheckRules(E_END_OF_NUMBER);
			cv::CheckRules(E_EXPLICIT_TYPE);
			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_SEMICOLON);

			//a + ~+ (+a / -+!a) --a;
			syntaxrules.expecting_identifier = false;
			//if(syntaxrules.expecting_expression)
			//	syntaxrules.expecting_semicolon = true;
			//else
			//	syntaxrules.expecting_semicolon = false;
			syntaxrules.postfix_allowed = true;
			syntaxrules.expecting_expression = false;
			syntaxrules.expecting_operand = false;
			syntaxrules.typename_allowed = false;
			syntaxrules.unary_allowed = false;
			syntaxrules.expecting_semicolon = true;
			syntaxrules.next_unary_is_not_an_operator = false;
			
			if (syntaxrules.expecting_variable_declaration) {
				syntaxrules.expecting_initializer = true;
				syntaxrules.expecting_semicolon = false;

			}
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
				syntaxrules.expecting_semicolon = false;
				syntaxrules.unary_allowed = true;
				syntaxrules.postfix_allowed = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode2(++it).code;

				return code;
			}
			CompilerError("expected an initializer");
			return code;
		}
		const auto op = before_space.front();

		if (IsPrefixOp(op)) {

			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_IDENTIFIER);
			cv::CheckRules(E_EXPLICIT_TYPE);

			syntaxrules.expecting_operand = true;
			syntaxrules.expecting_semicolon = false;


			if (!syntaxrules.Operator.empty() && op == '~') {
				syntaxrules.Operator.clear();
				syntaxrules.expecting_expression = true;
				syntaxrules.unary_allowed = true;
				syntaxrules.next_unary_is_not_an_operator = true;
			}

			if (syntaxrules.next_unary_is_not_an_operator) {
				syntaxrules.expecting_expression = true;
				if (ch == syntaxrules.Operator.front() && (ch == '-' || ch =='+')) {

					cv::CheckRules(E_INITIALIZER);
					cv::CheckRules(E_IDENTIFIER);
					cv::CheckRules(E_EXPLICIT_TYPE);
					cv::CheckRules(E_CONSTANT_NUMERIC);
					cv::CheckRules(E_END_OF_NUMBER);



					std::cout << "unary : " << syntaxrules.Operator << ch << '\n';

					if (!syntaxrules.unary_allowed && !syntaxrules.postfix_allowed) {
						if (!syntaxrules.unary_allowed)
							CompilerError("an unary operator is not allowed here");
						else
							CompilerError("a postfix operator is not allowed here");
						return code;
					}
					syntaxrules.Operator.clear();

					if (syntaxrules.unary_allowed) { //expecting an identifier after ++ or --

						cv::CheckRules(E_EXPRESSION);


						syntaxrules.expecting_identifier = true;
					}
					else if (syntaxrules.postfix_allowed) {
						syntaxrules.expecting_semicolon = true;
						syntaxrules.unary_allowed = false;
						syntaxrules.next_unary_is_not_an_operator = false;

					}

					syntaxrules.expecting_expression = false;
					syntaxrules.unary_allowed = false;
					syntaxrules.postfix_allowed = false;
					syntaxrules.expecting_operand = false;
				

				}

				
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode2(++it).code;
				syntaxrules.expecting_semicolon = false;
				return code;
			}

			if (!syntaxrules.expecting_expression && op == '~') {
				CompilerError('"', op, '"', " expected an identifier");
				return code;
			}
		}

		if (IsOperator(op)) {

			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_IDENTIFIER);
			cv::CheckRules(E_EXPLICIT_TYPE);
			//if (syntaxrules.expecting_expression && !IsPrefixOp(ch)) {
			//	CompilerError('"', op, '"', " is not an unary operator");
			//	return code;
			//}
			if (syntaxrules.expecting_expression && IsPrefixOp(ch) && syntaxrules.unary_allowed) { //unary
				syntaxrules.expecting_expression = true;
				syntaxrules.expecting_operand = true;
				syntaxrules.expecting_semicolon = false;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode2(++it).code;
				syntaxrules.expecting_semicolon = false;

				return code;
			}

			cv::CheckRules(E_EXPRESSION);

			cv::CheckRules(E_CONSTANT_NUMERIC);
			cv::CheckRules(E_END_OF_NUMBER);

			std::cout << "\npushing[" << op << "]\n";
			syntaxrules.Operator.push_back(op);
			syntaxrules.expecting_semicolon = false;

			//syntaxrules.unary_allowed = false;

			if (!IsAnOperator2(syntaxrules.Operator)) {
				CompilerError("expected an expression");
				return code;
			}
			if (EndOfOperator(syntaxrules.Operator)) {
				syntaxrules.expecting_operand = true;
				syntaxrules.expecting_expression = true;
				if ((syntaxrules.Operator == "++" || syntaxrules.Operator == "--")) {
					std::cout << "unary : " << syntaxrules.Operator << '\n';
					cv::CheckRules(E_POSTFIX);

					if (!syntaxrules.unary_allowed && !syntaxrules.postfix_allowed) {
						if(!syntaxrules.unary_allowed)
							CompilerError("an unary operator is not allowed here");
						else
							CompilerError("a postfix operator is not allowed here");
						return code;
					}

					if (syntaxrules.postfix_allowed) {
						
						syntaxrules.expecting_expression = false;
						syntaxrules.unary_allowed = false;
						syntaxrules.postfix_allowed = false;
						syntaxrules.expecting_operand = false;
						syntaxrules.expecting_semicolon = true;
					}
					else {

					}
				}

				syntaxrules.Operator.clear();
				//syntaxrules.unary_allowed = true;

				code.variable_declaration = false;
				code.statement = StatementType::NO_STATEMENT;
				code.code = parsed + Compiler_ReadNextCode2(++it).code;
				//syntaxrules.expecting_semicolon = false;

				//syntaxrules.next_unary_is_not_an_operator = false;

				return code;
			}
			//if (!IsPrefixOp(op)) {
				syntaxrules.next_unary_is_not_an_operator = true;
			//}
			


		}
		else if (!syntaxrules.Operator.empty()) { //operator ends here

			cv::CheckRules(E_INITIALIZER);
			cv::CheckRules(E_IDENTIFIER);
			cv::CheckRules(E_EXPLICIT_TYPE);

			--it;

			auto front = syntaxrules.Operator.front();
			if (syntaxrules.Operator.size() == 1 && front == '!' && !syntaxrules.expecting_expression) {
				CompilerError("an unary operator \"", front, "\" cannot be used as an operator between two operands");
				return code;
			}
			if (!IsAnOperator2(syntaxrules.Operator)) {
				CompilerError("expected an expression");
				return code;

			}
			if ((syntaxrules.next_unary_is_not_an_operator && IsPrefixOp(op)))
				++it;
			syntaxrules.next_unary_is_not_an_operator = false;
			std::cout << "\nyep: the operator " << "[ " << syntaxrules.Operator << " ] is completely valid!\n";
			syntaxrules.expecting_operand = true;
			syntaxrules.expecting_expression = true;
			syntaxrules.unary_allowed = true;
			syntaxrules.Operator.clear();
			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			

			return code;
		}
		else if (before_space == ".") {
			int ok = 1;
			1 != -(-(--ok));
			cv::CheckRules(E_END_OF_NUMBER);

			if (!syntaxrules.dot_is_allowed) {
				CompilerError("the character \".\" was unexpected"); //hardcoded for now
				return code;
			}

			syntaxrules.expecting_constant_numeric_value = true;
			syntaxrules.dot_is_allowed = false;
			syntaxrules.expecting_expression = false;
			syntaxrules.expecting_semicolon = true;
			syntaxrules.next_unary_is_not_an_operator = false;

			code.variable_declaration = false;
			code.statement = StatementType::NO_STATEMENT;
			code.code = parsed + Compiler_ReadNextCode2(++it).code;
			return code;
		}


		//////////////////////////////////////////////END OF THE WORD//////////////////////////////////////////////

	

		if (ch == '(') {
			syntaxrules.expecting_operand = true;
			syntaxrules.expecting_expression = true;
			syntaxrules.unary_allowed = true;
			syntaxrules.postfix_allowed = false;

		}
		else if (ch == ')') {
			syntaxrules.expecting_expression = false;
			syntaxrules.unary_allowed = false;
			syntaxrules.next_unary_is_not_an_operator = false;
			syntaxrules.postfix_allowed = false;

		}
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

		float ok = -45.20 - ~(+~+~~+-~~-+-+-1);
	}
}
void cec::Compiler_ReadSemicolon()
{
	if (syntaxrules.expecting_closing_paranthesis && (syntaxrules.opening_paranthesis != syntaxrules.closing_paranthesis)) {
		CompilerError("expected a ')'");
		return;
	}
	else if ((syntaxrules.opening_paranthesis != syntaxrules.closing_paranthesis)) {

		if (syntaxrules.opening_paranthesis < syntaxrules.closing_paranthesis)
			CompilerError("expected a \";\"");
		
		CompilerError("expected a \")\"");
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
	else if (!syntaxrules.Operator.empty()) {
		CompilerError("unexpected end of expression");
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