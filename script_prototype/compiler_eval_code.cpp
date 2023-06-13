#include "pch.h"

token_t cec::Compiler_ReadToken(std::string::iterator& it, CHAR expected_eof, std::string::iterator ref)
{


	token_t token; //entire string that can include whitespaces
	bool isspace, isalnum;
	char ch;
	token.eof_character = 0;
	if ((it == ref)) {
		auto ch = *(it - 1);
		if (ch != expected_eof && expected_eof)
			throw std::exception(std::format("Compiler_ReadToken(): unexpected end of file\nexpected {} instead of {}", expected_eof, ch).c_str());
		token.t_type = token_t::tokentype::OTHER;
		token.eof_character = ch;

		return token;
	}

	

	ch = *it;
	
	if (ch == expected_eof) {
		token.eof_character = ch;
		token.t_type = token_t::tokentype::OTHER;

		return token;
	}

	isspace = std::isspace(ch);
	isalnum = std::isalnum(ch);
	
	if (std::isdigit(ch)) {
		token.t_type = token_t::tokentype::DIGIT;
	}
	else if (IsAnyOperator(ch)) {
		token.t_type = token_t::tokentype::OPERATOR;
	}
	else if (std::isalpha(ch) || ch == '_') {
		token.t_type = token_t::tokentype::STRING;
	}
	else if (std::isspace(ch)) { //store whitespaces to maintain 1:1 to original code
		++it;
		token.t_type = token_t::tokentype::WHITESPACE;
		token.whitespace = ch;
		token.value.push_back(ch);
		return token;
	}
	else if (ch == '(' || ch == ')') {
		++it;
		token.t_type = token_t::tokentype::PARENTHESIS;
		token.value.push_back(ch);
		return token;
	}
	else if (ch == '"') {
		token.t_type = token_t::tokentype::STRING_LITERAL;

	}
	else if (ch == '\'') {
		token.t_type = token_t::tokentype::CHAR_LITERAL;
	}
	else {
		++it;
		token.t_type = token_t::tokentype::OTHER;
		token.value.push_back(ch);
		return token;
	}
	token.value.push_back(ch);
	ch = *(++it);

	while (it != ref){
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
			if ((it + 1) == f_str.end()) {
				std::cout << "omg it's the end :o and the token is: " << token.value << '\n';
					return token;
			}
			break;
		case token_t::tokentype::STRING:
			if (!std::isalnum(ch) && ch != '_') {
				return token;
			}
			break;
		case token_t::tokentype::STRING_LITERAL:
			if (ch == '"') {
				token.value.push_back(ch);
				++it;
				return token;
			}
			break;
		case token_t::tokentype::CHAR_LITERAL:
			if (ch == '\'') {
				token.value.push_back(ch);
				++it;
				return token;
			}
			break;
		default:
			throw std::exception("Compiler_ReadToken(): default case");
			break;


		}
		token.value.push_back(ch);
		ch = *(++it);
		

	}

	if (token.t_type == token_t::tokentype::OPERATOR) {
		if (!IsAnOperator2(token.value)) {
			token.value.pop_back();
			--it;
			return token;
		}
	}

	//if(token.value.front() != expected_eof && expected_eof)
	//	throw std::exception(std::format("Compiler_ReadToken(): unexpected end of file\nexpected {} instead of {}", expected_eof, token.value).c_str());

	return token;
}
inline CHAR end_token;
code_type cec::Compiler_ReadNextCode3(std::string::iterator& it)
{
	code_type code;
	token_t token;

	auto end = f_str.end();
	token = Compiler_ReadToken(it, ';', end);
	code.code += token.value;

	while (token.t_type == token_t::WHITESPACE) {
		token = Compiler_ReadToken(it, ';', end);
		code.code += token.value;
	}

	auto keyword = token.GetKeywordtype();

	if (keyword) {
		switch (keyword) {
		case token_t::keywordtype::STATEMENT:
			end_token = ')';
			code.type = code_type::code_type_e::STATEMENT;
			code.code = token.value;
			return code;
		case token_t::keywordtype::DECLARATION:
			code.type = code_type::DECLARATION;
			decl::EvaluateDeclaration(token.value, it);
			return code;
		default:
			break;
		}
	}
	else {
		end_token = ';';
		code.type = code_type::code_type_e::EXPRESSION;
	}

	code.code = token.value + Compiler_ParseExpression(end_token, it);
	
	//std::cout << "final code: " << code.code << '\n';

	return code;

}

std::string cec::Compiler_ParseExpression(CHAR end_token, std::string::iterator& it)
{
	auto end = f_str.end();
	token_t token = Compiler_ReadToken(it, end_token, end);
	auto keyword = token.GetKeywordtype();
	std::string full_expression;

	if (keyword == token_t::keywordtype::STATEMENT) {
		throw std::exception("a statement within an expression is not allowed");
	}
	char front;
	switch (token.t_type) {

		case token_t::tokentype::PARENTHESIS:
			front = token.value.front();
			if (front == end_token) {

				return token.value;
			}
			break;
		case token_t::tokentype::OTHER:
			front = token.eof_character;
			if (front == end_token) {
				return token.value;
			}
			break;
	}

	full_expression = token.value + Compiler_ParseExpression(end_token, it);

	return full_expression;
}