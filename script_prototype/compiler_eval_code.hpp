#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

enum SYNTAX_FLAGS : size_t
{
	S_NONE			= 0x00ULL,
	S_EXPRESSION	= 0x01ULL,
	S_SEMICOLON		= 0x02ULL,
	S_POSTFIX		= 0x04ULL,
	S_UNARY			= 0x08ULL,
	S_IDENTIFIER	= 0x010ULL
};
inline SYNTAX_FLAGS operator-=(const SYNTAX_FLAGS f, const size_t flag) { 
	return SYNTAX_FLAGS(f - (SYNTAX_FLAGS)flag); 
}
inline SYNTAX_FLAGS operator|=(const SYNTAX_FLAGS f, const size_t flag) {
	return SYNTAX_FLAGS(f | (SYNTAX_FLAGS)flag);
}
struct Syntax
{
	Syntax() = default;
	~Syntax() = default;
private:
	SYNTAX_FLAGS uFlags = S_NONE;

	std::vector<std::pair<size_t, const char*>> errorcodes
	{
		std::make_pair(S_EXPRESSION, "Expected an expression"),
		std::make_pair(S_SEMICOLON, "Expected a \";\""),
		std::make_pair(S_IDENTIFIER, "Expected an identifier")
	};

public:
	void SetFlags(const size_t flags) { uFlags = (SYNTAX_FLAGS)flags; }
	void AddFlag(const size_t flags) { uFlags |= (SYNTAX_FLAGS)flags; }

	void ClearFlags() { uFlags = S_NONE; }
	void ClearFlag(const size_t flag) { 
		if (((uFlags & flag) == 0))
			return;

		uFlags -= (SYNTAX_FLAGS)flag; }
	bool FlagActive(const size_t flag) const { return (uFlags & flag) != 0; }
	SYNTAX_FLAGS GetFlags() const { return uFlags; }
	void CheckRules(const size_t flags) const {
		if (!flags) return;

		if ((uFlags & flags) != 0) {
			for (const auto& [code, description] : errorcodes) {
				//std::cout << std::format("[{}, {}]\n", code, description);
				if ((flags & code) != 0)
					return CompilerError(description);
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
	std::function<bool(const token_t*)> eval_fc;
};
namespace cec
{
	token_t Compiler_ReadToken(std::string::iterator& it); //doesn't require std::optional<token_t> yet
	bool Compiler_SemiColon(const token_t* token);
	bool Compiler_WhiteSpace(const token_t* token);
	bool Compiler_EvaluateToken(const token_t* token);
	bool Compiler_NumericToken(const token_t* token);
	bool Compiler_StringToken(const token_t* token);
	bool Compiler_OperatorToken(const token_t* token);
	bool Compiler_SyntaxCheckOperator(const std::string_view& op);
	code_type Compiler_ReadNextCode3(std::string::iterator& it);

}

#endif