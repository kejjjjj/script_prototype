#include "pch.h"
bool cec::Compiler_SemiColon(const token_t* token)
{
	if (token->value == ";") {
		syntax.CheckRules(S_EXPRESSION);
		return true;
	}
	return false;
}
bool cec::Compiler_WhiteSpace(const token_t* token)
{

	if (!std::isspace(token->whitespace))
		return false;

	return true;
}

token_t cec::Compiler_ReadToken(std::string::iterator& it)
{


	token_t token; //entire string that can include whitespaces
	bool isspace, isalnum;
	char ch;

	

	ch = *it;
	std::cout << "first char: [" << ch << "]\n";
	isspace = std::isspace(ch);
	isalnum = std::isalnum(ch);

	if (std::isdigit(ch)) {
		token.t_type = token_t::tokentype::DIGIT;
	}
	else if (IsAnyOperator(ch)) {
		token.t_type = token_t::tokentype::OPERATOR;

	}
	else if (std::isalpha(ch)) {
		token.t_type = token_t::tokentype::STRING;
		//token.eval_fc = std::make_unique<std::function<bool( token_t*)>>(Compiler_StringToken);
	}
	else if (std::isspace(ch)) { //store whitespaces to maintain 1:1 to original code
		++it;
		token.t_type = token_t::tokentype::OTHER;
		token.whitespace = ch;
		return token;
	}
	else {
		++it;
		token.t_type = token_t::tokentype::OTHER;
		token.value.push_back(ch);
		return token;
	}
	//if (t_type == INVALID) {
	//	CompilerError("Unrecognized token type: \"", ch, "\"");
	//	return std::nullopt;
	//}
	token.value.push_back(ch);
	ch = *(++it);

	while (it != f_str.end()) {
		switch (token.t_type) {
		case token_t::tokentype::DIGIT:
			if (!std::isdigit(ch)) {
				return token;
			}
			break;
		case token_t::tokentype::OPERATOR:
			if (!IsAnOperator2(token.value)) {
				token.value.pop_back();
				--it;
				return token;
			}
			break;
		case token_t::tokentype::STRING:
			if (!std::isalnum(ch)) {
				return token;
			}
			break;
		default:
			CompilerError("Compiler_ReadToken(): default case");
			break;


		}
		token.value.push_back(ch);
		ch = *(++it);
		

	}
	if(token.value != ";")
		CompilerError("Compiler_ReadToken(): unexpected end of file");

	return token;
}

//only checks for syntax errors
//ignores the stack completely, so this will happily return uninitialized variables/functions
//this function is recursive, so stack overflows are possible if there are REALLY long expressions
code_type cec::Compiler_ReadNextCode3(std::string::iterator& it)
{
	code_type code;
	token_t token;

	//Sleep(500);

	token = Compiler_ReadToken(it);
	code.code += token.value;
	std::cout << "token: " << token.value << '\n';
	if (Compiler_SemiColon(&token)) {
		return code;
	}

	auto keyword = token.GetKeywordtype();

	if (keyword) {
		switch (keyword) {
		case token_t::keywordtype::STATEMENT:
			Compiler_SetStatementFlags();
			break;
		default:
			break;
		}
	}

	else {
		switch (token.t_type) {
		case token_t::tokentype::STRING:
			Compiler_SetIdentifierFlags();
			break;
		case token_t::tokentype::DIGIT:
			Compiler_SetNumberFlags();
			break;
		case token_t::tokentype::OPERATOR:
			Compiler_SetOperatorFlags();
			break;
		}
	
	}

	code.code += Compiler_ReadNextCode3(it).code;



	
	std::cout << "final code: " << code.code << '\n';

	return code;

}

void cec::Compiler_SetIdentifierFlags()
{
	syntax.CheckRules(S_SEMICOLON);

	syntax.AddFlag(S_SEMICOLON);
}
void cec::Compiler_SetStatementFlags()
{
	syntax.CheckRules(S_EXPRESSION);

	syntax.AddFlag(S_OPENING_PARENTHESIS);

}
void cec::Compiler_SetOperatorFlags()
{
	syntax.CheckRules(S_EXPRESSION);

	syntax.ClearFlag(S_SEMICOLON);

}
void cec::Compiler_SetNumberFlags()
{
	syntax.CheckRules(S_SEMICOLON);

	syntax.AddFlag(S_SEMICOLON);
}