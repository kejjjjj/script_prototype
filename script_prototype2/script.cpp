#include "script.hpp"
#include "filetools.hpp"
#include "datatype.hpp"
#include "statement.hpp"
#include "scope.hpp"

script_t::script_t(const std::string& filename)
{

	std::string path = filename;
	std::fstream f;
	path = fs::get_root_directory() + "\\" + path;
	std::cout << "path: " << path << '\n';
	this->filename = filename;
	if (!fs::open_file(f, path, fs::fileopen::FILE_IN)) {
		return;
	}
	std::cout << std::format("\"{}\" successfully opened!\n", path);

	//read file to a string buffer
	char buf[4096];
	while (f.read(buf, sizeof(buf)))
		this->buffer.append(buf, sizeof(buf));
	this->buffer.append(buf, f.gcount());

	if (this->buffer.empty()) {
		fs::close_file(f);
		return;
	}

	fs::close_file(f);

	this->script_p = this->buffer.begin();
	this->lastscript_p = script_p;
	this->scriptend_p = this->buffer.end();
	this->line = 1ull;
	this->column = 1ull;
	
}
void script_t::S_Tokenize()
{
	token_t token;

	while (S_ReadToken(token)) {
		tokens.push_back(token);
		token.reset();
	}
	token_it = tokens.begin();
}
std::optional<code_segment_t> script_t::create_code_segment(scr_scope_t** scope)
{
	auto& end = token_it;
	auto begin = token_it;

	if (begin->tt == tokenType::PUNCTUATION) {

		switch (LOWORD(end->extrainfo)) {

		case P_CURLYBRACKET_OPEN:
			*scope = create_scope(*this, *scope);
			return std::nullopt;
		case P_CURLYBRACKET_CLOSE:
			*scope = delete_scope(*this, *scope);
			return std::nullopt;
		default:
			break;
		}
	}
	else if (begin->tt == tokenType::FUNCTION){
		while (end != tokens.end()) {
			if (end->tt == tokenType::PUNCTUATION && (LOWORD(end->extrainfo) == P_CURLYBRACKET_OPEN)) {
				--end;
				return code_segment_t{ begin, begin, end };
			}
			end++;
		}

		throw scriptError_t(this, std::format("unexpected end of file"));

	}

	while (end != tokens.end()) {

		if (end->tt == tokenType::PUNCTUATION && (LOWORD(end->extrainfo) == P_SEMICOLON /*|| LOWORD(end->extrainfo) == P_CURLYBRACKET_CLOSE*/))
			return code_segment_t{ begin, begin, --end++ };

		end++;
	}

	const auto last_token = (--end++);

	if (last_token->tt == tokenType::PUNCTUATION && (LOWORD(last_token->extrainfo) == P_SEMICOLON || LOWORD(last_token->extrainfo) == P_CURLYBRACKET_CLOSE)) {
		end = tokens.end();
		return code_segment_t{ last_token, last_token, last_token };
	}

	throw scriptError_t(this, std::format("unexpected end of file"));
}
code_segment_t script_t::S_GiveRemaining() noexcept(true)
{
	return code_segment_t{ token_it, token_it, --tokens.end()};
}
bool script_t::S_ReadToken(token_t& token)
{
	lastscript_p = script_p;

	if (!S_ReadWhitespace())
		return 0;

	token.line = line;
	token.column = column;

	if (std::isdigit(*script_p) || *script_p == '.' && std::isdigit(*(script_p + 1))) {
		if (!S_ReadNumber(token)) {
			return 0;
		}
	}
	else if (std::isalpha(*script_p) || *script_p == '_') {
		if (!S_ReadName(token)) {
			return 0;
		}
	}
	else if (*script_p == '\'') {
		if (!S_ReadCharacterLiteral(token)) {
			return 0;
		}
	}
	else {
		if (!S_ReadPunctuation(token)) {
			throw scriptError_t(this, "an unexpected token");
			return 0;
		}
	}

	return 1;
}

bool script_t::S_ReadWhitespace()
{
	while (*script_p <= ' ') {


		if (script_p == scriptend_p)
			return 0;

		if (*script_p == '\n') {
			line++;
			column = 1ull;
		}
		else {
			if (*script_p == '\t')
				column += 4;
			else
				column++;
		}

		script_p++;
	}

	return 1;
}
bool script_t::S_ParseInt(token_t& token)
{
	if (script_p == scriptend_p)
		return 0;

	if (std::isdigit(*script_p)) {
		token.string.push_back(*script_p++);

		if (script_p == scriptend_p)
			return 0;

		while (isdigit(*script_p)) {
			token.string.push_back(*script_p++);

			if (script_p == scriptend_p)
				return 0;

		}
	}

	return 1;

}
bool script_t::S_ReadNumber(token_t& token)
{
		
	//a floating point literal with . prefix
	if (*script_p == '.') { //assumes that there is a number next

		//if the character after the dot is not a number, then stop
		if (!std::isdigit(*(script_p + 1))) {
			return 0;
		}

		token.string.push_back(*script_p++);
		token.tt = tokenType::FLOAT_LITERAL;

		//parse the integer literal after the .
		if (!S_ParseInt(token))
			return 0;

	}

	//an integral literal
	else if(std::isdigit(*script_p)){
		token.tt = tokenType::NUMBER_LITERAL;

		if (!S_ParseInt(token))
			return 0;

		//floating point decimal
		if (*script_p == '.') { 
			token.string.push_back(*script_p++);
			token.tt = tokenType::FLOAT_LITERAL;

			//parse the integer literal after the .
			if (!S_ParseInt(token)) 
				return 0;
		}
		else {
			token.extrainfo = std::underlying_type_t<literalSuffixType>(literalSuffixType::NONE);

			if (*script_p == 'u' || *script_p == 'U') {
				token.extrainfo = std::underlying_type_t<literalSuffixType>(literalSuffixType::UNSIGNED);
				token.string.push_back(*script_p++);
			}
			else if (*script_p == 'c' || *script_p == 'C') {
				token.extrainfo = std::underlying_type_t<literalSuffixType>(literalSuffixType::CHAR);
				token.string.push_back(*script_p++);
			}
		}

	}

	//token.print();
	column += token.string.length();
	return 1;
}
bool script_t::S_ReadName(token_t& token)
{
	token.tt = tokenType::NAME;
	token.string.push_back(*script_p++);

	while (std::isalnum(*script_p) || *script_p == '_') {

		if (script_p == scriptend_p)
			return 0;
		

		token.string.push_back(*script_p++);
	}

	//built-in datatype keyword
	if (const auto it = dataTypeTable::getInstance().find_builtin(token.string)) {

		if (it.has_value()) {
			token.tt = tokenType::BUILT_IN_TYPE;
			token.extrainfo = static_cast<DWORD>(it->second);
		}
	}
	//built-in statement keyword
	else if (const auto statement_it = statementKeywordTable::getInstance().find_builtin(token.string)) {
		token.tt = tokenType::STATEMENT;
		token.extrainfo = static_cast<std::underlying_type_t<statementKeywords_e>>((statement_it->first->second));
	}
	else if (token.string == "fn") {
		token.tt = tokenType::FUNCTION;
		token.extrainfo = 0;
	}

	//token.print();
	column += token.string.length();

	return 1;
}
bool script_t::S_ReadCharacterLiteral(token_t& token)
{
	token.tt = tokenType::CHAR_LITERAL;
	token.string.push_back(*script_p++);

	while (*script_p != '\'') {

		if (script_p == scriptend_p)
			return 0;

		token.string.push_back(*script_p++);

	}
	token.string.push_back(*script_p++);
	column += token.string.length();
	
	return 1;
}
bool script_t::S_ReadPunctuation(token_t& token)
{
	for (const auto& i : punctuations) {

		//check eof
		if ((script_p + i.identifier.length()-1ull) != scriptend_p) {

			
			const std::string parsed_str = std::string(script_p, (script_p + i.identifier.length())); 

			if (!parsed_str.compare(i.identifier)) {
				token.string = i.identifier;
				token.tt = tokenType::PUNCTUATION;

				MAKE_LOWORD(token.extrainfo, static_cast<WORD>(i.punc));
				MAKE_HIWORD(token.extrainfo, static_cast<DWORD>(i.priority));


				script_p += token.string.length();

				//token.print();
				column += token.string.length();

				

				return 1;
			}

			
		}

	}
	return 0;
}
