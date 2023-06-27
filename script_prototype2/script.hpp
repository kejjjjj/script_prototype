
#ifndef _sscript_
#define _sscript_

#include "pch.hpp"
#include "punctuation.hpp"
enum class tokenType : char
{
	INVALID_LITERAL,
	NUMBER_LITERAL,
	FLOAT_LITERAL,
	STRING_LITERAL,
	CHAR_LITERAL,
	BUILT_IN_TYPE,
	NAME,
	PUNCTUATION
};
enum class literalSuffixType : DWORD
{
	NONE,
	UNSIGNED,
	CHAR
};
struct token_t
{
	std::string string;
	tokenType tt = tokenType::INVALID_LITERAL;
	size_t line = 0;
	size_t column = 0;
	DWORD extrainfo = 0; //e.g. this will contain the index of a punctuation mark if tt == PUNCTUATION (LOWORD)

	void print() const noexcept
	{
		std::cout << "\ntoken: " << string << '\n'
			<< "type: " << int(tt) << '\n'
			<< "position: [" << line << ", " << column << "]\n";
	}
	void reset() noexcept
	{
		string = "";
		tt = tokenType::INVALID_LITERAL;
		line = 0;
		column = 0;
	}
};
struct token_statement_t
{
	std::list<token_t>::iterator it;
	std::list<token_t>::iterator begin;
	std::list<token_t>::iterator end;
};
struct script_t
{
	script_t(const std::string& filename);
	
	bool scriptReady() const noexcept { return buffer.empty() == false; }

	
	std::string filename;
	std::string buffer;
	std::string::iterator script_p;
	std::string::iterator lastscript_p;
	std::string::iterator scriptend_p;
	size_t line = 0;
	size_t column = 0;
	


	void S_Tokenize();
	token_statement_t S_CreateStatement();

private:
	bool eof_allowed = true;

	bool S_ReadToken(token_t& token);
	bool S_ReadWhitespace();
	bool S_ReadNumber(token_t& token);
	bool S_ReadType(token_t& token);

	bool S_ReadName(token_t& token);
	bool S_ReadCharacterLiteral(token_t& token);
	bool S_ReadPunctuation(token_t& token);
	bool S_ParseInt(token_t& token);

	std::list<token_t> tokens;
	decltype(tokens)::iterator token_it;
};


class scriptError_t
{
public:
	scriptError_t(const std::string& reason) : errReason(reason) {

	}
	scriptError_t(const token_t* _token, const std::string& reason) : token(_token), errReason(reason) {

		errReason = std::format("syntax error:\n{}\non line: [{},{}]", errReason, token->line, token->column);

	};
	scriptError_t(const script_t* _script, const std::string& reason) : script(_script), errReason(reason) {
		
		errReason = std::format("syntax error:\n{}\non line: [{},{}]", errReason, script->line, script->column);

	};
	scriptError_t() = delete;
	[[nodiscard]] const char* const what() const noexcept {
		return errReason.empty() ? "null exception" : errReason.c_str();
	}

	std::string errReason;
	const script_t* script = 0;
	const token_t* token = 0;

};

#endif