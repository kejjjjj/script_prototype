#include "pch.h"
bool cec::Compiler_SemiColon(const token_t* token)
{
	syntax.CheckRules(S_EXPRESSION);

	return token->value == ";";
}
void cec::Compiler_WhiteSpace(const token_t* token)
{
	if (!std::isspace(token->whitespace))
		return;

	syntax.Op.clear();

}

token_t cec::Compiler_ReadToken(std::string::iterator& it)
{
	enum tokentype : char
	{
		INVALID,
		DIGIT,
		OPERATOR,
		STRING,
		OTHER

	}t_type = INVALID;

	token_t token; //entire string that can include whitespaces
	bool isspace, isalnum;
	char ch;


	ch = *it;
	isspace = std::isspace(ch);
	isalnum = std::isalnum(ch);

	if (std::isdigit(ch))
		t_type = tokentype::DIGIT;
	else if (IsAnyOperator(ch))
		t_type = tokentype::OPERATOR;
	else if (std::isalpha(ch))
		t_type = tokentype::STRING;
	else if (std::isspace(ch)) { //store whitespaces to maintain 1:1 to original code
		++it;
		t_type == tokentype::OTHER;
		token.whitespace = ch;
		return token;
	}
	else {
		++it;
		t_type == tokentype::OTHER;
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
		switch (t_type) {
		case tokentype::DIGIT:
			if (!std::isdigit(ch)) {
				//--it; //go back one because we know this iterator is not valid here anymore
				return token;
			}
			break;
		case tokentype::OPERATOR:
			if (!IsAnyOperator(ch)) {
				//--it; //go back one because we know this iterator is not valid here anymore
				return token;
			}
			break;
		case tokentype::STRING:
			if (!std::isalnum(ch)) {
				//--it; //go back one because we know this iterator is not valid here anymore
				return token;
			}
			break;
		default:
			//CompilerError("Compiler_ReadToken(): default case");
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

	if (Compiler_SemiColon(&token)) {
		return code;
	}

	Compiler_WhiteSpace(&token);
	std::cout << "token: " << token.value << '\n';

		
	if (!Compiler_EvaluateToken(&token)) {
		CompilerError("Syntax Error!");
		return code;
	}

	code.code = token.value + Compiler_ReadNextCode3(it).code;

	
	

	//CompilerError("Compiler_ReadNextCode(): unexpected end of file");
	return code;
}
bool cec::Compiler_EvaluateToken(const token_t* token)
{
	syntax.ClearFlags();
	return 1;
}