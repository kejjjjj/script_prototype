#include "pch.h"
bool cec::Compiler_SemiColon(const token_t* token)
{
	syntax.CheckRules(S_EXPRESSION);

	return token->value == ";";
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
	isspace = std::isspace(ch);
	isalnum = std::isalnum(ch);

	if (std::isdigit(ch)) {
		token.t_type = token_t::tokentype::DIGIT;
		token.eval_fc = Compiler_NumericToken;
	}
	else if (IsAnyOperator(ch)) {
		token.t_type = token_t::tokentype::OPERATOR;
		token.eval_fc = Compiler_OperatorToken;

	}
	else if (std::isalpha(ch)) {
		token.t_type = token_t::tokentype::STRING;
		token.eval_fc = Compiler_StringToken;
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
			if (!IsAnyOperator(ch)) {
				return token;
			}
			break;
		case token_t::tokentype::STRING:
			if (!std::isalnum(ch)) {
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

	
	std::cout << "token: " << token.value << '\n';

	if (!Compiler_WhiteSpace(&token)) {
		if (!token.eval_fc(&token)) {
			CompilerError("Syntax Error!");
			return code;
		}
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
bool cec::Compiler_NumericToken(const token_t* token)
{
	if (!token)
		return false;

	return true;
}
bool cec::Compiler_StringToken(const token_t* token)
{
	if (!token)
		return false;

	return true;
}
bool cec::Compiler_OperatorToken(const token_t* token)
{
	if (!token)
		return false;

	std::optional<std::list<std::string>> tokens_opt;

	if (!(tokens_opt = TokenizeOperatorSequence(token->value))) {
		return false;
	}


	//tokens contains a list of tokenized operators
	const auto& tokens = tokens_opt.value(); 

	for (const auto& i : tokens) {
		std::cout << "op: " << i << '\n';
		if (!Compiler_SyntaxCheckOperator(i))
			return false;
	}
	return true;
}
bool cec::Compiler_SyntaxCheckOperator(const std::string_view& op)
{
	const auto front = op.front();
	if (IsUnaryOperator(op)) {
		if(front == '~' || front == '!')
			syntax.CheckRules(S_SEMICOLON);  //unary ~ and ! cannot be used after an expression
		

	}

	return true;
}