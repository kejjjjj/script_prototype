#pragma once


#ifndef cmpevlcd
#define cmpevlcd

#include "pch.h"

enum SYNTAX_FLAGS : unsigned int
{
	S_NONE = 0x00,
	S_EXPRESSION = 0x01,
	S_SEMICOLON = 0x02

};

struct Syntax
{
	Syntax() = default;
	~Syntax() = default;
private:
	SYNTAX_FLAGS uFlags = S_NONE;

	std::vector<std::pair<unsigned int, const char*>> errorcodes
	{
		std::make_pair(S_EXPRESSION, "Expected an expression"),
		std::make_pair(S_SEMICOLON, "Expected a \";\"")

	};

public:
	std::string Op; //operator

	void SetFlags(const unsigned int flags) { uFlags = (SYNTAX_FLAGS)flags; }
	void ClearFlags() { uFlags = S_NONE; }
	SYNTAX_FLAGS GetFlags() const { return uFlags; }
	void CheckRules(const unsigned int flags) const {

		if (!flags)
			return;

		if ((flags & uFlags) != 0) {
			for (const auto& [code, description] : errorcodes) {
				std::cout << std::format("[{}, {}]\n", code, description);
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
	enum class tokentype : char
	{
		INVALID,
		DIGIT,
		OPERATOR,
		STRING

	}t_type = tokentype::INVALID;

	char whitespace = false;
	std::string value; //does not include whitespaces
};
namespace cec
{
	token_t Compiler_ReadToken(std::string::iterator& it); //doesn't require std::optional<token_t> yet
	bool Compiler_SemiColon(const token_t* token);
	void Compiler_WhiteSpace(const token_t* token);
	bool Compiler_EvaluateToken(const token_t* token);

	code_type Compiler_ReadNextCode3(std::string::iterator& it);

}

#endif