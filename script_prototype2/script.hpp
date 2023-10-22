
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
	STATEMENT,
	NAME,
	PUNCTUATION,
	FUNCTION
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
		LOG( "\ntoken: " << string << '\n'
			<< "type: " << int(tt) << '\n'
			<< "position: [" << line << ", " << column << "]\n");
	}
	void reset() noexcept
	{
		string = "";
		tt = tokenType::INVALID_LITERAL;
		line = 0;
		column = 0;
	}
};
struct code_segment_t
{
	std::list<token_t>::iterator it;
	std::list<token_t>::iterator begin;
	std::list<token_t>::iterator end;

	void print() const noexcept {

		LOG( "code_segment_t::print():\n");
		for (auto i = it; i != end; i++) {
			LOG( i->string);
		}
		LOG( end->string << '\n');
	}
	std::list<token_t>::iterator find_token(tokenType tt, DWORD extrainfo) {

		for (auto i = it; i != end; i++) {
			if (i->tt == tt && i->extrainfo == extrainfo)
				return i;
		}

		return end;
	}

};
class scr_scope_t;
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
	
	//class scr_scope_t* global_scope;

	void S_Tokenize();
	std::optional<code_segment_t> create_code_segment(scr_scope_t** scope);
	code_segment_t S_GiveRemaining() noexcept(true);
	auto S_GetIterator() noexcept(true) { return token_it; }
	void S_SetIterator(std::list<token_t>::iterator it) { token_it = it; }
	void S_ResetIterator() noexcept { token_it = tokens.begin(); }
	decltype(auto) S_GetEnd() noexcept(true) { return --tokens.end(); }

	bool is_eof() const noexcept { return token_it == tokens.end(); }

private:
	bool eof_allowed = true;

	bool S_ReadToken(token_t& token);
	bool S_ReadWhitespace();
	bool S_ReadNumber(token_t& token);

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