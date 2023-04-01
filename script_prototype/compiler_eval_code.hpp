#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

enum SYNTAX_FLAGS : size_t
{
	S_NONE					= 0x00ULL,
	S_EXPRESSION			= 0x01ULL,
	S_SEMICOLON				= 0x02ULL,
	S_POSTFIX				= 0x04ULL,
	S_UNARY					= 0x08ULL,
	S_IDENTIFIER			= 0x010ULL,
	S_OPENING_PARENTHESIS	= 0x020ULL
};
inline size_t operator-=(const SYNTAX_FLAGS f, const size_t flag) { 
	return ((size_t)f - (size_t)flag);
}
inline size_t operator+=(const SYNTAX_FLAGS f, const size_t flag) {
	return ((size_t)f + (size_t)flag);
}
struct Syntax
{
	Syntax() = default;
	~Syntax() = default;
private:
	size_t uFlags = S_NONE;

	struct parantheses_s
	{
		uint32_t count;
	};
	std::vector<std::pair<size_t, const char*>> errorcodes
	{
		std::make_pair(S_EXPRESSION, "Expected an expression"),
		std::make_pair(S_SEMICOLON, "Expected a \";\""),
		std::make_pair(S_IDENTIFIER, "Expected an identifier"),
		std::make_pair(S_OPENING_PARENTHESIS, "Expected a \"(\"")
	};

public:
	parantheses_s parantheses;

	void SetFlags(const size_t flags) { uFlags = (SYNTAX_FLAGS)flags; }
	void AddFlag(const size_t flags) { 
		if (((uFlags & flags) != 0)) //already exists
			return;
		uFlags += flags;
	}

	void ClearFlags() { uFlags = S_NONE; }
	void ClearFlag(const size_t flag) { 
		if (((uFlags & flag) == 0))
			return;

		uFlags -= flag;
	}
	bool FlagActive(const size_t flag) const { return (uFlags & flag) != 0; }
	size_t GetFlags() const { return uFlags; }
	void CheckRules(const size_t flags) const {
		if (!flags) return;

		if ((uFlags & flags) != 0) {
			for (const auto& [code, description] : errorcodes) {
				//std::cout << std::format("[{}, {}]\n", code, description);
				if ((flags & code) != 0)
					throw std::exception(description);
			}

		}
	}
};

//contains the parsed code
struct code_type
{
	//StatementType statement = StatementType::NO_STATEMENT;
	//bool variable_declaration = false;
	std::string code;
};
struct token_t
{
	enum keywordtype
	{
		DEFAULT,
		STATEMENT,
		TYPE_SPECIFIER,
		TYPE_QUALIFIER
	};
	enum tokentype : char
	{
		INVALID,
		DIGIT,
		OPERATOR,
		STRING,
		OTHER

	}t_type = tokentype::INVALID;

	char whitespace = false;
	std::string value; //does not include whitespaces
	std::unique_ptr<std::function<bool(token_t*)>> eval_fc = 0;
	keywordtype GetKeywordtype() {
		if (t_type != tokentype::STRING)
			return keywordtype::DEFAULT;
		
		if (Compile_EvaluateStatement2(this->value) != StatementType::NO_STATEMENT)
			return keywordtype::STATEMENT;
		
		return keywordtype::DEFAULT;
	}

};
namespace cec
{
	token_t Compiler_ReadToken(std::string::iterator& it); //doesn't require std::optional<token_t> yet
	bool Compiler_SemiColon(const token_t* token);
	bool Compiler_WhiteSpace(const token_t* token);
	code_type Compiler_ReadNextCode3(std::string::iterator& it);

	void Compiler_SetIdentifierFlags();
	void Compiler_SetStatementFlags();
	void Compiler_SetOperatorFlags();
	void Compiler_SetNumberFlags();

}

#endif