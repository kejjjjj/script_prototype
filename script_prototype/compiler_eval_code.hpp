#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

struct code_iterator
{
public:

	code_iterator(std::string::iterator& i, const size_t r=0ull, const size_t c=0ull) 
		: it(i), row(r), column(c) 
	{

	}

	std::string::iterator& it;
	size_t row{ 0ull };
	size_t column{ 0ull };

private:
	void updatedata()
	{
		if (*it == '\n') {
			row += 1;
			column = 0;
			return;
		}

		column += 1;

	}
	code_iterator& operator++() 
	{
		++it;
		updatedata();
		return *this;
	}
	code_iterator& operator++(int)
	{
		it++;
		updatedata();
		return *this;
	}
};

inline struct srules_s
{
	bool typename_allowed = true;
	bool expecting_identifier = false;
	bool expected_end_of_number = false;

	void reset() {
		typename_allowed = true;
		expecting_identifier = false;
		expected_end_of_number = false;
	}

}srules;

enum SYNTAX_FLAGS : size_t
{
	S_NONE					= 0x0ULL,
	S_OPENING_PARENTHESIS	= 0x2ULL,
	S_END_OF_NUMBER			= 0x4ULL,
	S_UNARY					= 0x8ULL,
	S_IDENTIFIER			= 0x10ULL,
	S_SEMICOLON				= 0x20ULL,
	S_EXPRESSION			= 0x40ULL,
};

struct Syntax
{
	Syntax() = default;
	~Syntax() = default;
private:
	size_t uFlags = S_NONE;

	std::vector<std::pair<size_t, const char*>> errorcodes
	{
		std::make_pair(S_EXPRESSION, "Expected an expression"),
		std::make_pair(S_SEMICOLON, "Expected a \";\""),
		std::make_pair(S_IDENTIFIER, "Expected an identifier"),
		std::make_pair(S_OPENING_PARENTHESIS, "Expected a \"(\""),
		std::make_pair(S_END_OF_NUMBER, "extra text after expected end of number")

	};

public:

	void SetFlags(const size_t flags) { uFlags = (SYNTAX_FLAGS)flags; }
	void AddFlag(const size_t flags) { 
		//if (((uFlags & flags) != 0)) //already exists
		//	return;
		uFlags |= flags;
	}

	void ClearFlags() { uFlags = S_NONE; }
	void ClearFlag(const size_t flag) { 
		if (((uFlags & flag) == 0))
			return;
		constexpr int a = S_SEMICOLON;
		constexpr int b = 128 & 64;
		uFlags -= flag;
	}
	bool FlagActive(const size_t flag) const { return (uFlags & flag) != 0; }
	size_t GetFlags() const { return uFlags; }
	void CheckRules(const size_t flags) const {
		if (!flags) return;

		if ((uFlags & flags)) {
			for (const auto& [code, description] : errorcodes) {
				//std::cout << std::format("[{}, {}]\n", code, description);
				if ((flags & code)) {
					throw std::exception(description);
				}
			}

		}
	}
};

//contains the parsed code
struct code_type
{
	enum code_type_e : char
	{
		EXPRESSION,
		STATEMENT,
		DECLARATION
	};
	//StatementType statement = StatementType::NO_STATEMENT;
	//bool variable_declaration = false;
	std::string code;
	code_type_e type = EXPRESSION;
};
struct token_t
{
	enum keywordtype
	{
		DEFAULT,
		STATEMENT,
		DECLARATION,
		TYPE_SPECIFIER,
		TYPE_QUALIFIER
	};
	enum tokentype : char
	{
		INVALID,
		DIGIT,
		OPERATOR,
		STRING,
		WHITESPACE,
		PARENTHESIS,
		STRING_LITERAL,
		CHAR_LITERAL,
		OTHER

	}t_type = tokentype::INVALID;

	char whitespace = false;
	char eof_character = '\0';
	std::string value; //does not include whitespaces
	keywordtype GetKeywordtype() const {
		if (t_type != tokentype::STRING)
			return keywordtype::DEFAULT;

		for (const auto& i : VarTypes)
			if (this->value == i)
				return keywordtype::DECLARATION;
		
		return keywordtype::DEFAULT;
	}

};
namespace cec
{
	token_t Compiler_ReadToken(std::string::iterator& it, CHAR expected_eof, std::string::iterator ref); //doesn't require std::optional<token_t> yet
	code_type Compiler_ReadNextCode3(std::string::iterator& it);
	std::string Compiler_ParseExpression(CHAR end_token, std::string::iterator& it);


}

#endif